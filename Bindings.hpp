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

/*
    Enumeration describing a Permission. Currently only 2 permissions are used: AUTH -> Which enables and disables accounts,
    and Administrator which enables Elevation.
*/
enum class Permission{
	AUTH, ADD_EMPLOYEE, MODIFY_EMPLOYEES, UPDATE_INFO, DELETE_EMPLOYEES,
	ADMINISTRATOR
};

enum class Status{
	OFFLINE = 0, AWAY = 1, ONLINE = 2
};

/*
    Enum describing results of Authentication.
*/
enum class AuthenticationResult{
	FAIL_BAD_PASSWORD, FAIL_CANT_AUTHENTICATE, SUCCESS, SUCCESS_ADMIN,
	PASSWORD_CHANGED, NEW_PASSWORD_NOT_VALID
};

/*
    Class which describes an employee in the company.
    The class contains the Employee's Name, unique Identifier, pay,
    SHA-256 hashed authentication Credentials, permission list, and status.
    Employees extend the Abstract Hashable class to implement the hashcode feature.
*/
class Employee : public Hashable{
	string name;
	UUID id;
	double salary;
	unsigned char salt[32];
	unsigned char hash[32];
	EnumSet<Permission> permissions;
	Status s;
	bool dirty;
public:
    /*
        Default Constructor for Employee class.
        This constructor will initialize the id to NIL and name to an empty string.
    */
	Employee();
    /*
        Initialization Constructor for the Employee Class.
        this class initializes all non-transient fields of the Employee class to the provided values and sets the transient fields
        to their defaults (Status is set to OFFLINE and dirty is set to false).
        In general this Constructor should not be directly called and should only be called by Employees.load() and
        newEmployee()
    */
	Employee(const string&,const UUID&,double,const unsigned char(&)[32],const unsigned char(&)[32],const EnumSet<Permission>&);
    /*
        Factory Helper Method to create a new employee with a given name, pay, and password.
        The new Employee will be assigned a Unique Id which is guarenteed (with Overwealming probability) of being different
        from any other UUID generated. The UUID follows RFC 4122 for Version 1, Variant 2 Time based UUIDs with a random node:
        See https://en.wikipedia.org/wiki/Universally_unique_identifier for details about this type of UUID.
    */
	static Employee newEmployee(const string&,double,const string&);
    /*
        Computes the hashcode of the employee.
        The hashcode is computed using the name, pay, and UUID of the Employee
    */
	int32_t hashCode()const;
    /*
        Obtains a constant reference to the name of the employee.
    */
	const string& getName()const;
    /*
        Checks if the given permission exists on the employee.
        This call is effectively the same as getPermissions().contains(Permission)
    */
	bool hasPermission(Permission)const;
    /*
        Adds a given permission to the employee's Permission list.
        After this call, it is guarenteed that a call to hasPermission() for that permission will return true,
        unless the underlying permission set is changed, or a subsequent call to removePermission is made.
        This method is mutating and will mark the employee as "dirty".
    */
	void addPermission(Permission);
	void removePermission(Permission);
	void getSalt(unsigned char(&)[32])const;
	void getHash(unsigned char(&)[32])const;
	AuthenticationResult authenticate(const string&);
	void setStatus(Status);
	const UUID& getUUID()const;
	Status getStatus()const;
	double getPay()const;
	void setPay(double);
	const EnumSet<Permission>& getPermissions()const;
	AuthenticationResult changePassword(const string&,const string&);
	void markDirty();
	void markClean();
	bool isDirty()const;
	void setPassword(const string&);
	bool operator==(const Employee&)const;
	bool operator!=(const Employee&)const;
	bool operator<=(const Employee&)const;
	bool operator>=(const Employee&)const;
	bool operator< (const Employee&)const;
	bool operator> (const Employee&)const;
	operator const string&()const; 
	/*
		Employee e;
		string str = e;
	*/
};

class Employees:public Hashable{
	map<UUID,Employee*> employeeMap;
	vector<Employee> employeeRegistry;
	vector<UUID> employeesToDelete;
	vector<UUID> employeesToAdd;
public:
	typedef vector<Employee>::iterator iterator;
	typedef vector<Employee>::const_iterator const_iterator;
	typedef Employee value_type;
	typedef Employee& reference;
	typedef const Employee& const_reference;
	Employees();
	void load();
	void save();
	void removeEmployee(const UUID&);
	const UUID& addEmployee(const string&,double,const string&);
	void addEmployee(const Employee&);
	const_reference getEmployee(int i)const;
	reference getEmployee(const UUID&);
	const_reference getEmployee(const UUID&)const;
	iterator begin();
	const_iterator begin()const;
	iterator end();
	const_iterator end()const;
	int length()const;
	int hashCode()const;
	void sort();
	reference operator[](const UUID&);
	const_reference operator[](const UUID&)const;
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
