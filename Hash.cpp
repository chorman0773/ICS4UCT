#include "Hash.hpp"
#include <array>
#include <utility>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <typeinfo>
#include <cstdint>
#include <memory>
#include <chrono>


extern const int hashPrime(31);

template<> extern int32_t hashcode(const Hashable& h){
	return h.hashCode();
}
template<> extern int32_t hashcode(Hashable&& h){
	return h.hashCode();
}
template<> extern int32_t hashcode(Hashable* h){
	return h->hashCode();
}
template<> extern int32_t hashcode(int i){
	return i;
}
template<> extern int32_t hashcode(int64_t l){
	return (l>>32)^l&((((int64_t)1)<<32)-1);
}
template<> extern int32_t hashcode(uint64_t l){
	return hashcode((int64_t)l);
}
template<> extern int32_t hashcode(short s){
	return s;
}
template<> extern int32_t hashcode(char c){
	return c;
}
template<> extern int32_t hashcode(float f){
	return reinterpret_cast<int32_t&>(f);
}
template<> extern int32_t hashcode(double d){
	return hashcode(reinterpret_cast<int64_t&>(d));
}

using std::string;

template<> extern int32_t hashcode(string s){
	int32_t h = 0;
	for(int i = 0;i<s.length();i++){
		h*=hashPrime;
		h+=s[i];
	}
	return h;
}

template<> extern int32_t hashcode(char* s){
	int32_t h = 0;
	for(int i =0;i<strlen(s);i++){
		h*=hashPrime;
		h+=s[i];
	}
	return h;
}


template<> extern int32_t hashcode(nullptr_t n){
	return 0;
}

template<> extern int32_t hashcode(void* v){
	return (int32_t)v;
}

template<> extern int32_t hashcode(int& i){
	return hashcode((int)i);
}

template<> extern int32_t hashcode(int* i){
	return hashcode(*i);
}

template<> extern int32_t hashcode(int64_t& i){
	return hashcode(i);
}

template<> extern int32_t hashcode(int64_t* i){
	return hashcode(*i);
}

template<> extern int32_t hashcode(short& i){
	return hashcode(i);
}

template<> extern int32_t hashcode(short* i){
	return hashcode(*i);
}
template<> extern int32_t hashcode(char& i){
	return hashcode(i);
}

using std::vector;

template<typename T> extern int32_t hashcode(const vector<T>& v){
	int32_t h = 0;
	for(int i = 0;i<v.size();i++){
		h*=hashPrime;
		h+=hashcode(v[i]);
	}
	return h;
}

using std::array;

template<typename T,int size> extern int32_t hashcode(const array<T,size>& a){
	int32_t h = 0;
	for(int i = 0;i<size;i++){
		h*=hashPrime;
		h+=hashcode(a[i]);
	}
	return h;
} 

using std::unique_ptr;
using std::shared_ptr;

const int32_t uniqueSmartPrime = 11443717;
const int32_t sharedSmartPrime = 540283;

template<typename T> extern int32_t hashcode(const unique_ptr<T>&ptr){
	return hashcode(ptr.get())*uniqueSmartPrime;
}

template<typename T> extern int32_t hashcode(const shared_ptr<T>& ptr){
	return hashcode(ptr.get())*sharedSmartPrime;
}

using std::pair;
template<typename T,typename U> extern int32_t hashcode(const pair<T,U>& p){
	return hashcode(p.first)*hashPrime+hashcode(p.second);
}

template<typename T> extern int32_t hashcode(const pair<T*,int>& a){
	//Treat as array not as pair
	int32_t h = 0;
	for(int i =0;i<a.second;i++){
		h*=hashPrime;
		h+=hashcode(a.second()[i]);
	}
	return h;
}

using std::map;

template<typename K,typename V> extern int32_t hashcode(const map<K,V>& m){
	int32_t hash = 0;
	for(pair<K,V>& p:m){
		hash*=hashPrime;
		hash+=hashcode(p);
	}
	return hash;
}

using std::type_info;

template<> extern int32_t hashcode(const type_info& t){
	return t.hash_code();
}

int32_t Hashable::hashCode() const{
	return ::hashcode((void*)this);
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


