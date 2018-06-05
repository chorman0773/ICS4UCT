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
    
    The Employee class satisfies StringConvertible (it can be implicit converted to a string),
    satisfies DefaultConstructible, CopyConstructible, MoveConstructible, CopyAssignable, and MoveAssignable
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
	/*
	    Removes a given permission from the employee's Permission list.
        After this call, it is guarenteed that a call to hasPermission() for that permisison will return false,
        unless the underlying permission set is changed, or a subsequent call to addPermission is made.
        This method is mutating and will mark the employee as "dirty".
	*/
	void removePermission(Permission);
    /*
        Copys the salt array into the passed byte array.
        This method will mutate the incoming array.
    */
	void getSalt(unsigned char(&)[32])const;
    /*
        Copys the hash array into the passed byte array.
        This method will mutate the incoming array.
    */
	void getHash(unsigned char(&)[32])const;
    /*
        Attempts to authenticate the Employee with a given password.
        If the password is invalid, this method will return AuthenticationResult::FAILED_BAD_PASSWORD.
        Otherwise, if the user does not have the Permission "AUTH", this method will return AuthenticationResult::FAILED_CANT_AUTHENTICATE
        Otherwise, the method authenticates and updates the status to ONLINE.
        If the user has the "ADMINISTRATOR" permission then the method will return AuthenticationResult::SUCCESS_ADMIN,
        otherwise the method returns AuthenticationResult::SUCCESS.
    */
	AuthenticationResult authenticate(const string&);
    /*
        Updates the status of the employee.
    */
	void setStatus(Status);
    /*
        Obtains the UUID of this employee as a const reference.
    */
	const UUID& getUUID()const;
    /*
        Obtains the current status.
    */
	Status getStatus()const;
    /*
        Obtains the pay of this employee.
    */
	double getPay()const;
    /*
        Sets the value of the pay field.
        This method will mark this employee as "dirty".
    */
	void setPay(double);
    /*
        Obtains the set of Permissions active on the employee.
        This set provides a read-only view of the permission's set.
    */
	const EnumSet<Permission>& getPermissions()const;
    /*
        Attempts to update the password of this user.
        The first (original password) is used to reauthenticate to elevate the session.
        If the original password is not valid, then the result is AuthenticationResult::FAIL_BAD_PASSWORD.
        If the original password is valid, but the new password is less than 8 characters,
        then the result is AuthenticationResult::NEW_PASSWORD_NOT_VALID.
        Otherwise, the Authentication Credentials are regenerated for the new password, 
        and the result is AuthenticationResult::PASSWORD_CHANGED.
        If the password is changed then the method mutates the employee and marks it as "dirty".
    */
	AuthenticationResult changePassword(const string&,const string&);
    /*
        Marks this employee as dirty, ie. Changed.
        If the Employee Storage System is partial-mutation based (only update the changed portions),
        then marking it as dirty will cause the system to save this employee to the peristant storage system.
        If the Employee Storage System saves all employees during a save routine, then marking an employee as dirty
        has no effect.
    */
	void markDirty();
    /*
        Marks this employee as clean, ie. unchanged.
        If the Employee Storage System is partial-mutation based (only update the change portions),
        then marking it as clean will prevent the system from saving this employee to the persistant storage system.
        If the Employee Storage System saves all employees during a save routine, then marking an employee as clean
        has no effect.
    */
	void markClean();
    /*
        Checks if the employee is dirty.
        Methods that mutate persistant variables of the employee will implicit set the dirty field.
        isDirty() will return false if since Constructing the instance or the last call to markClean(),
        if markDirty() or any methods which mark the employee as "dirty" have not been called and true otherwise.
    */
	bool isDirty()const;
    /*
        Updates the Authentication Credentials of the account to set the password associated with the account
        to the provided new password.
    */
	void setPassword(const string&);
    /*
        Compares this employee to another for equality.
        2 Employees are equal if they have the same UUID.
    */
	bool operator==(const Employee&)const;
    /*
        Compares this employee to another for inequality.
        2 Employees are not equal if they have a different UUID.
    */
	bool operator!=(const Employee&)const;
    /*
        Compares this employee to another, ordering by equality and sequence
        (Less-than-equals)
    */
	bool operator<=(const Employee&)const;
    /*
        Compares this employee to another, ordering by equality and sequence
        (Greater-than-equals)
    */
	bool operator>=(const Employee&)const;
    /*
        Compares this employee to another ordering by sequence.
        Employees are ordered by Status, then by name.
        (Less-than)
    */
	bool operator< (const Employee&)const;
    /*
        Compares this employee to another ordering by sequence.
        Employees are ordered by Status, then by name.
        (Greater-than)
    */
	bool operator> (const Employee&)const;
    /*
        Implicitly converts this employee to a string.
        This conversion operator is a convience for getName(), and exists to Satisfy StringConvertible.
    */
	operator const string&()const; 
};

/*
   The Employees class contains a list of employees for tracking and persistance.
   The class wraps a list of Employee objects.
   The Employees class extends hashable, and the hashcode is computed from the hashcode of each controlled employee instance.
   Employees Satisfies Collection, Random-Access Iterable, and List of StringConvertible Types.
*/
class Employees:public Hashable{
	map<UUID,Employee*> employeeMap;
	vector<Employee> employeeRegistry;
public:
	typedef vector<Employee>::iterator iterator;
	typedef vector<Employee>::const_iterator const_iterator;
	typedef Employee value_type;
	typedef Employee& reference;
	typedef const Employee& const_reference;
    /*
        Default constructor for employees.
    */
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
