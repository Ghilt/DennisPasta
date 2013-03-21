#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <vector>
#include <string>

#include "article.h"

using namespace std;

namespace client_server {
    /* An Article object */
    class Newsgroup {
        
    public:
        /* Create an article */
        Newsgroup(const string& n, unsigned int i) : name(n), id(i){
            currentArticleID = 1;
        }

        ~Newsgroup();
        
        void createArticle(const string& title, const string& author, const string& text);

        void deleteArticle(const unsigned int id);

        const string& getName() const;
        
        unsigned int getID() const; 

        Article* operator[](unsigned int index);

        int size() const;

        Article* getArticle(unsigned int id) const;

    private:
        /* unique name */
        string name;
        /* The unique identification */
        unsigned int id;
        unsigned int currentArticleID;
        
        
        vector<Article*> articles;
        
    };
}

#endif