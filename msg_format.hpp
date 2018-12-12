#pragma once
#include <iostream>
#include "settings.hpp"

namespace msg
{

constexpr uint16_t DATA_LENGTH = 22;

enum eMsgType : uint16_t 
{
	WAKEUP = 1,
	WAKEUP_ACK,
	SETUP_REQ,
	SETUP_RSP,
	SETUP_ACK,
	SETUP_ERR,
	ERR,
	DATA
};

const char* toString(eMsgType type);

struct header_s
{
	header_s(eMsgType _type, uint16_t _size) : type{_type}, size{_size} {}

	eMsgType  type;
	uint16_t  size;
};

struct setup_req_s : header_s
{
	setup_req_s() : header_s(SETUP_REQ, sizeof(settings_s)) {}

	settings_s settings;
};

}