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
#include "protocol.h"

using namespace std;
using protocol::Protocol;
using client_server::Server;
using client_server::Connection;
using client_server::ConnectionClosedException;
using client_server::Newsgroup;

#define UNUSED_PARAM(p) (void)p

int readCommand(Connection* conn)
	throw(ConnectionClosedException) {

	unsigned char cmd = conn->read();
	return cmd;
}
int readIntParameter(Connection* conn) 
	throw(ConnectionClosedException){
	
	unsigned char pn = conn->read();
	assert(pn == Protocol::PAR_NUM);
	unsigned char num = conn->read();

	return num;
}
string readStringParameter(Connection* conn) 
	throw(ConnectionClosedException){
	
	unsigned char ps = conn->read();
	assert(ps == Protocol::PAR_STRING);

	unsigned char num = conn->read();

	string ret;
	for (int i = 0; i < num; ++i)
	{
		ret += conn->read();
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

	ret += Protocol::ANS_LIST_NG;

	ret += Protocol::PAR_NUM;
	ret += groups.size();

	for (auto it = groups.begin(); it != groups.end(); ++it) {
		Newsgroup* grp = *it;

		UNUSED_PARAM(grp);

		ret += Protocol::PAR_NUM;
		ret += 1; 		// TODO grp.id

		ret += Protocol::PAR_STRING;
		ret += 3; 		// TODO grp.name.size()
		ret += "c++"; 	// TODO grp.name
	}
	ret += Protocol::ANS_END;

	writeString(ret, conn);
}
void createNewsGroup(vector<Newsgroup*>& groups, Connection* conn) 
	throw(ConnectionClosedException){

	string ret;

	ret += Protocol::ANS_CREATE_NG;

	string name = readStringParameter(conn);

	for (auto it = groups.begin(); it != groups.end(); ++it) {
		Newsgroup* grp = *it;

		UNUSED_PARAM(grp);

		if (name == "c++") { // TODO grp.name
			ret += Protocol::ANS_NAK;
			ret += Protocol::ERR_NG_ALREADY_EXISTS;
			ret += Protocol::ANS_END;
			writeString(ret, conn);

			return;
		}
	}

	unsigned int id = 1;

	while (true) {
		
		++id;
		bool found = false;

		for (auto it = groups.begin(); it != groups.end(); ++it) {
			Newsgroup* grp = *it;

			UNUSED_PARAM(grp);

			if (id == 1337) {//TODO grp.id
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
				int nbr = readCommand(conn);
				switch (nbr) {
					case Protocol::COM_LIST_NG:
						listNewsGroups(groups, conn);
						break;
					case Protocol::COM_CREATE_NG:
						createNewsGroup(groups, conn);
						break;
					case Protocol::COM_DELETE_NG:
						break;
					case Protocol::COM_LIST_ART:
						break;
					case Protocol::COM_CREATE_ART:
						break;
					case Protocol::COM_DELETE_ART:
						break;
					case Protocol::COM_GET_ART:
						break;
					case Protocol::COM_END:
						break;
					default:
					{
						cerr << "NewsServer recieved unrecognized code " << nbr << ", exiting. " << endl;
						exit(1);
					}
						break;
				}

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

