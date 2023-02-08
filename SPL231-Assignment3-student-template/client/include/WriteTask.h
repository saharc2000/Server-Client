#pragma once

#include <iostream>
#include <atomic>
#include <thread>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"


class WriteTask{
private:
    ConnectionHandler &_connectionHandler;
    StompProtocol &_protocol;
    std::atomic<bool>& _isDone;

public:
    WriteTask(ConnectionHandler &connectionHandler,StompProtocol &protocol,std::atomic<bool>& isDone);
    void run();
    //void setIsDone();
};