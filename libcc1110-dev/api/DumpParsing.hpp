#pragma once

#include <vector>

namespace cc1110
{

void parsing_dump_by_msg(std::vector<uint8_t>& msg, const char* rx_filename);
void parsing_dump_by_txfile(const char* tx_filename, const char* rx_filename);

}
