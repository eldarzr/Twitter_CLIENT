//
// Created by spl211 on 07/01/2022.
//

#include "../include/BidiProtocol.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::replace;

bool shouldTerminate;

BidiProtocol::BidiProtocol(ConnectionHandler _connectionHandler): _connectionHandler(_connectionHandler) {}

void BidiProtocol::send(const std::string &frame, char delimiter) {

    std::string str(frame);
    //std::replace(str.begin(), str.end(), ' ', '\0');
    short num = 0;
    if(str.find("REGISTER") == 0) {
        num = 1;
        str = str.substr(9, str.length() - 9);
        str = str + "\0";
    }
    else if(str.find("LOGIN") == 0) {
        num = 2;
        str = str.substr(6, str.length() - 6);
        str = str + "\0";
    }
    else if(str.find("LOGOUT") == 0) {
        str = str.substr(6, str.length() - 6);
        num = 3;
    }
    else if(str.find("POST") == 0) {
        std::replace(str.begin(), str.end(), '\0', ' ');
        str = str.substr(5, str.length() - 5);
        num = 5;]
    }
    else if(str.find("FOLLOW") == 0) {
        str = str.substr(7, str.length() - 7);
        str = str + "\0";
        num = 4;

        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
        num = str[0] - '0';
        str = str.substr(2, str.length() - 2);

    }
    else if(str.find("BLOCK") == 0) {
        num = 12;
        str = str.substr(6, str.length() - 6);
        str = str + "\0";
    }
    else if(str.find("LOGSTAT") == 0) {
        num = 7;
        str = "";
    }
    else if(str.find("STAT") == 0) {
        num = 8;
        str = str.substr(5, str.length() - 5);
        str = str + "\0";
    }
    char* chh = new char();
    shortToBytes(num,chh);
    sendBytes(chh,2);
    bool result=sendBytes(str.c_str(),str.length());
    if(!result) return false;
    return sendBytes(";",1);
}