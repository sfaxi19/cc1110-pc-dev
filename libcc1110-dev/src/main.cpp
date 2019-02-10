#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>
#include <map>


#include "BoardClient.hpp"
#include "DumpParsing.hpp"

bool parsing_mode = false;
bool rx_mode = false;

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
    settings.CC1110_SYNC1 = 0xD3;
    settings.CC1110_SYNC0 = 0x91;
    settings.CC1110_PKTLEN = 0xFF;
    settings.CC1110_PKTCTRL1 = 0x04;
    settings.CC1110_PKTCTRL0 = 0x05;
    settings.CC1110_ADDR = 0x00;
    settings.CC1110_CHANNR = 0x00;
    settings.CC1110_FSCTRL1 = 0x0E;
    settings.CC1110_FSCTRL0 = 0x00;
    settings.CC1110_FREQ2 = 0x21;
    settings.CC1110_FREQ1 = 0x65;
    settings.CC1110_FREQ0 = 0x6A;
    settings.CC1110_MDMCFG4 = 0x0E;
    settings.CC1110_MDMCFG3 = 0x3B;
    settings.CC1110_MDMCFG2 = 0x73;
    settings.CC1110_MDMCFG1 = 0x42;
    settings.CC1110_MDMCFG0 = 0xF8;
    settings.CC1110_DEVIATN = 0x00;
    settings.CC1110_MCSM2 = 0x07;
    settings.CC1110_MCSM1 = 0x30;
    settings.CC1110_MCSM0 = 0x18;
    settings.CC1110_FOCCFG = 0x1D;
    settings.CC1110_BSCFG = 0x1C;
    settings.CC1110_AGCCTRL2 = 0xC7;
    settings.CC1110_AGCCTRL1 = 0x00;
    settings.CC1110_AGCCTRL0 = 0xB0;
    settings.CC1110_FREND1 = 0xB6;
    settings.CC1110_FREND0 = 0x10;
    settings.CC1110_FSCAL3 = 0xEA;
    settings.CC1110_FSCAL2 = 0x2A;
    settings.CC1110_FSCAL1 = 0x00;
    settings.CC1110_FSCAL0 = 0x1F;
    settings.CC1110_TEST2 = 0x88;
    settings.CC1110_TEST1 = 0x31;
    settings.CC1110_TEST0 = 0x09;
    settings.CC1110_PA_TABLE7 = 0x00;
    settings.CC1110_PA_TABLE6 = 0x00;
    settings.CC1110_PA_TABLE5 = 0x00;
    settings.CC1110_PA_TABLE4 = 0x00;
    settings.CC1110_PA_TABLE3 = 0x00;
    settings.CC1110_PA_TABLE2 = 0x00;
    settings.CC1110_PA_TABLE1 = 0x00;
    settings.CC1110_PA_TABLE0 = 0x50;
    settings.CC1110_IOCFG2 = 0x00;
    settings.CC1110_IOCFG1 = 0x00;
    settings.CC1110_IOCFG0 = 0x06;
    settings.CC1110_PARTNUM = 0x01;
    settings.CC1110_VERSION = 0x03;
    settings.CC1110_FREQEST = 0x00;
    settings.CC1110_LQI = 0x00;
    settings.CC1110_RSSI = 0x80;
    settings.CC1110_MARCSTATE = 0x01;
    settings.CC1110_PKTSTATUS = 0x00;
    settings.CC1110_VCO_VC_DAC = 0x94;
}

void check_arg(char* arg)
{
    if(arg != nullptr)
    {
        if (std::string(arg) == "--trace")   { cc1110::trace = true;   }
        if (std::string(arg) == "--log")     { cc1110::logging = true; }
        if (std::string(arg) == "--debug")   { cc1110::debug = true;   }
        if (std::string(arg) == "--rx")      { rx_mode = true;         }
        if (std::string(arg) == "--parsing") { parsing_mode = true;    }
    }
}

void callback_receive(std::vector<uint8_t>& msg)
{
    std::for_each(msg.begin(), msg.end() - 2, [](auto& val){ INFO("%02x ", val); });
    INFO("RSSI: %d  LQI: %u\n", cc1110::rssi_converter(msg[msg.size() - 2]), msg[msg.size() - 1]);
}

void board_client_rx(const char* linkpath, uint8_t msg_size)
{
    cc1110::BoardClient client{linkpath, cc1110::RADIO_MODE_RX, msg_size};
    Init500kbps(client.GetSettings());
    client.SetReceiveCallback(callback_receive);
    client.Run();

    while(true){}
}

void board_client_tx(const char* linkpath, std::vector<uint8_t>& msg, uint8_t msg_size)
{
    cc1110::BoardClient client{linkpath, cc1110::RADIO_MODE_TX, msg_size};
    Init500kbps(client.GetSettings());
    client.GetSettings().SetTransmissions(200); 
    client.Run();
    client.WaitForActive();

    if (!client.SendPacket(msg)) ERR("Failure message sending!\n");
}

int main(int args, char** argv)
{
    check_arg(argv[1]);
    check_arg(argv[2]);
    check_arg(argv[3]);
    check_arg(argv[4]);

    std::vector<uint8_t> msg{0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70};
    uint8_t MSG_SIZE = msg.size();
            
    if (!parsing_mode)
    {
        if (argv[1] == nullptr)
        {
            std::cerr << "File not found." << std::endl;
            return EXIT_FAILURE;
        }
        if (rx_mode)
        {
            board_client_rx(argv[1], MSG_SIZE);
        }
        else
        {
            board_client_tx(argv[1], msg, MSG_SIZE); 
        }
    }
    else
    {
        if (argv[2] && argv[3])
        {
            cc1110::parsing_dump_by_txfile(argv[2], argv[3]); 
        }
        else if (argv[2])
        {
            cc1110::parsing_dump_by_msg(msg, argv[2]);
        }
        else
        {
            std::cerr << "Filename not found." << std::endl;
            std::cerr << "The first file:  tx.dump" << std::endl;
            std::cerr << "The second file: rx.dump" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "The program successfully completed!" << std::endl;
	return EXIT_SUCCESS;
}





