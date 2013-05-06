
#include "servereventdiskwriter.h"

#include "servercommandhandler.h"
#include <sys/types.h>
#include <sys/stat.h>


ServerEventDiskWriter::ServerEventDiskWriter(ServerCommandHandler& sch) : sch(sch) {
	//load
	vector<Newsgroup*>& groups = sch.groups;

	DIR *dir = opendir("./db/");
	if (dir) {
		dirent* dp = readdir(dir);

		while (dp != NULL) {
			if (strcmp(dp->d_name, "info") == 0) {

				ifstream in("./db/info");
				in >> sch.currNewsGroupID;
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
				closedir(group);
			}
			dp = readdir(dir);
		}
		closedir(dir);
	}

	std::sort(groups.begin(), groups.end(), [](const Newsgroup* lhs, const Newsgroup* rhs) {
		return lhs->getID() < rhs->getID();
	});


}

ServerEventDiskWriter::~ServerEventDiskWriter() {

}

void ServerEventDiskWriter::onCreatedArticle(Newsgroup* grp, Article* art) {

	stringstream ss;
	ss << "./db/";
	unsigned int id = grp->getID();
	
	ss << id;
	ofstream myfile;
	stringstream res;
	res << ss.str()+"/";
	res << art->getID();
	myfile.open(res.str().c_str());
	myfile << art->getTitle() << "\n" << art->getAuthor() << "\n" << art->getText();
	myfile.close(); 
	
	ss << "/info";
	myfile.open(ss.str().c_str());
	myfile << grp->getName() << "\n" << grp->getCurrentArticleId();
	myfile.close();
}


void ServerEventDiskWriter::onCreatedNewsgroup(Newsgroup* grp){

	stringstream ss;
	ss << "./db/";
	unsigned int id = grp->getID();
	
	ss << id;
	mkdir("./db/", S_IRWXU|S_IRGRP|S_IXGRP);
	mkdir(ss.str().c_str(), S_IRWXU|S_IRGRP|S_IXGRP);

	ofstream myfile;
	myfile.open ((ss.str() + "/info").c_str());
	myfile << grp->getName() << "\n" << grp->getCurrentArticleId();
	myfile.close(); 

	myfile.open("./db/info");
	myfile << sch.currNewsGroupID;
	myfile.close();
}

int ServerEventDiskWriter::remove_directory(string path){
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
					closedir(t);
				}else {
					unlink(s.c_str());
				}
			}
			dp = readdir(d);
		}
		closedir(d);
		rmdir(path.c_str());
	}

	return 0;
}

void ServerEventDiskWriter::onDeletedArticle(const unsigned int& grpId, const unsigned int& artId){
	stringstream ss;
	ss << "./db/";
			
	ss << grpId;
	ss << "/" << artId;

	unlink(ss.str().c_str());
}

void ServerEventDiskWriter::onDeletedNewsgroup(const unsigned int& grpId) {
	stringstream ss;
	ss << "./db/";
			
	ss << grpId;

	remove_directory(ss.str());
}
