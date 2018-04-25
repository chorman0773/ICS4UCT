/*
 * Version.cpp
 *
 *  Created on: Apr 4, 2018
 *      Author: Connor Horman
 */
#include "Version.hpp"
#include "StringHelper.hpp"
#include <string>
#include <stdexcept>

using std::stoi;
using std::invalid_argument;

const extern int32_t hashPrime;

Version::Version():major(1),minor(0){}
Version::Version(int encoded){
	major = ((encoded/256)+1)%256;
	minor = encoded%256;
}
Version::Version(string ver){
	string mj = tokenize(ver,".");
	string mi = ver;
	major = stoi(mj);
	minor = stoi(mi);
	if(major<1||major>256||minor<0||minor>255)
		throw invalid_argument("Major must be between 1 and 256, and Minor must be between 0 and 255 (inclusive)");
}
Version::Version(int major,int minor):major(major),minor(minor){
	if(major<1||major>256||minor<0||minor>255)
			throw invalid_argument("Major must be between 1 and 256, and Minor must be between 0 and 255 (inclusive)");
}

Version::Version(InputStream& src){
	major = src.read()+1;
	minor = src.read();
}

int Version::getEncoded()const{
	return (major-1)*256|minor;
}
int Version::getMajor()const{
	return major;
}
int Version::getMinor()const{
	return minor;
}
Version Version::getOrigin()const{
	return Version(major,0);
}

int32_t Version::hashCode()const{
	return major*hashPrime+minor;
}

bool Version::operator==(const Version& o)const{
	return major==o.major&&minor==o.minor;
}
bool Version::operator<(const Version& o)const{
	return major<o.major||(major==o.major&&minor<o.minor);
}
bool Version::operator>(const Version& o)const{
	return major>o.major||(major==o.major&&minor>o.minor);
}
bool Version::operator<=(const Version& o)const{
	return *this==o||*this<o;
}
bool Version::operator>=(const Version& o)const{
	return *this==o||*this>o;
}
void Version::write(OutputStream& o)const{
	o.write(major-1);
	o.write(minor);
}

ostream& operator <<(ostream& o,const Version& v){
	return o<<v.toString();
}
istream& operator >>(istream& i,Version& v){
	string s;
	i >> s;
	v = s;
	return i;
}

DataOutputStream& operator <<(DataOutputStream& o,const Version& v){
	v.write(o);
	return o;
}

DataInputStream& operator>>(DataInputStream& i,Version& v){
	v = i;
	return i;
}



