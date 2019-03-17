#include "global.hpp"
#include "utils.hpp"

namespace cc1110
{

uint16_t crc16(uint8_t* data, size_t size)
{
	uint16_t p = 0x8005;
	uint16_t crc = 0;
	
	for (int d = 0; d < size; d++)
	{
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
	DEBUG("CRC CALCULATE: %s : %02x\n", toHexString(data, size).c_str(), crc);

	return crc;
}

void SaveToDumpFile(const char* filename, const char* data, size_t size)
{
    static std::ofstream file(filename);

    if (file.is_open())
    {
        uint8_t size8 = (uint8_t) size;
        file.write(reinterpret_cast<char*>(&size8), sizeof(size8));
        file.write(data, size);
        file.flush();
    }
}

}