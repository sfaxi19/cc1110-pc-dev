#include <iostream>
#include <stdexcept>

#include <chrono>
#include <thread>

#include "link_fsm.hpp"
#include "utils.hpp"
#include "global.hpp"
#include "BoardClient.hpp"

namespace cc1110
{

const char* toString(eMode mode)
{
	switch(mode)
	{
		case eMode::RADIO_MODE_TX: return "RADIO_MODE_TX";
		case eMode::RADIO_MODE_RX: return "RADIO_MODE_RX";
		default: return "UNKNOWN_RADIO_MODE";
	}
}

template<class T>
class singleton
{
public:
	static auto* getInstanse()
	{
		static T obj{};
		return &obj;
	}
};


class LinkFsmState
{
public:
	LinkFsmState(eStateId stateId) : m_stateId{stateId} {}
	virtual const char* name() = 0;

	virtual void OnStart(LinkFsm* link)                              { LOG("Unexpected event for %s state\n", name()); }
	virtual void OnMessage(LinkFsm* link, msg::packet_s& packet)     { LOG("Unexpected event for %s state\n", name()); }
	virtual void OnEntry(LinkFsm* link)                              { /*LOG("Unexpected event for %s state\n", name());*/ }
	virtual void OnExit(LinkFsm* link)                               { /*LOG("Unexpected event for %s state\n", name());*/ }
	virtual void OnTimeout(LinkFsm* link)                            { /*LOG("Unexpected event for %s state\n", name());*/ }
	virtual void OnConfigure(LinkFsm* link)                          { ChangeState<LinkFsmStateSetup>(link); }

	template<class STATE>
	void ChangeState(LinkFsm* link)
	{
		if (!std::is_base_of<LinkFsmState, STATE>::value)
		{
			throw std::runtime_error("ChangeState: State should be inherit of Link!");
		}

		OnExit(link);
		LinkFsmState* new_state = singleton<STATE>::getInstanse();
		INFO("%s -> %s\n", link->m_state->name(), new_state->name());
		link->m_state = new_state;
		link->m_state->OnEntry(link);
	}

	eStateId GetStateId() { return m_stateId; }
private:
	eStateId m_stateId;
};



class LinkFsmStateInit : public LinkFsmState
{
public:
	LinkFsmStateInit() : LinkFsmState(eStateId::INIT) {}
	const char* name() override { return "INIT"; }

	void OnStart(LinkFsm* link) override { ChangeState<LinkFsmStateConnecting>(link); }
};



class LinkFsmStateConnecting : public LinkFsmState
{
public:
	LinkFsmStateConnecting() : LinkFsmState(eStateId::CONNECTING) {}
	const char* name() override { return "CONNECTING"; }

	void OnMessage(LinkFsm* link, msg::packet_s& packet) override
	{
		switch(packet.header.type)
		{
			case msg::WAKEUP_ACK:
				LOG("WAKEUP_ACK\n");
				ChangeState<LinkFsmStateSetup>(link);
				break;
			default:
				LOG("Unexpected messate in %s state\n", name());
				break;
		}
	}

	void OnTimeout(LinkFsm* link) override
	{
		LOG("%s\n", __FUNCTION__);
		OnEntry(link);
	}

	void OnEntry(LinkFsm* link) override
	{ 
		link->SendWakeUp();
	}
};



class LinkFsmStateSetup : public LinkFsmState
{
public:
	LinkFsmStateSetup() : LinkFsmState(eStateId::SETUP) {}
	const char* name() override { return "SETUP"; }
	
	void OnMessage(LinkFsm* link, msg::packet_s& packet) override
	{
		switch(packet.header.type)
		{
			case msg::SETUP_ACK:
				LOG("SETUP_ACK\n");

				if (link->m_board_client->GetSettings().GetMode() == eMode::RADIO_MODE_TX)
				{
					ChangeState<LinkFsmStateTxActive>(link);
				}
				else
				{
					ChangeState<LinkFsmStateRxActive>(link);
				}

				break;
			case msg::SETUP_ERR:
				LOG("SETUP_ERR\n");
				ChangeState<LinkFsmStateEnd>(link);
				break;
			default:
				LOG("Unexpected messate in %s state\n", name());
				break;
    	}
	}

	void OnTimeout(LinkFsm* link) override
	{
		static int counter = 0;
		if(counter++ > 3)
		{
			OnEntry(link);	
		}
	}

	void OnEntry(LinkFsm* link) override
	{ 
		link->SendSetupRequest();
	}

	void OnExit(LinkFsm* link) override
	{ 
		/* */
	}
};



class LinkFsmStateTxActive : public LinkFsmState
{
public:
	LinkFsmStateTxActive() : LinkFsmState(eStateId::TX_ACTIVE) {}
	const char* name() override { return "TX_ACTIVE"; }

	void OnMessage(LinkFsm* link, msg::packet_s& packet) override
	{
		TRACE_FUNCTION();
		switch(packet.header.type)
		{
			case msg::DATA_ACK:
				LOG("DATA_ACK\n");
				link->m_board_client->PopPacket();
				OnEntry(link);
				
				break;
			default:
				LOG("Unexpected message in %s state\n", name());
				break;
    	}
	}

