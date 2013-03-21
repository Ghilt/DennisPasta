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
using client_server::Article;

#define UNUSED_PARAM(p) (void)p

unsigned char readCommand(Connection* conn)
	throw(ConnectionClosedException) {

	unsigned char cmd = conn->read();
	return cmd;
}
int readNumber(Connection* conn) {
    unsigned char byte1 = conn->read();
    unsigned char byte2 = conn->read();
    unsigned char byte3 = conn->read();
    unsigned char byte4 = conn->read();
    return (byte1 << 24) | (byte2 << 16) | 
        (byte3 << 8) | byte4;
}
void writeNumber(int num, string& s) {

#define w(off) { \
	unsigned char c = static_cast<unsigned char>(num >> off); \
	s += c; }

	w(24);
	w(16);
	w(8);
	w(0);
}
int readIntParameter(Connection* conn) 
	throw(ConnectionClosedException){
	
	unsigned char pn = conn->read();
	assert(pn == Protocol::PAR_NUM);

	return readNumber(conn);
}
string readStringParameter(Connection* conn) 
	throw(ConnectionClosedException){
	
	unsigned char ps = conn->read();

	assert(ps == Protocol::PAR_STRING);

	int num = readNumber(conn);

	string ret;
	for (int i = 0; i < num; ++i)
	{
		unsigned char c = conn->read();
		ret += c;
	}

	return ret;
}

void writeString(const string& s, Connection* conn)
	throw(ConnectionClosedException) {
	for (size_t i = 0; i < s.size(); ++i) {
		conn->write(s[i]);
	}
//	conn->write('$'); // `$' is end of string
}

void writeStringParameter(string param, string& ret) {
	ret += Protocol::PAR_STRING;
	writeNumber(param.size(), ret);
	ret += param;
}

void listNewsGroups(const vector<Newsgroup*>& groups, Connection* conn) 
	throw(ConnectionClosedException) {
	string ret;

	ret += Protocol::ANS_LIST_NG;

	ret += Protocol::PAR_NUM;
	writeNumber(groups.size(), ret);

	for (auto it = groups.begin(); it != groups.end(); ++it) {
		Newsgroup* grp = *it;

		ret += Protocol::PAR_NUM;
		writeNumber(grp->getID(), ret);

		writeStringParameter(grp->getName(), ret);
	}
	ret += Protocol::ANS_END;

	writeString(ret, conn);
}
void listArticles(const vector<Newsgroup*>& groups, Connection* conn) {
	unsigned int id = readIntParameter(conn);
	string ret;
	ret += Protocol::ANS_LIST_ART;
	auto it = find_if(groups.begin(), groups.end(), [id](Newsgroup* grp) {
		return grp->getID() == id;
	});

	if(it== groups.end()){
		ret += Protocol::ANS_NAK;
		ret += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{
		ret += Protocol::ANS_ACK;
		Newsgroup *n = *it;
		ret += Protocol::PAR_NUM;		
		writeNumber(n->size(),ret);
		for(int i = 0; i!=n->size(); ++i){
			Article* a = n->getArticle(i);
			
			ret += Protocol::PAR_NUM;
			writeNumber(a->getID(),ret);

			writeStringParameter(a->getTitle(), ret);
		}
	}

	ret += Protocol::ANS_END;

	writeString(ret,conn);
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

void createArticle(vector<Newsgroup*>& groups, Connection* conn){
	unsigned int id = readIntParameter(conn);
	string ret;
	ret += Protocol::ANS_CREATE_ART;
	auto it = find_if(groups.begin(), groups.end(), [id](Newsgroup* grp) {
		return grp->getID() == id;
	});

	if(it== groups.end()){
		ret += Protocol::ANS_NAK;
		ret += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{
		ret += Protocol::ANS_ACK;
		Newsgroup *n = *it;
		n->createArticle(
			readStringParameter(conn),
			readStringParameter(conn), 
			readStringParameter(conn));	
	}

	ret += Protocol::ANS_END;

	writeString(ret,conn);
}


void deleteArticle(vector<Newsgroup*>& groups, Connection* conn){
	unsigned int idgroup = readIntParameter(conn);
	unsigned int idArt = readIntParameter(conn);
	string ret;
	ret += Protocol::ANS_CREATE_ART;
	auto it = find_if(groups.begin(), groups.end(), [idgroup](Newsgroup* grp) {
		return grp->getID() == idgroup;
	});

	if(it== groups.end()){
		ret += Protocol::ANS_NAK;
		ret += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{		
		Newsgroup *n = *it;
		Article* a = (*n)[idArt];
		if( a ==0){
			ret+=Protocol::ANS_NAK;
			ret += Protocol::ERR_ART_DOES_NOT_EXIST;

		}else{
			n->deleteArticle(idArt);
			ret+= Protocol::ANS_ACK;
		}
	}

	ret += Protocol::ANS_END;

	writeString(ret,conn);
}

void getArticle(vector<Newsgroup*>& groups, Connection* conn){
	unsigned int idgroup = readIntParameter(conn);
	unsigned int idArt = readIntParameter(conn);
	string ret;
	ret += Protocol::ANS_GET_ART;
	auto it = find_if(groups.begin(), groups.end(), [idgroup](Newsgroup* grp) {
		return grp->getID() == idgroup;
	});

	if(it== groups.end()){
		ret += Protocol::ANS_NAK;
		ret += Protocol::ERR_NG_DOES_NOT_EXIST;
	}else{		
		Newsgroup *n = *it;
		Article* a = (*n)[idArt];
		if( a ==0){
			ret+=Protocol::ANS_NAK;
			ret += Protocol::ERR_ART_DOES_NOT_EXIST;

		}else{			
			ret+= Protocol::ANS_ACK;
			writeStringParameter(a->getTitle(), ret);
			writeStringParameter(a->getAuthor(), ret);
			writeStringParameter(a->getText(), ret);
		}
	}

	ret += Protocol::ANS_END;
	writeString(ret,conn);
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
						listArticles(groups, conn);
					}
						break;
					case Protocol::COM_CREATE_ART:
					{
						createArticle(groups,conn);
					}
						break;
					case Protocol::COM_DELETE_ART:
					{
						deleteArticle(groups,conn);
					}
						break;
					case Protocol::COM_GET_ART:
					{
						getArticle(groups,conn);
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

