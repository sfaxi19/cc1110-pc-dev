#pragma once
#include <iostream>

extern bool trace;
extern bool log;

#define LOG(...)   if(log){fprintf(stdout, __VA_ARGS__);}
#define TRACE(...) if(trace){fprintf(stdout, __VA_ARGS__);}
#define IFLOG(...) if(log)
#define ERR(...)   fprintf(stderr, __VA_ARGS__);
#define INFO(...)  fprintf(stdout, __VA_ARGS__);