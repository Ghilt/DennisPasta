#include "newsgroup.h"
#include <string>
#include "article.h"

using namespace std;

namespace client_server {


        
    Newsgroup::~Newsgroup(){}

    void Newsgroup::createArticle(const string& title, const string& author, const string& text){
    	articles.push_back(new Article(title, author, text, currentArticleID++));
    }

    const string& Newsgroup::getName() const{
    	return name;
    }      

    int Newsgroup::getID() const{
    	return id;
    }

}