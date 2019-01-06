#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>

#include "BoardClient.hpp"

bool check_mode = false;

void check_arg(char* arg)
{
    if(arg != nullptr)
    {
        if (std::string(arg) == "--trace") { trace = true;         }
        if (std::string(arg) == "--log")   { log = true;           }
        if (std::string(arg) == "--debug") { debug = true;         }
        if (std::string(arg) == "--rx")    { rx_mode = true;       }
        if (std::string(arg) == "--check") { check_mode = true;    }
    }
}

void check_by_files(const char* sent_filename, const char* recv_filename)
{
    std::ifstream sent_file(sent_filename);
    std::ifstream recv_file(recv_filename);

    if(sent_file.is_open() && recv_file.is_open())
    {
        char buffer1[1024];
        char buffer2[1024];
        uint8_t size1 = 0;
        uint8_t size2 = 0;
        uint32_t pktErrCnt = 0;
        uint32_t pktCnt = 0;


        while(sent_file || recv_file)
        {
            if (sent_file)
            {
                sent_file.read(reinterpret_cast<char*>(&size1), sizeof(size1));
                if (sent_file) 
                {
                    sent_file.read(buffer1, size1); 
                    INFO("[");
                    std::for_each(buffer1, buffer1 + size1, [](auto& val){ INFO("%02x ", (uint8_t)val); });
                    INFO("]");
                }
            }
            INFO("\t--->\t");  
            if (recv_file)
            {
                recv_file.read(reinterpret_cast<char*>(&size2), sizeof(size2));
                if (recv_file) 
                {
                    recv_file.read(buffer2, size2);
                    INFO("[");
                    std::for_each(buffer2, buffer2 + size2 - 2, [](auto& val){ INFO("%02x ", (uint8_t)val); });
                    int rssi = rssi_converter(buffer2[size2 - 2]);
                    uint8_t lqi = buffer2[size2 - 1];
                    INFO("] RSSI: %d  LQI: %u", rssi, lqi);
                }
            }

            if (sent_file && recv_file)
            {
                if (memcmp(buffer1, buffer2, size1) != 0)
                {
                    INFO("   PACKET_ERROR");
                    pktErrCnt++;
                }
                pktCnt++;
            }
            INFO("\n");
        }
        INFO("PACKETS: %d\n", pktCnt);
        INFO("ERRORS : %d\n", pktErrCnt);
        INFO("PER    : %f\n", (double)pktErrCnt/pktCnt);
    }
    else
    {
        ERR("%s: Fail!", __PRETTY_FUNCTION__);
    }
}



int main(int args, char** argv)
{
    if (argv[1] == nullptr)
    {
    	std::cerr << "File not found." << std::endl;
        return EXIT_FAILURE;
    }
    check_arg(argv[2]);
    check_arg(argv[3]);
    check_arg(argv[4]);

    if (!check_mode)
    {
        if (rx_mode)
        {
            BoardClient client{argv[1], eMode::RADIO_MODE_RX, 7, "rx.log"};
            client.Run();
        }
        else
        {
            BoardClient client{argv[1], eMode::RADIO_MODE_TX, 7, "tx.log"};  
            client.GetSettings().SetTransmissions(400); 
            client.Run();
            while(!client.IsActive())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            for(uint32_t i = 0; i < 20; i++)
            {
                std::vector<uint8_t> msg1{0x1a, 0x2b, 0x3c, 0x4d, i % 255, 0, i % 255};
                if (!client.SendPacket(msg1)) INFO("msg failure\n"); 
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    else
    {
        check_by_files("tx.log", "rx.log");
    }

    std::cout << "The program successfully completed!" << std::endl;
	return EXIT_SUCCESS;
}





