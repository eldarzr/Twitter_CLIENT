//
// Created by spl211 on 07/01/2022.
//

#ifndef BGSCLIENT_READTASK_H
#define BGSCLIENT_READTASK_H

#include "stdlib.h"
#include <connectionHandler.h>
#include <mutex>

extern bool shouldTerminate;

class ReadTask {
private:

    ConnectionHandler *_connectionHandler;

    int _id;

    std::mutex &_mutex;

public:
    ReadTask(ConnectionHandler *connectionHandler, int id, std::mutex &mutex);

    void run();

};
#endif //BGSCLIENT_READTASK_H
