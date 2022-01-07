CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: BGSClient
	g++ -o bin/BGSClient bin/connectionHandler.o bin/BGSClient.o bin/writeTask.o bin/readTask.o $(LDFLAGS)

BGSClient: bin/connectionHandler.o bin/BGSClient.o bin/readTask.o bin/writeTask.o

bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/readTask.o: src/ReadTask.cpp
	g++ $(CFLAGS) -o bin/readTask.o src/ReadTask.cpp

bin/writeTask.o: src/WriteTask.cpp
	g++ $(CFLAGS) -o bin/writeTask.o src/WriteTask.cpp

bin/BGSClient.o: src/BGSClient.cpp
	g++ $(CFLAGS) -o bin/BGSClient.o src/BGSClient.cpp src
	
.PHONY: clean
clean:
	rm -f bin/*
