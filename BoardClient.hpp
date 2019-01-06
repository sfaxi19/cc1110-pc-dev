#pragma once

#include <list>
#include <vector>
#include <mutex>

#include "global.hpp"
#include "link_fsm.hpp"
#include "ISerialPort.hpp"

#include "utils.hpp"
#include "msg_format.hpp"

class BoardClient
{
public:

	BoardClient(std::string board_path, eMode mode, uint8_t packet_len, const char* filename) 
		: m_link_fsm{this, m_serial_port, mode, filename}
        , m_mode{mode}
	    , m_board_path{board_path}
    {
        m_settings.MODE = m_mode;
        m_settings.SetCRCEnable(0);
        m_settings.SetPacketLength(packet_len);
    }

	~BoardClient()
	{
        terminate = true;
        m_thread.join();
		m_serial_port.Close();
	}

	bool Setup(LibSerial::ISerialPort& serial_port, std::string path);
	bool Run();
    void BaseLoop();
    bool IsActive() { return m_link_fsm.IsActive(); }

    bool                  SendPacket(uint8_t *data, size_t size);
    bool                  SendPacket(std::vector<uint8_t>& msg);
    bool                  IsPacketListEmpty() const;
    size_t                PacketListSize() const { return m_packets.size(); }

    std::vector<uint8_t>* FrontPacket();
    void                  PopPacket();
    settings_s&           GetSettings() { return m_settings; }

private:
	SerialPort_t                    m_serial_port;
	LinkFsm                         m_link_fsm;

    eMode                           m_mode;    
    std::list<std::vector<uint8_t>> m_packets;
    settings_s                      m_settings;

    std::string                     m_board_path;
    std::thread                     m_thread;
    std::mutex                      m_mtx;
    bool                            terminate = false;
};