#include <stdlib.h>
#include <connectionHandler.h>
#include <writeTask.h>
#include <thread>
#include <mutex>
#include <readTask.h>

/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
//bool shouldTerminate = false;

extern bool shouldTerminate;

int main (int argc, char *argv[]) {

/*    bool flg = true;
    while (flg){
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string frame(buf);
        int len=frame.length();
        std::replace( frame.begin(), frame.end(), ' ', '\0');
        std::cout << frame << std::endl;
//    char *c = const_cast<char *>(frame.c_str());
        short num = 0;
        if(frame.find("REGISTER") == 0)
            num = 1;
        std::string s(frame.substr(7,frame.length()-7));
        std::cout << s << std::endl;
        if(num == 0)
            flg = false;
    }*/

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    //From here we will see the rest of the ehco client implementation:

    std::mutex mutex;

    ReadTask taskRead(&connectionHandler,1, mutex);
    WriteTask taskWrite(&connectionHandler,2, mutex);


    std::thread read(&ReadTask::run, &taskRead);
    std::thread write(&WriteTask::run, &taskWrite);

    while(!shouldTerminate);

    read.detach();
    write.detach();

    return 0;
}
