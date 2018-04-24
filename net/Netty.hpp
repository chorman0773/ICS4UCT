#ifndef __Netty_hpp_2018_04_24_12_39
#define __Netty_hpp_2018_04_24_12_39
#include "Bindings.hpp"
#include "UUID.hpp"
#include "JTime.hpp"
#include <cstdint>
#include <string>
using std::string;
namespace net{
    class PacketBuffer;
    class Packet;

    class ClientBinding{
        SOCKET sock;
    public:
        ClientBinding(SOCKET);
        void send(PacketBuffer&);
        int recieve(PacketBuffer&);       
        void send(Packet&);
        void handle(Packet&);
    };

    class Packet{
    public:
        virtual void writeTo(PacketBuffer&)=0;
        virtual void readFrom(PacketBuffer&)=0;
        virtual int getId()=0;
    };

    class PacketPtr{
        Packet* p;
    public:
        PacketPtr(int);
        PacketPtr(Packet&);
        PacketPtr(Packet&&);
        ~PacketPtr();
        PacketPtr& operator=(Packet&);
        PacketPtr& operator=(Packet&&);
        Packet* operator->();
        Packet& operator*();
    };
    PacketPtr operator&(Packet&);

    class PacketBuffer{
        int size;
        int ptr;
        unsigned char* data;
        void resize(int);
    public:
        PacketBuffer(int,unsigned char*);
        const unsigned char* getData()const;
        int getSize();
        void write(unsigned char);
        int read();
        void writeByte(int);
        void writeShort(int);
        void writeInt(int);
        void writeLong(int64_t);
        void writeUtf(const string&);
        void writeEmployeeData(const Employee&);
        void writeUUID(const UUID&);
        void writeInstant(const Instant&);
        void writeDuration(const Duration&);
        char readByte();
        int readUnsignedByte();
        short readShort();
        int readUnsignedShort();
        int readInt();
        int64_t readLong();
        string readString();
        Employee readEmployeeData();
        UUID readUUID();
        Instant readInstant();
        Duration readDuration();

        PacketBuffer& operator<<(char);
        PacketBuffer& operator<<(unsigned char);
        PacketBuffer& operator<<(short);
        PacketBuffer& operator<<(unsigned short);
        PacketBuffer& operator<<(int);
        PacketBuffer& operator<<(int64_t);
        PacketBuffer& operator<<(const string&);
        PacketBuffer& operator<<(const Employee&);
        PacketBuffer& operator<<(const UUID&);
        PacketBuffer& operator<<(const Instant&);
        PacketBuffer& operator<<(const Duration&);
        template<size_t s> PacketBuffer& operator<<(unsigned char(&arr)[s]){
            for(size_t i = 0;i<s;i++)
                *this << arr[i];
            return *this;
        }

        PacketBuffer& operator>>(char&);
        PacketBuffer& operator>>(unsigned char&);
        PacketBuffer& operator>>(short&);
        PacketBuffer& operator>>(unsigned short&);
        PacketBuffer& operator>>(int&);
        PacketBuffer& operator>>(int64_t&);
        PacketBuffer& operator>>(string&);
        PacketBuffer& operator>>(Employee&);
        PacketBuffer& operator>>(UUID&);
        PacketBuffer& operator>>(Instant&);
        PacketBuffer& operator>>(Duration&);
        template<size_t s> PacketBuffer& operator>>(unsigned char(&arr)[s]){
            for(size_t i = 0;i<s;i++)
                *this >> arr[i];
            return *this;
        }
    };

    namespace client{
        using net::Packet;
        class  CPacketRequestPrivateKeyFile final:public Packet{
            unsigned char session[16];
        public:
            CPacketRequestPrivateKeyFile();
            CPacketRequestPrivateKeyFile(Client&);
            void getSessionKey(unsigned char*);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId();
        };

        class CPacketSetEmployee final:public Packet{
            UUID employee;
            unsigned char session[16];
        public:
            CPacketSetEmployee();
            CPacketSetEmployee(Client&);
            const UUID& getUUID();
            void getSessionKey(unsigned char*);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId();
        };

        class CPacketAuthenticate final:public Packet{
            unsigned char session[16];
            unsigned char signedSession[128];
        public:
            CPacketAuthenticate();
            CPacketAuthenticate(Client&,unsigned char(&)[128]);
            void verify(unsigned char*,unsigned char(&)[128]);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId();
        };
        class CPacketAddEmployee final:public Packet{
            string name;
            double salary;
        };
        class CPacketKeepAlive final:public Packet{
            UUID cycleId;
            Instant encodingTime;
        public:
            CPacketKeepAlive();
            CPacketKeepAlive(const UUID&);
            const Instant& getEncodingType()const;
            const UUID& getCycleId()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId();
        };
    };
    namespace server{
        using net::Packet;
        class SPacketKeepAlive final:public Packet{
            UUID cycleId;
            Instant encodingTime;
        public:
            SPacketKeepAlive();
            const Instant& getEncodingType()const;
            const UUID& getCycleId()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId();
        };
        class SPacketSendEncryptedPrivateKey final:public Packet{
            unsigned char privateKey[144];
        public:
            SPacketSendEncryptedPrivateKey();
            SPacketSendEncryptedPrivateKey(unsigned char*);
            void getEncryptedPrivateKey(unsigned char*);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId();
        };
        class SPacketAuthenticationSuccess final:public Packet{
        public:
            SPacketAuthenticationSuccess();
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId();
        };
        class SPacketAuthenticationFailure final:public Packet{
            string reason;
        public:
            SPacketAuthenticationFailure();
            SPacketAuthenticationFailure(const string&);
            const string& getReason()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId();
        };
    };
};


#endif