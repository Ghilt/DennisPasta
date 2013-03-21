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
        articles.push_back(new Article(title, author, text, currentArticleID++));
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

    const Article* Newsgroup::operator[](unsigned int i) const{

    	for(auto it = articles.begin(); it != articles.end(); ++it){
    		unsigned int art_id = (*it)->getID();
    		if(art_id == i)
    			return (*it);
    	}

    	return 0;
    }

    Article* Newsgroup::operator[](unsigned int i){

    	for(auto it = articles.begin(); it != articles.end(); ++it){
    		unsigned int art_id = (*it)->getID();
    		if(art_id == i)
    			return (*it);
    	}

    	return 0;
    }

    int Newsgroup::size() const{
        return articles.size();
    }

    Article* Newsgroup::getArticle(int index) const{
        return articles[index];
    }
}