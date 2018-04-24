#ifndef __Bindings_hpp_2018_04_24_09_28
#define __Bindings_hpp_2018_04_24_09_28
#include <string>
#include <map>
#include <vector>
#include "Hash.hpp"
#include "UUID.hpp"
#include "EnumSet.hpp"

using std::string;
using std::map;
using std::vector;

enum class Permission{
	AUTH, ADD_EMPLOYEE, MODIFY_EMPLOYEES, UPDATE_INFO, DELETE_EMPLOYEES,
	ADMINISTRATOR
};


class Employee : public Hashable{
	string name;
	UUID id;
	double salary;
	unsigned char iv[16];
	unsigned char salt[32];
	unsigned char pubKey[128];
	EnumSet<Permission> permissions;
public:
	Employee();
	Employee(const string&,const UUID&,double,const unsigned char(&)[16],const unsigned char(&)[32],const unsigned char(&)[128],const EnumSet<Permission>&);
	static Employee newEmployee(const string&,double);
	int32_t hashCode()const;
	const string& getName()const;
	bool hasPermission(Permission)const;
	void addPermission(Permission);
	void removePermission(Permission);
	void getIv(unsigned char*)const;
	void getSalt(unsigned char*)const;
	void getPublicKey(unsigned char*)const;
	const UUID& getUUID()const;
	double getPay()const;
	const EnumSet<Permission>& getPermissions()const;
};

class Employees:public Hashable{
	map<UUID,Employee> employeeMap;
	vector<Employee> employeeRegistry;
	typedef vector<Employee>::iterator iterator;
	typedef vector<Employee>::const_iterator const_iterator;
	Employees();
public:
	const static Employees instance;
	void load();
	void save()const;
	void removeEmployee(const UUID&);
	const UUID& addEmployee(const string&,double);
	Employee& getEmployee(const UUID&);
	iterator begin();
	const_iterator begin()const;
	iterator end();
	const_iterator end()const;
};





#endif