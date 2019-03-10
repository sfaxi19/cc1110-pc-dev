#include "msg_format.hpp"
#include "global.hpp"

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

std::vector<uint8_t> encode(packet_s& packet)
{
	std::vector<uint8_t> v(sizeof(packet.header) + packet.header.size + sizeof(crc_t));

	auto v_it = std::copy(reinterpret_cast<uint8_t*>(&packet.header), 
						  reinterpret_cast<uint8_t*>(&packet.data[packet.header.size]), 
						  v.begin());

	crc_t crc = crc16(v.data(), v.size() - sizeof(crc_t));

	std::copy(reinterpret_cast<uint8_t*>(&crc), 
			  reinterpret_cast<uint8_t*>(&crc) + sizeof(crc), 
			  v_it);
	return v;
}

std::pair<packet_s, bool> decode(std::vector<uint8_t>& v)
{
	header_s* hdr = reinterpret_cast<header_s*>(v.data());
	
	DEBUG("+---------------+--------------------+------------+\n");
	DEBUG("| INPUT MESSAGE | Type: %12s | Size: %4u |\n", toString(hdr->type), hdr->size);
	DEBUG("+---------------+--------------------+------------+\n");

	packet_s packet;
	std::copy(v.begin(), v.begin() + sizeof(packet.header) + hdr->size, reinterpret_cast<uint8_t*>(&packet));
	
	crc_t& crc = reinterpret_cast<crc_t&>(v[sizeof(packet.header) + hdr->size]);
	crc_t crc2 = crc16(v.data(), sizeof(packet.header) + hdr->size);
	bool crc_check = (crc2 == crc);

	DEBUG("CRC CHECK: %x <-> %x\n", crc, crc2);

	return std::pair(packet, crc_check);
}

}

