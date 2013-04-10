/* myclient.cc: sample client program */

#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <iostream>
#include <string>
#include <cstdlib>    /* for exit() and atoi() */
#include <cassert>
#include <sstream>

using protocol::Protocol;
using namespace std;
using client_server::Connection;
using client_server::ConnectionClosedException;

#define detailedAssert(cond, mes) \
    if(!cond) { \
        cerr << mes << endl; \
        exit(1); \
    }

#define serverAssert(cond) \
    detailedAssert(cond, "Server has given an invalid response.")


class CommandReader
{
public:    

    CommandReader(string src) : ss(src) {
        
    }

    int readInt() {
        int ret;
        ss >> ret;

        detailedAssert(ss, "Expected id");
        return ret;
    }
    string readString() {
        string ret;
        ss >> ret;

        detailedAssert(ss, "Expected id");

        return ret;
    }
    string readLongString() {
        string ret("");
        bool first = true;
        while (ss) {
            string tmp;
            ss >> tmp;

            if (!ss)
                break;
            
            if (!first)
                ret += " ";
            first = false;

            ret += tmp;
        }
        detailedAssert(ret.size() > 0, "Expected name");
        return ret;
    }
private:
    stringstream ss;

};

int readNumber(Connection* conn) {
    unsigned char byte1 = conn->read();
    unsigned char byte2 = conn->read();
    unsigned char byte3 = conn->read();
    unsigned char byte4 = conn->read();
    return (byte1 << 24) | (byte2 << 16) | 
    (byte3 << 8) | byte4;
}

void writeString(const string& s, Connection* conn)
throw(ConnectionClosedException) {
    for (size_t i = 0; i < s.size(); ++i) {
        conn->write(s[i]);
    }
}


int readIntParameter(Connection* conn) 
throw(ConnectionClosedException){

    unsigned char pn = conn->read();
    serverAssert(pn == Protocol::PAR_NUM);

    return readNumber(conn);
}
string readStringParameter(Connection* conn) 
throw(ConnectionClosedException){

    unsigned char ps = conn->read();

    serverAssert(ps == Protocol::PAR_STRING);

    int num = readNumber(conn);

    string ret;
    for (int i = 0; i < num; ++i)
    {
        unsigned char c = conn->read();
        ret += c;
    }

    return ret;
}

void writeNumber(int num, string& s) {

#define w(off) { \
    unsigned char c = static_cast<unsigned char>(num >> off); \
    s += c; }

    w(24);
    w(16);
    w(8);
    w(0);
}



string listNewsgroup(Connection* conn, CommandReader& reader)
throw(ConnectionClosedException){
    (void)reader;

    string com;

    com += Protocol::COM_LIST_NG;
    com += Protocol::COM_END;
    writeString(com, conn);
    

    unsigned char ans = conn->read();

    serverAssert(ans == Protocol::ANS_LIST_NG);


    string ret;
    int numberOfNG = readIntParameter(conn);

    for(int i = 0; i < numberOfNG; ++i){
        string temp;
        int id = readIntParameter(conn);
        stringstream ss;
        ss << id;
        ss >> temp;
        ret += temp;
        ret += " ";
        ret += readStringParameter(conn);
        ret += "\n";

    }

    unsigned char end = conn->read();
    serverAssert(end == Protocol::ANS_END);
   
    return ret;
}

