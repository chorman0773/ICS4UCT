#include "Hash.hpp"
#include <array>
#include <utility>
#include <string>
#include <cstring>
#include <cwchar>
#include <map>
#include <vector>
#include <typeinfo>
#include <cstdint>
#include <memory>
#include <chrono>


extern const int hashPrime(31);


int32_t hashcode(int i){
	return i;
}
int32_t hashcode(int64_t l){
	return (l>>32)^l&((((int64_t)1)<<32)-1);
}
int32_t hashcode(uint64_t l){
	return hashcode((int64_t)l);
}
int32_t hashcode(short s){
	return s;
}
int32_t hashcode(char c){
	return c;
}
int32_t hashcode(float f){
	return reinterpret_cast<int32_t&>(f);
}
int32_t hashcode(double d){
	return hashcode(reinterpret_cast<int64_t&>(d));
}

int32_t hashcode(bool b){
	return b?1331:1337;
}

using std::string;



int32_t hashcode(char* s){
	int32_t h = 0;
	for(int i =0;i<strlen(s);i++){
		h*=hashPrime;
		h+=s[i];
	}
	return h;
}

int32_t hashcode(wchar_t* c){
	int32_t h = 0;
	const int32_t prime = 31;
	for(int i =0;i<wcslen(c);i++){
		h*=hashPrime;
		h+=c[i];
	}
	return h;
}

int32_t hashcode(nullptr_t n){
	return 0;
}

int32_t hashcode(void* v){
	return (int32_t)v;
}


int32_t hashcode(const string& s){
	int32_t h = 0;
	const int32_t prime = 31;
	for(char c:s){
		h *= prime;
		h += hashcode(c);
	}
	return h;
}


using std::type_info;

int32_t hashcode(const type_info& t){
	return t.hash_code();
}


namespace std{
	template<> class hash<Hashable>{
	public:
		typedef Hashable argument_type;
		typedef size_t result_type;
		size_t operator()(const Hashable& h)throw(){
			return h.hashCode();
		}
	};
};


