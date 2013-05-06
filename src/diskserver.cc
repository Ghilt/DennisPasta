#include "servercommandhandler.h"
//#include "ServerEventDiskWriter.h"

int main(int argc, char* argv[]){
	if (argc != 2) {
		cerr << "Usage: diskserver port-number" << endl;
		exit(1);
	}
	Server server(atoi(argv[1]));

	if (! server.isReady()) {
		cerr << "Server initialization error" << endl;
		exit(1);
	}
	ServerCommandHandler sch;
	//ServerEventListener* writer = new ServerEventDiskWriter(sch);
	//sch.setEventListener(NULL);
	sch.init(server);
}

