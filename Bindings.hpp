#ifndef __Bindings_hpp_2018_04_24_09_28
#define __Bindings_hpp_2018_04_24_09_28
#include <string>
#include <map>
#include <vector>
#include <set>
#include "Hash.hpp"
#include "UUID.hpp"
#include "EnumSet.hpp"

using std::string;
using std::map;
using std::vector;
using std::set;

enum class Permission{
	AUTH, ADD_EMPLOYEE, MODIFY_EMPLOYEES, UPDATE_INFO, DELETE_EMPLOYEES,
	ADMINISTRATOR
};

enum class Status{
	OFFLINE, AWAY, ONLINE
};

class Employee : public Hashable{
	string name;
	UUID id;
	double salary;
	unsigned char iv[16];
	unsigned char salt[32];
	unsigned char pubKey[128];
	EnumSet<Permission> permissions;
	Status s;
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
	void setStatus(Status);
	const UUID& getUUID()const;
	Status getStatus()const;
	double getPay()const;
	const EnumSet<Permission>& getPermissions()const;
	bool operator==(const Employee&)const;
	bool operator!=(const Employee&)const;
	bool operator<=(const Employee&)const;
	bool operator>=(const Employee&)const;
	bool operator< (const Employee&)const;
	bool operator> (const Employee&)const;
};

class Employees:public Hashable{
	map<UUID,Employee> employeeMap;
	vector<Employee> employeeRegistry;
	typedef vector<Employee>::iterator iterator;
	typedef vector<Employee>::const_iterator const_iterator;
public:
	Employees();
	void load();
	void save()const;
	void removeEmployee(const UUID&);
	const UUID& addEmployee(const string&,double);
	void addEmployee(const Employee&);
	Employee& getEmployee(const UUID&);
	iterator begin();
	const_iterator begin()const;
	iterator end();
	const_iterator end()const;
	int length()const;
	int hashCode()const;
	void sort();
};

enum class Units: unsigned char{
	ammount, kg, L	
};

class Product:public Hashable{
	UUID productId;
	string name;
	string supplierMailingAddress;
	string supplierName;
	string supplierPhoneNumber;
	double cost;
	Units units;
public:
	Product();
	Product(const UUID&,const string&,const string&,const string&,double,Units);
	const UUID& getUUID()const;
	const string& getName()const;
	const string& getSupplierMailingAddress()const;
	const string& getSupplierName()const;
	const string& getSupplierPhoneNumber()const;
	double getCost()const;
	Units getUnits()const;
	int hashCode()const;
	bool operator==(const Product&)const;
	bool operator<=(const Product&)const;
	bool operator>=(const Product&)const;
	bool operator< (const Product&)const;
	bool operator> (const Product&)const;
	bool operator!=(const Product&)const;
};

class Products:public Hashable{
	map<UUID,Product> productMap;
	set<Product> products;
	typedef set<Product>::iterator iterator;
	typedef set<Product>::const_iterator const_iterator;
public:
	Products();
	void load();
	void save()const;
	void removeProduct(const UUID&);
	const UUID& addProduct(const string&,const string&,const string&,double,Units);
	const Product& getProduct(const UUID&)const;
	iterator begin();
	iterator end();
	const_iterator cbegin()const;
	const_iterator cend()const;
};





#endif
