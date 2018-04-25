#ifndef __Client_hpp_2018_04_24_12_19
#define __Client_hpp_2018_04_24_12_19

#include "Bindings.hpp"
#include "JTime.hpp"
namespace net{
class ClientBinding;//Forward Declare the Binding Class
};

class Client{
    net::ClientBinding& binding;
    Employee boundEmployee;
    Instant elevationTime;
    bool isElevatedMode;
    unsigned char sessionKey[16];
    unsigned char elevatedSessionKey[16];
public:
    Client(net::ClientBinding&);
    void setUser(const Employee&);
    void authenticate();
    const Employee& getEmployee();
    void getSessionKey(unsigned char*);
    void setElevatedMode(unsigned char*);
    void getElevatedKey(unsigned char*);
};


#endif