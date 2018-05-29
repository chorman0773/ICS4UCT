#include "UUID.hpp"
#include <string>
#include "Hash.hpp"
#include <iomanip>
#include <sstream>
#include "StringHelper.hpp"
#include <cstdlib>
#include "Random.hpp"
#include "JTime.hpp"

extern const int32_t hashPrime;


Random uuidRandom;



using std::string;
using std::stoull;
using std::ostringstream;
using std::ostream;
using std::istream;

const string sep("-");
const int sizes[] = {8,4,4,4,12};

int32_t UUID::hashCode()const{
	return hashcode(this->high)*hashPrime+hashcode(this->low);
}

UUID::UUID(uint64_t high,uint64_t low):high(high),low(low){}
UUID::UUID():high(0),low(0){}

UUID::UUID(string str){
	string currPart;
	string highPart;
	string lowPart;

	int i;
	for(i = 0;i<5;i++)
		currPart += tokenize(str,sep);
	highPart.assign(currPart,0,16);
	lowPart.assign(currPart,16,16);
	high = stoull(highPart,nullptr,16);
	low = stoull(lowPart,nullptr,16);
}


UUID UUID::fromString(string str){
	return UUID(str);
}

uint64_t UUID::getHigh()const{
	return this->high;
}
uint64_t UUID::getLow()const{
	return this->low;
}

uint64_t randomVersion = 0x4000;
uint64_t versionMask = 0xf000;



ostream& operator<<(ostream& o,const UUID& id){
	uint64_t high = id.getHigh();
	uint64_t low = id.getLow();
	uint64_t high1 = high>>32;
	uint64_t high2 = (high>>16)&0xFFFF;
	uint64_t high3 = high&0xFFFF;
	uint64_t low1 = low>>48;
	uint64_t low2 = low&((1LL<<48)-1);
	o <<std::hex<<std::setw(8)<<std::setfill('0')<<high1<<
		sep<<std::hex<<std::setw(4)<<std::setfill('0')<<high2<<
		sep<<std::hex<<std::setw(4)<<std::setfill('0')<<high3<<
		sep<<std::hex<<std::setw(4)<<std::setfill('0')<<low1<<
		sep<<std::hex<<std::setw(12)<<std::setfill('0')<<low2;
	return o;
}

string UUID::toString()const{
	ostringstream str;
	str<<*this;
	return str.str();
}

istream& operator>>(istream& i,UUID& id){
	string s;
	i >> s;
	id= UUID::fromString(s);
	return i;
}

string operator+(const string& s,const UUID& id){
	return s+(id.toString());
}

Instant UUID_EPOCH = Instant::fromEpochSecond(-12244089600);

UUID UUID::ofNow(){
	Instant now = Instant::now();
	Duration d = Duration::between(UUID_EPOCH, now);
	uint64_t ts = d.getSeconds()*10000000+d.getNanos()/100;
	uint64_t rnode = (uuidRandom.nextLong()&0x7FFFFFFFFFFF)|0x800000000000;
	uint64_t low = rnode|((uint64_t)(0xE000|(d.hashCode()&0xCFFF)))<<56LL;
	uint64_t high = (ts&0xFFFFFFFF)<<32|((ts>>32)&0xFFFF)<<16|0x1000|((ts>>48)&0xFFF);
	return UUID(high,low);
}

bool UUID::operator==(const UUID& u)const{
	return high==u.high&&low==u.low;
}

bool UUID::operator!=(const UUID& u)const{
	return high!=u.high||low!=u.low;
}

