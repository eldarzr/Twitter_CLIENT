//
// Created by spl211 on 07/01/2022.
//

#ifndef BGSCLIENT_WRITETASK_H
#define BGSCLIENT_WRITETASK_H

#include "stdlib.h"
#include <connectionHandler.h>
#include <mutex>

extern bool shouldTerminate;

class WriteTask {
private:

    ConnectionHandler *_connectionHandler;

    int _id;

    std::mutex &_mutex;

public:
    WriteTask(ConnectionHandler *connectionHandler, int id, std::mutex &mutex);

    void run();

};

#endif //BGSCLIENT_WRITETASK_H
