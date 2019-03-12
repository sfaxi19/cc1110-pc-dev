#include "settings.hpp"

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

// Rf settings for CC1110
void Init500kbps(cc1110::settings_s& settings) {
    settings.CC1110_SYNC1          = 0xD3;
    settings.CC1110_SYNC0          = 0x91;
    settings.CC1110_PKTLEN         = 0xFF;
    settings.CC1110_PKTCTRL1       = 0x04;
    settings.CC1110_PKTCTRL0       = 0x05;
    settings.CC1110_ADDR           = 0x00;
    settings.CC1110_CHANNR         = 0x00;
    settings.CC1110_FSCTRL1        = 0x0E;
    settings.CC1110_FSCTRL0        = 0x00;
    settings.CC1110_FREQ2          = 0x21;
    settings.CC1110_FREQ1          = 0x65;
    settings.CC1110_FREQ0          = 0x6A;
    settings.CC1110_MDMCFG4        = 0x0E;
    settings.CC1110_MDMCFG3        = 0x3B;
    settings.CC1110_MDMCFG2        = 0x73;
    settings.CC1110_MDMCFG1        = 0x42;
    settings.CC1110_MDMCFG0        = 0xF8;
    settings.CC1110_DEVIATN        = 0x00;
    settings.CC1110_MCSM2          = 0x07;
    settings.CC1110_MCSM1          = 0x30;
    settings.CC1110_MCSM0          = 0x18;
    settings.CC1110_FOCCFG         = 0x1D;
    settings.CC1110_BSCFG          = 0x1C;
    settings.CC1110_AGCCTRL2       = 0xC7;
    settings.CC1110_AGCCTRL1       = 0x00;
    settings.CC1110_AGCCTRL0       = 0xB0;
    settings.CC1110_FREND1         = 0xB6;
    settings.CC1110_FREND0         = 0x10;
    settings.CC1110_FSCAL3         = 0xEA;
    settings.CC1110_FSCAL2         = 0x2A;
    settings.CC1110_FSCAL1         = 0x00;
    settings.CC1110_FSCAL0         = 0x1F;
    settings.CC1110_TEST2          = 0x88;
    settings.CC1110_TEST1          = 0x31;
    settings.CC1110_TEST0          = 0x09;
    settings.CC1110_PA_TABLE7      = 0x00;
    settings.CC1110_PA_TABLE6      = 0x00;
    settings.CC1110_PA_TABLE5      = 0x00;
    settings.CC1110_PA_TABLE4      = 0x00;
    settings.CC1110_PA_TABLE3      = 0x00;
    settings.CC1110_PA_TABLE2      = 0x00;
    settings.CC1110_PA_TABLE1      = 0x00;
    settings.CC1110_PA_TABLE0      = 0x50;
    settings.CC1110_IOCFG2         = 0x00;
    settings.CC1110_IOCFG1         = 0x00;
    settings.CC1110_IOCFG0         = 0x06;
    settings.CC1110_PARTNUM        = 0x01;
    settings.CC1110_VERSION        = 0x03;
    settings.CC1110_FREQEST        = 0x00;
    settings.CC1110_LQI            = 0x00;
    settings.CC1110_RSSI           = 0x80;
    settings.CC1110_MARCSTATE      = 0x01;
    settings.CC1110_PKTSTATUS      = 0x00;
    settings.CC1110_VCO_VC_DAC     = 0x94;
}

// Address Config = No address check 
// Base Frequency = 868.299866 
// CRC Enable = true 
// Carrier Frequency = 868.299866 
// Channel Number = 0 
// Channel Spacing = 199.951172 
// Data Rate = 249.939 
// Deviation = 126.953125 
// Device Address = 0 
// Manchester Enable = false 
// Modulated = true 
// Modulation Format = GFSK 
// PA Ramping = false 
// Packet Length = 255 
// Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Preamble Count = 4 
// RX Filter BW = 541.666667 
// Sync Word Qualifier Mode = 30/32 sync word bits detected 
// TX Power = 0 
// Whitening = false 

