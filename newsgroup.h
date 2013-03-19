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

        const string& getName() const;
        
        unsigned int getID() const; 

        const Article* operator[](unsigned int i) const;

        Article* operator[](unsigned int i);

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