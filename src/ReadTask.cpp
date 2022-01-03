//
// Created by spl211 on 28/12/2021.
//
#include <stdlib.h>
#include <connectionHandler.h>
#include <mutex>

extern bool shouldTerminate;

class ReadTask {
private:

    ConnectionHandler* _connectionHandler;

    int _id;

    std::mutex &_mutex;


public:


    ReadTask(ConnectionHandler* connectionHandler, int id, std::mutex &mutex) :
    _connectionHandler(connectionHandler), _id(id), _mutex(mutex) {}


    void run() {
        while (!shouldTerminate) {
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize);
            std::string line(buf);
            int len=line.length();
            if (!_connectionHandler->sendLine(line)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            // connectionHandler.sendLine(line) appends '\n' to the message. Therefor we send len+1 bytes.
            std::cout << "Sent " << len+1 << " bytes to server" << std::endl;
        }


    }

};