#include "msg_format.hpp"
#include "global.hpp"
#include "utils.hpp"

namespace cc1110::msg
{

const char* toString(msg::eMsgType type) {
	using namespace msg;
	switch(type)
	{
		case WAKEUP:     return "WAKEUP";
		case WAKEUP_ACK: return "WAKEUP_ACK";
		case SETUP_REQ:  return "SETUP_REQ";
		case SETUP_ACK:  return "SETUP_ACK";
		case SETUP_ERR:  return "SETUP_ERR";
		case DATA_REQ:   return "DATA_REQ";
		case DATA_ACK:   return "DATA_ACK";
		case ERR:        return "ERR";
		case NONE:       return "NONE";
		default:         return "UNKNOWN";
	}
}

void print_packet(const char* dir, packet_s& packet)
{
	TRACE("+-----------------+--------------------+------------+\n");
	TRACE("| %6s  MESSAGE | Type: %12s | Size: %4u |\n", dir, toString(packet.header.type), packet.header.size);
	TRACE("+-----------------+--------------------+------------+\n");
	if (packet.header.size)
	{
		const int BYTES_IN_LINE = 16;
		int modulo = packet.header.size % BYTES_IN_LINE;
		int i;
		for (i = 0; i < int(packet.header.size / BYTES_IN_LINE); i++)
		{
			std::string out = toHexString(packet.data + i * BYTES_IN_LINE, BYTES_IN_LINE);
			TRACE("|  %s |\n", out.c_str());
		}
		if (modulo)
		{
			std::string out = toHexString(packet.data + i * BYTES_IN_LINE, modulo);
			for (int j = 0; j < BYTES_IN_LINE - modulo; j++)
			{
				out.append("   ");
			}
			TRACE("|  %s |\n", out.c_str());
		}
		

		TRACE("+---------------------------------------------------+\n");
	}
}

std::vector<uint8_t> encode(packet_s& packet)
{
	TRACE_FUNCTION();

	std::vector<uint8_t> v(sizeof(packet.header) + packet.header.size + sizeof(crc_t));

	auto v_it = std::copy(reinterpret_cast<uint8_t*>(&packet.header), 
						  reinterpret_cast<uint8_t*>(&packet.data[packet.header.size]), 
						  v.begin());

	crc_t crc = crc16(v.data(), v.size() - sizeof(crc_t));

	std::copy(reinterpret_cast<uint8_t*>(&crc), 
			  reinterpret_cast<uint8_t*>(&crc) + sizeof(crc), 
			  v_it);

	print_packet("OUTPUT", packet);
	
	return v;
}

std::pair<packet_s, bool> decode(std::vector<uint8_t>& v)
{
	TRACE_FUNCTION();

	header_s* hdr = reinterpret_cast<header_s*>(v.data());

	packet_s packet;
	std::copy(v.begin(), v.begin() + sizeof(packet.header) + hdr->size, reinterpret_cast<uint8_t*>(&packet));
	
	crc_t& crc = reinterpret_cast<crc_t&>(v[sizeof(packet.header) + hdr->size]);
	crc_t crc2 = crc16(v.data(), sizeof(packet.header) + hdr->size);
	bool crc_check = (crc2 == crc);

	print_packet("INPUT", packet);

	DEBUG("CRC CHECK: %x <-> %x - %s\n", crc, crc2, (crc_check) ? "SUCCESS" : "FAILURE");

	return std::pair(packet, crc_check);
}

}

