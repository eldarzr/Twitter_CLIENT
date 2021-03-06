#include <connectionHandler.h>
#include <chrono>


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
    //std::cout << s << std::endl;
    frame = frame.substr(2, frame.length()-2);
    if(s == 10)
        ack(frame,delimiter);
    else if(s == 11)
        error(frame,delimiter);
/*    if(s == 10 || s == 11){
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
    }*/
    else if(s==9){
        notification(frame,delimiter);
        //s = bytesToShort(const_cast<char *>(frame.c_str()));
        //std::cout << s << std::endl;
/*        frame = frame.substr(2, frame.length()-2);
        std::cout << frame << std::endl;*/
    }

    return true;
}

bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
    std::string str(frame);
    std::replace(str.begin(), str.end(), ' ', '\0');
    short num = 0;
    int captcha = -1;
    bool back = false;
    if(str.find("REGISTER") == 0) {
        num = 1;
        str = str.substr(9, str.length() - 9);
        //str = str + "\0";
        back = true;
    }
    else if(str.find("LOGIN") == 0) {
        num = 2;
        str = str.substr(6, str.length() - 6);
        //str = str + "\0";
        back = true;
        if(str[str.length()-1] == '1')
            captcha = 1;
        else captcha = 0;
        str = str.substr(0, str.length() - 2);
    }
    else if(str.find("LOGOUT") == 0) {
        str = str.substr(6, str.length() - 6);
        num = 3;
    }
    else if(str.find("POST") == 0) {
        std::replace(str.begin(), str.end(), '\0', ' ');
        str = str.substr(5, str.length() - 5);
        num = 5;
    }
    else if(str.find("PM") == 0) {
        std::replace(str.begin(), str.end(), '\0', ' ');
        str = str.substr(3, str.length() - 3);
        std::time_t now;
        int a = str.find(' ');
        str[a] = '\0';

        now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string s(30, '\0');
        std::strftime(&s[0], s.size(), "%d-%m-%Y", std::localtime(&now));
        s = s.substr(0,10);
        str = str + '\0' +s+ '\0';
        num = 6;
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
        //str = str + "\0";
        back = true;
    }
    else if(str.find("LOGSTAT") == 0) {
        num = 7;
        str = "";
    }
    else if(str.find("STAT") == 0) {
        num = 8;
        str = str.substr(5, str.length() - 5);
        //str = str + "\0";
        back = true;
    }
    char* chh = new char();
    shortToBytes(num,chh);
    sendBytes(chh,2);
    bool result=sendBytes(str.c_str(),str.length());
    if(!result) return false;
    if(back) sendBytes("\0",1);
    if(captcha == 1) sendBytes("\1",1);
    else if(captcha == 0)  sendBytes("\0",1);
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

void ConnectionHandler::ack(std::string& frame, char delimiter) {
    std::cout << "ACK ";
    short s = bytesToShort(const_cast<char *>(frame.c_str()));
    std::cout << s;
    frame = frame.substr(2, frame.length() - 2);

    if (s == 7 || s == 8) {
        for (int i = 0; i < 4; i++) {
            s = bytesToShort(const_cast<char *>(frame.c_str()));
            frame = frame.substr(2, frame.length() - 2);
            std::cout << " " << s;
        }
    }
    if (s == 3)
        shouldTerminate = true;
    std::cout << std::endl;
}

void ConnectionHandler::error(std::string& frame, char delimiter){
    std::cout << "ERROR ";
    short s = bytesToShort(const_cast<char *>(frame.c_str()));
    std::cout << s << std::endl;
}

void ConnectionHandler::notification(std::string& frame, char delimiter){
    std::cout << "NOTIFICATION ";
    if(frame[0] == '\0')
        std::cout << "public ";
    else if(frame[0] == '\1')
        std::cout << "PM ";
    frame = frame.substr(1, frame.length()-3);
    std::replace(frame.begin(), frame.end(), '\0', ' ');
    std::cout << frame << std::endl;
}
