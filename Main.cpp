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

AuthenticationResult authenticate(Employee& target){
	string pwd;
	
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

void requestproduct (Products& p){
	int y(0);
	int x(0);
	double enter(0);
	Menu request ("Request Product",p.getNameVector());
	x = menu(request);
	Product& p2 = p.getProduct(x);
	do
	{
		system("cls");
		fflush(stdin);
		cout << "You have selected " << (string) p2 << endl << "Please select the amount you would like to order.";
		cin >> enter;
		if (enter < 0){
			cout << "Error. Amount cannot be negetive";
			getch();
		}
	}
	while (enter < 0);
	cout << "You have selected " << enter << "amount of product " << (string) p2 << "please confirm that this infomation is correct";
	if(confirm("Is this correct?")){
		p2.request(enter);
		cout << "Reqest sucessful. Returning to main menu. Press any key to exit.";
		getch();
		return;
	}
	else{
		cout << "Cancelling request. Returning to main menu. Press any key to exit.";
		getch();
		return;
	}
}

void addemployee(Employee*& e, Employees& es){
	string edit[] = {"Name", "Pay", "Password"};
	Menu editMenu ("Edit Infomation", edit);
	string name;
	string pass;
	float pay;
	int x(0);
	int y(0);
	int z(0);
	char c;

		cout << "Enter new employee name: ";
		cin >> name;
		while(y==0){
			system("cls");
			fflush(stdin);
			cout << "Enter pay for " << name << " :";
			cin >> pay;
			if(pay>0){
				cout << "Error. Pay cannot be negetive";
				getch();
			}
			else
				y = 1;
		}
		y = 0;
		while(y==0){
			system("cls");
			fflush(stdin);
			cout << "Enter password for " << name << " :";
			cout << "Please enter the password for " << (string) *e << ">";
			while((c=getch())!='\r'&&c!='\n'){
				if(c=='\b'&&pass.size()!=0)
					pass.erase(pass.size()-1);
				else
					pass += c;
				system("cls");
				cout << "Please enter the password for " << (string) *e << ">" << string(pass.length(),'*');
			};
			if(pass.length<=7){
				cout << "Error. Password must be at least 8 characters";
				getch();
			}
			else
				y = 1;
		}
	do
	{
		system("cls");
		fflush(stdin);
		cout << name << "\t\t" << pay << "\t\tCANNOT WRITE PASS DUE TO SECERITY REASONS" << endl
			<< "Is the infomation above correct?";
		x = getch();
		if(x==78){
			//add employee 
		}
		else if (x==89){
			z = menu(editMenu);
			switch(z)
			{
			case 0:
				cout << "Enter new employee name: ";
				cin >> name;
				break;
			case 1:
				y=0;
				while(y==0){
					system("cls");
					fflush(stdin);
					cout << "Enter pay for " << name << " :";
					cin >> pay;
					if(pay>0){
						cout << "Error. Pay cannot be negetive";
						getch();
					}
					else
						y = 1;
				}
				break;
			case 2:
				y=0;
				while(y==0){
				system("cls");
				fflush(stdin);
				cout << "Enter password for " << name << " :";
				cout << "Please enter the password for " << (string) *e << ">";
				while((c=getch())!='\r'&&c!='\n'){
					if(c=='\b'&&pass.size()!=0)
						pass.erase(pass.size()-1);
					else
						pass += c;
					system("cls");
					cout << "Please enter the password for " << (string) *e << ">" << string(pass.length(),'*');
				};
				if(pass.length<=7){
					cout << "Error. Password must be at least 8 characters";
					getch();
				}
				else
					y = 1;
			}
				break;
			}
		}
	}
	while(x==89);

}

void manageaccounts(Employee*& e, Employees& es){
	AuthenticationResult auth;
	string manage[] = {"Add Employee", "Modify Employee", "Erase Employee", "Back"};
	Menu manageaccount ("Manage Accounts", manage);
	
	auth = authenticate(*e);
	if(auth==AuthenticationResult::SUCCESS){
		cout <<  "Manage Accounts is only available for Admin accounts";
		getch();
		return;
	}
	else if(auth!=AuthenticationResult::SUCCESS_ADMIN){
		cout << "Authentication failed";
		getch();
		return;
	}
	switch(menu(manageaccount)){
	case 0:
			cout << "Add Employee" << endl << "Press any key to continue";
			getch();
		break;
	case 1:
			cout << "Modify Employee" << endl << "Press any key to continue";
			getch();
		break;
	case 2:
			cout << "Erase Employee" << endl << "Press any key to continue";
			getch();
		break;
	}

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
	Products p;
	p.load();
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
				requestproduct(p);
				break;
			case 2:
				cout << "Purchase Order" << endl << "Press any key to continue";
				getch();
				break;
			case 3:
				//change password
				changepassword(*employee);
				break;
			case 4:
				//log out
				if(confirm("Do you want to log out"))
					mode = LOGGED_OUT;
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