#include <connectionHandler.h>
 
using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::replace;

bool shouldTerminate;

ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){
    shouldTerminate = false;
}
    
ConnectionHandler::~ConnectionHandler() {
    close();
}
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to " 
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
            if(tmp <2)
            tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, ';');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, ';');
}
 
bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    // Stop when we encounter the null character. 
    // Notice that the null character is not appended to the frame string.
    try {
		do{
			if(!getBytes(&ch, 1))
                break;
            frame.append(1, ch);
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    short s = bytesToShort(const_cast<char *>(frame.c_str()));
    std::cout << s << std::endl;
    frame = frame.substr(2, frame.length()-2);
    if(s == 10 || s == 11){
        s = bytesToShort(const_cast<char *>(frame.c_str()));
        std::cout << s << std::endl;
        if(s == 7 || s == 8){
            for(int i =0; i<4; i++) {
                frame = frame.substr(2, frame.length() - 2);
                s = bytesToShort(const_cast<char *>(frame.c_str()));
                std::cout << s << std::endl;
            }
        }
        if(s == 3)
            shouldTerminate = true;
    }
    else if(s==9){
        //s = bytesToShort(const_cast<char *>(frame.c_str()));
        //std::cout << s << std::endl;
        frame = frame.substr(2, frame.length()-2);
        std::cout << frame << std::endl;
    }

    return true;
}
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
    std::string str(frame);
    std::replace(str.begin(), str.end(), ' ', '\0');
//    char *c = const_cast<char *>(frame.c_str());
    short num = 0;
    if(str.find("REGISTER") == 0) {
        num = 1;
        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
        str = str.substr(9, str.length() - 9);
        str = str + "\0";
    }
    else if(str.find("LOGIN") == 0) {
        num = 2;
        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
        str = str.substr(6, str.length() - 6);
        str = str + "\0";
    }
    else if(str.find("LOGOUT") == 0) {
        str = str.substr(6, str.length() - 6);
        //str = str + "\0";
        num = 3;
        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
    }
    else if(str.find("POST") == 0) {
        std::replace(str.begin(), str.end(), '\0', ' ');
        str = str.substr(5, str.length() - 5);
        //str = str + "\0";
        num = 5;
        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
    }
    else if(str.find("FOLLOW") == 0) {
        str = str.substr(7, str.length() - 7);
        str = str + "\0";
        num = 4;

        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
        chh = new char();
        num = str[0] - '0';
        shortToBytes(num,chh);
        sendBytes(chh,2);
        //sendBytes(";",1);
        str = str.substr(2, str.length() - 2);

/*        string n = "44";
        char *ch = const_cast<char *>(str.c_str());
        shortToBytes(num,ch);
        str = n + str;
        //sendBytes(str.c_str(),str.length());
        num = 4;*/
    }
    else if(str.find("BLOCK") == 0) {
        num = 12;
        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
        str = str.substr(6, str.length() - 6);
        str = str + "\0";
    }
    else if(str.find("LOGSTAT") == 0) {
        num = 7;
        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
        str = "";
        //str = str + "\0";
    }
    else if(str.find("STAT") == 0) {
        num = 8;
        char* chh = new char();
        shortToBytes(num,chh);
        sendBytes(chh,2);
        str = str.substr(5, str.length() - 5);
        str = str + "\0";
    }
    //char *c = const_cast<char *>(str.c_str());
    //shortToBytes(num,c);
    bool result=sendBytes(str.c_str(),str.length());
    if(!result) return false;
    return sendBytes(";",1);
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}


short ConnectionHandler::bytesToShort(char* bytesArr)
{
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

void ConnectionHandler::shortToBytes(short num, char* bytesArr)
{
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}
