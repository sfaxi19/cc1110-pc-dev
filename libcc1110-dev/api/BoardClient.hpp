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

// inline const char* getDumpFileName(const char* filename)
// {
//     static char dump_name[20];

//     if (!filename)
//     {
//         uint8_t num = 1;
//         do
//         {
//             if (mode == RADIO_MODE_TX) 
//                 { sprintf(dump_name, "tx.%03u.dump", num++);}
//             else                       
//                 { sprintf(dump_name, "rx.%03u.dump", num++);}
//         } while(file_exist(dump_name) && num < 255);

//         return dump_name;
//         return nullptr;
//     }
//     else
//     {
//         return filename;
//     }
// }

class BoardClient
{
public:

    using receive_callback_t = std::function<void(std::vector<uint8_t>& msg, int rssi, int lqi)>;

	explicit BoardClient(std::string board_path, const char* filename = nullptr);
    ~BoardClient();

    BoardClient(const BoardClient& other) = delete;
    BoardClient(BoardClient&& other) = delete;

    BoardClient& operator=(const BoardClient& other) = delete;
    BoardClient& operator=(BoardClient&& other) = delete;

    void                  Configure(settings_s settings, eMode mode, uint8_t msg_size);
	bool                  Run();
    void                  Stop();
    bool                  IsActive() { return m_link_fsm.IsActive() && terminate == false; }

    bool                  SendPacket(std::vector<uint8_t>& msg, uint32_t transmissions = 1);
    bool                  SendPacketAsync(std::vector<uint8_t> msg, uint32_t transmissions = 1);

    std::tuple<std::vector<uint8_t>, int, int>  ReceivePacket(size_t timeout_ms = 0);
    void                  ReceivePacketAsync(receive_callback_t callback);

    settings_s&           GetSettings() { return m_settings; }
    settings_s&           SetSettings(settings_s& settings) { m_settings = settings; return m_settings; }
    
    void                  Waiting(size_t ms);

private:
    void                  WaitingActive();
    void                  WaitingSend();
    void                  WaitingReceive();


    bool                  IsPacketListEmpty() const;
    size_t                PacketListSize() const { return m_packets.size(); }

    std::vector<uint8_t>* FrontPacket();
    void                  PopPacket();

    bool                  SendPacket(uint8_t *data, size_t size);
    void                  SetReceiveCallback(receive_callback_t callback);
    void                  ReceiveCallback(std::vector<uint8_t>& msg);
    
    bool                  Setup(SerialPort_t& serial_port, std::string path);
    void                  BaseLoop();

    uint16_t                        m_tx_delay{0};
    
	SerialPort_t                    m_serial_port;
	LinkFsm                         m_link_fsm;

    std::list<std::vector<uint8_t>> m_packets;
    settings_s                      m_settings;

    std::string                     m_board_path;
    std::thread                     m_thread;
    std::mutex                      m_mtx;
    std::mutex                      m_recv_mtx;
    bool                            terminate = false;
    bool                            async_receive_enable {false};

    receive_callback_t              m_receive_callback;

    friend class LinkFsm;
};

}