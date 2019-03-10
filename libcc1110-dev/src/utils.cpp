#include "global.hpp"
#include "utils.hpp"
#include "msg_format.hpp"

namespace cc1110
{

void print_message(std::string title, const std::vector<uint8_t>& msg, msg::header_s* hdr)
{
	TRACE("%s:\n\t", title.c_str());
	if (hdr)
	{
		std::for_each(reinterpret_cast<uint8_t *>(hdr), 
					  reinterpret_cast<uint8_t *>(hdr) + sizeof(msg::header_s), 
					  [](auto& val){ TRACE("%02x ", val); });
		std::for_each(msg.begin(), msg.begin() + hdr->size, [](auto& val){ TRACE("%02x ", val); });
	}
	else
	{
		std::for_each(msg.begin(), msg.end(), [](auto& val){ TRACE("%02x ", val); });
	}
	TRACE("\n");
}

uint16_t crc16(uint8_t* data, uint16_t size)
{
	uint16_t p = 0x8005;
	uint16_t crc = 0;
	DEBUG("CRC CALCULATE: ");
	for (int d = 0; d < size; d++)
	{
		DEBUG("%02x ", data[d]);
		crc ^= data[d];
		int shifts = (d == size-1) ? 16 : 8;

		for (int i = 0; i < shifts; ++i)
		{
			if (crc & 0x8000)
				crc = (crc << 1) ^ p;
			else
				crc = (crc << 1);
		}
	}
	DEBUG(" : %02x\n", crc);

	return crc;
}

}