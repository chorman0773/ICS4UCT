#ifndef Menu_hpp_2018_02_22_8_50
#define Menu_hpp_2018_02_22_8_50
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#include "TypingManagement.hpp"




using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;

/*
	Pulled Menuing code from one I wrote for Revised Treasure Island for C++
	I guarentee this code belongs to me.
	Updated menuing to use std::vector rather than dynamic array (update 2018-02-22)
*/
class Menu{

private:
	//Menu values. Controls how the menu operates and displays but are mostly static
	string name;
	vector<string> options;
	int num;
	bool wraps;
	string terminalOpt;
	int optCount;
	
	//Menu state, which can control the menuing system.
	bool hasTerminalOpt;
	bool showTerminalOpt;
	int selection;

	//Menu control system, which prevent mutation
	bool lock;
	bool requiresInit;

	void updateOptCounts();

public:
	void init();
	Menu(string name, string options[], size_t num, bool wrapping);
	Menu(string name, string options[], size_t num);
	template<size_t N> Menu(string name,string (&options)[N],bool wrapping=true):name(name),wraps(wrapping),lock(false){
		loadOptions(options,N);
	}
	Menu(string name);
	Menu(string name, bool wrapping);
	

	template<typename T,size_t N> Menu(string name,T(&arr)[N],bool wrapping=true,typename std::enable_if<std::is_convertible<T,string>::value,bool>::type=true):
			name(name),wraps(wrapping),lock(false){
		vector<string> options;
		for(const T& t:arr)
			options.push_back(string(t));
		loadOptions(options);
	}

	template<typename Iterable> Menu(string name,const Iterable& itr,bool wraps=true,
			typename std::enable_if<std::is_convertible<typename Iterable::value_type,string>::value,bool>::type=true,
			typename std::conditional<false,typename std::conditional<false,decltype(std::declval<Iterable>().begin()),
		decltype(std::declval<Iterable>().end())>::type,bool>::type=false):name(name),wraps(wraps),lock(false)
	{
		typedef typename Iterable::value_type type;
		vector<string> options = vector<string>();
		for(const type& t:itr)
			options.push_back(string(t));
		loadOptions(options);
	}
			
	Menu();

	
	void loadOptions(string options[],int num);
	void loadOptions(vector<string> options);
	
	void selectionIncrease();
	void selectionDecrease();
	void drawVertical();

	int getSelection();
	void addOption(string option);
	void removeOption(int opt);
	void setTerminalOption(string endOption);
	bool isTerminalOption();
	void startMenu();
	void endMenu();

	void setTerminalOptionVisibility(bool visibility);
	void clearOptions();
};

class ActionListener{
public:
	virtual bool onAction(int& code) = 0;
};

/**
	Draws a Menu on the screen. The menu is mostly seemless (some flickering) but will advance if the input is up, down, w, or s and chooses the selection on Enter
*/
int menu(Menu& m);

/**
	Draws a Menu on the screen. The menu is mostly seemless (some flickering) but will advance if the input is up, down, w, or s and chooses the selection on Enter
*/
int menu(Menu& m,ActionListener& extra);


bool confirm(string text,bool invert = false);
bool secureConfirm(string text);

#endif