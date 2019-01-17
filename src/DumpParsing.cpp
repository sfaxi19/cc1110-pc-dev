#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <cmath>

#include "global.hpp"
#include "DumpParsing.hpp"
#include "utils.hpp"

namespace cc1110
{

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

void parsing_dump_by_msg(std::vector<uint8_t>& msg, const char* rx_filename)
{
    std::ifstream rxDumpFile(rx_filename);

    std::map<int, uint32_t> rssi_cnt_map;
    std::map<int, uint32_t> rssi_error_map;
    std::map<int, uint32_t> rssi_bit_err_map;

    if(rxDumpFile.is_open())
    {
        char buffer2[1024];
        uint8_t size2 = 0;
        uint32_t pktErrCnt = 0;
        uint32_t pktCnt = 0;


        while(rxDumpFile)
        {
         
            INFO("[");
            std::for_each(msg.begin(), msg.end(), [](auto& val){ INFO("%02x ", (uint8_t)val); });
            INFO("]");

            INFO("\t--->\t");  
            if (rxDumpFile)
            {
                rxDumpFile.read(reinterpret_cast<char*>(&size2), sizeof(size2));
                if (rxDumpFile) 
                {
                    rxDumpFile.read(buffer2, size2);
                    INFO("[");
                    std::for_each(buffer2, buffer2 + size2 - 2, [](auto& val){ INFO("%02x ", (uint8_t)val); });
                    int rssi = rssi_converter(buffer2[size2 - 2]);
                    uint8_t lqi = buffer2[size2 - 1];
                    INFO("] RSSI: %d  LQI: %u", rssi, lqi);
                }
            }

            if (rxDumpFile)
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

void parsing_dump_by_txfile(const char* tx_filename, const char* rx_filename)
{
    std::ifstream txDumpFile(tx_filename);
    std::ifstream rxDumpFile(rx_filename);

    std::map<int, uint32_t> rssi_cnt_map;
    std::map<int, uint32_t> rssi_error_map;
    std::map<int, uint32_t> rssi_bit_err_map;

    if(txDumpFile.is_open() && rxDumpFile.is_open())
    {
        char buffer1[1024];
        char buffer2[1024];
        uint8_t size1 = 0;
        uint8_t size2 = 0;
        uint32_t pktErrCnt = 0;
        uint32_t pktCnt = 0;


        while(txDumpFile || rxDumpFile)
        {
            if (txDumpFile)
            {
                txDumpFile.read(reinterpret_cast<char*>(&size1), sizeof(size1));
                if (txDumpFile) 
                {
                    txDumpFile.read(buffer1, size1); 
                    INFO("[");
                    std::for_each(buffer1, buffer1 + size1, [](auto& val){ INFO("%02x ", (uint8_t)val); });
                    INFO("]");
                }
            }
            INFO("\t--->\t");  
            if (rxDumpFile)
            {
                rxDumpFile.read(reinterpret_cast<char*>(&size2), sizeof(size2));
                if (rxDumpFile) 
                {
                    rxDumpFile.read(buffer2, size2);
                    INFO("[");
                    std::for_each(buffer2, buffer2 + size2 - 2, [](auto& val){ INFO("%02x ", (uint8_t)val); });
                    int rssi = rssi_converter(buffer2[size2 - 2]);
                    uint8_t lqi = buffer2[size2 - 1];
                    INFO("] RSSI: %d  LQI: %u", rssi, lqi);
                }
            }

            if (txDumpFile && rxDumpFile)
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

}