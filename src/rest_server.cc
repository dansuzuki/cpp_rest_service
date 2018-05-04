/* 
   Mathieu Stefani, 07 février 2016
   
   Example of a REST endpoint with routing
*/

#include <algorithm>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "json.hpp"
#include "mysql_client.hpp"

using namespace std;
using namespace Pistache;

using json = nlohmann::json;


namespace Generic {

void handleReady(const Rest::Request&, Http::ResponseWriter response) {
    response.send(Http::Code::Ok, "1");
}

}

class ServiceEndpoint {
public:
    ServiceEndpoint(Address addr, MySQLClient* _mySQLClient)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
    { 
		this->mySQLClient = _mySQLClient;
	}

    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
            .threads(thr)
            .flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown() {
        httpEndpoint->shutdown();
    }

private:
    void setupRoutes() {
        using namespace Rest;
		Routes::Delete(router, "/contacts/:id", Routes::bind(&ServiceEndpoint::deleteContact, this));
		Routes::Get(router, "/contacts", Routes::bind(&ServiceEndpoint::getContacts, this));
		Routes::Get(router, "/contacts/:id", Routes::bind(&ServiceEndpoint::getContact, this));
		Routes::Post(router, "/contacts", Routes::bind(&ServiceEndpoint::newContact, this));
		Routes::Put(router, "/contacts/:id", Routes::bind(&ServiceEndpoint::updateContact, this));
		
		Routes::Post(router, "/_admin/shutdown", Routes::bind(&ServiceEndpoint::triggerShutdown, this));
    }

	void deleteContact(const Rest::Request& request, Http::ResponseWriter response) {
		auto id = request.param(":id").as<int>();
		if (mySQLClient->deleteContactById(id)) {
			response.send(Http::Code::No_Content, "");
		}
		else {
			response.send(Http::Code::Bad_Request, "");
		}
	}
	
	void getContacts(const Rest::Request& request, Http::ResponseWriter response) {
		json payload;
		std::vector<ContactRecord> contacts = mySQLClient->getContacts();
		for(ContactRecord contact: contacts) {
			json record;
			record["id"] = contact.getId();
			record["name"] = contact.getName();
			record["mobile_number"] = contact.getMobileNumber();
			payload.push_back(record);
		}
		response.send(Http::Code::Ok, payload.dump());
	}
	
	void getContact(const Rest::Request& request, Http::ResponseWriter response) {
		auto id = request.param(":id").as<int>();
		ContactRecord* contactRecord = mySQLClient->getContactById(id);
		if(contactRecord == NULL) {
			response.send(Http::Code::Not_Found, "");
		}
		else {
			json payload;
			payload["id"] = contactRecord->getId();
			payload["name"] = contactRecord->getName();
			payload["mobile_number"] = contactRecord->getMobileNumber();
		
			response.send(Http::Code::Ok, payload.dump());
		}
	}
	
	void newContact(const Rest::Request& request, Http::ResponseWriter response) {
		auto payload = json::parse(request.body());
		std::string name = payload["name"];
		std::string mobileNumber = payload["mobile_number"];
		
		json result;
		result["id"] = mySQLClient->newContact(name, mobileNumber);
		response.send(Http::Code::Ok, result.dump());
	}
	
	void updateContact(const Rest::Request& request, Http::ResponseWriter response) {
		auto payload = json::parse(request.body());
		std::string name = payload["name"];
		std::string mobileNumber = payload["mobile_number"];
		auto id = request.param(":id").as<int>();
		if (mySQLClient->updateContact(id, name, mobileNumber)) {
			response.send(Http::Code::Ok, "");
		}
		else {
			response.send(Http::Code::Bad_Request, "");
		}
	}
	
	/** admin stuff */
	void triggerShutdown(const Rest::Request& request, Http::ResponseWriter response) {
		/** FIXME: still buggy */
		response.send(Http::Code::Ok, "Bye");
		this->shutdown();
	}
	
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
	MySQLClient *mySQLClient;
};

int main(int argc, char *argv[]) {
    Port port(9080);

    int thr = 2;

    if (argc >= 2) {
        port = std::stol(argv[1]);

        if (argc == 3)
            thr = std::stol(argv[2]);
    }

    Address addr(Ipv4::any(), port);

	cout << "Starting server..." << std::endl;
	MySQLClient mySQLClient;
    ServiceEndpoint service(addr, &mySQLClient);

    service.init(thr);
	cout << "Listending at port " << port << std::endl;
    service.start();
}
