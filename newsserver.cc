/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include "newsgroup.h"
#include <cassert>
#include <algorithm>
#include "protocol.h"

using namespace std;
using protocol::Protocol;
using client_server::Server;
using client_server::Connection;
using client_server::ConnectionClosedException;
using client_server::Newsgroup;

#define UNUSED_PARAM(p) (void)p

unsigned char readCommand(Connection* conn)
	throw(ConnectionClosedException) {

	unsigned char cmd = conn->read();
	return cmd;
}
unsigned char readIntParameter(Connection* conn) 
	throw(ConnectionClosedException){
	
	unsigned char pn = conn->read();
	assert(pn == Protocol::PAR_NUM);
	unsigned char num = conn->read();

	return num;
}
string readStringParameter(Connection* conn) 
	throw(ConnectionClosedException){
	
	unsigned char ps = conn->read();

	cout << "should be 40 = " << ps << endl;
	assert(ps == Protocol::PAR_STRING);

	unsigned char num = conn->read();
	cout << "str num = <" << num << ">" << endl;;

	int numInt = static_cast<int>(num);

	cout << "int representation = <" << numInt << ">" << endl;

	string ret;
	for (int i = 0; i < num; ++i)
	{
		unsigned char c = conn->read();
		cout << "read string part " << c << endl;
		ret += c;
	}

	while (1) {
		unsigned char c = conn->read();
		cout << "read char <" << c << ">, (int) = " << ((int)c) << endl;
	}

	return ret;
}

void writeString(const string& s, Connection* conn)
	throw(ConnectionClosedException) {
	for (size_t i = 0; i < s.size(); ++i) {
		conn->write(s[i]);
	}
	conn->write('$'); // `$' is end of string
}

void listNewsGroups(const vector<Newsgroup*>& groups, Connection* conn) 
	throw(ConnectionClosedException) {
	string ret;

	cout << "HEJ" << endl;

	ret += Protocol::ANS_LIST_NG;

	ret += Protocol::PAR_NUM;
	ret += groups.size();

	for (auto it = groups.begin(); it != groups.end(); ++it) {
		Newsgroup* grp = *it;

		ret += Protocol::PAR_NUM;
		ret += grp->getID(); 

		ret += Protocol::PAR_STRING;
		ret += grp->getName().size(); 		
		ret += grp->getName(); 
	}
	ret += Protocol::ANS_END;

	cout << "listNewsGroups answering with: " << ret << endl;
	cout << "l = " << ret.size() << endl;

	writeString(ret, conn);
}
void createNewsGroup(vector<Newsgroup*>& groups, Connection* conn) 
	throw(ConnectionClosedException){

	string ret;

	ret += Protocol::ANS_CREATE_NG;

	string name = readStringParameter(conn);

	cout << "creating gropup with name <" << name << ">" << endl;

	auto it = find_if(groups.begin(), groups.end(), [name](Newsgroup* grp) {
		return grp->getName() == name;
	});

	if (it != groups.end()) {
		ret += Protocol::ANS_NAK;
		ret += Protocol::ERR_NG_ALREADY_EXISTS;
		ret += Protocol::ANS_END;
		writeString(ret, conn);
		return;
	}

	unsigned int id = 1;

	while (true) {
		
		++id;
		bool found = false;

		for (auto it = groups.begin(); it != groups.end(); ++it) {
			Newsgroup* grp = *it;


			if (id == grp->getID()) {
				found = true;
				break;
			}
		}
		if (!found)
			break;
	}
	
	Newsgroup* newGroup = new Newsgroup(name, id);
	groups.push_back(newGroup);

	ret += Protocol::ANS_ACK;
	ret += Protocol::ANS_END;

	writeString(ret, conn);
}
void deleteNewsGroup(vector<Newsgroup*>& groups, Connection* conn) 
	throw(ConnectionClosedException){

	string ret;

	ret += Protocol::ANS_DELETE_NG;

	unsigned int id = readIntParameter(conn);

	auto it = find_if(groups.begin(), groups.end(), [id](Newsgroup* grp) {
		return grp->getID() == id;
	});

	if (it == groups.end()) {
		ret += Protocol::ANS_NAK;
		ret += Protocol::ERR_NG_DOES_NOT_EXIST;
	} else {
		groups.erase(it);
		ret += Protocol::ANS_ACK;
		
	}
	ret += Protocol::ANS_END;
	writeString(ret, conn);
}

int main(int argc, char* argv[]){

	if (argc != 2) {
		cerr << "Usage: myserver port-number" << endl;
		exit(1);
	}
	Server server(atoi(argv[1]));

	if (! server.isReady()) {
		cerr << "Server initialization error" << endl;
		exit(1);
	}
	vector<Newsgroup*> groups;

	while (true) {
		Connection* conn = server.waitForActivity();
		if (conn != 0) {
			try {
				unsigned int nbr = readCommand(conn);
				cout << "received cmd " << nbr << endl;
				switch (nbr) {
					case Protocol::COM_LIST_NG:
					{
						listNewsGroups(groups, conn);
					}
						break;
					case Protocol::COM_CREATE_NG:
					{
						createNewsGroup(groups, conn);
					}
						break;
					case Protocol::COM_DELETE_NG:
					{
						deleteNewsGroup(groups, conn);
					}
						break;
					case Protocol::COM_LIST_ART:
					{

					}
						break;
					case Protocol::COM_CREATE_ART:
					{

					}
						break;
					case Protocol::COM_DELETE_ART:
					{

					}
						break;
					case Protocol::COM_GET_ART:
					{

					}
						break;
					case Protocol::COM_END:
					{

					}
						break;
					default:
					{
						cerr << "NewsServer recieved unrecognized code " << nbr << ", exiting. " << endl;
						exit(1);
					}
						break;
				}
				unsigned int end = readCommand(conn);

				cout << "end cmd = " << end << endl;

				assert(end == Protocol::COM_END);

			} catch (ConnectionClosedException&) {
				server.deregisterConnection(conn);
				delete conn;
				cout << "Client closed connection" << endl;
			}
		} else {
			server.registerConnection(new Connection);
			cout << "New client connects" << endl;
		}
	}
}

