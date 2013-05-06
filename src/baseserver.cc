#include "servercommandhandler.h"


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
	sch.init(server);
}
