#define UUID_DEFINED

#include <string>
#include <iostream>
#include <cstdio>
#include <fstream>
#include "getch.h"
#include "Version.hpp"
#include "UUID.hpp"
#include "Menu.hpp"
#include "Random.hpp"
#include "StringHelper.hpp"
#include "JTime.hpp"
#include "Hash.hpp"
#include "Config.hpp"
#include "Bindings.hpp"



using std::ofstream;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::getline;

enum Mode{
	LOGGED_OUT,NORMAL,ELEVATED
};

AuthenticationResult authenticate(Employees& e,int opt, Employee*& authenticate){
	string pwd;
	Employee& target = e[e.getEmployee(opt).getUUID()];
	cout << "Please enter the password for " << (string) target << ">";
	char c;
	while((c=getch())!='\r'&&c!='\n'){
		if(c=='\b'&&pwd.size()!=0)
			pwd.erase(pwd.size()-1);
		else
			pwd += c;
		system("cls");
		cout << "Please enter the password for " << (string) target << ">" << string(pwd.length(),'*');
	};
	system("cls");
	authenticate = &target;
	return target.authenticate(pwd);
}


AuthenticationResult changepassword(Employee& e){
	string pwd;
	string npwd;
	cout << "Please enter the password for " << (string) e << ">";
	char c;
	while((c=getch())!='\r'&&c!='\n'){
		if(c=='\b'&&pwd.size()!=0)
			pwd.erase(pwd.size()-1);
		else
			pwd += c;
		system("cls");
		cout << "Please enter the password for " << (string) e << ">" << string(pwd.length(),'*');
	};
	system("cls");
	cout << "Please enter the new password for " << (string) e << ">";
	char c;
	while((c=getch())!='\r'&&c!='\n'){
		if(c=='\b'&&npwd.size()!=0)
			npwd.erase(npwd.size()-1);
		else
			npwd += c;
		system("cls");
		cout << "Please enter the new password for " << (string) e << ">" << string(npwd.length(),'*');
	};
	system("cls");
	return e.changePassword(pwd,npwd);
}

int main(int argc, const char** argv)
{
	AuthenticationResult res;

	Version currVer = CurrentVersion();
	Version currOrigin = CurrentOrigin();

	Employee *employee;
	string user[] = {"Set Status", "Request Product", "Purchase Order", "Change Password", "Log Out"};
	Menu userMenu ("Main Menu",user);
	string adminopts[] = {"Set Status", "Request Product", "Purchase Order", "Manage Accounts", "Manage Products", "Log Out"};
	Menu adminMenu ("Admin Menu",adminopts);
	string statusopts[] = { "Online", "Away", "Invisable"};
	Menu status ("Status", statusopts);

	Status statusList[] = {Status::ONLINE,Status::AWAY,Status::OFFLINE};
	Employees e;
	e.load();
	Menu m2 ("Employees", e);
	
	bool running(true);
	Mode mode(LOGGED_OUT);

	userMenu.setTerminalOption("Exit");
	m2.setTerminalOption("Exit");
	adminMenu.setTerminalOption("Exit");
	status.setTerminalOption("Exit");
	int opt;
	
	while (running)
	{
		
		switch (mode)
		{

		case NORMAL:
			switch(menu(userMenu))
			{
			case -1:
				///exit
				running  = false;
				break;
			case 0:
				opt = menu(status);
				if(opt == -1)
					break;
				employee->setStatus(statusList[opt]);
				break;
			case 1:
				cout << "Request Product" << endl << "Press any key to continue";
				getch();
				break;
			case 2:
				cout << "Purchase Order" << endl << "Press any key to continue";
				getch();
				break;
			case 3:
				
				break;
			}
			break;
		case LOGGED_OUT:
			opt = menu(m2);
			if (opt == -1)
				return EXIT_SUCCESS;
			res  = authenticate (e, opt, employee);
			if(res==AuthenticationResult::SUCCESS)
				mode = NORMAL;
			else if(res==AuthenticationResult::SUCCESS_ADMIN)
				mode = ELEVATED;
			else{
				cout << "Failed to Authenticate" << endl
					<< "Press any key to exit";
				getch();
				return EXIT_FAILURE;
			}
			break;
		case ELEVATED:
			switch(menu(adminMenu))
			{
			case -1:
				running = false;
				break;
			case 0:
				cout << "SET STATUS" << endl << "Press any key to continue";
				getch();
				break;
			case 1:
				cout << "REQUEST PRODUCT" << endl << "Press any key to continue";
				getch();
				break;
			case 2:
				cout << "PURCHASE ORDER" << endl << "Press any key to continue";
				getch();
				break;
			case 3:
				cout << "MANAGE ACCOUNTS" << endl << "Press any key to continue";
				getch();
				break;
			case 4:
				cout << "MANAGE PRODUCTS" << endl << "Press any key to continue";
				getch();
				break;
			case 5:
				if(confirm("Do you want to log out"))
					mode = LOGGED_OUT;
				break;
			}
			break;

		}
	}
	e.save();
	return EXIT_SUCCESS;
}