#ifndef __Bindings_hpp_2018_04_24_09_28
#define __Bindings_hpp_2018_04_24_09_28
#include <string>
#include "Hash.hpp"
#include "UUID.hpp"

using std::string;

class User : public Hashable{
	string name;
	UUID id;
	double pay;
	unsigned char publicKey[128];
public:
	User();
	User newUser(const string&);
	int32_t hashCode()const;
};


#endif