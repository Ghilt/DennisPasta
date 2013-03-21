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
    assert(pn == Protocol::PAR_NUM);

    return readNumber(conn);
}
string readStringParameter(Connection* conn) 
throw(ConnectionClosedException){

    unsigned char ps = conn->read();

    assert(ps == Protocol::PAR_STRING);

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

string listNewsgroup(Connection* conn)
throw(ConnectionClosedException){
    string com;

    com += Protocol::COM_LIST_NG;
    com += Protocol::COM_END;
    writeString(com, conn);
    

    unsigned char ans = conn->read();
    assert(ans == Protocol::ANS_LIST_NG);


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
    assert(end == Protocol::ANS_END);
   
    return ret;
}

string deleteNewsgroup(Connection* conn){
    conn->read();
    return "";

}
string listArticles(Connection* conn){
    conn->read();
    return "";
    
}
string createArticle(Connection* conn){
    conn->read();
    return "";
    
}
string deleteArticle(Connection* conn){

    conn->read();
    return "";    
}
string getArticle(Connection* conn){

    conn->read();
    return "";
}


string createNewsgroup(Connection* conn)
throw(ConnectionClosedException){
    string com;
    com += Protocol::COM_CREATE_NG;
    com += Protocol::PAR_STRING;
    string ng_name, num;

    getline(cin, ng_name);

    ng_name = ng_name.substr(1,ng_name.size()-1);


    writeNumber(ng_name.size(), num);
    com += num;
    com += ng_name;
    com += Protocol::COM_END;

    writeString(com, conn);

    unsigned char ans = conn->read();
    assert(ans == Protocol::ANS_CREATE_NG);
    
    string ret;

    unsigned char success = conn->read();

    if(success == Protocol::ANS_ACK){
        ret = "Newsgroup created successfully";
    } else {
        ret = "Error is: ";
        unsigned char err = conn->read();
        if(err == Protocol::ERR_NG_ALREADY_EXISTS){
            ret += "Newsgroup already exists";
        } else{
            ret += "dunno :(";
        }
    }

    unsigned char end = conn->read();
    assert(end == Protocol::ANS_END);

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
    
    cout << "Type a command: ";
    int nbr;
    while (cin >> nbr) {

        try{
            string ans;

            switch (nbr) {
                case Protocol::COM_LIST_NG:
                {
                    ans = listNewsgroup(conn);
                }
                break;
                case Protocol::COM_CREATE_NG:
                {
                    ans = createNewsgroup(conn);
                }
                break;
                case Protocol::COM_DELETE_NG:
                {
                   ans = deleteNewsgroup(conn);
                }
                break;
                case Protocol::COM_LIST_ART:
                {
                   ans = listArticles(conn);
                }
                break;
                case Protocol::COM_CREATE_ART:
                {
                    ans = createArticle(conn);
                }
                break;
                case Protocol::COM_DELETE_ART:
                {
                    ans = deleteArticle(conn);
                }
                break;
                case Protocol::COM_GET_ART:
                {
                    ans = getArticle(conn);
                }
                break;
                default:
                {
                    cerr << "Software do not recognise command" << nbr << ", exiting. //Adam" << endl;
                    exit(1);
                }
                break;
            }



            cout << ans << endl;
        } catch (ConnectionClosedException&){
            cerr << "Server closed down!" << endl;
            exit(1);
        }
        cout << "Type a command: ";
    }
}

