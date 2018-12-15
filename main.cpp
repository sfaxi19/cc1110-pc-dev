#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

#include "SerialPort.h"
#include "ISerialPort.hpp"
#include "link_fsm.hpp"
#include "msg_format.hpp"
#include "utils.hpp"
#include "global.hpp"

#define STUB

// Specify a timeout value (in milliseconds).
const size_t timeout_milliseconds = 1000;

std::vector<uint8_t> test_data = 
{
        msg::WAKEUP_ACK, 0x00, 0x00, 0x00,
        msg::SETUP_RSP, 0x00,  0x38, 0x00, 
                        0xd3, 0x91, 0xff, 0x04, 0x05, 0x00, 0x00, 
                        0x0e, 0x00, 0x21, 0x65, 0x6a, 0x0e, 0x3b, 
                        0x73, 0x42, 0xf8, 0x00, 0x07, 0x30, 0x18, 
                        0x1d, 0x1c, 0xc7, 0x00, 0xb0, 0xb6, 0x10, 
                        0xea, 0x2a, 0x00, 0x1f, 0x88, 0x31, 0x09, 
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                        0x50, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 
                        0x00, 0x80, 0x01, 0x00, 0x94, 0x00, 0x00,
        msg::DATA_ACK,  0x00, 0x04, 0x00, 
                        0x11, 0x22, 0x33, 0x44
};


void print_test_data(std::vector<uint8_t>& data)
{
    for(int i=0;i < data.size(); i++)
    {
        printf("%02d ", i);
    }
    printf("\n");
    for(int i=0;i < data.size(); i++)
    {
        printf("%02x ", data[i]);
    }
    printf("\n");
}

class FakeSerialPort : public LibSerial::ISerialPort
{
public:
	void Read(LibSerial::DataBuffer& dataBuffer, size_t numberOfBytes = 0, size_t msTimeout = 0, size_t startPosition = 0) override 
	{
        dataBuffer.resize(numberOfBytes);
		if (index >= std::size(test_data) || numberOfBytes > 100)
        {
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout_milliseconds));
			throw LibSerial::ReadTimeout(LibSerial::ERR_MSG_READ_TIMEOUT);
		}
		dataBuffer.resize(numberOfBytes);
        std::copy(test_data.begin() + index, test_data.begin() + (index + numberOfBytes), dataBuffer.begin());
		index += numberOfBytes;
		//print_test_data(dataBuffer);
	}

    void Write(const LibSerial::DataBuffer& dataBuffer) override
    {
        //print_message("Sent", dataBuffer);
    }

    uint32_t index = 0;
};

bool serial_port_setup(LibSerial::ISerialPort& serial_port, std::string path)
{
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


int main(int args, char** argv)
{

    //print_test_data(test_data);
    //LibSerial::SerialPort serial_port;// serial_port;
	FakeSerialPort serial_port;

    if (argv[1] == nullptr)
    {
    	std::cerr << "File not found." << std::endl;
        return EXIT_FAILURE;
    }
    if (argv[2] != nullptr)
    {
        if (std::string(argv[2]) == "--trace") trace = true;
        if (std::string(argv[2]) == "--log")   log = true;
    }

    if (argv[3] != nullptr)
    {
        if (std::string(argv[3]) == "--trace") trace = true;
        if (std::string(argv[3]) == "--log")   log = true;
    }

    if (!serial_port_setup(serial_port, argv[1]))
    {
        return EXIT_FAILURE;
    }

    //std::vector<uint8_t> msg(25);

    LinkFsm fsm(serial_port);
    fsm.activate();

    const uint32_t rx_buffer_size = 1024;
    std::vector<uint8_t> buffer(rx_buffer_size);

	while(true)
    {
        try
        {
        	LOG("Waiting for %zu bytes...\n", sizeof(msg::header_s));
            
            serial_port.Read(buffer, sizeof(msg::header_s), timeout_milliseconds);
            msg::header_s msg_header = *reinterpret_cast<msg::header_s*>(buffer.data());
            LOG("Type: %s(%d)\nSize: %d\n", toString(msg_header.type), msg_header.type, msg_header.size);
            if (msg_header.size)
            {
                LOG("Waiting for %u bytes...\n", msg_header.size);
                serial_port.Read(buffer, msg_header.size, timeout_milliseconds);
            }
            print_message("Receive", buffer, &msg_header);
            fsm.OnMessage(msg_header, buffer);
        }
        catch (const LibSerial::ReadTimeout& err)
        {
            INFO("--- %lu milliseconds passed ---\n", timeout_milliseconds);
        	fsm.OnTimeout();
    	}
	}
    serial_port.Close();
    std::cout << "The program successfully completed!" << std::endl;
	return EXIT_SUCCESS;
}





