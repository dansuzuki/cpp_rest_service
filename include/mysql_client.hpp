#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/scoped_ptr.hpp>

#include<jdbc/mysql_connection.h>
#include<jdbc/mysql_driver.h>
#include<jdbc/mysql_error.h>

#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

#ifndef _MYSQL_CLIENT_
#define _MYSQL_CLIENT_
using namespace std;
class ContactRecord {
public:
	ContactRecord(int id, std::string name, std::string mobileNumber);
	int getId();
	std::string getName();
	std::string getMobileNumber();
	
	
protected:
	int id;
	std::string name;
	std::string mobileNumber;
};


class MySQLClient {

public:
	MySQLClient();
	~MySQLClient();
	
	bool deleteContactById(int id);
	ContactRecord* getContactById(int id);
	std::vector<ContactRecord> getContacts();
	int newContact(std::string name, std::string mobileNumber);
	bool updateContact(int id, std::string name, std::string mobileNumber);

private:
	sql::Connection *connection;
	
protected:

};


#endif