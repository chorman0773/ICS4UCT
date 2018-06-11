#ifndef __Hash_hpp_2018_03_23_09_48
#define __Hash_hpp_2018_03_23_09_48
#include <cstdint>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include <memory>
#include <utility>
#include <map>
#include <array>
#include <string>
class Hashable{
public:
	virtual int hashCode()const = 0;
};


int32_t hashcode(int);
int32_t hashcode(char);
int32_t hashcode(short);
int32_t hashcode(unsigned char);
int32_t hashcode(unsigned short);
int32_t hashcode(unsigned int);
int32_t hashcode(unsigned long long);
int32_t hashcode(long long);
int32_t hashcode(const char*);
int32_t hashcode(const wchar_t*);
int32_t hashcode(const void*);
int32_t hashcode(nullptr_t);
int32_t hashcode(double);
int32_t hashcode(float);
int32_t hashcode(bool);



 int32_t hashcode(const std::string& str);



using std::vector;

template<typename T> int32_t hashcode(const vector<T>& v){
	int32_t h = 0;
	for(int i = 0;i<v.size();i++){
		h*=31;
		h+=hashcode(v[i]);
	}
	return h;
}

using std::array;

template<typename T,int size>  int32_t hashcode(const array<T,size>& a){
	int32_t h = 0;
	for(int i = 0;i<size;i++){
		h*=31;
		h+=hashcode(a[i]);
	}
	return h;
} 

using std::unique_ptr;
using std::shared_ptr;

const int32_t uniqueSmartPrime = 11443717;
const int32_t sharedSmartPrime = 540283;

template<typename T> int32_t hashcode(const unique_ptr<T>&ptr){
	return hashcode(ptr.get())*uniqueSmartPrime;
}

template<typename T> int32_t hashcode(const shared_ptr<T>& ptr){
	return hashcode(ptr.get())*sharedSmartPrime;
}

using std::pair;
template<typename T,typename U> int32_t hashcode(const pair<T,U>& p){
	return hashcode(p.first)*31+hashcode(p.second);
}

template<typename T> int32_t hashcode(const pair<T*,int>& a){
	//Treat as array not as pair
	int32_t h = 0;
	for(int i =0;i<a.second;i++){
		h*=31;
		h+=hashcode(a.second()[i]);
	}
	return h;
}

using std::map;

template<typename K,typename V> int32_t hashcode(const map<K,V>& m){
	int32_t hash = 0;
	for(pair<K,V>& p:m){
		hash*=31;
		hash+=hashcode(p);
	}
	return hash;
}

template<typename E> int32_t hashcode(typename std::enable_if<std::is_enum<E>::value,E>::type e){
	decltype(auto) val = reinterpret_cast<typename std::underlying_type<E>::type>(e);
	return hashcode<decltype(val)>(val);
}

#endif