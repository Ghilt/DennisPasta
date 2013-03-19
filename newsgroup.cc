#include "newsgroup.h"
#include <string>
#include "article.h"

using namespace std;

namespace client_server {


        
    Newsgroup::~Newsgroup(){
    	for(auto it = articles.begin(); it != articles.end(); ++it){
    		delete (*it);
    	}
    }

    void Newsgroup::createArticle(const string& title, const string& author, const string& text){
    	articles.push_back(new Article(title, author, text, currentArticleID++));
    }

    const string& Newsgroup::getName() const{
    	return name;
    }      

    unsigned int Newsgroup::getID() const{
    	return id;
    }

    const Article* Newsgroup::operator[](unsigned int i) const{

    	for(auto it = articles.begin(); it != articles.end(); ++it){
    		if((*it)->getID() == i)
    			return (*it);
    	}

    	return 0;
    }

    Article* Newsgroup::operator[](unsigned int i){

    	for(auto it = articles.begin(); it != articles.end(); ++it){
    		if((*it)->getID() == i)
    			return (*it);
    	}

    	return 0;
    }


}