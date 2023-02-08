#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/WriteTask.h"
#include "../include/StompProtocol.h"
#include <iostream>
#include <atomic>
#include <thread>
#include<sstream>
#include <string.h>
#include <unordered_map>
using namespace std;

int main(int argc, char *argv[]) {
    while(1){
        const short bufsize = 1024;
        char buf[bufsize];
        std::cout << "send login command to connect" << std::endl;
        cin.getline(buf, bufsize);
        string line(buf);
        int pos = line.find(" ");
        string command = line.substr(0,pos);
        if(command.compare("login")!=0)
            continue;
        string hostPort = line;
        hostPort = hostPort.erase(0,pos+1);
        pos = hostPort.find(":");
        string host = hostPort.substr(0,pos);
        hostPort = hostPort.erase(0,pos+1);
        pos = hostPort.find(" ");
        short port = stoi(hostPort.substr(0,pos));
        hostPort = hostPort.erase(0,pos);
        ConnectionHandler connectionHandler(host, port);
        if (!connectionHandler.connect()) {
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
            continue;
        }
        StompProtocol protocol(connectionHandler);
        line = protocol.processWrite(line);

        if (!connectionHandler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            continue;
        } 
        std::atomic<bool> isDone(false);
        WriteTask task1(connectionHandler,protocol,isDone);
        thread th1(&WriteTask::run,&task1);

        while (1) {
            string answer;
            if(!connectionHandler.getLine(answer)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                isDone=true;
                th1.join();
                break;
            }
            answer = protocol.processRead(answer);        
            if (answer.substr(0,3) == "bye") {
                std::cout << answer.substr(3,answer.size()) << std::endl;
                std::cout << "Exiting..." << std::endl;
                isDone=true;
                th1.join();
                break;
            }
            std::cout << answer <<"\n"; 
        }
    }
    return 0;
}