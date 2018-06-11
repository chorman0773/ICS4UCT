#ifndef __Config_hpp_2018_04_25_09_25
#define __Config_hpp_2018_04_25_09_25


#ifndef CFG_FILE
#ifdef _SERVER
#define CFG_FILE "server-config.json"
#else
#define CFG_FILE "client-config.json"
#endif
#endif
#include <json/json.h>
#include <string>
#include "UUID.hpp"

using std::string;

enum class AuditAction{
	Authenticate,AuthenticateAdmin,ManageAccounts,ChangeOwnPassword,
	ChangeOtherPassword,ManageProducts,MakeRequisition,FillRequisition,EnterReciept
};

enum class FileGroup{
	EmployeeList,ProductList,Requisitions,AuditLog,Reciepts
};

enum class HashValidationMode{
	None, Loose, Strict
};

class Configuration{
    Json::Value cfg;
public:
    Configuration();
    void reload();
    const Json::Value& getConfig()const;
    string getFile(FileGroup g)const;
	bool isAuditAction(AuditAction a)const;
	HashValidationMode getValidationMode(FileGroup g)const;
};


#endif
