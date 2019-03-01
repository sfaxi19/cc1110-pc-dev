#pragma once

#include <list>
#include <vector>
#include <mutex>
#include <functional>

#include "global.hpp"
#include "link_fsm.hpp"
#include "ISerialPort.hpp"

#include "utils.hpp"
#include "msg_format.hpp"

namespace cc1110
{

inline const char* getDumpFileName(const char* filename, eMode mode)
{
    static char dump_name[20];

    if (!filename)
    {
        uint8_t num = 1;
        do
        {
            if (mode == RADIO_MODE_TX) 
                { sprintf(dump_name, "tx.%03u.dump", num++);}
            else                       
                { sprintf(dump_name, "rx.%03u.dump", num++);}
        } while(file_exist(dump_name) && num < 255);

        return dump_name;
    }
    else
    {
        return filename;
    }
}

class BoardClient
{
public:

    using receive_callback_t = std::function<void(std::vector<uint8_t>& msg)>;

	BoardClient(std::string board_path, eMode mode, uint8_t packet_len, const char* filename = nullptr) 
		: m_link_fsm{this, m_serial_port, mode, getDumpFileName(filename, mode)}
        , m_mode{mode}
        , m_packet_len{packet_len}
	    , m_board_path{board_path}
    {
    }

	~BoardClient()
	{
        terminate = true;
        m_thread.join();
		m_serial_port.Close();
	}

	bool Run();
    bool IsActive() { return m_link_fsm.IsActive(); }

    bool                  SendPacket(uint8_t *data, size_t size);
    bool                  SendPacket(std::vector<uint8_t>& msg);
    bool                  IsPacketListEmpty() const;
    size_t                PacketListSize() const { return m_packets.size(); }

    std::vector<uint8_t>* FrontPacket();
    void                  PopPacket();
    settings_s&           GetSettings() { return m_settings; }
    settings_s&           SetSettings(settings_s& settings) { m_settings = settings; return m_settings; }

    void                  SetReceiveCallback(receive_callback_t recv_callback) { m_recv_callback = recv_callback; }

    void                  WaitForActive();

private:
    bool Setup(SerialPort_t& serial_port, std::string path);
    void BaseLoop();

	SerialPort_t                    m_serial_port;
	LinkFsm                         m_link_fsm;

    eMode                           m_mode;
    uint8_t                         m_packet_len;
    std::list<std::vector<uint8_t>> m_packets;
    settings_s                      m_settings;

    std::string                     m_board_path;
    std::thread                     m_thread;
    std::mutex                      m_mtx;
    bool                            terminate = false;

    receive_callback_t              m_recv_callback;

    friend class LinkFsm;
};

}