// Rf settings for CC1110
void Init250kbps(cc1110::settings_s& settings) {
    settings.CC1110_SYNC1          = 0xD3;
    settings.CC1110_SYNC0          = 0x91;
    settings.CC1110_PKTLEN         = 0xFF;
    settings.CC1110_PKTCTRL1       = 0x04;
    settings.CC1110_PKTCTRL0       = 0x05;
    settings.CC1110_ADDR           = 0x00;
    settings.CC1110_CHANNR         = 0x00;
    settings.CC1110_FSCTRL1        = 0x0C;
    settings.CC1110_FSCTRL0        = 0x00;
    settings.CC1110_FREQ2          = 0x21;
    settings.CC1110_FREQ1          = 0x65;
    settings.CC1110_FREQ0          = 0x6A;
    settings.CC1110_MDMCFG4        = 0x2D;
    settings.CC1110_MDMCFG3        = 0x3B;
    settings.CC1110_MDMCFG2        = 0x13;
    settings.CC1110_MDMCFG1        = 0x22;
    settings.CC1110_MDMCFG0        = 0xF8;
    settings.CC1110_DEVIATN        = 0x62;
    settings.CC1110_MCSM2          = 0x07;
    settings.CC1110_MCSM1          = 0x30;
    settings.CC1110_MCSM0          = 0x18;
    settings.CC1110_FOCCFG         = 0x1D;
    settings.CC1110_BSCFG          = 0x1C;
    settings.CC1110_AGCCTRL2       = 0xC7;
    settings.CC1110_AGCCTRL1       = 0x00;
    settings.CC1110_AGCCTRL0       = 0xB0;
    settings.CC1110_FREND1         = 0xB6;
    settings.CC1110_FREND0         = 0x10;
    settings.CC1110_FSCAL3         = 0xEA;
    settings.CC1110_FSCAL2         = 0x2A;
    settings.CC1110_FSCAL1         = 0x00;
    settings.CC1110_FSCAL0         = 0x1F;
    settings.CC1110_TEST2          = 0x88;
    settings.CC1110_TEST1          = 0x31;
    settings.CC1110_TEST0          = 0x09;
    settings.CC1110_PA_TABLE7      = 0x00;
    settings.CC1110_PA_TABLE6      = 0x00;
    settings.CC1110_PA_TABLE5      = 0x00;
    settings.CC1110_PA_TABLE4      = 0x00;
    settings.CC1110_PA_TABLE3      = 0x00;
    settings.CC1110_PA_TABLE2      = 0x00;
    settings.CC1110_PA_TABLE1      = 0x00;
    settings.CC1110_PA_TABLE0      = 0x50;
    settings.CC1110_IOCFG2         = 0x00;
    settings.CC1110_IOCFG1         = 0x00;
    settings.CC1110_IOCFG0         = 0x06;
    settings.CC1110_PARTNUM        = 0x01;
    settings.CC1110_VERSION        = 0x03;
    settings.CC1110_FREQEST        = 0x00;
    settings.CC1110_LQI            = 0x00;
    settings.CC1110_RSSI           = 0x80;
    settings.CC1110_MARCSTATE      = 0x01;
    settings.CC1110_PKTSTATUS      = 0x00;
    settings.CC1110_VCO_VC_DAC     = 0x94;
}

// Address Config = No address check 
// Base Frequency = 868.299866 
// CRC Enable = true 
// Carrier Frequency = 868.299866 
// Channel Number = 0 
// Channel Spacing = 199.951172 
// Data Rate = 38.3835 
// Deviation = 20.629883 
// Device Address = 0 
// Manchester Enable = false 
// Modulated = true 
// Modulation Format = GFSK 
// PA Ramping = false 
// Packet Length = 255 
// Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Preamble Count = 4 
// RX Filter BW = 101.562500 
// Sync Word Qualifier Mode = 30/32 sync word bits detected 
// TX Power = 0 
// Whitening = false 

