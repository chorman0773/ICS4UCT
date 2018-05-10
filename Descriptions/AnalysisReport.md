						CULMINATING TASK Analysis Report
#Proposed System
The System is a general purpose Employee Management, Payroll, and Status Control System.
It tracks when Employees are online, and allows them to change there status or go offline.
The tracking works remotely by connecting to the internet over a secure channel.
The features can be customized on a case-by-case basis.
The system is menu driven at all levels, and the menu selection can be change with W or Up Arrow (Up), and D or Down Arrow (Down),
And the selection is confirmed with enter.


Process Directory
				Process				 |				Data From			|		Data To	
=========================================================================================================================
1	Log In to Server (Authentication)| Users' Data, User				|  User Menu, Manage Accounts
2	User Menu						 | Authentication, User				|
3   Set Status						 | User								| Users' Data
4   Log Out of Server (Log Out)		 | User								| Users' Data
5   Exit							 | User								|
6   Elevate Session (Enter Sudo Mode)| User, Users' Data, Authentication| Users' Data, Change Password, Manage Accounts
7   Change Password					 | User, Enter Sudo Mode			| Users' Data
8	Admin Menu 						 | User, Authentication				| User Menu, Enter Sudo Mode
9	Manage Accounts					 | User, Enter Sudo Mode			| Add Account, Delete Account, Modify Account
10	Add Account						 | User, Manage Accounts			| Users' Data
11  Delete Account					 | User, Manage Accounts,Users' Data| 
12  Modify Account					 | User, Manage Accounts			| Users' Data


File Directory
			File					| 			Data From				|	Data To
===================================================================================================================
1	Users' Data						|P3, P4, P6, P7, P10, P12			| P1, P6, P11


(c) 2018, Connor Horman and Kyle Adomeit
