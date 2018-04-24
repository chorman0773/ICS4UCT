#ifndef __Client_hpp_2018_04_24_12_19
#define __Client_hpp_2018_04_24_12_19

#include "Bindings.hpp"
namespace net{
class ClientBinding;//Forward Declare the Binding Class
};

class Client{
    net::ClientBinding& binding;
    Employee boundEmployee;
    unsigned char sessionKey[16];
public:
    Client(net::ClientBinding&);
    void setUser(const Employee&);
    void authenticate();
    const Employee& getEmployee();
    void getSessionKey(unsigned char*);
};


#endif