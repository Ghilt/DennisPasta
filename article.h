#ifndef ARTICLE_H
#define ARTICLE_H

#include <vector>

namespace client_server {
    /* An Article object */
    class Article {
        
    public:
        /* Create an article */
        Article(const string& tit, const string& auth, const string& tex, int i) : title(tit), author(auth), text(tex), id(i) {

        }

        
        
    private:
        /* The unique identification */
        int id;

        /* title, author and article text */
        string title, author, text;
        
    };
}

#endif