// Rf settings for CC1110
void Init38kbps(cc1110::settings_s& settings) {
    settings.CC1110_SYNC1          = 0xD3;
    settings.CC1110_SYNC0          = 0x91;
    settings.CC1110_PKTLEN         = 0xFF;
    settings.CC1110_PKTCTRL1       = 0x04;
    settings.CC1110_PKTCTRL0       = 0x05;
    settings.CC1110_ADDR           = 0x00;
    settings.CC1110_CHANNR         = 0x00;
    settings.CC1110_FSCTRL1        = 0x08;
    settings.CC1110_FSCTRL0        = 0x00;
    settings.CC1110_FREQ2          = 0x21;
    settings.CC1110_FREQ1          = 0x65;
    settings.CC1110_FREQ0          = 0x6A;
    settings.CC1110_MDMCFG4        = 0xCA;
    settings.CC1110_MDMCFG3        = 0x83;
    settings.CC1110_MDMCFG2        = 0x93;
    settings.CC1110_MDMCFG1        = 0x22;
    settings.CC1110_MDMCFG0        = 0xF8;
    settings.CC1110_DEVIATN        = 0x35;
    settings.CC1110_MCSM2          = 0x07;
    settings.CC1110_MCSM1          = 0x30;
    settings.CC1110_MCSM0          = 0x18;
    settings.CC1110_FOCCFG         = 0x16;
    settings.CC1110_BSCFG          = 0x6C;
    settings.CC1110_AGCCTRL2       = 0x43;
    settings.CC1110_AGCCTRL1       = 0x40;
    settings.CC1110_AGCCTRL0       = 0x91;
    settings.CC1110_FREND1         = 0x56;
    settings.CC1110_FREND0         = 0x10;
    settings.CC1110_FSCAL3         = 0xE9;
    settings.CC1110_FSCAL2         = 0x2A;
    settings.CC1110_FSCAL1         = 0x00;
    settings.CC1110_FSCAL0         = 0x1F;
    settings.CC1110_TEST2          = 0x88;
    settings.CC1110_TEST1          = 0x31;
    settings.CC1110_TEST0          = 0x09;
    settings.CC1110_PA_TABLE7      = 0x00;
    settings.CC1110_PA_TABLE6      = 0x00;
    settings.CC1110_PA_TABLE5      = 0x00;
    settings.CC1110_PA_TABLE4      = 0x00;
    settings.CC1110_PA_TABLE3      = 0x00;
    settings.CC1110_PA_TABLE2      = 0x00;
    settings.CC1110_PA_TABLE1      = 0x00;
    settings.CC1110_PA_TABLE0      = 0x50;
    settings.CC1110_IOCFG2         = 0x00;
    settings.CC1110_IOCFG1         = 0x00;
    settings.CC1110_IOCFG0         = 0x06;
    settings.CC1110_PARTNUM        = 0x01;
    settings.CC1110_VERSION        = 0x03;
    settings.CC1110_FREQEST        = 0x00;
    settings.CC1110_LQI            = 0x00;
    settings.CC1110_RSSI           = 0x80;
    settings.CC1110_MARCSTATE      = 0x01;
    settings.CC1110_PKTSTATUS      = 0x00;
    settings.CC1110_VCO_VC_DAC     = 0x94;
}


// Address Config = No address check 
// Base Frequency = 868.299866 
// CRC Enable = true 
// Carrier Frequency = 868.299866 
// Channel Number = 0 
// Channel Spacing = 199.951172 
// Data Rate = 2.39897 
// Deviation = 5.157471 
// Device Address = 0 
// Manchester Enable = false 
// Modulated = true 
// Modulation Format = GFSK 
// PA Ramping = false 
// Packet Length = 255 
// Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Preamble Count = 4 
// RX Filter BW = 58.035714 
// Sync Word Qualifier Mode = 30/32 sync word bits detected 
// TX Power = 0 
// Whitening = false 

// Rf settings for CC1110
void Init2_4kbps(cc1110::settings_s& settings) {
    settings.CC1110_SYNC1          = 0xD3;
    settings.CC1110_SYNC0          = 0x91;
    settings.CC1110_PKTLEN         = 0xFF;
    settings.CC1110_PKTCTRL1       = 0x04;
    settings.CC1110_PKTCTRL0       = 0x05;
    settings.CC1110_ADDR           = 0x00;
    settings.CC1110_CHANNR         = 0x00;
    settings.CC1110_FSCTRL1        = 0x06;
    settings.CC1110_FSCTRL0        = 0x00;
    settings.CC1110_FREQ2          = 0x21;
    settings.CC1110_FREQ1          = 0x65;
    settings.CC1110_FREQ0          = 0x6A;
    settings.CC1110_MDMCFG4        = 0xF6;
    settings.CC1110_MDMCFG3        = 0x83;
    settings.CC1110_MDMCFG2        = 0x13;
    settings.CC1110_MDMCFG1        = 0x22;
    settings.CC1110_MDMCFG0        = 0xF8;
    settings.CC1110_DEVIATN        = 0x15;
    settings.CC1110_MCSM2          = 0x07;
    settings.CC1110_MCSM1          = 0x30;
    settings.CC1110_MCSM0          = 0x18;
    settings.CC1110_FOCCFG         = 0x17;
    settings.CC1110_BSCFG          = 0x6C;
    settings.CC1110_AGCCTRL2       = 0x03;
    settings.CC1110_AGCCTRL1       = 0x40;
    settings.CC1110_AGCCTRL0       = 0x91;
    settings.CC1110_FREND1         = 0x56;
    settings.CC1110_FREND0         = 0x10;
    settings.CC1110_FSCAL3         = 0xE9;
    settings.CC1110_FSCAL2         = 0x2A;
    settings.CC1110_FSCAL1         = 0x00;
    settings.CC1110_FSCAL0         = 0x1F;
    settings.CC1110_TEST2          = 0x88;
    settings.CC1110_TEST1          = 0x31;
    settings.CC1110_TEST0          = 0x09;
    settings.CC1110_PA_TABLE7      = 0x00;
    settings.CC1110_PA_TABLE6      = 0x00;
    settings.CC1110_PA_TABLE5      = 0x00;
    settings.CC1110_PA_TABLE4      = 0x00;
    settings.CC1110_PA_TABLE3      = 0x00;
    settings.CC1110_PA_TABLE2      = 0x00;
    settings.CC1110_PA_TABLE1      = 0x00;
    settings.CC1110_PA_TABLE0      = 0x50;
    settings.CC1110_IOCFG2         = 0x00;
    settings.CC1110_IOCFG1         = 0x00;
    settings.CC1110_IOCFG0         = 0x06;
    settings.CC1110_PARTNUM        = 0x01;
    settings.CC1110_VERSION        = 0x03;
    settings.CC1110_FREQEST        = 0x00;
    settings.CC1110_LQI            = 0x00;
    settings.CC1110_RSSI           = 0x80;
    settings.CC1110_MARCSTATE      = 0x01;
    settings.CC1110_PKTSTATUS      = 0x00;
    settings.CC1110_VCO_VC_DAC     = 0x94;
}


