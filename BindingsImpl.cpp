#include "Bindings.hpp"
#include <SHA256.hpp>
#include <SecureRandom.hpp>
#include <Quicksort.hpp>

#include <sqlite3/sqlite3.h>
#include <UUID.hpp>
#include <Hash.hpp>
#include <random>
#include <Base64.hpp>
#include <JTime.hpp>

#include <cstring>

#include <json/json.h>

#include <fstream>

#include <Config.hpp>

using std::mutex;

/*
	Global Mutexes for files (when Concurrency is a thing).
*/
mutex logMtx;
mutex requisitionMutex;
mutex employeesMutex;
mutex productsMutex;
mutex recieptsMutex;

using std::random_device;

SecureRandom employeeRandom;

void log(AuditAction action,const UUID& employee,const string& name,const string& actionString){
	std::lock_guard<mutex> lock(logMtx);
	Configuration cfg;
	if(cfg.isAuditAction(action)){
		std::ofstream strm(cfg.getFile(FileGroup::AuditLog),std::ios::app);
		strm << name << "(" <<employee<<")"<<actionString<<std::endl;
	}
}

unsigned char* saltPwd(const unsigned char* pwd,unsigned int size,const unsigned char (&salt)[32]){
  int outSize = size+32-(size%32);
  unsigned char* ret = new unsigned char[outSize];
  for(int i = 0;i<outSize;i++)
    ret[i] = pwd[i%size]^salt[i%32];
  return ret;
}

Employee::Employee(){}
Employee::Employee(const string& name,const UUID& id,double salary,const unsigned char (&salt)[32],const unsigned char (&hash)[32],
  const EnumSet<Permission>& permissions):name(name),id(id),salary(salary),permissions(permissions),s(Status::OFFLINE),dirty(false){
	  memcpy(this->salt,salt,32);
	  memcpy(this->hash,hash,32);
}

Employee::Employee(const Employee& e):name(e.name),id(e.id),salary(e.salary),permissions(e.permissions),s(e.s),dirty(e.dirty){
	memcpy(this->salt,e.salt,32);
	memcpy(this->hash,e.hash,32);
}

Employee& Employee::operator=(const Employee& e){
	std::lock_guard<recursive_mutex> sync(lock);
	if(&e==this)
		return *this;
	name = e.name;
	id = e.id;
	salary = e.salary;
	permissions = e.permissions;
	s = e.s;
	dirty = e.dirty;
	memcpy(this->salt,e.salt,32);
	memcpy(this->hash,e.hash,32);
	return *this;
}

Employee Employee::newEmployee(const string& name,double salary,const string& pwd){
	unsigned char salt[32];
	unsigned char hash[32];
	char* saltedPwd;
	UUID id = UUID::ofNow();
	int outSize = name.length()+32-(name.length()%32);
	employeeRandom.nextBytes(salt,32);
	saltedPwd =(char*) saltPwd((const unsigned char*)pwd.c_str(),pwd.length(),salt);
	SHA256(saltedPwd,outSize,(char(&)[32])hash);
	delete[] saltedPwd; 
	return Employee(name,id,salary,salt,hash,EnumSet<Permission>(Permission::AUTH));
}

int32_t Employee::hashCode()const{
  int32_t hash = 0;
  const int32_t prime = 31;
  hash += hashcode(name);
  hash *= prime;
  hash += hashcode(id);
  hash *= prime;
  hash += hashcode(salary);
  return hash;
}

void Employee::setStatus(Status s){
	std::lock_guard<recursive_mutex> sync(lock);
	this->s = s;
}

const string& Employee::getName()const{
  return name;
}

bool Employee::hasPermission(Permission p)const{
  return permissions.contains(p);
}

void Employee::addPermission(Permission p){
  std::lock_guard<recursive_mutex> sync(lock);
  markDirty();
  permissions.add(p);
}

void Employee::removePermission(Permission p){
  std::lock_guard<recursive_mutex> sync(lock);
  markDirty();
  permissions.remove(p);
}

