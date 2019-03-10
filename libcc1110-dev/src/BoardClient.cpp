#include "BoardClient.hpp"

namespace cc1110
{

BoardClient::BoardClient(std::string board_path, const char* filename) 
    : m_link_fsm{this, m_serial_port, getDumpFileName(filename)}
    , m_board_path{board_path}
{
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
            DEBUG("Waiting for %zu bytes...\n", sizeof(msg::header_s));
            
            m_serial_port.Read(recv_data, sizeof(msg::header_s), timers::timeout_read_ms);

            auto b_it = std::copy(recv_data.begin(), recv_data.end(), buffer.begin());
            msg::header_s msg_header = *reinterpret_cast<msg::header_s*>(buffer.data());
            
 
            DEBUG("Waiting for %zu bytes...\n", msg_header.size + sizeof(msg::crc_t));
            m_serial_port.Read(recv_data, msg_header.size + sizeof(msg::crc_t), timers::timeout_read_ms);
            std::copy(recv_data.begin(), recv_data.end(), b_it);

            print_message("Receive", buffer, &msg_header);

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
            LOG("--- %lu milliseconds passed ---\n", timers::timeout_read_ms);
            m_link_fsm.OnTimeout();
        }
    }
}

bool BoardClient::Run()
{
    TRACE_FUNCTION();

    if (!m_settings.IsEnabled())
    {
        ERR("Settings is not configured!\n");
        Stop();
        return false;
    }

	if (!Setup(m_serial_port, m_board_path))
    {
        return false;
    }
    std::thread thr(&BoardClient::BaseLoop, this);
    m_thread = std::move(thr);

    WaitForActive();

	return true;
}

void BoardClient::Configure(settings_s settings, eMode mode, uint8_t packet_length)
{
    m_settings = settings;
    m_settings.SetMode(mode);
    m_settings.SetCRCEnable(0);
    m_settings.SetPacketLength(packet_length);
    m_settings.Enable();

    INFO("+----------------CONFIGURE---------------+\n");
    INFO("| BOARD PATH      : %20s |\n", m_board_path.c_str());
    INFO("| RADIO MODE      : %20s |\n", toString(GetSettings().GetMode()));
    INFO("| PACKET LENGTH   : %20u |\n", GetSettings().GetPacketLength());
    INFO("+----------------------------------------+\n");

    m_link_fsm.Configure();
}

bool BoardClient::SendPacket(std::vector<uint8_t>& msg, uint32_t transmissions)
{
    for (int i = 0; i < sizeof(transmissions); i++)
    {
        msg.push_back(((uint8_t*)&transmissions)[i]);
    }

	return SendPacket(msg.data(), msg.size());
}

bool BoardClient::SendPacket(uint8_t *data, size_t size)
{
	TRACE_FUNCTION();
    size_t data_size = size - sizeof(uint32_t);
    
	if (data_size != GetSettings().GetPacketLength())
	{
		ERR("Packet length error! %zu != %u\n", data_size, GetSettings().GetPacketLength());
		return false;
	}
	std::lock_guard<std::mutex> lock(m_mtx);

    m_packets.emplace_back(data, data + size);
    return true;
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

void BoardClient::WaitForActive()
{
    while(!IsActive())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
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
    serial_port.SetBaudRate(LibSerial::BaudRate::BAUD_57600);
    // Set the number of data bits.
    serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    // Set the hardware flow control.
    serial_port.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
    // Set the parity.
    serial_port.SetParity(LibSerial::Parity::PARITY_NONE);
    // Set the number of stop bits.
    serial_port.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
    return true;
}

}