#include "settings.hpp"

namespace cc1110
{

void settings_s::SetPacketLength(uint8_t len)
{
	SET_BITS(CC1110_PKTCTRL0, 2, 0, 0);
	SET_BITS(CC1110_PKTLEN, 8, 0, len);
}

void settings_s::SetCRCEnable(uint8_t enable)
{
	SET_BIT(CC1110_PKTCTRL0, 2, enable);
}

void settings_s::SetMode(eMode mode)
{
	MODE = mode;
}

}