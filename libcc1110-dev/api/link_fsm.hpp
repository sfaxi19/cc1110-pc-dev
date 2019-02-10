#pragma once

#include <vector>
#include <fstream>

#include "global.hpp"
#include "settings.hpp"

namespace cc1110
{

class BoardClient;
class LinkFsmState;
class LinkFsmStateInit;
class LinkFsmStateConnecting;
class LinkFsmStateSetup;
class LinkFsmStateTxActive;
class LinkFsmStateRxActive;
class LinkFsmStateEnd;

enum eStateId
{
	UNKNOWN,
	SETUP,
	INIT, 
	CONNECTING, 
	TX_ACTIVE,
	RX_ACTIVE,
	END
};

enum eMode
{
	RADIO_MODE_TX = 0x10,
	RADIO_MODE_RX = 0x20
};

const char* toString(eMode);

namespace msg
{
class header_s;	
}

class LinkFsm
{
public:
	LinkFsm(BoardClient* board_client, SerialPort_t& serial_port, eMode mode, const char*);

	void activate();

	void OnMessage(msg::header_s msg_header, std::vector<uint8_t>&);

	void OnTimeout();

	void SendWakeUp();

	void SendSetupRequest();
	void SendSetupAck();
	void SendSetupErr();
	
	void SendTxDataReq();
	void SendTxDataAck();

	bool IsActive();

	void SaveToFile(const char* data, size_t size);

	void ReceiveCallback(std::vector<uint8_t>& msg);

private:

	BoardClient*             m_board_client;
	LinkFsmState*            m_state;
	SerialPort_t&            m_serial_port;
	eMode                    m_mode;
	std::ofstream            m_file;
	
	friend LinkFsmState;
	friend LinkFsmStateInit;
	friend LinkFsmStateConnecting;
	friend LinkFsmStateTxActive;
	friend LinkFsmStateRxActive;
	friend LinkFsmStateSetup;
	friend LinkFsmStateEnd;
};

}