#ifndef SERVER_COMMAND_HANDLER_H

#define SERVER_COMMAND_HANDLER_H

#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include "newsgroup.h"
#include <algorithm>
#include "protocol.h"
#include "dirent.h"
#include <cstring>
#include <fstream>
#include <cerrno>
#include <sstream>
#include "ServerEventListener.h"

using namespace std;
using protocol::Protocol;
using client_server::Server;
using client_server::Connection;
using client_server::ConnectionClosedException;
using client_server::Newsgroup;
using client_server::Article;


class ServerCommandHandler
{

public:
	ServerCommandHandler();
	~ServerCommandHandler();

	vector<Newsgroup*> groups;
	unsigned int currNewsGroupID;

	void setEventListener(ServerEventListener* listener);


	void init(Server& server);

	unsigned char readCommand(Connection* conn)
	throw(ConnectionClosedException);

	int readNumber(Connection* conn);

	void writeNumber(int num, string& s);

	int readIntParameter(Connection* conn) 
	throw(ConnectionClosedException);

	string readStringParameter(Connection* conn) 
	throw(ConnectionClosedException);


	void writeString(const string& s, Connection* conn)
	throw(ConnectionClosedException);

	void writeStringParameter(string param, string& ret);

	void listNewsGroups(const vector<Newsgroup*>& groups, Connection* conn) 
	throw(ConnectionClosedException);

	void listArticles(const vector<Newsgroup*>& groups, Connection* conn);

	void createNewsGroup(vector<Newsgroup*>& groups, Connection* conn, unsigned int& currNewsGroupID) 
	throw(ConnectionClosedException);

	void deleteNewsGroup(vector<Newsgroup*>& groups, Connection* conn) 
	throw(ConnectionClosedException);

	void createArticle(vector<Newsgroup*>& groups, Connection* conn);


	void deleteArticle(vector<Newsgroup*>& groups, Connection* conn);

	void getArticle(vector<Newsgroup*>& groups, Connection* conn);


private:
	ServerEventListener* listener;

};

#endif
