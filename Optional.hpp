#pragma once

#include <type_traits>
#include <utility>
#include <stdexcept>

struct in_place_type_t{};

struct  nullopt_t{};

const nullopt_t nullopt;
const in_place_type_t in_place_type;

class bad_optional:public std::exception{
public:
	virtual const char* what()const final override{
		return "Attempt to use a optional without a value";
	}
};

template<typename T> struct optional{
private:
	union underlying{
		char placeholder[sizeof(T)];
		T val;
		underlying()=default;
		underlying(const T& t):val(t){}
		underlying(T&& t):val(t){}
		template<typename... Args> underlying(Args&&... args):val(std::forward<Args>(args)...){}
		underlying& operator=(const T& t){
			val = t;
			return *this;
		}
		underlying& operator=(T&& t){
			val = t;
			return *this;
		}
		underlying& operator=(nullopt_t _){
			val.~T();
			return *this;
		}
	};
	underlying val;
	bool hasValue=false;
	typedef T type;
	typedef const T const_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
public:
	optional()throw(){}
	optional(nullopt_t _)throw(){}
	optional(const T& t):val(t),hasValue(true){}
	optional(T&& t):val(t),hasValue(true){}
	optional(const optional<T>& t):hasValue(t.hasValue){
		if(hasValue)
			val = t.val.val;
	}
	optional(optional<T>&& t):hasValue(t.hasValue){
		if(hasValue)
			val = std::move(t.val.val);
	}
	template<typename U> optional(typename std::conditional<false,decltype(T(std::declval<U>())),const optional<U>&>::type opt):hasValue(opt.hasValue){
		if(hasValue)
			val  = t.val.val;
	}
	template<typename U> optional(typename std::conditional<false,decltype(T(std::declval<U>())),optional<U>&&>::type opt):hasValue(opt.hasValue){
		if(hasValue)
			val  = std::move(t.val.val);
	}
	template<typename... Args> optional(typename std::enable_if<std::is_constructible<T,Args...>::value,in_place_type_t>::type _,Args... args):hasValue(true),val(args){}
	
	reference get(){
		if(!hasValue)
			throw bad_optional();
		return val.val;
	}
	const_reference get()const{
		if(!hasValue)
			throw bad_optional();
		return val.val;
	}
	reference operator*(){
		return get();
	}
	const_reference operator*()const{
		return get();
	}
	pointer operator->(){
		if(!hasValue)
			throw bad_optional();
		return &val.val;
	}
	const_pointer operator->()const{
		if(!hasValue)
			throw bad_optional();
		return &val.val;
	}

	explicit operator bool()const{
		return hasValue;
	}
	bool has_value()const{
		return hasValue;
	}
	template<typename U> T get_or(typename std::enable_if<std::is_convertible<U,T>::value,U>::type&& u)const{
		if(!hasValue)
			return u;
		return val.val;
	}
};

