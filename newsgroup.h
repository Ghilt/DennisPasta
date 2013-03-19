#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <vector>

namespace client_server {
    /* An Article object */
    class Newsgroup {
        
    public:
        /* Create an article */
        Newsgroup(const string& n, int i) : name(n), id(i){}
        
        
    private:
        /* The unique identification */
        int id;

        /* unique name */
        string name;
        vector<Article> articles;
        
    };
}

#endif