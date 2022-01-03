CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: EchoClient
	g++ -o bin/echoExample bin/connectionHandler.o bin/echoClient.o bin/writeTask.o bin/readTask.o $(LDFLAGS)

EchoClient: bin/connectionHandler.o bin/echoClient.o bin/readTask.o bin/writeTask.o

bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/readTask.o: src/ReadTask.cpp
	g++ $(CFLAGS) -o bin/readTask.o src/ReadTask.cpp

bin/writeTask.o: src/WriteTask.cpp
	g++ $(CFLAGS) -o bin/writeTask.o src/WriteTask.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp src
	
.PHONY: clean
clean:
	rm -f bin/*
