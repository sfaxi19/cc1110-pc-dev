#include <iostream>
#include <stdexcept>

#include <chrono>
#include <thread>

#include "SerialPort.h"
#include "link_fsm.hpp"
#include "msg_format.hpp"
#include "utils.hpp"


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
	virtual const char* name() = 0;

	virtual void OnStart(LinkFsm* link)                              { printf("Unexpected event for %s state\n", name()); }
	virtual void OnMessage(LinkFsm* link, 
						   msg::header_s msg_header, 
						   std::vector<uint8_t>& msg)                { printf("Unexpected event for %s state\n", name()); }
	virtual void OnEntry(LinkFsm* link)                              { /*printf("Unexpected event for %s state\n", name());*/ }
	virtual void OnExit(LinkFsm* link)                               { /*printf("Unexpected event for %s state\n", name());*/ }
	virtual void OnTimeout(LinkFsm* link)                            { /*printf("Unexpected event for %s state\n", name());*/ }

	template<class STATE>
	void ChangeState(LinkFsm* link)
	{
		if (!std::is_base_of<LinkFsmState, STATE>::value)
		{
			throw std::runtime_error("ChangeState: State should be inherit of Link!");
		}

		OnExit(link);
		LinkFsmState* new_state = singleton<STATE>::getInstanse();
		printf("%s -> %s\n", link->m_state->name(), new_state->name());
		link->m_state = new_state;
		link->m_state->OnEntry(link);
	}
};


class LinkFsmStateInit : public LinkFsmState
{
	const char* name() override { return "INIT"; }

	void OnStart(LinkFsm* link) override { ChangeState<LinkFsmStateConnecting>(link); }
};

class LinkFsmStateConnecting : public LinkFsmState
{
	const char* name() override { return "CONNECTING"; }

	void OnMessage(LinkFsm* link, msg::header_s msg_header, std::vector<uint8_t>& msg) override
	{
		//print_message("Received", msg);
			
		switch(msg_header.type)
		{
			case msg::WAKEUP_ACK:
				printf("WAKEUP_ACK\n");
				ChangeState<LinkFsmStateSetup>(link);
				break;
			case msg::ERR:
				printf("ERR\n");
				ChangeState<LinkFsmStateEnd>(link);
				break;
			default:
				printf("Unexpected messate in %s state\n", name());
				break;
		}
	}


	void OnTimeout(LinkFsm* link) override
	{
		printf("%s\n", __FUNCTION__);
		OnEntry(link);
	}

	void OnEntry(LinkFsm* link) override
	{ 
		link->SendWakeUp();
	}
};

class LinkFsmStateSetup : public LinkFsmState
{
	const char* name() override { return "SETUP"; }
	
	void OnMessage(LinkFsm* link, msg::header_s msg_header, std::vector<uint8_t>& msg) override
	{
		switch(msg_header.type)
		{
			case msg::SETUP_RSP:
				printf("SETUP_RSP\n");
				if(msg_header.size ==  0 /*msg.size()*/)
				{
					link->SendSetupAck();
					ChangeState<LinkFsmStateActive>(link);
				}
				else
				{
					link->SendSetupErr();
					printf("Failure! SETUP_RSP message has bad format.\n");
				}
				break;
			case msg::SETUP_ERR:
				printf("SETUP_ERR\n");
				ChangeState<LinkFsmStateEnd>(link);
				break;
			default:
				printf("Unexpected messate in %s state\n", name());
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


class LinkFsmStateActive : public LinkFsmState
{
	const char* name() override { return "ACTIVE"; }

	void OnMessage(LinkFsm* link, msg::header_s msg_header, std::vector<uint8_t>& msg) override
	{

	}
};

class LinkFsmStateEnd : public LinkFsmState
{
	const char* name() override { return "END"; }
};



LinkFsm::LinkFsm(LibSerial::ISerialPort& serial_port) 
	: m_serial_port{serial_port}
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
	//printf("()\n");
	print_message("SendWakeUp", msg);
}

void LinkFsm::SendSetupRequest()
{
	//std::vector<uint8_t> msg = {msg::SETUP_REQ}
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	msg::setup_req_s setup_req;
	std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&setup_req), reinterpret_cast<uint8_t*>(&setup_req) + sizeof(setup_req));
	//msg::header_s header(msg::SETUP_REQ, 0);
	//std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(msg::header_s));

	m_serial_port.Write(msg);

	print_message("SendSetupRequest", msg);
}

void LinkFsm::SendSetupAck()
{
	msg::header_s header(msg::SETUP_ACK, 0);
	std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(msg::header_s));

	m_serial_port.Write(msg);

	print_message("SendSetupAck", msg);	
}


void LinkFsm::SendSetupErr()
{
	msg::header_s header(msg::SETUP_ERR, 0);
	std::vector<uint8_t> msg(reinterpret_cast<uint8_t*>(&header), reinterpret_cast<uint8_t*>(&header) + sizeof(msg::header_s));

	m_serial_port.Write(msg);

	print_message("SendSetupErr", msg);	
}