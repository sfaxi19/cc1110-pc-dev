#include "global.hpp"
#include "utils.hpp"

void print_message(std::string title, const std::vector<uint8_t>& msg, msg::header_s* hdr)
{
	TRACE("%s:\n\t", title.c_str());
	if (hdr)
	{
		std::for_each(reinterpret_cast<uint8_t *>(hdr), reinterpret_cast<uint8_t *>(hdr) + sizeof(msg::header_s), [](auto& val){ TRACE("%02x ", val); });
		std::for_each(msg.begin(), msg.begin() + hdr->size, [](auto& val){ TRACE("%02x ", val); });
	}
	else
	{
		std::for_each(msg.begin(), msg.end(), [](auto& val){ TRACE("%02x ", val); });
	}
	TRACE("\n");
}