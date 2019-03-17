#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

namespace cc1110
{

//void print_message(std::string title, const std::vector<uint8_t>& msg);

constexpr int rssi_offset = 75;


inline int rssi_converter(uint8_t rssi_dec)
{
	return (rssi_dec >= 128) ? ((rssi_dec - 256)/2 - rssi_offset) : (rssi_dec/2 - rssi_offset);
}


inline bool file_exist(const char* filename) 
{
    std::ifstream infile(filename);
    return infile.good();
}

inline std::string toHexString(uint8_t* msg, size_t size)
{
	std::stringstream ss;
    ss  << std::hex;

    for (int i = 0; i < size; i++)
    {
        ss << std::setw(2) << std::setfill('0') << (int)msg[i] << " ";
    }
    
    return ss.str();
}

inline std::string toHexString(std::vector<uint8_t>& msg)
{
	return toHexString(msg.data(), msg.size());
}

void SaveToDumpFile(const char* filename, const char* data, size_t size);

uint16_t crc16(uint8_t* data, size_t size);


}