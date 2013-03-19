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
        Newsgroup(const string& n, int i) : name(n), id(i){
            currentArticleID = 1;
        }

        ~Newsgroup();
        
        void createArticle(const string& title, const string& author, const string& text);


    private:
        /* unique name */
        string name;
        /* The unique identification */
        int id;
        int currentArticleID;
        
        
        vector<Article*> articles;
        
    };
}

#endif