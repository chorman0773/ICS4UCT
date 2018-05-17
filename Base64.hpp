#ifndef __Base64_hpp_2018_04_27_10_13
#define __Base64_hpp_2018_04_27_10_13
#include <string>

using std::string;

string encodeBase64(unsigned const char*,int len);
void decodeBase64(unsigned char*,const string&);


#endif