#ifndef __SocketLib_hpp_2018_05_18_12_24
#define __SocketLib_hpp_2018_05_18_12_24
extern "C"{
#ifdef _WIN32
#include "winsock2_32.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
typedef decltype(socket(0,0,0)) SOCKET;
#endif
};
#include <vector>
#include <memory>
#include <string>

using std::string;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;

class Version;
class UUID;

class Socket{
private:
    SOCKET sock;
    bool closed;
public:
    ~Socket();
    Socket();
    Socket(const string&,unsigned short);
    Socket(SOCKET);
    void connect(const string&,unsigned short);
    void close();
    Socket& operator<<(char);
    Socket& operator<<(unsigned char);
    Socket& operator<<(short);
    Socket& operator<<(unsigned short);
    Socket& operator<<(int);
    Socket& operator<<(long long);
    Socket& operator<<(const string&);
    Socket& operator<<(const char*);
    Socket& operator<<(Version);
    Socket& operator<<(const UUID&);
    Socket& operator>>(char&);
    Socket& operator>>(unsigned char&);
    Socket& operator>>(short&);
    Socket& operator>>(unsigned short&);
    Socket& operator>>(int&);
    Socket& operator>>(unsigned long long&);
    Socket& operator>>(string&);
    Socket& operator>>(Version&);
    Socket& operator>>(UUID&);
};

class ServerSocket{
private:
    SOCKET sSock;
    vector<Socket> sockets;
    bool closed;
public:
    ~ServerSocket();
    ServerSocket();
    ServerSocket(const string&,unsigned short);
    void bind(const string&,unsigned short);
    void close();
    Socket& accept();
};

#endif
