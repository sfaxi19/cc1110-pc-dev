//for compile this file: g++-7 --std=c++17 example_tx.cpp -o example_tx -lcc1110-dev
//Run with sudo: sudo ./example_tx
#include <cc1110-dev/BoardClient.hpp>

void InitSpeed500kbps(cc1110::settings_s& settings) {
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

int main(int args, char** argv)
{
    const int DATA_SIZE = 10;

    cc1110::logging = true;

    cc1110::BoardClient client{"/dev/ttyUSB1"};

    cc1110::settings_s settings;
    InitSpeed500kbps(settings);

    client.Configure(settings, cc1110::RADIO_MODE_TX, DATA_SIZE);
    
    std::vector<uint8_t> msg(DATA_SIZE);

    for (int i = 0; i < 10; i++)
    {
        for (auto& it : msg)
        {
            it = rand() % 256;
        }

        std::string hex = cc1110::toHexString(msg);
        printf("SENDING DATA: %s\n", hex.c_str());

        client.SendPacket(msg, 4); //Send message 4 times
    }
}