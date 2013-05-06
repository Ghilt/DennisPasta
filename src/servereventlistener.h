
#ifndef SERVER_EVENT_LISTENER_H
#define SERVER_EVENT_LISTENER_H 

#include "article.h"
#include "newsgroup.h"

using client_server::Article;
using client_server::Newsgroup;

class ServerEventListener {

public:
	
	virtual void onCreatedArticle(Newsgroup* grp, Article* a) = 0;
	virtual void onDeletedArticle(const unsigned int& grpId, const unsigned int& artId) = 0;
	
	virtual void onCreatedNewsgroup(Newsgroup* grp) = 0;
	virtual void onDeletedNewsgroup(const unsigned int& grpId) = 0;
};


#endif