string deleteNewsgroup(Connection* conn, CommandReader& reader){
    string com;
    com += Protocol::COM_DELETE_NG;
    com += Protocol::PAR_NUM;
    string num = reader.readString();

    writeNumber(atoi(num.c_str()), com);
    com += Protocol::COM_END;

    writeString(com, conn);

    unsigned char ans = conn->read();
    serverAssert(ans == Protocol::ANS_DELETE_NG);
    
    string ret;

    unsigned char success = conn->read();

    if(success == Protocol::ANS_ACK){
        ret = "Newsgroup deleted successfully";
    } else {
        ret = "Error: ";
        unsigned char err = conn->read();
        serverAssert(err == Protocol::ERR_NG_DOES_NOT_EXIST);
        ret += "Newsgroup doesn't exist";
    }

    unsigned char end = conn->read();
    serverAssert(end == Protocol::ANS_END);

    return ret;

}
string listArticles(Connection* conn, CommandReader& reader){

    string com;
    com += Protocol::COM_LIST_ART;
    com += Protocol::PAR_NUM;
    string num = reader.readString();

    writeNumber(atoi(num.c_str()), com);
    com += Protocol::COM_END;

    writeString(com, conn);


    unsigned char ans = conn->read();
    serverAssert(ans == Protocol::ANS_LIST_ART);

    string ret;

    unsigned char success = conn->read();

    if(success == Protocol::ANS_ACK){

        unsigned int numberOfArts = readIntParameter(conn);

//if noa == 0 skriv ut
        for(size_t i = 0; i < numberOfArts; ++i){

            string temp;
            int id = readIntParameter(conn);
            stringstream ss;
            ss << id;
            ss >> temp;
            ret += temp;
            ret += " ";
            ret += readStringParameter(conn);
            ret += "\n";

        }

    } else {
        ret = "Error: ";
        unsigned char err = conn->read();
        serverAssert(err == Protocol::ERR_NG_DOES_NOT_EXIST);
        ret += "Newsgroup doesn't exist";
    }

    unsigned char end = conn->read();
    serverAssert(end == Protocol::ANS_END);

    return ret;




}
string createArticle(Connection* conn, CommandReader& reader){

    string com;
    com += Protocol::COM_CREATE_ART;
    com += Protocol::PAR_NUM;
    string num = reader.readString();
    writeNumber(atoi(num.c_str()), com);

#define r(type) \
    cout << "Article " #type ": "; \
    string type; \
    getline(cin, type); \
    com += Protocol::PAR_STRING; \
    writeNumber(type.size(), com); \
    com += type;

    r(title);
    r(author);
    r(text);

    com += Protocol::COM_END;

    writeString(com, conn);


    unsigned char ans = conn->read();
    serverAssert(ans == Protocol::ANS_CREATE_ART);

    string ret;

    unsigned char success = conn->read();

    if(success == Protocol::ANS_ACK){

        ret += "Article created successfully";

    } else {
        ret = "Error: ";
        unsigned char err = conn->read();
        serverAssert(err == Protocol::ERR_NG_DOES_NOT_EXIST);
        ret += "Newsgroup doesn't exist";
    }

    unsigned char end = conn->read();
    serverAssert(end == Protocol::ANS_END);

    return ret;

}
string deleteArticle(Connection* conn, CommandReader& reader){
    string com;
    com += Protocol::COM_DELETE_ART;

    for (int i=0; i<2; ++i){
        com += Protocol::PAR_NUM;
        writeNumber(reader.readInt(), com);
    }

    com += Protocol::COM_END;
    writeString(com, conn);

    unsigned char ans = conn->read();
    serverAssert(ans == Protocol::ANS_DELETE_ART);

    string ret;

    unsigned char success = conn->read();

    if(success == Protocol::ANS_ACK){

        ret += "Article deleted successfully";

    } else {
        ret = "Error: ";
        unsigned char err = conn->read();
        if(err == Protocol::ERR_NG_DOES_NOT_EXIST){
            ret += "Newsgroup doesn't exist";
        } else if (err == Protocol::ERR_ART_DOES_NOT_EXIST) {
            ret += "Article doesn't exist";
        } else {
            serverAssert(false);
        }
    }

    unsigned char end = conn->read();
    serverAssert(end == Protocol::ANS_END);

    return ret;
}
string getArticle(Connection* conn, CommandReader& reader){
    
    string com;
    com += Protocol::COM_GET_ART;
    
    for (int i=0; i<2; ++i){
        com += Protocol::PAR_NUM;
        writeNumber(reader.readInt(), com);
    }

    com += Protocol::COM_END;
    writeString(com, conn);


    unsigned char ans = conn->read();
    serverAssert(ans == Protocol::ANS_GET_ART);

    string ret;

    unsigned char success = conn->read();

    if(success == Protocol::ANS_ACK){

        ret += "Title: "   + readStringParameter(conn) + "\n";
        ret += "Author: "  + readStringParameter(conn) + "\n";
        ret += "Message: " + readStringParameter(conn) + "\n";

    } else {
        ret = "Error: ";
        unsigned char err = conn->read();
        if(err == Protocol::ERR_NG_DOES_NOT_EXIST){
            ret += "Newsgroup doesn't exist";
        } else if (err == Protocol::ERR_ART_DOES_NOT_EXIST) {
            ret += "Article doesn't exist";
        } else {
            serverAssert(false);
        }
    }

    unsigned char end = conn->read();
    serverAssert(end == Protocol::ANS_END);

    return ret;
}


