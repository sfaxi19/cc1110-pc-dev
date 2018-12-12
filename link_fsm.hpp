#pragma once

#include <vector>

class LinkFsmState;
class LinkFsmStateInit;
class LinkFsmStateConnecting;
class LinkFsmStateSetup;
class LinkFsmStateActive;
class LinkFsmStateEnd;


namespace LibSerial
{
class SerialPort;
}

namespace msg
{
class header_s;	
}

class LinkFsm
{
public:
	LinkFsm(LibSerial::ISerialPort& serial_port);

	void activate();

	void OnMessage(msg::header_s msg_header, std::vector<uint8_t>&);

	void OnTimeout();

	void SendWakeUp();

	void SendSetupRequest();
	void SendSetupAck();
	void SendSetupErr();

private:

	LinkFsmState*            m_state;
	LibSerial::ISerialPort&  m_serial_port;
	
	friend LinkFsmState;
	friend LinkFsmStateInit;
	friend LinkFsmStateConnecting;
	friend LinkFsmStateActive;
	friend LinkFsmStateSetup;
	friend LinkFsmStateEnd;
};