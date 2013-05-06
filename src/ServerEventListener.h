
#ifndef SERVER_EVENT_LISTENER_H
#define SERVER_EVENT_LISTENER_H 

#include "article.h"
#include "newsgroup.h"

using client_server::Article;
using client_server::Newsgroup;

class ServerEventListener {

public:
	
	virtual void onCreatedArticle(Newsgroup* grp, Article* a);
	virtual void onDeletedArticle(const unsigned int& grpId, const unsigned int& artId);
	
	virtual void onCreatedNewsgroup(Newsgroup* grp);
	virtual void onDeletedNewsgroup(const unsigned int& grpId);
};


#endif
