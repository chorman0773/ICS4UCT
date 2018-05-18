#include "Config.hpp"
#include <json/json.h>
#include <string>
#include <fstream>
#include <istream>
#include <cstdlib>


using std::string;
using std::ifstream;
using std::ios;

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
		throw"Could not open database, file does not exist";
    strm >> cfg;
}

const Json::Value& Configuration::getConfig()const{
    return cfg;
}

string Configuration::getHostname()const{
    Json::Value host = cfg["host"];
    if(host.isNull())
        throw"Host block not found";
    Json::Value name = host["name"];
    if(name.isNull())
        throw"Host name not found";
    return name.asString();
}

int Configuration::getPort()const{
    Json::Value host = cfg["host"];
    if(host.isNull())
        throw"Host block not found";
    Json::Value port = host["port"];
    if(port.isNull())
        throw"Port not found";
	return port.asInt();
};

string Configuration::getDatabaseRef()const{
    Json::Value path = cfg["path"];
    if(path.isNull())
        throw"Database path not found";
    Json::Value database = path["db"];
    if(database.isNull())
        throw"Database path not found";
    string target = "sqlite:"+database.asString();
    return target;
}

string Configuration::getPrivateKeyFile(const UUID& id)const{
    Json::Value path = cfg["path"];
    if(path.isNull())
        throw"Users Directory not found";
    Json::Value userPath = path["users"];
    if(userPath.isNull())
        throw"Users Directory not found";
	string privkeyPath = userPath.asString()+"/PRIVKEY-"+id;
    return privkeyPath;
}