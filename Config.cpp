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
    return getenv("__APPDIR__");
}

string readFully(ifstream& strm){
    string ret;
    string tmp
    while(strm.eof()){
        strm.getline(tmp);
        ret += tmp;
    }
    return ret;
}

Configuration::Configuration(){
    reload();
}

void Configuration::reload(){
    ifstream strm;
    strm.open(getExecutablePath()+CFG_FILE,ios::read);
    string json = readFully(strm);
    if(!parse(json,cfg))
        throw"Failed to initialize configuration, json parsing failed";
}

const Json::Value& Configuration::getConfig()const{
    return cfg;
}

string Configuration::getHostname()const{
    Json::Value host = cfg["host"];
    if(host.isNull())
        throw"Hostname not found";
    Json::Value name = cfg["name"];
    if(name.isNull())
        throw"Hostname not found";
    return name.asString();
}

int Configuration::getPort()const{
    Json::Value host = cfg["host"];
    if(host.isNull())
        throw"Port not found";
    Json::Value port = cfg["port"];
    if(port.isNull())
        throw"Port not found";
    return port.asInteger();
};

string Configuration::getDatabaseRef(){
    Json::Value path = cfg["path"];
    if(path.isNull())
        throw"Database path not found";
    Json::Value database = path["db"];
    if(database.isNull())
        throw"Database path not found";
    string target = "sqlite:"+getExecutablePath()+database.asString();
    return target;
}

string Configuration::getPrivateKeyFile(const UUID& id){
    Json::Value path = cfg["path"];
    if(path.isNull())
        throw"Users Directory not found";
    Json::Value userPath = path["users"];
    if(userPath.isNull())
        throw"Users Directory not found";
    string path = getExecutablePath()+userPath+"/PRIVKEY-"+id;
    return path;
}