#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>

#include "settings.hpp"
#include "utils.hpp"

namespace cc1110::msg
{

//constexpr uint16_t DATA_LENGTH = 22;

using crc_t = uint16_t;

enum eMsgType : uint16_t 
{
	WAKEUP = 1,
	WAKEUP_ACK,
	
	SETUP_REQ,
	SETUP_ACK,
	SETUP_ERR,

	DATA_REQ,
	DATA_ACK,

	ERR,
	NONE
};

const char* toString(eMsgType type);

struct header_s
{
	header_s(eMsgType _type, uint16_t _size) : type{_type}, size{_size} {}

	eMsgType  type;
	uint16_t  size;
};

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

struct packet_s
{
	enum{ MAX_DATA_SIZE = 512 };

	packet_s(eMsgType msg_type = eMsgType::NONE, uint16_t data_size = 0) : header{msg_type, data_size}
	{
		if (data_size > MAX_DATA_SIZE) 
		{
			fprintf(stderr, "Too much data size!");
			exit(1);
		}
	}

	void assign(void* buffer, uint16_t size)
	{
		uint8_t* data_arr = reinterpret_cast<uint8_t*>(buffer);
		std::copy(data_arr, data_arr + size, data);
	}


	header_s        header;
	uint8_t         data[MAX_DATA_SIZE]{0};
};

#pragma pack(pop)

std::vector<uint8_t> encode(packet_s& packet);
std::pair<packet_s, bool> decode(std::vector<uint8_t>& v);


// struct settings_packet_s : packet_s<eMsgType::SETUP_REQ, sizeof(settings_s)>
// {

// };

struct setup_req_s : header_s
{
	setup_req_s(settings_s _settings) 
		: header_s(SETUP_REQ, sizeof(settings_s))
		, settings{_settings}
		{}

	settings_s settings;
};

}