// Address Config = No address check 
// Base Frequency = 868.299866 
// CRC Enable = true 
// Carrier Frequency = 868.299866 
// Channel Number = 0 
// Channel Spacing = 199.951172 
// Data Rate = 1.19948 
// Deviation = 5.157471 
// Device Address = 0 
// Manchester Enable = false 
// Modulated = true 
// Modulation Format = GFSK 
// PA Ramping = false 
// Packet Length = 255 
// Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word 
// Preamble Count = 4 
// RX Filter BW = 58.035714 
// Sync Word Qualifier Mode = 30/32 sync word bits detected 
// TX Power = 0 
// Whitening = false 

// Rf settings for CC1110
void Init1_2(cc1110::settings_s& settings) {
    settings.CC1110_SYNC1          = 0xD3;
    settings.CC1110_SYNC0          = 0x91;
    settings.CC1110_PKTLEN         = 0xFF;
    settings.CC1110_PKTCTRL1       = 0x04;
    settings.CC1110_PKTCTRL0       = 0x05;
    settings.CC1110_ADDR           = 0x00;
    settings.CC1110_CHANNR         = 0x00;
    settings.CC1110_FSCTRL1        = 0x06;
    settings.CC1110_FSCTRL0        = 0x00;
    settings.CC1110_FREQ2          = 0x21;
    settings.CC1110_FREQ1          = 0x65;
    settings.CC1110_FREQ0          = 0x6A;
    settings.CC1110_MDMCFG4        = 0xF5;
    settings.CC1110_MDMCFG3        = 0x83;
    settings.CC1110_MDMCFG2        = 0x13;
    settings.CC1110_MDMCFG1        = 0x22;
    settings.CC1110_MDMCFG0        = 0xF8;
    settings.CC1110_DEVIATN        = 0x15;
    settings.CC1110_MCSM2          = 0x07;
    settings.CC1110_MCSM1          = 0x30;
    settings.CC1110_MCSM0          = 0x18;
    settings.CC1110_FOCCFG         = 0x17;
    settings.CC1110_BSCFG          = 0x6C;
    settings.CC1110_AGCCTRL2       = 0x03;
    settings.CC1110_AGCCTRL1       = 0x40;
    settings.CC1110_AGCCTRL0       = 0x91;
    settings.CC1110_FREND1         = 0x56;
    settings.CC1110_FREND0         = 0x10;
    settings.CC1110_FSCAL3         = 0xE9;
    settings.CC1110_FSCAL2         = 0x2A;
    settings.CC1110_FSCAL1         = 0x00;
    settings.CC1110_FSCAL0         = 0x1F;
    settings.CC1110_TEST2          = 0x88;
    settings.CC1110_TEST1          = 0x31;
    settings.CC1110_TEST0          = 0x09;
    settings.CC1110_PA_TABLE7      = 0x00;
    settings.CC1110_PA_TABLE6      = 0x00;
    settings.CC1110_PA_TABLE5      = 0x00;
    settings.CC1110_PA_TABLE4      = 0x00;
    settings.CC1110_PA_TABLE3      = 0x00;
    settings.CC1110_PA_TABLE2      = 0x00;
    settings.CC1110_PA_TABLE1      = 0x00;
    settings.CC1110_PA_TABLE0      = 0x50;
    settings.CC1110_IOCFG2         = 0x00;
    settings.CC1110_IOCFG1         = 0x00;
    settings.CC1110_IOCFG0         = 0x06;
    settings.CC1110_PARTNUM        = 0x01;
    settings.CC1110_VERSION        = 0x03;
    settings.CC1110_FREQEST        = 0x00;
    settings.CC1110_LQI            = 0x00;
    settings.CC1110_RSSI           = 0x80;
    settings.CC1110_MARCSTATE      = 0x01;
    settings.CC1110_PKTSTATUS      = 0x00;
    settings.CC1110_VCO_VC_DAC     = 0x94;
}

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