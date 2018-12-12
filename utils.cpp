#include "utils.hpp"


void print_message(std::string title, std::vector<uint8_t>& msg)
{
	printf("%s:\n\t", title.c_str());
	std::for_each(msg.begin(), msg.end(), [](auto& val){ printf("%02x ", val); });
	printf("\n");
}

void print_message(std::string title, const std::vector<uint8_t>& msg)
{
	printf("%s:\n\t", title.c_str());
	std::for_each(msg.begin(), msg.end(), [](auto& val){ printf("%02x ", val); });
	printf("\n");
}