void Employee::getSalt(unsigned char(&out)[32])const{
  memcpy(out,this->salt,32);
}

void Employee::getHash(unsigned char(&out)[32])const{
  memcpy(out,this->hash,32);
}

AuthenticationResult Employee::authenticate(const string& s){
  std::lock_guard<recursive_mutex> sync(lock);
  int outSize = name.length()+32-(name.length()%32);
  char buffer[32];
  
  char* saltedPwd =(char*) saltPwd((const unsigned char*) s.c_str(),s.length(),salt);
  SHA256(saltedPwd,outSize,buffer);
  delete[] saltedPwd;
  if(memcmp(hash,buffer,32)!=0)
    return AuthenticationResult::FAIL_BAD_PASSWORD;
  else if(!permissions.contains(Permission::AUTH))
    return AuthenticationResult::FAIL_CANT_AUTHENTICATE;
  else if(permissions.contains(Permission::ADMINISTRATOR))
    return AuthenticationResult::SUCCESS_ADMIN;
  else
    return AuthenticationResult::SUCCESS;
}

void Employee::setPassword(const string& pwd){
  std::lock_guard<recursive_mutex> sync(lock);
  unsigned char salt[32];
  char hash[32];
  char* saltedPwd;
  UUID id = UUID::ofNow();
  int outSize = name.length()+32-(name.length()%32);
  employeeRandom.nextBytes(salt,32);
  saltedPwd =(char*) saltPwd((const unsigned char*)pwd.c_str(),pwd.length(),salt);
  SHA256(saltedPwd,outSize,hash);
  memcpy(this->salt,salt,32);
  memcpy(this->hash,hash,32);
  markDirty();
}

AuthenticationResult Employee::changePassword(const string& currPwd,const string& newPwd){
  std::lock_guard<recursive_mutex> sync(lock);
  if(authenticate(currPwd)==AuthenticationResult::FAIL_BAD_PASSWORD)
    return AuthenticationResult::FAIL_BAD_PASSWORD;
  else if(newPwd.length()<8)
    return AuthenticationResult::NEW_PASSWORD_NOT_VALID;
  setPassword(newPwd);
  return AuthenticationResult::PASSWORD_CHANGED;
}

const UUID& Employee::getUUID()const{
 return id; 
}

Status Employee::getStatus()const{
 return s; 
}

double Employee::getPay()const{
 return salary; 
}

const EnumSet<Permission>& Employee::getPermissions()const{
 return permissions; 
}

void Employee::setPay(double salary){
  std::lock_guard<recursive_mutex> sync(lock);
  this->salary = salary; 
  markDirty();
}

bool Employee::operator==(const Employee& o)const{
 return id==o.id; 
}

bool Employee::operator!=(const Employee& o)const{
 return id!=o.id; 
}

bool Employee::operator< (const Employee& o)const{
 return s<o.s||(s==o.s&&name<o.name); 
}

bool Employee::operator> (const Employee& o)const{
 return s>o.s||(s==o.s&&name>o.name); 
}

bool Employee::operator>=(const Employee& o)const{
 return *this>o||*this==o; 
}

bool Employee::operator<=(const Employee& o)const{
 return *this<o||*this==o; 
}

void Employee::markDirty(){
  std::lock_guard<recursive_mutex> sync(lock);
  dirty = true; 
}
void Employee::markClean(){
  std::lock_guard<recursive_mutex> sync(lock);
  dirty = false; 
}

bool Employee::isDirty()const{
 return dirty; 
}

Employee::operator const std::string&()const{
	return name;
}

Employees::Employees(){}


