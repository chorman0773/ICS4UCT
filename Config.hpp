#ifndef __Config_hpp_2018_04_25_09_25
#define __Config_hpp_2018_04_25_09_25

#include "ComplierCfg.hpp"

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

class Configuration{
    Json::Value cfg;
public:
    Configuration();
    void reload();
    const Json::Value& getConfig()const;
    string getHostname()const;
    int getPort()const;
    string getDatabaseRef()const;
    string getPrivateKeyFile(const UUID&)const;
};


#endif