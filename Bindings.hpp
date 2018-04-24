#ifndef __Bindings_hpp_2018_04_24_09_28
#define __Bindings_hpp_2018_04_24_09_28
#include <string>
#include "Hash.hpp"
#include "UUID.hpp"
#include "EnumSet.hpp"

using std::string;

enum class Permission{
	AUTH, ADD_EMPLOYEE, MODIFY_EMPLOYEES, UPDATE_INFO, DELETE_EMPLOYEES,
	ADMINISTRATOR
};

class Employee : public Hashable{
	string name;
	UUID id;
	double pay;
	unsigned char salt[16];
	unsigned char publicKey[128];
	EnumSet<Permission> permissions;
public:
	Employee();
	Employee newEmployee(const string&);
	int32_t hashCode()const;
	const string& getName()const;
	bool checkAuth(char(&)[128]);
	void hashSHA256(char(&)[32]);
	bool hasPermission(Permission);
	void addPermission(Permission);
	void removePermission(Permission);
};





#endif