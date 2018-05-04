#include "mysql_client.hpp"

using namespace sql;
using namespace std;


ContactRecord::ContactRecord(int id, std::string name, std::string mobileNumber) {
	this->id = id;
	this->name = name;
	this->mobileNumber = mobileNumber;
}

int ContactRecord::getId() { return this->id; }
std::string ContactRecord::getName() { return this->name; }
std::string ContactRecord::getMobileNumber() { return this->mobileNumber; }


MySQLClient::MySQLClient() {
	const std::string url("tcp://127.0.0.1:3306");
	const std::string user("rest_user");
	const std::string pass("rest_user");
	
	sql::Driver * driver = sql::mysql::get_driver_instance();	
	this->connection = driver->connect(url, user, pass);
}

MySQLClient::~MySQLClient() {
	delete this->connection;
}

bool MySQLClient::deleteContactById(int id) {
	stringstream sql;
	sql::Statement *statement = this->connection->createStatement();
	statement->execute("USE phonebookdb");
	sql.str("");
	sql << "DELETE FROM contacts WHERE id = " << id;
	sql::ResultSet *result = statement->executeQuery(sql.str());
	result->next();
	std::cout << "delete rows count: " << result->rowsCount();
	delete result, statement;
	return true;
	
}

ContactRecord* MySQLClient::getContactById(int id) {
	stringstream sql;
	
	sql::Statement *statement = this->connection->createStatement();
	statement->execute("USE phonebookdb");
	
	sql.str("");
	sql << "SELECT * FROM contacts WHERE id = " << id;
	
	sql::ResultSet *result = statement->executeQuery(sql.str());
	ContactRecord *contactRecord = NULL;
	if(result->next()) {
		contactRecord = new ContactRecord(result->getInt(1), 
			result->getString(2), 
			result->getString(3));
	}
	delete result, statement;
	return contactRecord;
}

std::vector<ContactRecord> MySQLClient::getContacts() {
	std:vector<ContactRecord> contacts;
	
	sql::Statement *statement = this->connection->createStatement();
	statement->execute("USE phonebookdb");
	
	sql::ResultSet *result = statement->executeQuery("SELECT * FROM contacts ORDER BY id");
	ContactRecord *contactRecord = NULL;
	while(result->next()) {
		ContactRecord contactRecord(result->getInt(1), 
			result->getString(2), 
			result->getString(3));
		contacts.push_back(contactRecord);
	}
	return contacts;
}

int MySQLClient::newContact(std::string name, std::string mobileNumber) {
	stringstream sql;
	
	sql::Statement *statement = this->connection->createStatement();
	statement->execute("USE phonebookdb");
	
	sql.str("");
	sql << "INSERT INTO contacts(name, mobile_number) VALUES('" 
		<< name << "', '" 
		<< mobileNumber << "')";
	statement->execute(sql.str());
	
	sql::ResultSet *result = statement->executeQuery("SELECT LAST_INSERT_ID()");
	result->next();
	int id = result->getInt64(1);
	
	delete result, statement;
	return id;
}

bool MySQLClient::updateContact(int id, std::string name, std::string mobileNumber) {
	stringstream sql;
	sql::Statement *statement = this->connection->createStatement();
	statement->execute("USE phonebookdb");
	sql.str("");
	sql << "UPDATE contacts "
		<< "SET name = '" << name << "', "
		<< "mobile_number = '" << mobileNumber << "' "
		<< "WHERE id = " << id;
	std::cout << "updated rows count: " << statement->executeUpdate(sql.str());
	delete statement;
	return true;
}
