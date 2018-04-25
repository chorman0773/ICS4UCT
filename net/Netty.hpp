#ifndef __Netty_hpp_2018_04_24_12_39
#define __Netty_hpp_2018_04_24_12_39
#include "Bindings.hpp"
#include "UUID.hpp"
#include "JTime.hpp"
#include <cstdint>
#include <string>
#include <type_traits>
#include "Hash.hpp"
using std::string;
class Client;
namespace net{
    class PacketBuffer;
    class Packet;
    class Connection;
    class ClientBinding{
        Connection& con;
        Client* client;
    public:
        ClientBinding(Connection&);
        void send(PacketBuffer&);      
        void send(Packet&);
        void handle(Packet&);
        void handleNext();
        void setClient(Client&);
        void disconnect(const string&);
    };

    

    class Packet : public Hashable{
    public:
        virtual void writeTo(PacketBuffer&)=0;
        virtual void readFrom(PacketBuffer&)=0;
        virtual int getId()const=0;
    };

    class PacketBuffer{
        int size;
        int ptr;
        unsigned char* data;
        void resize(int);
    public:
        PacketBuffer();
        PacketBuffer(int,unsigned char*);
        PacketBuffer(const PacketBuffer&);
        PacketBuffer& operator=(const PacketBuffer&);
        ~PacketBuffer();
        const unsigned char* getData()const;
        int getSize()const;
        int length()const;
        void write(unsigned char);
        int read();
        void writeByte(int);
        void writeShort(int);
        void writeInt(int);
        void writeLong(int64_t);
        void writeFloat(float);
        void writeDouble(double);
        void writeUtf(const string&);
        void writeEmployeeData(const Employee&);
        void writeUUID(const UUID&);
        void writeInstant(const Instant&);
        void writeDuration(const Duration&);
        void writeBytes(unsigned char*,size_t,size_t);
        void writeVersion(Version);
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
        int readBytes(unsigned char*,size_t,size_t);
        Version readVersion();

        PacketBuffer& operator<<(char);
        PacketBuffer& operator<<(unsigned char);
        PacketBuffer& operator<<(short);
        PacketBuffer& operator<<(unsigned short);
        PacketBuffer& operator<<(int);
        PacketBuffer& operator<<(int64_t);
        PacketBuffer& operator<<(float);
        PacketBuffer& operator<<(double);
        PacketBuffer& operator<<(const string&);
        PacketBuffer& operator<<(const Employee&);
        PacketBuffer& operator<<(const UUID&);
        PacketBuffer& operator<<(const Instant&);
        PacketBuffer& operator<<(const Duration&);
        PacketBuffer& operator<<(Version);
        PacketBuffer& operator<<(const PacketBuffer&);

        PacketBuffer& operator>>(char&);
        PacketBuffer& operator>>(unsigned char&);
        PacketBuffer& operator>>(short&);
        PacketBuffer& operator>>(unsigned short&);
        PacketBuffer& operator>>(int&);
        PacketBuffer& operator>>(int64_t&);
        PacketBuffer& operator>>(float&);
        PacketBuffer& operator>>(double&);
        PacketBuffer& operator>>(string&);
        PacketBuffer& operator>>(Employee&);
        PacketBuffer& operator>>(UUID&);
        PacketBuffer& operator>>(Instant&);
        PacketBuffer& operator>>(Duration&);
        PacketBuffer& operator>>(Version&);
        
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
            int getId()const;
            int hashCode()const;
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
            int getId()const;
            int hashCode()const;
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
            int getId()const;
            int hashCode()const;
        };
        class CPacketAddEmployee final:public Packet{
            string name;
            double salary;
        public:
            CPacketAddEmployee(const string&,double);
            CPacketAddEmployee();
            const string& getName()const;
            double getSalary()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class CPacketModifyEmployee{
            enum class Action{
                DELETE, SET_SALARY, ADD_PERMISSION, REMOVE_PERMISSON
            };
            Action action;
            UUID employee;
            double nSalary;
            Permission affectedPermission;
        public:
            CPacketModifyEmployee(const UUID&);
            CPacketModifyEmployee(const UUID&,double);
            CPacketModifyEmployee(cosnt UUID&,Action a,Permission p);
            CPacketModifyEmployee();
            Action getAction()const;
            double getNewSalary()const;
            Permission getAffectedPermission()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };

