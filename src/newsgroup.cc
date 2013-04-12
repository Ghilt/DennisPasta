#include "newsgroup.h"
#include <string>
#include <algorithm>
#include <iostream>


using namespace std;

namespace client_server {


        
    Newsgroup::~Newsgroup(){
    	for(auto it = articles.begin(); it != articles.end(); ++it){
    		delete (*it);
    	}
    }

    void Newsgroup::createArticle(const string& text, const string& author, const string& title){
        insertArticle(new Article(title, author, text, currentArticleID++));
    }
    void Newsgroup::insertArticle(Article* art) {
        articles.push_back(art);

	sort(articles.begin(), articles.end(), [](const Article* lhs, const Article* rhs) {
		return lhs->getID() < rhs->getID();
	});
    }

    void Newsgroup::deleteArticle(const unsigned int id){
        auto it = find_if(articles.begin(), articles.end(), [id](Article* a)->bool {
            return (a->getID() == id);
        });
        if(it!=articles.end()){
            articles.erase(it);
        }
    }


    const string& Newsgroup::getName() const{
    	return name;
    }      

    unsigned int Newsgroup::getID() const{
    	return id;
    }

    Article* Newsgroup::operator[](unsigned int pos){
        return articles[pos];
    }

    int Newsgroup::size() const{
        return articles.size();
    }

    Article* Newsgroup::getArticle(unsigned int id) const{
        auto it = find_if(articles.begin(), articles.end(), [id](Article* a) {
            return a->getID() == id;
        });
        if (it == articles.end())
            return NULL;
        else
            return *it;
    }

    unsigned int Newsgroup::getCurrentArticleId() const{
        return currentArticleID;
    }
}

