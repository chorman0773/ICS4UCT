#ifndef __Bindings_hpp_2018_04_24_09_28
#define __Bindings_hpp_2018_04_24_09_28
#include <string>
#include <map>
#include <vector>
#include <set>
#include "Hash.hpp"
#include "UUID.hpp"
#include "EnumSet.hpp"
#include "Config.hpp"
#include <mutex>

using std::recursive_mutex;

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
	In addition, the employee class is usable in a multithreaded context. All mutation methods are synchronized.
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
	recursive_mutex lock;
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
		Copies an employee.
		The new employee will be distinct from the old employee but will have the same id.
	*/
	Employee(const Employee&);

	/*
		Employee copy-assignment.
		All fields from the passed employee will be copied, except for the employee's underlying lock.
	*/
	Employee& operator=(const Employee&);
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
	map<UUID,Employee> employeeMap;
	vector<Employee*> employeeRegistry;
	Configuration cfg;
	mutable recursive_mutex lock;
public:
	typedef vector<Employee*>::iterator iterator;
	typedef vector<Employee*>::const_iterator const_iterator;
	typedef Employee value_type;
	typedef Employee& reference;
	typedef const Employee& const_reference;
    /*
        Default constructor for employees.
    */
	Employees();
    /*
        Loads the employee list from the persistant storage.
        This method will invalidate all iterators, and References obtained from the Employees object.
    */
	void load();
    /*
        Saves the employee list to persistant storage.
        This method will not modify the state of the list
    */
	void save();
	/*
        Removes an employee from the list given its UUID.
        If the Employee with that given UUID exists then the employee is removed from the list.
        All References and Iterators to elements after the removed element are invalidated.
        If no employee with the given UUID exists, then the method has no effect.
    */
    void removeEmployee(const UUID&);
    /*
        Adds a new employee to this list.
        This method effectively combines Constructing an employee with Employee::newEmployee,
        calling addEmployee with the constructed Employee,
        and returning a reference to the new employee's UUID.
        This method does not invalidate iterators or references.
    */
	const UUID& addEmployee(const string&,double,const string&);
    /*
        Adds an employee to the list.
        This method does not invalidate iterators or references.
    */
	void addEmployee(const Employee&);
    /*
        Obtains a reference to the nth employee.
        This method only returns constant references.
        Note: By default the list of employees in unordered. Obtaining a reference using this method 
        may not produce consistant results.
        If employees are reloaded or removed from the list then this reference is invalidated.
        If there is no employee with the given index, this method will throw an exception.
    */
	const_reference getEmployee(int i)const;
    /*
        Obtains a non-constant reference to an employee based on its UUID.
        Unless the employee is removed, obtaining multiple references with the same UUID from this method will produce the same
        Employee Reference.
        If employees are reloaded or removed, then this reference is invalidated.
        If there is no employee with the given UUID, this method will throw an exception.
    */
	reference getEmployee(const UUID&);
    /*
        Obtains a constant reference to an employee based on its UUID.
        This method provides the same consistancy gaurentee as the non-const qualified version.
        If employees are reloaded or removed, then this reference is invalidated.
        If there is no employee with the given UUID, this method will throw an exception.
    */
	const_reference getEmployee(const UUID&)const;
    /*
        Obtains an iterator to the first element of this list.
        The iterator is guarenteed to satisfy RandomAccessIterator and BiDirectionalIterator.
        The iterator is invalidated if employees are reloaded or an employee is removed (after the position)
    */
	iterator begin();
    /*
        Obtains an constant iterator to the first element of this list.
        The iterator is guarenteed to satisfy RandomAccessIterator and BiDirectionalIterator.
        The iterator is invalidated if employees are reloaded or an employee is removed (after the position)
    */
	const_iterator begin()const;
    /*
        Obtains an iterator pointing to the end of this list.
        The iterator is guarenteed to satisfy RandomAccessIterator and BiDirectionalIterator.
        The iterator is invalidated if employees are reloaded or an employee is removed
    */
	iterator end();
    /*
        Obtains an constant iterator pointing to the end of this list.
        The iterator is guarenteed to satisfy RandomAccessIterator and BiDirectionalIterator.
        The iterator is invalidated if employees are reloaded or an employee is removed
    */
	const_iterator end()const;
    /*
        Obtains the length of the Employee List
    */
	int length()const;
	int hashCode()const;
    /*
        Sorts this employee list.
        This function will sort the underlying list and will invalidate all iterators and references obtained prior.
        This function should only be used if absolutely necessary.
    */
	void sort();
    /*
        Obtains a reference to an employee given by its UUID.
        This method is an alias of the equivalent getEmployee
    */
	reference operator[](const UUID&);
    /*
        Obtains a reference to an employee given by its UUID.
        This method is an alias of the equivalent getEmployee
    */
	const_reference operator[](const UUID&)const;

	vector<string> getNameVector()const;
};

