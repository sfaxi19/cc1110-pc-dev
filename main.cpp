#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>
#include <map>


#include <cc1110-dev/BoardClient.hpp>
#include <cc1110-dev/DumpParsing.hpp>

bool parsing_mode = false;
bool rx_mode = false;

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
    cc1110::BoardClient client{linkpath};

    cc1110::settings_s settings;
    Init250kbps(settings);
    client.SetReceiveCallback(callback_receive);

    client.Configure(settings, cc1110::RADIO_MODE_RX, msg_size);
    client.WaitingFor(1000000);
}

void board_client_tx(const char* linkpath, std::vector<uint8_t>& msg, uint8_t msg_size)
{
    cc1110::BoardClient client{linkpath};

    cc1110::settings_s settings;
    Init250kbps(settings);

    client.Configure(settings, cc1110::RADIO_MODE_TX, msg_size);
    client.SendPacketBlock(msg, 1);

    //client.Configure(settings, cc1110::RADIO_MODE_RX, msg_size);
    //client.WaitingFor(1000);

    //client.Configure(settings, cc1110::RADIO_MODE_TX, msg_size);
    client.SendPacketBlock(msg, 1);
    //std::vector<uint8_t> recv_data = client.ReceivePacketBlock();
}

int main(int args, char** argv)
{
    check_arg(argv[1]);
    check_arg(argv[2]);
    check_arg(argv[3]);
    check_arg(argv[4]);
    check_arg(argv[5]);

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