	void OnEntry(LinkFsm* link) override
	{
		TRACE_FUNCTION();
		if (!link->m_board_client->IsPacketListEmpty())
		{
			link->SendTxDataReq(*link->m_board_client->FrontPacket());
			//HACK: last four bytes of packet is TRANSMISSIONS info
			link->m_tx_delay = *(uint32_t*)&(*link->m_board_client->FrontPacket())[link->m_board_client->FrontPacket()->size() - sizeof(uint32_t)];
		}
	}

	void OnExit(LinkFsm* link) override
	{
		/* */
	}

	void OnTimeout(LinkFsm* link) override
	{
		TRACE_FUNCTION();
		static uint32_t cnt = 0;
		if (++cnt >= link->m_tx_delay)
		{
			OnEntry(link);
			cnt = 0;
		}
		
	}
};



class LinkFsmStateRxActive : public LinkFsmState
{
public:
	LinkFsmStateRxActive() : LinkFsmState(eStateId::RX_ACTIVE) {}
	const char* name() override { return "RX_ACTIVE"; }

	void OnMessage(LinkFsm* link, msg::packet_s& packet) override // msg::header_s msg_header, std::vector<uint8_t>& msg) override
	{
		switch(packet.header.type)
		{
			case msg::DATA_REQ:
			{
				LOG("DATA_REQ\n");
				std::vector<uint8_t> msg(packet.data, packet.data + packet.header.size);

				link->SaveToFile(reinterpret_cast<char*>(msg.data()), msg.size());
				link->ReceiveCallback(msg);

				LOG("Data has received: [");
				std::for_each(msg.begin(), msg.end() - 2, [](auto& val){ LOG("%02x ", val); });

				int rssi = rssi_converter(msg[msg.size() - 2]);
				uint8_t lqi = msg[msg.size() - 1];
				LOG("] RSSI: %d  LQI: %u\n", rssi, lqi);
				
				break;
			}
			default:
				LOG("Unexpected messate in %s state\n", name());
				break;
    	}
	}

	void OnEntry(LinkFsm* link) override
	{
		/* */
	}

	void OnExit(LinkFsm* link) override
	{
		/* */
	}
};



class LinkFsmStateEnd : public LinkFsmState
{
public:
	LinkFsmStateEnd() : LinkFsmState(eStateId::END) {}
	const char* name() override { return "END"; }
};



LinkFsm::LinkFsm(BoardClient* board_client, SerialPort_t& serial_port, const char* filename) 
	: m_board_client{board_client}
	, m_serial_port{serial_port}
{
	if (filename)
	{
		m_file = std::ofstream(filename);
	}
	m_state = singleton<LinkFsmStateInit>::getInstanse();
}

void LinkFsm::Activate()
{ 
	m_state->OnStart(this);
}

void LinkFsm::Configure()
{
	if (m_state->GetStateId() != eStateId::INIT)
	{
		m_state->OnConfigure(this);
	}
}

void LinkFsm::OnMessage(msg::packet_s& packet)//msg::header_s msg_header, std::vector<uint8_t>& msg)
{ 
	m_state->OnMessage(this, packet);//msg_header, msg); 
}

void LinkFsm::OnTimeout()
{
	m_state->OnTimeout(this);
}

void LinkFsm::SendWakeUp()
{
	msg::packet_s packet(msg::WAKEUP);

	std::vector<uint8_t> msg = msg::encode(packet);

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);
}

void LinkFsm::SendSetupRequest()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(timers::timeout_send_ms));

	msg::packet_s packet(msg::SETUP_REQ, sizeof(m_board_client->GetSettings()));
	packet.assign(&m_board_client->GetSettings(), sizeof(m_board_client->GetSettings()));

	std::vector<uint8_t> msg = msg::encode(packet);

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);
}

void LinkFsm::SendSetupAck()
{
	msg::packet_s packet(msg::SETUP_ACK);
	std::vector<uint8_t> msg = msg::encode(packet);

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);	
}


void LinkFsm::SendSetupErr()
{
	msg::packet_s packet(msg::SETUP_ERR);
	std::vector<uint8_t> msg = msg::encode(packet);

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);	
}

void LinkFsm::SendTxDataReq(std::vector<uint8_t>& data)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(timers::timeout_send_ms));
	
	msg::packet_s packet(msg::DATA_REQ, data.size());
	packet.assign(data.data(), data.size());
	std::vector<uint8_t> msg = msg::encode(packet);

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);
}

void LinkFsm::SendTxDataAck()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(timers::timeout_send_ms));

	msg::packet_s packet(msg::DATA_ACK);
	std::vector<uint8_t> msg = msg::encode(packet);

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);	
}

void LinkFsm::SendErr()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(timers::timeout_send_ms));

	msg::packet_s packet(msg::ERR);
	std::vector<uint8_t> msg = msg::encode(packet);

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);	
}

bool LinkFsm::IsActive()
{ 
	return m_state->GetStateId() == eStateId::TX_ACTIVE; 
}

void LinkFsm::SaveToFile(const char* data, size_t size)
{
	if (m_file.is_open())
	{
		uint8_t size8 = (uint8_t) size;
		m_file.write(reinterpret_cast<char*>(&size8), sizeof(size8));
		m_file.write(data, size);
		m_file.flush();
	}
	// else
	// {
	// 	ERR("%s: Fail!", __PRETTY_FUNCTION__);
	// }
}

void LinkFsm::ReceiveCallback(std::vector<uint8_t>& msg)
{
	m_board_client->m_recv_callback(msg);
}

}