char hexmap[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

unsigned char fromHexByte(const char* str){
	unsigned char out = 0;
	const char& high = str[0];
	const char& low = str[1];
	if(high>='0'&&high<='9')
		out |= high-'0';
	else if(high>='A'&&high<='F')
		out |= (high-'A')+10;
	out <<= 4;
	if(low>='0'&&low<='9')
		out |= low-'0';
	else if(low>='A'&&low<='F')
		out |= (low-'A')+10;
	return out;
}

void Employees::load(){
	std::lock_guard<recursive_mutex> sync(lock);
	std::lock_guard<mutex> lock(employeesMutex);
	Json::Value employees;
	std::ifstream employeeFile(cfg.getFile(FileGroup::EmployeeList));
	if(!employeeFile.is_open())
		return;
	HashValidationMode validationMode = cfg.getValidationMode(FileGroup::EmployeeList);
	employeeFile >> employees;
	employeeFile.close();
	Json::Value list = employees["list"];
	for(const Json::Value& state:list){
		bool hasHash = false;
		int32_t hashCode;
		Json::Value hashJson = state["hashcode"];
		if(validationMode!=HashValidationMode::None)
			hasHash = !hashJson.isNull();
		if(hasHash)
			hashCode = int(hashJson.asLargestInt());
		else if(validationMode==HashValidationMode::Strict)
			continue; //If No hashcode availble and we are in strict mode, then ignore the employee.
		unsigned char salt[32];
		unsigned char hash[32];
		string xsalt = state["salt"].asString();
		for(int i = 0;i<32;i++)
			salt[i] = fromHexByte(&xsalt[2*i]);
		string xhash = state["hash"].asString();
		for(int i = 0;i<32;i++)
			hash[i] = fromHexByte(&xhash[2*i]);
		UUID id = state["uuid"].asString();
		string name = state["name"].asString();
		double pay = state["pay"].asDouble();
		EnumSet<Permission> permissions(state["permissions"].asLargestUInt());
		Employee e(name,id,pay,salt,hash,permissions);
		if(hasHash)
			if(hashCode!=e.hashCode())
				continue;//If hashcode is stored, we are validating hashes, and there is a hashcode mismatch, then ignore the employee
		addEmployee(e);
	}
}
void Employees::save(){
	std::lock_guard<recursive_mutex> sync(lock);
	std::lock_guard<mutex> lock(employeesMutex);
	Json::Value employees(Json::objectValue);
	Json::Value employeeList(Json::arrayValue);
	for(const Employee* e:this->employeeRegistry){
		Json::Value state(Json::objectValue);
		state["uuid"]=e->getUUID().toString();
		unsigned char salt[32];
		unsigned char hash[32];
		e->getSalt(salt);
		e->getHash(hash);
		string xsalt;
		for(unsigned char c:salt){
			xsalt += hexmap[c>>4];
			xsalt += hexmap[c&0xf];
		}
		string xhash;
		for(unsigned char c:hash){
			xhash += hexmap[c>>4];
			xhash += hexmap[c&0xf];
		}
		state["salt"] = xsalt;
		state["hash"] = xhash;
		state["name"] = e->getName();
		state["pay"] = e->getPay();
		state["permissions"] = e->getPermissions().toMap();
		state["hashCode"] = e->hashCode();
		employeeList.append(state);
	}
	employees["list"] = employeeList;
	std::ofstream employeeFile(cfg.getFile(FileGroup::EmployeeList));
	employeeFile << employees;
	employeeFile.close();
}

void Employees::removeEmployee(const UUID& u){
	std::lock_guard<recursive_mutex> sync(lock);
	employeeMap.erase(u);
	for(int i = 0;i<employeeRegistry.size();i++){
		if(employeeRegistry[i]->getUUID()==u){
			iterator itr = employeeRegistry.begin();
			itr+=i;
			employeeRegistry.erase(itr);
			break;
		}
	}
}

const UUID& Employees::addEmployee(const string& name,double salary,const string& pwd){
  std::lock_guard<recursive_mutex> sync(lock);
  Employee toAdd = Employee::newEmployee(name,salary,pwd);
  addEmployee(toAdd);
  return getEmployee(toAdd.getUUID()).getUUID();
}

void Employees::addEmployee(const Employee& e){
	std::lock_guard<recursive_mutex> sync(lock);
	employeeMap[e.getUUID()] = e;
	employeeRegistry.push_back(&employeeMap[e.getUUID()]);
}

Employees::reference Employees::getEmployee(const UUID& u){
 return employeeMap.at(u); 
}

Employees::const_reference Employees::getEmployee(const UUID& u)const{
	return employeeMap.at(u);
}

Employees::reference Employees::operator[](const UUID& u){
	return employeeMap.at(u);
}

Employees::const_reference Employees::operator[](const UUID& u)const{
	return employeeMap.at(u);
}

Employees::iterator Employees::begin(){
 return employeeRegistry.begin(); 
}

Employees::const_iterator Employees::begin()const{
 return employeeRegistry.begin(); 
}

Employees::iterator Employees::end(){
 return employeeRegistry.end(); 
}

Employees::const_iterator Employees::end()const{
 return employeeRegistry.end(); 
}

int Employees::length()const{
 return employeeRegistry.size(); 
}

int Employees::hashCode()const{
  int hash = 0;
  const int prime = 31;
  for(const Employee* e:employeeRegistry){
   hash *= prime;
   hash += e->hashCode();
  }
  return hash;
}

Employees::const_reference Employees::getEmployee(int i)const{
	return **(begin()+i);
}

void Employees::sort(){
/*
	quicksort(this->employeeRegistry);
	this->employeeMap.clear();
	for(Employee& e:employeeRegistry)
		employeeMap[e.getUUID()] = &e;
*/
}

const char configDefault[] = "{\"files\":{\n\t\"employees\":\"employees.json\",\n\t\"audits\":\"audits.json\",\n\t"\
"\"products\":\"products.json\",\n\t\"requisitions\":\"requisitions.json\",\n\t\"reciepts\":\"reciepts.json\"},"\
"\"audit-actions\":{\n\t\"authenticate\":false,\t\n\"authenticate-admin\":true,\n\t\"manage-accounts\":true,\n\t\"change-own-password\":true,"\
"\n\t\"manage-products\":true,\n\t\"make-requistion\":true,\n\t\"fill-requistion\":true,n\t\"enter-reciept\":true\n},"\
"\n\t\"hash-validation\":{\n\t\"employees\":\"strict\",\n\t\"products\":\"strict\",\n\t\"audits\":\"none\",\n\t\"requistions\":\"strict\","\
"\n\t\"reciepts\":\"strict\"\n\t}\n}";

const char defaultAdminName[] = "sysadmin";
const char defaultAdminPassword[] = "password";

using std::ofstream;
using std::ifstream;

void checkInstallation(){
	ofstream out;
	Configuration cfg;
	if(cfg.getConfig().isNull()){
		out = ofstream(CFG_FILE);
		out << configDefault;
		out.close();
	}
	Employees e;
	e.load();
	if(e.length()==0){
		UUID u = e.addEmployee(string(defaultAdminName),0.0,string(defaultAdminPassword));
		Employee& target = e[u];
		target.addPermission(Permission::ADMINISTRATOR);
		e.save();
	}
}

Product::Product(){}
Product::Product(const UUID& id,const string& name,const string& supplierName,const string& supplierMailingAddress,const string& supplierPhoneNumber,double cost,Units units):
	productId(id),name(name),supplierName(supplierName),supplierMailingAddress(supplierMailingAddress),supplierPhoneNumber(supplierPhoneNumber),cost(cost),units(units){}

Product::Product(const Product& other):productId(other.productId),name(other.name),supplierName(other.supplierName),
	supplierMailingAddress(other.supplierMailingAddress),supplierPhoneNumber(other.supplierPhoneNumber),cost(other.cost),units(other.units){}

Product& Product::operator=(const Product& other){
	std::lock_guard<recursive_mutex> sync(lock);
	if(&other==this)
		return *this;
	productId = other.productId;
	name = other.name;
	supplierName = other.supplierName;
	supplierMailingAddress = other.supplierMailingAddress;
	cost = other.cost;
	units = other.units;
	return *this;
}

const UUID& Product::getUUID()const{
	return productId;
}

const string& Product::getName()const{
	return name;
}

const string& Product::getSupplierName()const{
	return supplierName;
}

const string& Product::getSupplierMailingAddress()const{
	return supplierMailingAddress;
}

const string& Product::getSupplierPhoneNumber()const{
	return supplierMailingAddress;
}

double Product::getCost()const{
	return cost;
}

Units Product::getUnits()const{
	return units;
}

string toString(Units unit){
	switch(unit){
	case Units::L:
		return "L";
	break;
	case Units::kg:
		return "kg";
	break;
	case Units::ammount:
		return "bulk";
	break;
	}
}

Units fromString(const string& str){
	if(str=="L")
		return Units::L;
	else if(str=="kg")
		return Units::kg;
	else
		return Units::ammount;
}

void Product::request(double ammount){
	std::lock_guard<recursive_mutex> sync(lock);
	std::lock_guard<mutex> requisitionSync(requisitionMutex);
	Configuration cfg;
	Json::Value requisitions(Json::objectValue);
	Json::Value requisitionList;
	ifstream requisitionFile(cfg.getFile(FileGroup::Requisitions));
	if(requisitionFile.is_open()){
		requisitionFile >> requisitions;
		requisitionList = requisitions["list"];
		requisitionFile.close();
	}
	if(requisitionList.isNull())
		requisitionList = Json::arrayValue;
	Json::Value requisition(Json::objectValue);
	requisition["productId"] = string(this->productId);
	requisition["productName"] = this->name;
	Json::Value supplier(Json::objectValue);
	supplier["name"] = supplierName;
	supplier["mailingAddress"] = supplierMailingAddress;
	supplier["phoneNumber"] = supplierPhoneNumber;
	requisition["supplier"] = supplier;
	requisition["units"] = toString(units);
	requisition["ammount"] = ammount;
	requisitionList.append(requisition);
	requisitions["list"] = requisitionList;
	ofstream output(cfg.getFile(FileGroup::Requisitions));
	output << requisitions;
	output.close();
}

void Products::load(){
	std::lock_guard<recursive_mutex> sync(lock);
	std::lock_guard<mutex> lockFile(productsMutex);
	HashValidationMode validationMode = cfg.getValidationMode(FileGroup::ProductList);
	ifstream file(cfg.getFile(FileGroup::ProductList));
	if(file.is_open()){
		Json::Value products;
		file >> products;
		Json::Value list = products["list"];
		for(const Json::Value& product:list){
			bool hasHash = false;
			int32_t hashcode;
			if(validationMode!=HashValidationMode::None){
				Json::Value hash = product["hashcode"];
				if(!hash.isNull()){
					hasHash = true;
					hashcode = hash.asInt();
				}
			}
			if(validationMode==HashValidationMode::Strict&&!hasHash)
				continue;
			
			UUID id = product["uuid"].asString();
			string name = product["name"].asString();
			string supplierName = product["supplierName"].asString();
			string supplierPhoneNumber = product["supplierPhoneNumber"].asString();
			string supplierMailingAddress = product["supplierMailingAddress"].asString();
			double cost = product["cost"].asDouble();
			Units units = fromString(product["units"].asString());
			Product p(id,name,supplierName,supplierMailingAddress,supplierPhoneNumber,cost,units);
			if(!hasHash||p.hashCode()==hashcode)
				addProduct(p);
		}
		file.close();
	}
}

void Products::save(){
	std::lock_guard<recursive_mutex> sync(lock);
	std::lock_guard<mutex> lockFile(productsMutex);
	Json::Value products(Json::objectValue);
	products["list"] = Json::arrayValue;
	Json::Value& list = products["list"];
	for(const Product* product:*this){
		Json::Value val(Json::objectValue);
		val["uuid"] = string(product->getUUID());
		val["name"] = product->getName();
		val["supplierName"] = product->getSupplierName();
		val["supplierMailingAddress"] = product->getSupplierMailingAddress();
		val["supplierPhoneNumber"] = product->getSupplierPhoneNumber();
		val["cost"] = product->getCost();
		val["units"] = toString(product->getUnits());
		list.append(val);
	}
	ofstream file("products.json");
	file << products;
	file.close();
}

void Products::removeProduct(const UUID& u){
	productMap.erase(u);
	for(int i = 0;i<products.size();i++){
		if(products[i]->getUUID()==u){
			products.erase(products.begin()+i,products.begin()+i);
			break;
		}
	}	
}

void Products::addProduct(const Product& p){
	std::lock_guard<recursive_mutex> sync(lock);
	productMap[p.getUUID()] = p;
	products.push_back(&productMap[p.getUUID()]);
}

const UUID& Products::addProduct(const string& name,const string& supplierName,const string& supplierMailingAddress,const string& supplierPhoneNumber,double cost,Units units){
	Product p(UUID::ofNow(),name,supplierName,supplierMailingAddress,supplierPhoneNumber,cost,units);
	addProduct(p);
	return p.getUUID();
}

const Product& Products::getProduct(const UUID& u)const{
	return productMap.at(u);
}

Product& Products::getProduct(const UUID& u){
	return productMap.at(u);
}

const Product& Products::getProduct(int i)const{
	return *products[i];
}
Product& Products::getProduct(int i){
	return *products[i];
}

Products::iterator Products::begin(){
	return products.begin();
}

Products::const_iterator Products::begin()const{
	return products.begin();
}

Products::iterator Products::end(){
	return products.end();
}

Products::const_iterator Products::end()const{
	return products.end();
}

OrderItem::OrderItem(){}
OrderItem::OrderItem(const Product& p,double ammount,bool Void):p(p.getUUID()),ammount(ammount*p.getCost()),Void(Void){}
OrderItem::OrderItem(const OrderItem& o):p(o.p),ammount(o.ammount),Void(o.Void){}
OrderItem& OrderItem::operator=(const OrderItem& o){
	std::lock_guard<recursive_mutex> sync(lock);
	p = o.p;
	ammount = o.ammount;
	Void = o.Void;
	return *this;
}

int OrderItem::hashCode()const{
	int hash = 0;
	const int prime = 31;
	hash = hashcode(p);
	hash *= prime;
	hash += hashcode(ammount);
	hash *= prime;
	hash += hashcode(Void);
	return hash;
}

const UUID& OrderItem::getProduct()const{
	return p;
}

double OrderItem::getTotalAmmount()const{
	return Void?-ammount:ammount;
}

bool OrderItem::isVoid()const{
	return Void;
}

Order::Order(){}
Order::Order(const Employee& e):fillerEmployee(e.getUUID()){}

Order::iterator Order::begin()const{
	return items.begin();
}
Order::iterator Order::end()const{
	return items.end();
}

double Order::getTotal()const{
	double total(0.0);
	for(const OrderItem& i:*this)
		total += i.getTotalAmmount();
	return total;
}

int Order::hashCode()const{
	int32_t hash = 0;
	const int32_t prime = 31;
	for(const OrderItem& i:*this){
		hash *= prime;
		hash += i.hashCode();
	}
	return hash;
}

void Order::addItem(const Product& p,double ammount){
	items.push_back(OrderItem(p,ammount,false));
}
void Order::voidItem(const Product& p,double ammount){
	items.push_back(OrderItem(p,ammount,true));
}

void Order::finish(){
	std::lock_guard<mutex> lock(recieptsMutex);
	Configuration cfg;
	ofstream output(cfg.getFile(FileGroup::Reciepts),std::ios::app);
	output <<"Order: " << fillerEmployee << " total was "<<getTotal()<<std::endl;
}