        class CPacketDisconnect final:public Packet{
        public:
            CPacketDisconnect();
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
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
            int getId()const;
            int hashCode()const;
        };

        class CPacketChangePassword final:public Packet{
            char* newPassword;
        public:
            CPacketChangePassword();
            CPacketChangePassword(char*);
            void getNewPassword(char*);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class CPacketConnect final:public Packet{
            Version ver;
        public:
            CPacketConnect();
            CPacketConnect(Version);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            Version getVersion()const;
            int getId()const;
            int hashCode()const;
        };
        class CPacketRequestElevation final:public Packet{
        public:
            CPacketRequestElevation();
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class CPacketUpdateStatus final:public Packet{
            Status s;
        public:
            CPacketUpdateStatus();
            CPacketUpdateStatus(Status);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };

        class CPacketAuthenticateElevated final:public Packet{
            unsigned char sessionKey[16];
            unsigned char elevatedSessionKey[16];
            unsigned char signedSessionKey[128];
            unsigned char signedElevatedKey[128];
        public:
            CPacketAuthenticateElevated();
            CPacketAuthenticateElevated(Client&);
            void getSession(unsigned char*);
            void getElevatedSession(unsigned char*);
            void verifySession(const unsigned char*);
            void verifyElevated(const unsigned char*);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
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
            int getId()const;
            int hashCode()const;
        };
        class SPacketSendEncryptedPrivateKey final:public Packet{
            size_t len;
            unsigned char* privateKey;
        public:
            SPacketSendEncryptedPrivateKey();
            SPacketSendEncryptedPrivateKey(size_t,unsigned char*);
            size_t getEncryptedPrivateKey(unsigned char*);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketAuthenticationSuccess final:public Packet{
        public:
            SPacketAuthenticationSuccess();
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketAuthenticationFailure final:public Packet{
            string reason;
        public:
            SPacketAuthenticationFailure();
            SPacketAuthenticationFailure(const string&);
            const string& getReason()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketDisconnect final:public Packet{
            string reason;
        public:
            SPacketAuthenticationFailure();
            SPacketAuthenticationFailure(const string&);
            const string& getReason()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketEmployeeList final:public Packet{
            Employees employeeList;
        public:
            SPacketEmployeeList(const Employees&);
            SPacketEmployeeList();
            const Employees& getEmployees()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketOperationResult final:public Packet{
            string msg;
            bool res;
        public:
            SPacketOperationSuccess(const string&,bool);
            SPacketOperatorSuccess();
            const string& getMessage()const;
            bool didSucceed()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketConnectSuccess final:public Packet{
            unsigned char session[16];
        public:
            SPacketConnectSuccess();
            SPacketConnectSuccess(unsigned char(&)[16]);
            void getSessionKey(unsigned char*)const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketUpdateEmployeePublic final:public Packet{
            UUID employee;
            int publicKeyLength;
            unsigned char* publicKey;
        public:
            SPacketUpdateEmployeePublic();
            SPacketUpdateEmployeePublic(const UUID&);
            void getPublicKey(unsigned char*);
            int getPublicKeyLength();
            const UUID& getEmployee()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketUpdateEmployeeStatus final:public Packet{
            UUID employee;
            Status status;
        public:
            SPacketUpdateEmployeeStatus();
            SPacketUpdateEmployeeStatus(const UUID&);
            const UUID& getEmployee()const;
            Status getStatus()const;
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
        class SPacketElevateSession final:public Packet{
            unsigned char elevatedSession[16];
        public:
            SPacketElevateSession();
            SPacketElevateSession(unsigned char*);
            void getElevatedKey(unsigned char*);
            void writeTo(PacketBuffer&);
            void readFrom(PacketBuffer&);
            int getId()const;
            int hashCode()const;
        };
    };

    class Connection{
        SOCKET sock;
        unsigned char aesKey[32];
        client::CPacketKeepAlive lastClientKeepAlive;
        server::SPacketKeepAlive lastServerKeepAlive;
        uint64_t latency;
    public:
        Connection(SOCKET);
        ~Connection();
        void handshake();
        void send(PacketBuffer&);
        int recieve(PacketBuffer&,int32_t&);
        void close();
        void handle(Packet&);
    }
};


#endif