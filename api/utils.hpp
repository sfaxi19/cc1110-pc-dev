#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

namespace msg
{
	class header_s;
}

void print_message(std::string title, const std::vector<uint8_t>& msg, msg::header_s* hdr = nullptr);

constexpr int rssi_offset = 75;

inline int rssi_converter(uint8_t rssi_dec)
{
	return (rssi_dec >= 128) ? ((rssi_dec - 256)/2 - rssi_offset) : (rssi_dec/2 - rssi_offset);
}

/*
inline bool file_exist(const char* filename) 
{
  struct stat buffer;
  return (stat(filename, &buffer) == 0); 
}
*/
inline bool file_exist(const char* filename) 
{
    std::ifstream infile(filename);
    return infile.good();
}