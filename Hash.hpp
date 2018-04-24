#ifndef __Hash_hp_2018_03_23_09_48
#define __Hash_hp_2018_03_23_09_48
#pragma once
#include <cstdint>
#include <type_traits>
#include <typeinfo>
class Hashable{
public:
	virtual int hashCode()const = 0;
};

template<typename T> extern int32_t hashcode(T);



#endif