/*
	Enumeration which represents a Unit.
	So far 3 units are supported: bulk, mass (kg), and volume (L).
*/
enum class Units: unsigned char{
	ammount, kg, L	
};

/*
	Class which represents a Product.
	The Product class extends Hashable, is thread-safe, and satisfies StringConvertible, DefaultConstrutible, CopyConstructible, MoveConstructible,
	CopyAssignable, and MoveAssignable.
	All mutation methods will aquire a reenterant instance specific lock.
*/
class Product:public Hashable{
	UUID productId;
	string name;
	string supplierMailingAddress;
	string supplierName;
	string supplierPhoneNumber;
	double cost;
	Units units;
	recursive_mutex lock;
public:
	/*
		Constructs a null Product.
	*/
	Product();
	/*
		Creates a new Product based on the given id, name, supplier details, cost/unit, and unit
	*/
	Product(const UUID&,const string&,const string&,const string&,const string&,double,Units);
	/*
		Copies a given product.
	*/
	Product(const Product&);
	/*
		Copies a given product.
	*/
	Product& operator=(const Product&);
	/*
		Gets the Product's UUID.
	*/
	const UUID& getUUID()const;
	/*
		Gets the product's name.
	*/
	const string& getName()const;
	/*
		Gets the mailing address of the Supplier.
	*/
	const string& getSupplierMailingAddress()const;
	/*
		Gets the name of the Supplier.
	*/
	const string& getSupplierName()const;
	/*
		Gets the phone number of the Supplier.
	*/
	const string& getSupplierPhoneNumber()const;
	/*
		Gets the cost/unit of the Product.
	*/
	double getCost()const;
	/*
		Gets the units this product uses
	*/
	Units getUnits()const;
	/*
		Computes the hashcode of this product
	*/
	int hashCode()const;
	/*
		Creates a new supply requisition for a given quantity of the product.
	*/
	void request(double quantity);
	/*
		Converts this product to a string.
		This function is effectively the same as getName(), but exists to satisfy StringConvertible.
	*/
	operator const string&()const;
};


class Products:public Hashable{
	map<UUID,Product> productMap;
	vector<Product*> products;
	Configuration cfg;
	recursive_mutex lock;
	typedef vector<Product*>::iterator iterator;
	typedef vector<Product*>::const_iterator const_iterator;
public:
	Products();
	void load();
	void save();
	void removeProduct(const UUID&);
	void addProduct(const Product&);
	const UUID& addProduct(const string&,const string&,const string&,const string&,double,Units);
	const Product& getProduct(const UUID&)const;
	Product& getProduct(const UUID&);
	const Product& getProduct(int)const;
	Product& getProduct(int);
	Product& operator[](const UUID&);
	const Product& operator[](const UUID&)const;
	iterator begin();
	iterator end();
	const_iterator begin()const;
	const_iterator end()const;
	vector<string> getNameVector()const;
};

class OrderItem:public Hashable{
private:
	UUID p;
	double ammount;
	bool Void;
	recursive_mutex lock;
public:
	OrderItem();
	OrderItem(const Product&,double,bool=false);
	OrderItem(const OrderItem&);
	OrderItem& operator=(const OrderItem&);
	int hashCode()const;
	const UUID& getProduct()const;
	double getTotalAmmount()const;
	bool isVoid()const;
};

class Order:public Hashable{
private:
	UUID fillerEmployee;
	vector<OrderItem> items;
	typedef vector<OrderItem>::const_iterator iterator;
public:
	/*
		Constructs an empty order
		The order cannot be used because it does not have a valid employee assigned
	*/
	Order();
	Order(const Employee&);
	iterator begin()const;
	iterator end()const;
	void addItem(const Product&,double);
	void voidItem(const Product&,double);
	double getTotal()const;
	void finish();
	int hashCode()const;
};

/*
	Logs a particular action if the configuration enables it.
*/
void log(AuditAction,const UUID&,const string&,const string&);



#endif
