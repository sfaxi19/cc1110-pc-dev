#include <iostream>
#include <stdexcept>

#include <chrono>
#include <thread>

#include "SerialPort.h"
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
	virtual void OnMessage(LinkFsm* link, 
						   msg::header_s msg_header, 
						   std::vector<uint8_t>& msg)                { LOG("Unexpected event for %s state\n", name()); }
	virtual void OnEntry(LinkFsm* link)                              { /*LOG("Unexpected event for %s state\n", name());*/ }
	virtual void OnExit(LinkFsm* link)                               { /*LOG("Unexpected event for %s state\n", name());*/ }
	virtual void OnTimeout(LinkFsm* link)                            { /*LOG("Unexpected event for %s state\n", name());*/ }

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

	void OnMessage(LinkFsm* link, msg::header_s msg_header, std::vector<uint8_t>& msg) override
	{
		//print_message("Received", msg);
			
		switch(msg_header.type)
		{
			case msg::WAKEUP_ACK:
				LOG("WAKEUP_ACK\n");
				ChangeState<LinkFsmStateSetup>(link);
				break;
			case msg::ERR:
				LOG("ERR\n");
				ChangeState<LinkFsmStateEnd>(link);
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
	
	void OnMessage(LinkFsm* link, msg::header_s msg_header, std::vector<uint8_t>& msg) override
	{
		switch(msg_header.type)
		{
			case msg::SETUP_RSP:
				LOG("SETUP_RSP\n");
				if(msg_header.size ==  msg.size())
				{
					link->SendSetupAck();
					if (link->m_mode == eMode::RADIO_MODE_TX)
					{
						ChangeState<LinkFsmStateTxActive>(link);
					}
					else
					{
						ChangeState<LinkFsmStateRxActive>(link);
					}
				}
				else
				{
					link->SendSetupErr();
					ERR("Failure! SETUP_RSP message has bad format.\n");
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

	void OnMessage(LinkFsm* link, msg::header_s msg_header, std::vector<uint8_t>& msg) override
	{
		TRACE_FUNCTION();
		switch(msg_header.type)
		{
			case msg::DATA_RSP:
				if (!link->m_board_client->IsPacketListEmpty())
				{
					if (msg.size() == msg_header.size && msg == *link->m_board_client->FrontPacket())
					{
						uint32_t pktCnt = 0;
						while(pktCnt++ < link->m_board_client->GetSettings().GetTransmissions())
						{
							link->SaveToFile(reinterpret_cast<char*>(msg.data()), msg.size());
						}
						link->m_board_client->PopPacket();
						link->SendTxDataAck();
						INFO("Data has sent: [");
						std::for_each(msg.begin(), msg.end(), [](auto& val){ INFO("%02x ", val); });
						INFO("]\n");
					}
					OnEntry(link);
				}
				break;
			default:
				LOG("Unexpected messate in %s state\n", name());
				break;
    	}
	}

	void OnEntry(LinkFsm* link) override
	{
		TRACE_FUNCTION();
		if (!link->m_board_client->IsPacketListEmpty())
		{
			link->SendTxDataReq();
		}
	}

	void OnExit(LinkFsm* link) override
	{
		/* */
	}

	void OnTimeout(LinkFsm* link) override
	{
		TRACE_FUNCTION();
		OnEntry(link);
	}
};



class LinkFsmStateRxActive : public LinkFsmState
{
public:
	LinkFsmStateRxActive() : LinkFsmState(eStateId::RX_ACTIVE) {}
	const char* name() override { return "RX_ACTIVE"; }

	void OnMessage(LinkFsm* link, msg::header_s msg_header, std::vector<uint8_t>& msg) override
	{
		switch(msg_header.type)
		{
			case msg::DATA_REQ:
			{
				LOG("DATA_REQ\n");
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



LinkFsm::LinkFsm(BoardClient* board_client, SerialPort_t& serial_port, eMode mode, const char* filename) 
	: m_board_client{board_client}
	, m_serial_port{serial_port}
	, m_mode{mode}
	, m_file{filename}
{
	m_state = singleton<LinkFsmStateInit>::getInstanse();
}

void LinkFsm::activate()
{ 
	m_state->OnStart(this);
}

void LinkFsm::OnMessage(msg::header_s msg_header, std::vector<uint8_t>& msg)
{ 
	m_state->OnMessage(this, msg_header, msg); 
}

void LinkFsm::OnTimeout()
{
	m_state->OnTimeout(this);
}

void LinkFsm::SendWakeUp()
{
	msg::header_s header(msg::WAKEUP, 0);
	std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(msg::header_s));

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);
}

void LinkFsm::SendSetupRequest()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	msg::setup_req_s setup_req{m_board_client->GetSettings()};
	std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&setup_req), reinterpret_cast<uint8_t*>(&setup_req) + sizeof(setup_req));

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);
}

void LinkFsm::SendSetupAck()
{
	msg::header_s header(msg::SETUP_ACK, 0);
	std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(msg::header_s));

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);	
}


void LinkFsm::SendSetupErr()
{
	msg::header_s header(msg::SETUP_ERR, 0);
	std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(msg::header_s));

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);	
}

void LinkFsm::SendTxDataReq()
{
	static uint8_t counter = 0;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::vector<uint8_t>& data = *m_board_client->FrontPacket();
	uint32_t DATA_SIZE = data.size();
	uint32_t MSG_SIZE = DATA_SIZE + sizeof(msg::header_s);

	std::vector<uint8_t> msg(MSG_SIZE);

	msg::header_s header(msg::DATA_REQ, DATA_SIZE);
	std::copy((uint8_t *)&header, (uint8_t *)(&header + sizeof(header)), msg.begin());
	std::copy(data.begin(), data.end(), msg.begin() + sizeof(header));

	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);
}

void LinkFsm::SendTxDataAck()
{
	msg::header_s header(msg::DATA_ACK, 0);
	std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(msg::header_s));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	m_serial_port.Write(msg);
	print_message(__FUNCTION__, msg);	
}

bool LinkFsm::IsActive()
{ 
	return m_state->GetStateId() == eStateId::TX_ACTIVE; 
}

void LinkFsm::SaveToFile(const char* data, size_t size)
{
	uint8_t size8 = (uint8_t) size;
	if (m_file.is_open())
	{
		m_file.write(reinterpret_cast<char*>(&size8), sizeof(size8));
		m_file.write(data, size);
		m_file.flush();
	}
	else
	{
		ERR("%s: Fail!", __PRETTY_FUNCTION__);
	}
}

void LinkFsm::ReceiveCallback(std::vector<uint8_t>& msg)
{
	m_board_client->m_recv_callback(msg);
}

}