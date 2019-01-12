#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>
#include <map>

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

uint16_t weight(uint8_t n)
{ 
    uint16_t count = 0; 
    while (n) 
    { 
      n &= (n-1) ; 
      count++; 
    } 
    return count; 
} 

void check_by_msg_and_file(std::vector<uint8_t>& msg, const char* recv_filename)
{
    std::ifstream recv_file(recv_filename);

    std::map<int, uint32_t> rssi_cnt_map;
    std::map<int, uint32_t> rssi_error_map;
    std::map<int, uint32_t> rssi_bit_err_map;

    if(recv_file.is_open())
    {
        char buffer2[1024];
        uint8_t size2 = 0;
        uint32_t pktErrCnt = 0;
        uint32_t pktCnt = 0;


        while(recv_file)
        {
         
            INFO("[");
            std::for_each(msg.begin(), msg.end(), [](auto& val){ INFO("%02x ", (uint8_t)val); });
            INFO("]");

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

            if (recv_file)
            {
                int round_rssi = rssi_converter(buffer2[size2 - 2]);
                round_rssi = round_rssi - (round_rssi % 5);

                if (memcmp(msg.data(), buffer2, msg.size()) != 0)
                {
                    pktErrCnt++;
                    auto[it_err, success_err] = rssi_error_map.insert(std::make_pair(round_rssi, 1));
                    if (!success_err)
                    {
                        it_err->second++;
                    }

                    uint32_t bit_err_cnt = 0;
                    for(int i=0; i < msg.size(); i++)
                    {
                        if(msg[i] != buffer2[i])
                        {
                            bit_err_cnt += weight(msg[i] ^ buffer2[i]);
                        }
                    }
                    auto[it_berr, success_berr] = rssi_bit_err_map.insert(std::make_pair(round_rssi, bit_err_cnt));
                    if (!success_berr)
                    {
                        it_berr->second += bit_err_cnt;
                    }
                    INFO("   PACKET_ERROR [%4d bit errors]", bit_err_cnt);
                }
                pktCnt++;
                auto[it, success] = rssi_cnt_map.insert(std::make_pair(round_rssi, 1));
                if (!success)
                {
                    it->second++;
                }
            }
            INFO("\n");
        }
        INFO("PACKETS: %d\n", pktCnt);
        INFO("ERRORS : %d\n", pktErrCnt);
        INFO("PER    : %f\n", (double)pktErrCnt/pktCnt);

        std::ofstream per_file{"per.csv"};
        std::ofstream ber_file{"ber.csv"};
        for(auto& it_err : rssi_error_map)
        {
            per_file << it_err.first << ",";
            ber_file << it_err.first << ",";
        }
        per_file << std::endl;
        ber_file << std::endl;
        auto it_cnt = rssi_cnt_map.cbegin();
        auto it_berr = rssi_bit_err_map.cbegin();
        for(auto& it_err : rssi_error_map)
        {
            double per = (double)it_err.second/it_cnt->second;
            double ber = (double)it_berr->second/(it_cnt->second * 8 * msg.size());
            per_file << per << ",";
            ber_file << ber << ",";
            INFO("RSSI: %3d\t PACKETS: %4u\t pERRORS: %4u\t PER: %6.4f \tbERRORS: %4u\t BER: %6.4f\n", it_err.first, it_cnt->second, it_err.second, per, it_berr->second, ber);
            it_cnt++;
            it_berr++;
        }
    }
    else
    {
        ERR("%s: Fail!", __PRETTY_FUNCTION__);
    }
}
void check_by_files(const char* sent_filename, const char* recv_filename)
{
    std::ifstream sent_file(sent_filename);
    std::ifstream recv_file(recv_filename);

    std::map<int, uint32_t> rssi_cnt_map;
    std::map<int, uint32_t> rssi_error_map;
    std::map<int, uint32_t> rssi_bit_err_map;

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
                int round_rssi = rssi_converter(buffer2[size2 - 2]);
                round_rssi = round_rssi - (round_rssi % 5);

                if (memcmp(buffer1, buffer2, size1) != 0)
                {
                    pktErrCnt++;
                    auto[it_err, success_err] = rssi_error_map.insert(std::make_pair(round_rssi, 1));
                    if (!success_err)
                    {
                        it_err->second++;
                    }

                    uint32_t bit_err_cnt = 0;
                    for(int i=0; i < size1; i++)
                    {
                        if(buffer1[i] != buffer2[i])
                        {
                            bit_err_cnt += weight(buffer1[i] ^ buffer2[i]);
                        }
                    }
                    auto[it_berr, success_berr] = rssi_bit_err_map.insert(std::make_pair(round_rssi, bit_err_cnt));
                    if (!success_berr)
                    {
                        it_berr->second += bit_err_cnt;
                    }
                    INFO("   PACKET_ERROR [%4d bit errors]", bit_err_cnt);
                }
                pktCnt++;
                auto[it, success] = rssi_cnt_map.insert(std::make_pair(round_rssi, 1));
                if (!success)
                {
                    it->second++;
                }
            }
            INFO("\n");
        }
        INFO("PACKETS: %d\n", pktCnt);
        INFO("ERRORS : %d\n", pktErrCnt);
        INFO("PER    : %f\n", (double)pktErrCnt/pktCnt);

        std::ofstream per_file{"per.csv"};
        std::ofstream ber_file{"ber.csv"};
        for(auto& it_err : rssi_error_map)
        {
            per_file << it_err.first << ",";
            ber_file << it_err.first << ",";
        }
        per_file << std::endl;
        ber_file << std::endl;
        auto it_cnt = rssi_cnt_map.cbegin();
        auto it_berr = rssi_bit_err_map.cbegin();
        for(auto& it_err : rssi_error_map)
        {
            double per = (double)it_err.second/it_cnt->second;
            double ber = (double)it_berr->second/(it_cnt->second * 8 * size1);
            per_file << per << ",";
            ber_file << ber << ",";
            INFO("RSSI: %3d\t PACKETS: %4u\t pERRORS: %4u\t PER: %6.4f \tbERRORS: %4u\t BER: %6.4f\n", it_err.first, it_cnt->second, it_err.second, per, it_berr->second, ber);
            it_cnt++;
            it_berr++;
        }
    }
    else
    {
        ERR("%s: Fail!", __PRETTY_FUNCTION__);
    }
}



int main(int args, char** argv)
{
    check_arg(argv[1]);
    check_arg(argv[2]);
    check_arg(argv[3]);
    check_arg(argv[4]);

    std::vector<uint8_t> msg{0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f, 0x70};
            
    if (!check_mode)
    {
        if (argv[1] == nullptr)
        {
            std::cerr << "File not found." << std::endl;
            return EXIT_FAILURE;
        }
        if (rx_mode)
        {
            BoardClient client{argv[1], eMode::RADIO_MODE_RX, 7};
            client.Run();
        }
        else
        {
            BoardClient client{argv[1], eMode::RADIO_MODE_TX, 7};  
            client.GetSettings().SetTransmissions(1000000); 
            client.Run();
            while(!client.IsActive())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (!client.SendPacket(msg)) INFO("msg failure\n"); 
        }
    }
    else
    {
        if (argv[2] && argv[3])
        {
            check_by_files(argv[2], argv[3]); 
        }
        else if (argv[2])
        {
            check_by_msg_and_file(msg, argv[2]);
        }
        else
        {
            std::cerr << "File not found." << std::endl;
            std::cerr << "The first file: tx.log" << std::endl;
            std::cerr << "The second file: rx.log" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "The program successfully completed!" << std::endl;
	return EXIT_SUCCESS;
}





