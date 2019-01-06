#include "BoardClient.hpp"


bool BoardClient::Setup(LibSerial::ISerialPort& serial_port, std::string path)
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

void BoardClient::BaseLoop()
{
    const uint32_t rx_buffer_size = 1024;
    std::vector<uint8_t> buffer(rx_buffer_size);
    m_link_fsm.activate();

    while(!(terminate && IsPacketListEmpty() &&  m_mode == RADIO_MODE_TX))
    {
        try
        {
            DEBUG("Waiting for %zu bytes...\n", sizeof(msg::header_s));
            
            m_serial_port.Read(buffer, sizeof(msg::header_s), timeout_milliseconds);
            msg::header_s msg_header = *reinterpret_cast<msg::header_s*>(buffer.data());
            LOG("Type: %s(%d)\nSize: %d\n", toString(msg_header.type), msg_header.type, msg_header.size);
            if (msg_header.size)
            {
                DEBUG("Waiting for %u bytes...\n", msg_header.size);
                m_serial_port.Read(buffer, msg_header.size, timeout_milliseconds);
            }
            print_message("Receive", buffer, &msg_header);
            m_link_fsm.OnMessage(msg_header, buffer);
        }
        catch (const LibSerial::ReadTimeout& err)
        {
            LOG("--- %lu milliseconds passed ---\n", timeout_milliseconds);
            m_link_fsm.OnTimeout();
        }
    }
}

bool BoardClient::Run()
{
    TRACE_FUNCTION();
    INFO("PATH: %s\n", m_board_path.c_str());
    INFO("MODE: %s\n\n", toString(m_mode));

	if (!Setup(m_serial_port, m_board_path))
    {
        return false;
    }
    std::thread thr(&BoardClient::BaseLoop, this);
    m_thread = std::move(thr);
	return true;
}

bool BoardClient::SendPacket(std::vector<uint8_t>& msg)
{
	return SendPacket(msg.data(), msg.size());
}

bool BoardClient::SendPacket(uint8_t *data, size_t size)
{
	TRACE_FUNCTION();
	if (size != GetSettings().GetPacketLength())
	{
		ERR("Packet length error! %zu != %u\n", size, GetSettings().GetPacketLength());
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
