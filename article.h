#ifndef ARTICLE_H
#define ARTICLE_H

#include <vector>
#include <string>

using namespace std;

namespace client_server {
    /* An Article object */
    class Article {
        
    public:
        /* Create an article */
        Article(const string& tit, const string& auth, const string& tex, int i) : title(tit), author(auth), text(tex), id(i){}

        const string& getTitle() const;
        const string& getAuthor() const;
        const string& getText() const;
        
        int getID() const;
        
    private:
        

        /* title, author and article text */
        string title;
        string author;
        string text;

        /* The unique identification */
        int id;
        
    };
}

#endif