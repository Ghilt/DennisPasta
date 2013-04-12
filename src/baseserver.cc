#include "ServerCommandHandler.h"

unsigned int currNewsGroupID = 0;


int main(int argc, char* argv[]){
	if (argc != 2) {
		cerr << "Usage: baseserver port-number" << endl;
		exit(1);
	}
	Server server(atoi(argv[1]));

	if (! server.isReady()) {
		cerr << "Server initialization error" << endl;
		exit(1);
	}
	ServerCommandHandler sch;
	vector<Newsgroup*> groups;

	currNewsGroupID = 0;

	while (true) {
		Connection* conn = server.waitForActivity();
		if (conn != 0) {
			try {
				unsigned int nbr = sch.readCommand(conn);

				try {
					switch (nbr) {
						case Protocol::COM_LIST_NG:
						{
							sch.listNewsGroups(groups, conn);
						}
							break;
						case Protocol::COM_CREATE_NG:
						{
							sch.createNewsGroup(groups, conn, currNewsGroupID);
						}
							break;
						case Protocol::COM_DELETE_NG:
						{
							sch.deleteNewsGroup(groups, conn);
						}
							break;
						case Protocol::COM_LIST_ART:
						{
							sch.listArticles(groups, conn);
						}
							break;
						case Protocol::COM_CREATE_ART:
						{
							sch.createArticle(groups,conn);
						}
							break;
						case Protocol::COM_DELETE_ART:
						{
							sch.deleteArticle(groups,conn);
						}
							break;
						case Protocol::COM_GET_ART:
						{
							sch.getArticle(groups,conn);
						}
							break;
						default:
						{
							cerr << "NewsServer recieved unrecognized code " << nbr << ", exiting. " << endl;
							clientAssert(false);
						}
							break;
					}

					unsigned int end = sch.readCommand(conn);

					clientAssert(end == Protocol::COM_END);

				} catch (naughty_client& cli) {
					cout << "Naughty client caught! You going to jail son." << endl;
					server.deregisterConnection(conn);
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


