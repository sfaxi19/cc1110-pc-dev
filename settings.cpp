#include "global.hpp"

void settings_s::SetPacketLength(uint8_t len)
{
	SET_BITS(CC1110_PKTCTRL0, 2, 0, 0);
	SET_BITS(CC1110_PKTLEN, 8, 0, len);
}

void settings_s::SetCRCEnable(uint8_t enable)
{
	SET_BIT(CC1110_PKTCTRL0, 2, enable);
}

uint8_t settings_s::GetPacketLength() const
{
	return CC1110_PKTLEN;
}

void settings_s::SetTransmissions(uint32_t cnt)
{
	TRANSMISSIONS = cnt;
}

//Number of transmissions = if (TRANSMISSIONS > 0) -> "100 * TRANSMISSIONS", else -> "1"
uint32_t settings_s::GetTransmissions()
{
	return TRANSMISSIONS;
}