#pragma once

#include <vector>
#include <fstream>

#include "global.hpp"
#include "settings.hpp"

namespace cc1110::msg
{
	class packet_s;
}

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

const char* toString(eMode);

namespace msg
{
class header_s;	
}

class LinkFsm
{
public:
	LinkFsm(BoardClient* board_client, SerialPort_t& serial_port, const char*);

	void Activate();

	void OnMessage(msg::packet_s& packet);//msg::header_s msg_header, std::vector<uint8_t>&);

	void OnTimeout();

	void Configure();

	void SendWakeUp();

	void SendSetupRequest();
	void SendSetupAck();
	void SendSetupErr();
	
	void SendTxDataReq(std::vector<uint8_t>& data);
	void SendTxDataAck();

	void SendPacket();

	void SendErr();

	bool IsActive();

	eStateId GetStateId();

	void SaveToFile(const char* data, size_t size);

	void ReceiveCallback(std::vector<uint8_t>& msg);

    bool                  IsPacketListEmpty() const;
    size_t                PacketListSize() const;
    std::vector<uint8_t>* FrontPacket();
    void                  PopPacket();
private:

	BoardClient*             m_board_client;
	LinkFsmState*            m_state;
	SerialPort_t&            m_serial_port;
	std::ofstream            m_file;

	uint32_t                 m_tx_delay{1};
	
	friend LinkFsmState;
	friend LinkFsmStateInit;
	friend LinkFsmStateConnecting;
	friend LinkFsmStateTxActive;
	friend LinkFsmStateRxActive;
	friend LinkFsmStateSetup;
	friend LinkFsmStateEnd;
};

}