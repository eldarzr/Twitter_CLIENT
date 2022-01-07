//
// Created by spl211 on 07/01/2022.
//


#ifndef BOOST_ECHO_CLIENT_BIDIPROTOCOL_H
#define BOOST_ECHO_CLIENT_BIDIPROTOCOL_H

#include "connectionHandler.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class BidiProtocol {
public:
    void send(const std::string& frame, char delimiter);


private:
    connectionHandler _connectionHandler;
public:
    BidiProtocol(connectionHandler _connectionHandler);
};
#endif //BOOST_ECHO_CLIENT_BIDIPROTOCOL_H
