#include "article.h"
#include <string>

using namespace std;

namespace client_server {


    string Article::getTitle() const{
        return title;
    }

    string Article::getAuthor() const{
        return author;
    }

    string Article::getText() const{
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