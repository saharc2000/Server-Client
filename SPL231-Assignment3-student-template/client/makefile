CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all:StompWCIClient

StompWCIClient: bin/ConnectionHandler.o bin/WriteTask.o bin/StompProtocol.o bin/event.o bin/UserSummary.o bin/StompClient.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/WriteTask.o bin/StompProtocol.o bin/event.o bin/UserSummary.o bin/StompClient.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/WriteTask.o: src/WriteTask.cpp
	g++ $(CFLAGS) -o bin/WriteTask.o src/WriteTask.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/UserSummary.o: src/UserSummary.cpp
	g++ $(CFLAGS) -o bin/UserSummary.o src/UserSummary.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

.PHONY: clean
clean:
	rm -f bin/*
	