#include "article.h"
#include <string>

using namespace std;

namespace client_server {


    const string& Article::getTitle() const{
        return title;
    }

    const string& Article::getAuthor() const{
        return author;
    }

    const string& Article::getText() const{
        return text;
    }

    int Article::getID() const {
        return id;
    }

        

}

/*

        title = tit;
        author = auth;
        text = tex;
        id = i;
        */