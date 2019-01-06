#pragma once

#include <iostream>
#include <chrono>
#include <thread>

#include "SerialPort.h"
#include "msg_format.hpp"

extern bool trace;
extern bool log;
extern bool debug;
extern bool rx_mode;

#define LOG(...)   if(log){fprintf(stdout, __VA_ARGS__);}
#define TRACE(...) if(trace){fprintf(stdout, __VA_ARGS__);}
#define DEBUG(...) if(debug)fprintf(stdout, __VA_ARGS__);
#define IFLOG(...) if(log)
#define ERR(...)   fprintf(stderr, __VA_ARGS__);
#define INFO(...)  fprintf(stdout, __VA_ARGS__);

#define SET_BIT(BYTE, IDX, VALUE) BYTE = (BYTE & !(IDX)) | ((VALUE & 0x01) << IDX);
#define SET_BITS(BYTE, RANGE, SHIFT, VALUE) BYTE = (BYTE & !((1 << RANGE) - 1)) | ((VALUE & ((1 << RANGE) - 1)) << SHIFT);

class trace_func {
	const char* m_file;	const char* m_name;
public:
	trace_func(const char* file, const char* name) : m_file{file}, m_name{name}	{
		DEBUG("=== %s: %s\t\\\n", m_file, m_name);
	}
	~trace_func(){
		DEBUG("=== %s: %s\t/\n", m_file, m_name);
	}
};

#define TRACE_FUNCTION() trace_func TRACE_FUNCTION{__FILE__, __PRETTY_FUNCTION__};


const size_t timeout_milliseconds = 150;



const std::vector<uint8_t> test_data = 
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
        msg::DATA_RSP,  0x00, 0x04, 0x00, 
                        0x11, 0x22, 0x33, 0x44,
        msg::DATA_RSP,  0x00, 0x04, 0x00, 
                        0x11, 0x22, 0x33, 0x45,
        msg::DATA_RSP,  0x00, 0x04, 0x00, 
                        0x11, 0x22, 0x33, 0x46,
};

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

//#define STUB 0

#ifndef STUB
    using SerialPort_t = LibSerial::SerialPort;
#else
    using SerialPort_t = FakeSerialPort;
#endif
