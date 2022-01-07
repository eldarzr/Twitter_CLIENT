//
// Created by spl211 on 28/12/2021.
//
#include <../include/writeTask.h>

    WriteTask::WriteTask(ConnectionHandler* connectionHandler, int id, std::mutex &mutex) :
    _connectionHandler(connectionHandler), _id(id), _mutex(mutex) {}


    void WriteTask::run() {
        while (!shouldTerminate) {
            //const short bufsize = 1024;
            //char buf[bufsize];
            std::string answer;
            // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
            // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
            if (!_connectionHandler->getLine(answer)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }

            int len=(int)answer.length();
            // A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
            // we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
            answer.resize(len-1);
            //std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
        }



    }
