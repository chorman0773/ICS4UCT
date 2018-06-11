#include "Bindings.hpp"
#include <SHA256.hpp>
#include <SecureRandom.hpp>
#include <Quicksort.hpp>

#include <sqlite3/sqlite3.h>
#include <UUID.hpp>
#include <Hash.hpp>
#include <random>
#include <Base64.hpp>

#include <cstring>

#include <json/json.h>

#include <fstream>

#include <Config.hpp>

using std::random_device;

SecureRandom employeeRandom;


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
	this->s = s;
}

const string& Employee::getName()const{
  return name;
}

bool Employee::hasPermission(Permission p)const{
  return permissions.contains(p);
}

void Employee::addPermission(Permission p){
  markDirty();
  permissions.add(p);
}

void Employee::removePermission(Permission p){
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
 dirty = true; 
}
void Employee::markClean(){
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
	Json::Value employees(Json::objectValue);
	Json::Value employeeList(Json::arrayValue);
	for(const Employee& e:this->employeeRegistry){
		Json::Value state(Json::objectValue);
		state["uuid"]=e.getUUID().toString();
		unsigned char salt[32];
		unsigned char hash[32];
		e.getSalt(salt);
		e.getHash(hash);
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
		state["name"] = e.getName();
		state["pay"] = e.getPay();
		state["permissions"] = e.getPermissions().toMap();
		state["hashCode"] = e.hashCode();
		employeeList.append(state);
	}
	employees["list"] = employeeList;
	std::ofstream employeeFile(cfg.getFile(FileGroup::EmployeeList));
	employeeFile << employees;
	employeeFile.close();
}

void Employees::removeEmployee(const UUID& u){
	const Employee& e = *employeeMap[u];
	employeeMap.erase(u);
	for(int i = 0;i<employeeRegistry.size();i++){
		if(employeeRegistry[i]==e){
			iterator itr = employeeRegistry.begin();
			itr+=i;
			employeeRegistry.erase(itr);
			break;
		}
	}
}

const UUID& Employees::addEmployee(const string& name,double salary,const string& pwd){
 Employee toAdd = Employee::newEmployee(name,salary,pwd);
 addEmployee(toAdd);
 return getEmployee(toAdd.getUUID()).getUUID();
}

void Employees::addEmployee(const Employee& e){
	employeeRegistry.push_back(e);
	Employee& target = employeeRegistry.back();
	employeeMap[target.getUUID()] = &target;
}

Employees::reference Employees::getEmployee(const UUID& u){
 return *employeeMap.at(u); 
}

Employees::const_reference Employees::getEmployee(const UUID& u)const{
	return *employeeMap.at(u);
}

Employees::reference Employees::operator[](const UUID& u){
	return *employeeMap.at(u);
}

Employees::const_reference Employees::operator[](const UUID& u)const{
	return *employeeMap.at(u);
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
  for(const Employee& e:employeeRegistry){
   hash *= prime;
   hash += e.hashCode();
  }
  return hash;
}

Employees::const_reference Employees::getEmployee(int i)const{
	return *(begin()+i);
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
