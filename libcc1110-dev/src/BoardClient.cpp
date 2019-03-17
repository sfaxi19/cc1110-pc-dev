#include "BoardClient.hpp"

#define CHECK_ACTIVE() if (!IsActive()) { ERR("%s function failed. BoardClient is not active!\n", __FUNCTION__); Stop(); exit(1); }

namespace cc1110
{

BoardClient::BoardClient(std::string board_path, const char* filename) 
    : m_link_fsm{this, m_serial_port, filename}
    , m_board_path{board_path}
{
    if (logging)
    {
        std::string fname;
        fname.append("cc1110_").append(board_path.substr(board_path.find_last_of('/') + 1)).append(".log");

        logfile = fopen(fname.c_str(), "w");

        if (!logfile) 
        {
            ERR("File \"%s\" has not created!\n", fname.c_str());
        }
    }
}

BoardClient::~BoardClient()
{
    Stop();
}

void BoardClient::Stop()
{
    terminate = true;
    if (m_thread.joinable()) 
    {
        m_thread.join();
    }
    if (m_serial_port.IsOpen())
    {
        m_serial_port.Close();
    }
}

void BoardClient::BaseLoop()
{
    const uint32_t rx_buffer_size = 512;
    std::vector<uint8_t> buffer(rx_buffer_size);
    m_link_fsm.Activate();

    while(!(terminate && IsPacketListEmpty()))
    {
        try
        {
            std::vector<uint8_t> recv_data;
            DEBUG("Waiting for HEADER[%zu] bytes...\n", sizeof(msg::header_s));
            
            m_serial_port.Read(recv_data, sizeof(msg::header_s), time::timeout_read_ms);

            auto b_it = std::copy(recv_data.begin(), recv_data.end(), buffer.begin());
            msg::header_s msg_header = *reinterpret_cast<msg::header_s*>(buffer.data());
            
            if (msg_header.size > rx_buffer_size)
            {
                ERR("Packet format error! Type: %s, Size: %u.\n", toString(msg_header.type), msg_header.size);
                continue;
            }
 
            DEBUG("Waiting for DATA[%zu] bytes...\n", msg_header.size + sizeof(msg::crc_t));
            m_serial_port.Read(recv_data, msg_header.size + sizeof(msg::crc_t), time::timeout_read_ms);
            std::copy(recv_data.begin(), recv_data.end(), b_it);

            auto [packet, success] = msg::decode(buffer);
            if (!success)
            {
                ERR("Decode packet error!\n");
                //m_link_fsm.SendErr();
                continue;
            }

            m_link_fsm.OnMessage(packet);
        }
        catch (const LibSerial::ReadTimeout& err)
        {
            DEBUG("--- %lu milliseconds passed ---\n", time::timeout_read_ms);
            m_link_fsm.OnTimeout();
        }
    }
}

bool BoardClient::Run()
{
    TRACE_FUNCTION();
    if (IsActive())
    {
        return true;
    }

    if (!m_settings.IsEnabled())
    {
        ERR("Settings is not configured!\n");
        return false;
    }

	if (!Setup(m_serial_port, m_board_path))
    {
        return false;
    }
    std::thread thr(&BoardClient::BaseLoop, this);
    m_thread = std::move(thr);

	return true;
}

void BoardClient::Configure(settings_s settings, eMode mode, uint8_t packet_length)
{
    TRACE_FUNCTION();
    m_settings = settings;
    m_settings.activate(mode, packet_length);

    INFO("+----------------CONFIGURE---------------+\n");
    INFO("| BOARD PATH      : %20s |\n", m_board_path.c_str());
    INFO("| RADIO MODE      : %20s |\n", toString(GetSettings().GetMode()));
    INFO("| PACKET LENGTH   : %20u |\n", GetSettings().GetPacketLength());
    INFO("+----------------------------------------+\n");

    if (m_link_fsm.GetStateId() == eStateId::INIT)
    {
        Run();
    }
    else
    {
        m_link_fsm.Configure();      
    }
    WaitingActive();
}

bool BoardClient::SendPacket(std::vector<uint8_t>& msg, uint32_t transmissions)
{
    TRACE_FUNCTION();
    CHECK_ACTIVE();

    SendPacketAsync(msg, transmissions);
    WaitingSend();
}

bool BoardClient::SendPacketAsync(std::vector<uint8_t> msg, uint32_t transmissions)
{
    TRACE_FUNCTION();
    CHECK_ACTIVE();
 
    if (msg.size() != GetSettings().GetPacketLength())
    {
        ERR("Packet length error! Actual: %zu != Expected: %u\n", msg.size(), GetSettings().GetPacketLength());
        return false;
    }

    for (int i = 0; i < sizeof(transmissions); i++)
    {
        msg.push_back(((uint8_t*)&transmissions)[i]);
    }

	return SendPacket(msg.data(), msg.size());
}

bool BoardClient::SendPacket(uint8_t *data, size_t size)
{
	TRACE_FUNCTION();

    bool do_send = false; 
    {
    	std::lock_guard<std::mutex> lock(m_mtx);

        do_send = IsPacketListEmpty();
        m_packets.emplace_back(data, data + size);
    }//unlock

    if (do_send)
    {
        m_link_fsm.SendPacket();
    }
    return true;
}

inline std::tuple<std::vector<uint8_t>, int, int> preparation(std::vector<uint8_t>& msg)
{
    return {std::vector<uint8_t>(msg.begin(), msg.end() - 2), cc1110::rssi_converter(msg[msg.size() - 2]), msg[msg.size() - 1]};
}

std::tuple<std::vector<uint8_t>, int, int> BoardClient::ReceivePacket(size_t timeout_ms)
{
    CHECK_ACTIVE();

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    bool packet_received = false;
    std::vector<uint8_t> recv_msg;
    int rssi = 0;
    int lqi = 0;

    auto callback = [&](std::vector<uint8_t>& _data, int _rssi, int _lqi)
    {
        rssi = _rssi;
        lqi = _lqi;
        recv_msg = std::move(_data); 
        packet_received = true; 
    };

    SetReceiveCallback(callback);

    while(!packet_received)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time::waitingfor_timer_ms));
       
        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        size_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (timeout_ms != 0 && ms >= timeout_ms)
        {
            break;
        }
    }

    SetReceiveCallback(nullptr);
    return std::make_tuple(recv_msg, rssi, lqi);
}

