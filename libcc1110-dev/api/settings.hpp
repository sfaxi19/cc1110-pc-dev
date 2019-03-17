#pragma once

#include "global.hpp"

namespace cc1110
{

struct settings_s;

// Functions for speed configure:
void Init500kbps(settings_s& settings); // 500 kb/s
void Init250kbps(settings_s& settings); // 250 kb/s
void Init38kbps(settings_s& settings);  // 38.4 kb/s
void Init2_4kbps(settings_s& settings); // 2.4 kb/s
void Init1_2kbps(settings_s& settings); // 1.2 kb/s


enum eMode
{
	RADIO_MODE_TX = 0x10,
	RADIO_MODE_RX = 0x20
};


struct settings_s
{
	uint8_t CC1110_SYNC1 =       0xD3;
	uint8_t CC1110_SYNC0 =       0x91;
	uint8_t CC1110_PKTLEN =      0x04;
	uint8_t CC1110_PKTCTRL1 =    0x04;
	uint8_t CC1110_PKTCTRL0 =    0x05;
	uint8_t CC1110_ADDR =        0x00;
	uint8_t CC1110_CHANNR =      0x00;
	uint8_t CC1110_FSCTRL1 =     0x0E;
	uint8_t CC1110_FSCTRL0 =     0x00;
	uint8_t CC1110_FREQ2 =       0x21;
	uint8_t CC1110_FREQ1 =       0x65;
	uint8_t CC1110_FREQ0 =       0x6A;
	uint8_t CC1110_MDMCFG4 =     0x0E;
	uint8_t CC1110_MDMCFG3 =     0x3B;
	uint8_t CC1110_MDMCFG2 =     0x73;
	uint8_t CC1110_MDMCFG1 =     0x42;
	uint8_t CC1110_MDMCFG0 =     0xF8;
	uint8_t CC1110_DEVIATN =     0x00;
	uint8_t CC1110_MCSM2 =       0x07;
	uint8_t CC1110_MCSM1 =       0x30;
	uint8_t CC1110_MCSM0 =       0x18;
	uint8_t CC1110_FOCCFG =      0x1D;
	uint8_t CC1110_BSCFG =       0x1C;
	uint8_t CC1110_AGCCTRL2 =    0xC7;
	uint8_t CC1110_AGCCTRL1 =    0x00;
	uint8_t CC1110_AGCCTRL0 =    0xB0;
	uint8_t CC1110_FREND1 =      0xB6;
	uint8_t CC1110_FREND0 =      0x10;
	uint8_t CC1110_FSCAL3 =      0xEA;
	uint8_t CC1110_FSCAL2 =      0x2A;
	uint8_t CC1110_FSCAL1 =      0x00;
	uint8_t CC1110_FSCAL0 =      0x1F;
	uint8_t CC1110_TEST2 =       0x88;
	uint8_t CC1110_TEST1 =       0x31;
	uint8_t CC1110_TEST0 =       0x09;
	uint8_t CC1110_PA_TABLE7 =   0x00;
	uint8_t CC1110_PA_TABLE6 =   0x00;
	uint8_t CC1110_PA_TABLE5 =   0x00;
	uint8_t CC1110_PA_TABLE4 =   0x00;
	uint8_t CC1110_PA_TABLE3 =   0x00;
	uint8_t CC1110_PA_TABLE2 =   0x00;
	uint8_t CC1110_PA_TABLE1 =   0x00;
	uint8_t CC1110_PA_TABLE0 =   0x50;
	uint8_t CC1110_IOCFG2 =      0x00;
	uint8_t CC1110_IOCFG1 =      0x00;
	uint8_t CC1110_IOCFG0 =      0x06;
	uint8_t CC1110_PARTNUM =     0x01;
	uint8_t CC1110_VERSION =     0x03;
	uint8_t CC1110_FREQEST =     0x00;
	uint8_t CC1110_LQI =         0x00;
	uint8_t CC1110_RSSI =        0x80;
	uint8_t CC1110_MARCSTATE =   0x01;
	uint8_t CC1110_PKTSTATUS =   0x00;
	uint8_t CC1110_VCO_VC_DAC =  0x94;

private:
	uint8_t MODE =               0x00;
	uint8_t STATUS =             0x00;

	void     activate(eMode mode, uint8_t packet_length);

	void     SetPacketLength(uint8_t len);
	void     SetCRCEnable(uint8_t enable);
	void     SetMode(eMode mode);
	void     Enable() { STATUS = 1; }

public:
	eMode      GetMode()          const { return static_cast<eMode>(MODE); }
	uint8_t    GetPacketLength()  const { return CC1110_PKTLEN;            }
	bool       IsEnabled()        const { return STATUS > 0;           }


	friend class BoardClient;
};

}
// Address Config = No address check 
// Base Frequency = 868.299866 
// CRC Enable = true 
// Carrier Frequency = 868.299866 
// Channel Number = 0 
// Channel Spacing = 199.951172 
// Data Rate = 499.878 
// Device Address = 0 
// Manchester Enable = false 
// Modulated = true 
// Modulation Format = MSK 
// PA Ramping = false 
// Packet Length = 255 
// Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Phase Transition Time = 0 
// Preamble Count = 8 
// RX Filter BW = 812.500000 
// Sync Word Qualifier Mode = 30/32 sync word bits detected 
// TX Power = 0 
// Whitening = false 