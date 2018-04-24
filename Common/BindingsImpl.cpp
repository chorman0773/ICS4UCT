#include "Bindings.hpp"

Employee::Employee():name(),uuid("00000000-0000-0000-0000-000000000000"),salary(0){};
Employee::Employee(const string& name,const UUID& id,double salary,const unsigned char (&iv)[16],
                    const unsigned char(&salt)[32],const unsigned char(&pub)[128],const EnumSet<Permission>& permissions)
                    :name(name),uuid(id),salary(salary),permissions(permissions){
                        memcpy(this->iv,iv,16);
                        memcpy(this->salt,salt,32);
                        memcpy(this->pubKey,pub,128);
                    }

int32_t Employee::hashCode(){
    int32_t hash = hashcode(name);
    const int32_t hashPrime = 31;
    hash *= hashPrime;
    hash += hashcode(id);
    hash *= hashPrime;
    hash += hashcode(salary);
    return hash;
}

bool Employee::hasPermission(Permission perm)const{
    return permissions.contains(perm);
}

void Employee::addPermission(Permission perm){
    permissions.add(perm);
}
void Employee::removePermission(Permission perm){
    permissions.remove(perm);
}

const UUID& Employee::getUUID()const{
    return uuid;
}

double Employee::getPay()const{
    return salary;
}

const EnumSet<Permissions>& Employee::getPermissions()const{
    return permissions;
}

void Employee::getIv(unsigned char* out)const{
    memcpy(out,iv,16);
}
void Employee::getSalt(unsigned char* out)const{
    memcpy(out,salt,32);
}
void Employee::getPublicKey(unsigned char* out)const{
    memcpy(out,pubKey,128);
}

const Employees Employees::instance;

Employees::Employees(){}

Employees::iterator Employees::begin(){
    return employeeRegistry.begin();
}
Employees::const_iterator Employees::begin()const{
    return employeeRegistry.cbegin();
}
Employees::iterator Employees::end(){
    return employeeRegistry.end();
}

Employees::iterator Employees::end()const{
    return employeeRegistry.cend();
}
