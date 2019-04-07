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
bool tx_mode = false;
bool loop_test = false;
bool swap_test = false;
bool speed_test = false;

void check_arg(char* arg)
{
    if(arg != nullptr)
    {
        if (std::string_view(arg) == "--trace")      { cc1110::trace = true;   }
        if (std::string_view(arg) == "--log")        { cc1110::logging = true; }
        if (std::string_view(arg) == "--debug")      { cc1110::debug = true;   }
        if (std::string_view(arg) == "--rx")         { rx_mode = true;         }
        if (std::string_view(arg) == "--tx")         { tx_mode = true;         }
        if (std::string_view(arg) == "--parsing")    { parsing_mode = true;    }
        if (std::string_view(arg) == "--loop_test")  { loop_test = true;       }
        if (std::string_view(arg) == "--swap_test")  { swap_test = true;       }
        if (std::string_view(arg) == "--speed_test") { speed_test = true;      }
    }
}

void cc1110_speed_test(const char* linkpath)
{
    cc1110::BoardClient client{linkpath};

    std::vector<uint8_t> msg(200);

    cc1110::settings_s settings;
    cc1110::Init500kbps(settings);

    if (rx_mode)
    {
        client.Configure(settings, cc1110::RADIO_MODE_RX, msg.size());

        int packet_cnt = 0;
        std::chrono::system_clock::time_point start;
        while(true)
        {
            if (packet_cnt == 1)
            {
                start = std::chrono::system_clock::now();
            }
            
            client.ReceivePacket();

            auto end = std::chrono::system_clock::now();
            packet_cnt++;
            
            size_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            printf("Packets: %d,  Time: %zu,  Speed: %4.2f kbit/s\n", packet_cnt, ms, ((double)(packet_cnt*(msg.size() + 6)*8)/((double)ms/1000))/1000);
        }
    } 
    else if (tx_mode)
    {
        std::srand(unsigned(std::time(0)));

        client.Configure(settings, cc1110::RADIO_MODE_TX, msg.size());

        while (true)
        {
            for (auto& it : msg)
            {
                it = std::rand() % 256;
            }
            client.SendPacket(msg, 1);
        }
    }
    else
    {
        printf("You should select radio mode \"--tx\" or \"--rx\"!\n");
    }
    
}

void cc1110_swap_test(const char* linkpath)
{
    std::vector<uint8_t> msg{0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70};
    
    cc1110::BoardClient client{linkpath};

    cc1110::settings_s settings;
    cc1110::Init500kbps(settings);

    while(true)
    {
        client.Waiting(200);
        client.Configure(settings, cc1110::RADIO_MODE_TX, msg.size());
        for (int i = 0; i < 10; i++)
        {
            msg[0] = i;
            client.SendPacket(msg, 1);
        }

        client.Configure(settings, cc1110::RADIO_MODE_RX, msg.size());
        for (int i = 0; i < 10; i++)
        {
            auto [msg_recv, rssi, lqi] = client.ReceivePacket(1000);
            std::string hex = cc1110::toHexString(msg_recv);
            printf("%s  RSSI: %d  LQI: %u\n", hex.c_str(), rssi, lqi);
        }
    }
}

void cc1110_loop_test()
{
    std::srand(unsigned(std::time(0)));

    const char* path1 = "/dev/ttyUSB0";
    const char* path2 = "/dev/ttyUSB1";
    uint8_t msg_size = 10;

    cc1110::settings_s settings;
    cc1110::Init500kbps(settings);

    cc1110::BoardClient client_tx{path1};
    client_tx.Configure(settings, cc1110::RADIO_MODE_TX, msg_size);

    cc1110::BoardClient client_rx{path2};
    client_rx.Configure(settings, cc1110::RADIO_MODE_RX, msg_size);

    std::vector<uint8_t> msg(msg_size);

    for (int i = 0; i < 10; i++)
    {
        for (auto& it : msg)
        {
            it = std::rand() % 256;
        }

        client_tx.SendPacketAsync(msg);

        auto [msg_recv, rssi, lqi] = client_rx.ReceivePacket(2000);
        std::string hex = cc1110::toHexString(msg_recv);

        if (!msg_recv.empty()) printf("%s  RSSI: %d  LQI: %u\n", hex.c_str(), rssi, lqi);

        if (!msg_recv.empty() && std::equal(msg.begin(), msg.end(), msg_recv.begin()))
        {
            printf("SUCCESS!\n");
        }
        else
        {
            printf("FAILURE!\n");
        }
    }
}

void callback_receive(std::vector<uint8_t>& msg, int rssi, int lqi)
{
    std::string hex = cc1110::toHexString(msg);
    printf("%s  RSSI: %d  LQI: %u\n", hex.c_str(), rssi, lqi);
}

void board_client_rx(const char* linkpath, std::vector<uint8_t>& msg, uint8_t msg_size)
{
    cc1110::BoardClient client{linkpath};

    cc1110::settings_s settings;
    cc1110::Init500kbps(settings);

    client.Configure(settings, cc1110::RADIO_MODE_RX, msg_size);

    while(true)
    {
        auto [msg_recv, rssi, lqi] = client.ReceivePacket();

        std::string hex = cc1110::toHexString(msg_recv);
        printf("%s  RSSI: %d  LQI: %u  - %s\n", hex.c_str(), rssi, lqi, (msg == msg_recv) ? "SUCCESS" : "FAILURE!");
    }
}

void board_client_tx(const char* linkpath, std::vector<uint8_t>& msg, uint8_t msg_size)
{
    cc1110::BoardClient client{linkpath};

    cc1110::settings_s settings;
    cc1110::Init500kbps(settings);

    client.Configure(settings, cc1110::RADIO_MODE_TX, msg.size());
    
    for (int i = 0; i < 10; i++)
    {
        msg[0] = i % 256;
        client.SendPacket(msg);
    }
}

int main(int args, char** argv)
{
    for (int i = 0; i < args; i++)
    {
        check_arg(argv[i]);
    }

    std::vector<uint8_t> msg{0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70};
    uint8_t MSG_SIZE = msg.size();
            
    if (!parsing_mode)
    {
        if (!cc1110::file_exist(argv[1]))
        {
            std::cerr << "File \"" << argv[1] << "\" not found." << std::endl;
            return EXIT_FAILURE;
        }
        if (loop_test)
        {
            cc1110_loop_test();
        }
        else if (swap_test)
        {
            cc1110_swap_test(argv[1]);
        }
        else if (speed_test)
        {
            cc1110_speed_test(argv[1]);
        }
        else if (rx_mode)
        {
            board_client_rx(argv[1], msg, MSG_SIZE);
        }
        else if (tx_mode)
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