string createNewsgroup(Connection* conn, CommandReader& reader)
throw(ConnectionClosedException){
    string com;
    com += Protocol::COM_CREATE_NG;
    com += Protocol::PAR_STRING;
    string ng_name = reader.readLongString();
    
    writeNumber(ng_name.size(), com);
    com += ng_name;
    com += Protocol::COM_END;

    writeString(com, conn);

    unsigned char ans = conn->read();

    serverAssert(ans == Protocol::ANS_CREATE_NG);

    string ret;
    unsigned char success = conn->read();

    
    if(success == Protocol::ANS_ACK){
        ret = "Newsgroup created successfully";
    } else {
        ret = "Error: ";
        unsigned char err = conn->read();
        serverAssert(err == Protocol::ERR_NG_DOES_NOT_EXIST);
        ret += "Newsgroup doesn't exist";
    }

    unsigned char end = conn->read();
    serverAssert(end == Protocol::ANS_END);

    return ret;
}



int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: newsclient host-name port-number" << endl;
        exit(1);
    }
    
    Connection* conn = new Connection(argv[1], atoi(argv[2]));
    if (! conn->isConnected()) {
        cerr << "Connection attempt failed" << endl;
        exit(1);
    }
    
    bool keepRunning = true;

    cout << "Type a command:" << endl <<
    "List Newsgroups:\t1" << endl <<
    "Create Newsgroup:\t2 <Name>" << endl <<
    "Delete Newsgroup:\t3 <Newsgroup ID>" << endl <<
    "List articles:\t\t4 <Newsgroup ID>" << endl <<
    "Create Article:\t\t5 <Newsgroup ID>" << endl <<
    "Delete Article:\t\t6 <Newsgroup ID, Article ID>" << endl <<
    "Get Article:\t\t7 <Newsgroup ID, Article ID>" << endl << 
    "Quit:\t\t\t8" << endl;
        cout << "> ";
    string line;
    while (keepRunning) {
        getline(cin, line);
        CommandReader cr(line);

        int nbr = cr.readInt();

        try{
            string ans;

            switch (nbr) {
                case Protocol::COM_LIST_NG:
                {
                    ans = listNewsgroup(conn, cr);
                }
                break;
                case Protocol::COM_CREATE_NG:
                {
                    ans = createNewsgroup(conn, cr);
                }
                break;
                case Protocol::COM_DELETE_NG:
                {
                   ans = deleteNewsgroup(conn, cr);
                }
                break;
                case Protocol::COM_LIST_ART:
                {
                   ans = listArticles(conn, cr);
                }
                break;
                case Protocol::COM_CREATE_ART:
                {
                    ans = createArticle(conn, cr);
                }
                break;
                case Protocol::COM_DELETE_ART:
                {
                    ans = deleteArticle(conn, cr);
                }
                break;
                case Protocol::COM_GET_ART:
                {
                    ans = getArticle(conn, cr);
                }
                break;
                case 8:
                {
                    keepRunning = false;
                    ans = "Quitting. Thanks for participating in this awesome newsgroup forum megamash dot com";
                }
                break;
                default:
                {
                    cerr << "Software do not recognise command \"" << nbr << "\", exiting. //Adam" << endl;
                    exit(1);
                }
                break;
            }



            cout << ans << endl;
        } catch (ConnectionClosedException&){
            cerr << "Server closed down!" << endl;
            exit(1);
        }
        cout << "Type a command:" << endl <<
    "List Newsgroups:\t1" << endl <<
    "Create Newsgroup:\t2 <Name>" << endl <<
    "Delete Newsgroup:\t3 <Newsgroup ID>" << endl <<
    "List articles:\t\t4 <Newsgroup ID>" << endl <<
    "Create Article:\t\t5 <Newsgroup ID>" << endl <<
    "Delete Article:\t\t6 <Newsgroup ID, Article ID>" << endl <<
    "Get Article:\t\t7 <Newsgroup ID, Article ID>" << endl << 
    "Quit:\t\t\t8" << endl;
        cout << "> ";
    }
    delete conn;
}

