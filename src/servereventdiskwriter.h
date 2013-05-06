#ifndef SERVER_EVENT_DISK_WRITER
#define SERVER_EVENT_DISK_WRITER

#include "servereventlistener.h"

class ServerCommandHandler;

class ServerEventDiskWriter : public ServerEventListener{

public:
	ServerEventDiskWriter(ServerCommandHandler&);
	~ServerEventDiskWriter();

	virtual void onCreatedArticle(Newsgroup* grp, Article* art);

	virtual void onCreatedNewsgroup(Newsgroup* grp);

	virtual void onDeletedArticle(const unsigned int& grpId, const unsigned int& artId);
	virtual void onDeletedNewsgroup(const unsigned int& grpId);

private:
	ServerCommandHandler& sch;
	int remove_directory(string path);

};

#endif
