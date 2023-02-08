#include "../include/WriteTask.h"
#include<sstream>
#include <string.h>
using namespace std;

WriteTask::WriteTask(ConnectionHandler& connectionHandler, StompProtocol& protocol,atomic<bool>& isDone):_connectionHandler(connectionHandler),_protocol(protocol),_isDone(isDone){
	 
}

void WriteTask::run(){
	//From here we will see the rest of the ehco client implementation:
        while (1) {
            const short bufsize = 1024;
            char buf[bufsize];
            if(_isDone) break;
            cin.getline(buf, bufsize);
            string line(buf);
            if(_isDone) break;
            
            string frame = _protocol.processWrite(line);

            if (!_connectionHandler.sendLine(frame)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            
        }
    }
