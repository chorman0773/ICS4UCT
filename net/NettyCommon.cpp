#include "Netty.hpp"
#include <functional>
#include <cstring>
using std::function;

float fromRawIntBits(int bits){
    return reinterpret_cast<float&>(bits);
}
int toRawIntBits(float f){
    return reinterpret_cast<int&>(f);
}

double fromRawLongBits(uint64_t bits){
    return reinterpret_cast<double&>(bits);
}
uint64_t toRawLongBits(double d){
    return reinterpret_cast<uint64_t&>(d);
}

using namespace net;
using namespace server;
using namespace client;
function<Packet*()> ctors[] ={
    [](){
        return new CPacketConnect;
    },
    [](){
        return new SPacketConnectSuccess;
    },
    [](){
        return new SPacketEmployeeList;
    },
    [](){
        return new SPacketKeepAlive;
    },
    [](){
        return new CPacketKeepAlive;
    },
    [](){
        return new CPacketRequestPrivateKeyFile;
    },
    [](){
        return new CPacketAuthenticate;
    },
    [](){
        return new CPacketRequestElevation;
    },
    [](){
        return new CPacketAuthenticateElevated;
    },
    [](){
        return new SPacketSendEncryptedPrivateKey;
    },
    [](){
        return new SPacketAuthenticateSuccess;
    },
    [](){
        return new SPacketAuthenticateFailure;
    },
    [](){
        return new SPacketUpdateEmployeeStatus;
    },
    [](){
        return new SPacketUpdateEmployeePublic;
    },
    [](){
        return new SPacketElevateSession;
    },
    [](){
        return new CPacketUpdateStatus;
    },
    [](){
        return new CPacketModifyEmployee;
    },
    [](){
        return new CPacketChangePassword;
    },
    [](){
        return new CPacketDisconnect;
    },
    [](){
        return new SPacketDisconnect;
    }
};


CPacketConnect::CPacketConnect(){}
CPacketConnect::CPacketConnect(Version v):ver(v){}
void CPacketConnect::writeTo(PacketBuffer& buff){
    buff << ver;
}
void CPacketConnect::readFrom(PacketBuffer& buff){
    buff >> ver;
}
int CPacketConnect::getId()const{
    return 0;
}
int CPacketConnect::hashCode()const{
    return ver.hashCode();
}
Version CPacketConnect::getVersion()const{
    return ver;
}



SPacketConnectSuccess::SPacketConnectSuccess(){}
SPacketConnectSuccess::SPacketConnectSuccess(const string& msg):msg(msg){}
void SPacketConnectSuccess::writeTo(PacketBuffer& buff){
    buff << msg;
}
void SPacketConnectSuccess::readFrom(PacketBuffer& buff){
    buff >> msg;
}
int SPacketConnectSuccess::getId()const{
    return 1;
}
int SPacketConnectSuccess::hashCode()const{
    return getId()*31+hashcode(msg);
}

SPacketEmployeeList::SPacketEmployeeList(){}
SPacketEmployeeList::SPacketEmployeeList(const Employees& list):employeeList(list){}

void SPacketEmployeeList::writeTo(PacketBuffer& buff){
    buff << employeeList.length();
    for(const Employee& e:employeeList)
        buff << e;
}
void SPacketEmployeeList::readFrom(PacketBuffer& buff){
    int length;
    buff >> length;
    Employee tmp;
    for(int i = 0;i<length;i++){
        buff >> tmp;
        employeeList.addEmployee(tmp);
    }
}
const Employees& SPacketEmployeeList::getEmployees()const{
    return employeeList;
}
int SPacketEmployeeList::getId()const{
    return 2;
}
int SPacketEmployeeList::hashCode()const{
    return getId()*31+hashcode(employeeList);
}

SPacketKeepAlive::SPacketKeepAlive(){}

void SPacketKeepAlive::writeTo(PacketBuffer& buff){
    buff << Instant::now();
    buff << UUID::ofNow();
}
void SPacketKeepAlive::readFrom(PacketBuffer& buff){
    buff >> encodingTime;
    buff >> cycleId;
}

int SPacketKeepAlive::getId()const{
    return 3;
}
int SPacketKeepAlive::hashCode()const{
    int h = 0;
    const int hashPrime = 31;
    h = getId();
    h *= hashPrime;
    h += hashcode(encodingTime);
    h *= hashPrime;
    h += hashcode(cycleId);
    return h;
}



ClientBinding::ClientBinding(Connection& con):con(con){}

void ClientBinding::send(PacketBuffer& buff){
    con.send(buff);
}
void ClientBinding::send(Packet& packet){
    PacketBuffer encode;
    PacketBuffer data;
    encode.writeByte(packet.getId());
    encode.writeInt(packet.hashCode());
    packet.writeTo(data);
    encode << data.length();
    encode << data;
    send(encode);
}


const int extra = 10;

PacketBuffer::PacketBuffer():size(0),ptr(0),data(nullptr){

}
PacketBuffer::PacketBuffer(int size,unsigned const char* data):size(size),ptr(0),data(new unsigned char[size]){
    memcpy(this->data,data,size);
}
PacketBuffer::PacketBuffer(const PacketBuffer& src):size(src.size),ptr(src.ptr),data(new unsigned char[size]){
    memcpy(this->data,src.data,src.size);
}

PacketBuffer::~PacketBuffer(){
    delete[] data;
}

PacketBuffer& operator=(const PacketBuffer& src){
    if(&src==this)
        return *this;
    else{
        if(data!=nullptr)
            delete[] data;
        this->size = src.size;
        this->ptr = src.ptr;
        this->data = new unsigned char[size];
        memcpy(this->data,src.data,size);
    }
    return *this;
}

void PacketBuffer::resize(int newSize){
    if(data==nullptr)
        data = new char[newSize];
    else{
        unsigned char* nData = new unsigned char[newSize];
        memcpy(nData,data,size);
        delete[] data;
        data = nData;
    }
    size = newSize;
}

void PacketBuffer::write(unsigned char c){
    if(ptr>=size)
        resize(extra);
    data[ptr++] = c;
}

int PacketBuffer::read(){
    if(ptr>=size)
        return -1;
    else
        return data[ptr++];
}

int PacketBuffer::getSize()const{
    return size;
}

int PacketBuffer::length()const{
    return ptr;
}

const unsigned char* PacketBuffer::getData()const{
    return data;
}

void PacketBuffer::writeByte(int b){
    write(b&0xff);
}
void PacketBuffer::writeShort(int s){
    write(s&0xff);
    write((s>>8)&0xff);
}
void PacketBuffer::writeInt(int i){
    write(i&0xff);
    write((i>>8)&0xff);
    write((i>>16)&0xff);
    write((i>>24)&0xff);
}
void PacketBuffer::writeLong(uint64_t l){
    writeInt(l&0xffffffff);
    writeInt(l>>32);
}

void PacketBuffer::writeFloat(float f){
    writeInt(toRawIntBits(f));
}

void PacketBuffer::writeDouble(double d){
    writeLong(toRawLongBits(d));
}

void PacketBuffer::writeUtf(string s){
    writeInt(s.length());
    for(char c:s)
        writeByte(c);
}
