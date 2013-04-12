#include "servercommandhandler.h"

#include <sys/types.h>
#include <sys/stat.h>

unsigned int currNewsGroupID = 0;

int remove_directory(string path){
	DIR *d = opendir(path.c_str());

	if (d){
		dirent* dp = readdir(d);

		while (dp != NULL) {
			if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){					
				string s = path + "/" +dp->d_name;
				DIR *t = opendir(s.c_str());
				if(t){
					remove_directory(s.c_str());
					rmdir(s.c_str());
				}else {
					unlink(s.c_str());
				}
				closedir(t);
			}
			dp = readdir(d);
		}
		closedir(d);
	}

	return 0;
}

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
	vector<Newsgroup*> groups;

	currNewsGroupID = 0;
	//load
 
	DIR *dir = opendir("./db/");
	if (dir) {
		dirent* dp = readdir(dir);

		while (dp != NULL) {
			if (strcmp(dp->d_name, "info") == 0) {

				ifstream in("./db/info");
				in >> currNewsGroupID;
				in.close();

			} else if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) { //mapp

				int id = atoi(dp->d_name);
				stringstream groupDir;
				groupDir << "./db/";
				groupDir << dp->d_name;
				
				DIR *group = opendir(groupDir.str().c_str());
				groupDir << "/info";
				ifstream nameStream(groupDir.str().c_str());
				string name;
				string idPos;
				getline(nameStream, name);
				getline(nameStream, idPos);
				
				nameStream.close();

				Newsgroup* ng = new Newsgroup(name, id, atoi(idPos.c_str()));


				dirent* article = readdir(group);

				while (article != NULL) {

					if (strcmp(article->d_name, "info") != 0 &&
						strcmp(article->d_name, ".") != 0 &&
						strcmp(article->d_name, "..") != 0) {
						string file = "./db/";
						file += dp->d_name;
						file += "/";
						file += article->d_name;

						ifstream articleFile(file.c_str());

						string title;
						string author;
						string text;
						getline(articleFile, title);
						getline(articleFile, author);

						string textBuffer;
						while (getline(articleFile, textBuffer)) {
							text += textBuffer;
							text += "\n";
						}

						Article* art = new Article(title, author, text, atoi(article->d_name));
						ng->insertArticle(art);
					}
					article = readdir(group);
				}

				groups.push_back(ng);
			}

			dp = readdir(dir);
		}
		closedir(dir);
	}

	std::sort(groups.begin(), groups.end(), [](const Newsgroup* lhs, const Newsgroup* rhs) {
		return lhs->getID() < rhs->getID();
	});


	int num = 5;
	while (num-- > 0) {
		Connection* conn = server.waitForActivity();
		if (conn != 0) {
			try {
				unsigned int nbr = sch.readCommand(conn);
				bool shouldSave = false;

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
							shouldSave = true;
						}
							break;
						case Protocol::COM_DELETE_NG:
						{
							sch.deleteNewsGroup(groups, conn);
							shouldSave = true;
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
							shouldSave = true;
						}
							break;
						case Protocol::COM_DELETE_ART:
						{
							sch.deleteArticle(groups,conn);
							shouldSave = true;
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
				
				//save

				if (shouldSave) {
					remove_directory("./db/");
					mkdir("./db/", S_IRWXU|S_IRGRP|S_IXGRP);
					ofstream myfile;
  					myfile.open ("./db/info");
 					myfile << currNewsGroupID;
  					myfile.close(); 

					for (auto it = groups.begin(); it != groups.end(); ++it) {
						Newsgroup* grp = *it;
						stringstream ss;

						ss << "./db/";
						unsigned int id = grp->getID();
						
						ss << id;
						mkdir(ss.str().c_str(), S_IRWXU|S_IRGRP|S_IXGRP);

						ofstream myfile;
  						myfile.open ((ss.str() + "/info").c_str());
 						myfile << grp->getName() << "\n" << grp->getCurrentArticleId();
  						myfile.close(); 						
						

  						for(int i = 0; i < grp->size(); ++i ){
  							Article *a = (*grp)[i];

  							ofstream myfile;
  							stringstream res;

  							
  							res << ss.str()+"/";
  							res << a->getID();
  							myfile.open(res.str().c_str());
 							myfile << a->getTitle() << "\n" << a->getAuthor() << "\n" << a->getText();
  							myfile.close(); 
  						}


					}

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

	for (unsigned int i=0; i<groups.size(); ++i) {
		Newsgroup* ng = groups[i];
		delete ng;
	}
}


