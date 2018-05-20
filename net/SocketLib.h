#ifndef __SocketLib_h_2018_05_18_12_24
#define __SocketLib_h_2018_05_18_12_24
#ifdef __cplusplus
extern "C"{
#endif
#ifdef _WIN32
#include "winsock2_32.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif
#ifdef __cplusplus
};
#endif
#endif
