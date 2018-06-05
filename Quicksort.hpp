#pragma once

#include <type_traits>
#include <vector>
#include "TypingManagement.hpp"

using std::declval;
using std::vector;


template<typename T> struct is_comparable:std::false_type{};

template<typename T> struct is_comparable<typename std::conditional<false,decltype(declval<const T&>()<declval<const T&>()),
	T>::type>:true_type{};

template<typename T> struct is_sortable:std::false_type{};


template<typename T> struct is_sortable<typename std::enable_if<std::is_copy_constructible<T>::value&&std::is_copy_assignable<T>::value&&is_comparable<T>::value,T>::type>:true_type{};

template<typename T> void quicksort(vector<T>& vec,typename std::enable_if<is_sortable<T>::value,bool>::type=false){
	typedef vector<T>::iterator iterator;
	iterator middle = vec.begin()+vec.length()/2;
	vector<T> firstHalf;
	vector<T> secondHalf;
	T& pivot = *middle;
	for(const T& t:vec){
		if(t<pivot)
			firstHalf.push_back(t);
		else
			secondHalf.push_back(t);
	}
	if(firstHalf.size()>1)
		quicksort(firstHalf);
	if(secondHalf.size()>1)
		quicksort(secondHalf);
	vec.clear();
	for(const T& t:firstHalf)
		vec.push_back(t);
	for(const T& t:secondHalf)
		vec.push_back(t);
}

