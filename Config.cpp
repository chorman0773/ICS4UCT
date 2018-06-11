#include "Config.hpp"
#include <json/json.h>
#include <string>
#include <fstream>
#include <istream>
#include <cstdlib>


using std::string;
using std::ifstream;
using std::ios;
string getJsonString(AuditAction a){
	switch(a){
	case AuditAction::Authenticate:
		return "authenticate";
	break;
	case AuditAction::AuthenticateAdmin:
		return "authenticate-admin";
	break;
	case AuditAction::ManageAccounts:
		return "manage-accounts";
	break;
	case AuditAction::ChangeOwnPassword:
		return "change-own-password";
	break;
	case AuditAction::ChangeOtherPassword:
		return "change-other-password";
	break;
	case AuditAction::ManageProducts:
		return "manage-products";
	break;
	case AuditAction::MakeRequisition:
		return "make-requisiton";
	break;
	case AuditAction::FillRequisition:
		return "fill-requisition";
	break;
	case AuditAction::EnterReciept:
		return "enter-reciept";
	break;
	}
	throw"Bad Enum Constant";
}
string getJsonString(FileGroup f){
	switch(f){
	case FileGroup::EmployeeList:
		return "employees";
	break;
	case FileGroup::ProductList:
		return "products";
	break;
	case FileGroup::AuditLog:
		return "audits";
	break;
	case FileGroup::Requisitions:
		return "requisitions";
	break;
	case FileGroup::Reciepts:
		return "reciepts";
	break;
	}
	throw"Bad Enum Constant";
}

HashValidationMode getValidationMode(const string& str){
	if(str=="strict")
		return HashValidationMode::Strict;
	else if(str=="week")
		return HashValidationMode::Loose;
	else
		return HashValidationMode::None;
}

string getExecutablePath(){
    return getenv("PWD");
}

string readFully(ifstream& strm){
    string ret;
    string tmp;
    while(strm.eof()){
        std::getline(strm,tmp);
        ret += tmp;
    }
    return ret;
}

Configuration::Configuration(){
    reload();
}

void Configuration::reload(){
    ifstream strm;
    strm.open(CFG_FILE,ios::in);
	if(!strm.is_open())
		cfg=Json::nullValue;
    strm >> cfg;
}

const Json::Value& Configuration::getConfig()const{
    return cfg;
}

string Configuration::getFile(FileGroup g)const{
	Json::Value files = cfg["files"];
	if(files.isNull())
		throw "Failed to get the file list";
	Json::Value f = files[getJsonString(g)];
	if(f.isNull())
		throw "Failed to get the requested file file";
	return f.asString();
}

bool Configuration::isAuditAction(AuditAction a)const{
	Json::Value auditActions = cfg["audit-actions"];
	if(auditActions.isNull())
		throw "Failed to get audit action list";
	Json::Value action = auditActions[getJsonString(a)];
	if(action.isNull()||!action.asBool())
		return false;
	return true;
}

HashValidationMode Configuration::getValidationMode(FileGroup g)const{
	Json::Value hashValidation = cfg["hash-validation"];
	if(hashValidation.isNull())
		throw "Failed to get hash-validation list";
	Json::Value type = hashValidation[getJsonString(g)];
	if(type.isNull())
		return HashValidationMode::None;
	return ::getValidationMode(type.asString());
}