void BoardClient::ReceivePacketAsync(receive_callback_t callback)
{            
    SetReceiveCallback(callback);
}

void BoardClient::SetReceiveCallback(receive_callback_t callback)
{
    std::lock_guard<std::mutex> lock(m_recv_mtx);
    m_receive_callback = callback; 
}

void BoardClient::ReceiveCallback(std::vector<uint8_t>& msg)
{
    std::lock_guard<std::mutex> lock(m_recv_mtx);
    if (m_receive_callback)
    {
        auto [data, rssi, lqi] = preparation(msg);
        m_receive_callback(data, rssi, lqi);
    }
}

bool BoardClient::IsPacketListEmpty() const
{
	return m_packets.empty();
}

std::vector<uint8_t>* BoardClient::FrontPacket()
{
	TRACE_FUNCTION();
	std::lock_guard<std::mutex> lock(m_mtx);

    if (m_packets.empty())
    {
        return nullptr;
    }
    return &m_packets.front();
} 

void BoardClient::PopPacket()
{
	TRACE_FUNCTION();
	std::lock_guard<std::mutex> lock(m_mtx);

    m_packets.pop_front();
} 

void BoardClient::WaitingActive()
{
    while(!IsActive())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time::waitingfor_timer_ms));
    }
}

void BoardClient::WaitingSend()
{
    CHECK_ACTIVE();
    while(!IsPacketListEmpty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(time::waitingfor_timer_ms));
    }
}

void BoardClient::Waiting(size_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool BoardClient::Setup(SerialPort_t& serial_port, std::string path)
{
    TRACE_FUNCTION();
    // Open the Serial Ports at the desired hardware devices.
    serial_port.Open(path);
    // Verify that the serial ports opened.
    if (!serial_port.IsOpen())
    {
        std::cerr << "The serial ports did not open correctly." << std::endl;
        return false;
    }
    // Set the baud rates.
    serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_230400); //BAUD_57600 BAUD_230400
    // Set the number of data bits.
    serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    // Set the hardware flow control.
    serial_port.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
    // Set the parity.
    serial_port.SetParity(LibSerial::Parity::PARITY_NONE);
    // Set the number of stop bits.
    serial_port.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
    serial_port.FlushIOBuffers();
    return true;
}

}