#include "SocketLib.hpp"
#include <UUID.hpp>
#include <Version.hpp>

void toBE(unsigned int i,char (&out)[4]){
    out[0] = (i>>24)&0xff;
    out[1] = (i>>16)&0xff;
    out[2] = (i>>8 )&0xff;
    out[3] = i      &0xff;
}

void toBE(unsigned long long l,char (&out)[8]){
    out[0] = (l>>56)&0xff;
    out[1] = (l>>48)&0xff;
    out[2] = (l>>40)&0xff;
    out[3] = (l>>32)&0xff;
    out[4] = (l>>24)&0xff;
    out[5] = (l>>16)&0xff;
    out[6] = (l>>8 )&0xff;
    out[7] = l      &0xff;
}

void toLE(unsigned int& i,const char (&out)[4]){
    i = ((unsigned int)out[0])<<24|((unsigned int)out[1])<<16|((unsigned int)out[2])<<8|(unsigned int)out[3];
}
void toLE(unsigned long long& l,const char (&out)[8]){
    l = ((unsigned long long)(((unsigned int)out[0])<<24|((unsigned int)out[1])<<16|((unsigned int)out[2])<<8|(unsigned int)out[3]))
        <<32 | ((unsigned int)out[4])<<24|((unsigned int)out[5])<<16|((unsigned int)out[6])<<8|(unsigned int)out[7];
}


Socket::Socket():closed(false){
    sock = socket(AF_INET,SOCK_STREAM,6);
}

Socket::Socket(const string& host,unsigned short port):closed(false){
    sock = socket(AF_INET,SOCK_STREAM,6);
    connect(host,port);
}
Socket::Socket(SOCKET sock):closed(false),sock(sock){}

Socket::~Socket(){
    if(!closed)
        close();
}

void Socket::close(){
    ::close(sock);
    closed = true;
}

void Socket::connect(const string& host,unsigned short port){
    sockaddr_in addr;
    
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.S_un = inet_addr(host.c_str());
    ::connect(sock,(const socketaddr*)&addr,sizeof(addr));
}

Socket& Socket::operator<<(char v){
    send(sock,&v,1,0);
    return *this;   
}

Socket& Socket::operator<<(unsigned char v){
    send(sock,(const char*)&v,1,0);
    return *this;
}
Socket& Socket::operator<<(short s){
    return (*this)<<(unsigned short)s;
}

Socket& Socket::operator<<(unsigned short s){
    char val[2];
    val[0] = (s>>8)&0xFF;
    val[1] = s&0xFF;
    send(sock,&val[0],sizeof(s),0);
    return *this;
}

Socket& Socket::operator<<(int i){
    char val[4];
    toBE(i,val);
    send(sock,&val[0],sizeof(i),0);
    return *this;
}

Socket& Socket::operator<<(long long l){
    char val[8];
    toBE(l,val);
    send(sock,&val[0],sizeof(l),0);
}

Socket& Socket::operator<<(const UUID& u){
    uint64_t low = u.getLow();
    uint64_t high = u.getHigh();
    return (*this)<<high<<low;
}

Socket& Socket::operator<<(const Version& v){
    char ver[2];
    ver[0] = (v.getMajor()-1)&0xff;
    ver[1] = v.getMinor()&0xff;
    send(sock,&ver[0],sizeof(ver),0);
    return *this;
}
Socket& Socket::operator<<(const string& str){
    (*this)<<(unsigned short)str.length();
    send(sock,str.data(),str.length(),0);
    return *this;
}

Socket& Socket::operator>>(char& c){
    recv(sock,&c,1,0);
    return *this;
}

Socket& Socket::operator>>(short& v){
    char val[2];
    recv(sock,&val[0],sizeof(val),0);
    v = ((unsigned short)val[0])<<8|val[1];
    return *this;
}

Socket& Socket::operator>>(unsigned char& c){
    recv(sock,(char*)&c,1,0);
    return *this;
}

Socket& Socket::operator>>(unsigned short& v){
    char val[2];
    recv(sock,&val[0],sizeof(val),0);
    v = ((unsigned short)val[0])<<8|val[1];
    return *this;
}

Socket& Socket::operator>>(int& i){
    char val[4];
    recv(sock,&val[0],sizeof(val),0);
    toLE(i,val);
    return *this;
}

Socket& Socket::operator>>(long long& l){
    char val[8];
    recv(sock,&val[0],sizeof(val),0);
    toLE(l,val);
    return *this;
}

Socket& Socket::operator>>(UUID& u){
    uint64_t high, low;
    (*this)>>high>>low;
    u = UUID(high,low);
    return *this;
}

Socket& Socket::operator>>(Version& v){
    char ver[2];
    (*this)>>ver[0]>>ver[1];
}

Socket& Socket::operator>>(string& str){
    unsigned short s;
    char* buffer;
    (*this)>>s;
    buffer = new char[s];
    str.clear();
    recv(sock,buffer,s,0);
    str.assign(buffer,s);
    return *this;
}

ServerSocket::ServerSocket():closed(false){}
ServerSocket::ServerSocket(const string& str,unsigned short s):closed(false){
    sock = socket(AF_INET,SOCK_STREAM,6);
    bind(str,s);
}
ServerSocket::~ServerSocket(){
    if(!closed)
        close();
}

void ServerSocket::bind(const string& str,unsigned short s){
    sockaddr_in addr;
    
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.S_un = inet_addr(host.c_str());
    ::bind(sSock,(const socketaddr*)&addr,sizeof(addr));
    listen(sSock,1);
}

void close(){
    for(Socket& s:sockets)
        s.close();
    ::close(sSock);
}

Socket& accept(){
    SOCKET sock;
    while((sock = accept(sSock))<0);
    sockets.push_back(Socket(sock));
    return sockets.back();
}





