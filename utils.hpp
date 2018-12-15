#include <iostream>
#include <vector>
#include <algorithm>

#include "msg_format.hpp"

void print_message(std::string title, const std::vector<uint8_t>& msg, msg::header_s* hdr = nullptr);