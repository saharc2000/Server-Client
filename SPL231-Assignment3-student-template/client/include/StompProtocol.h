#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include "../include/UserSummary.h"
#include <mutex>
#include <unordered_map>

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    int subId;
    int receiptId;
    ConnectionHandler &connectionHandler;
    std::unordered_map<std::string,int> mapSubIdToTopic;
    std::string requestedUser="";
    std::string approvedUser="";
    std::unordered_map<std::string,UserSummary> mapUserToUSummary;
    std::unordered_map <std::string,std::string> mapReceiptIdToMsg;
    std::unordered_map <std::string,std::string> mapReceiptIdToSubId;
    std::string convertEventToMessage(Event e);
    std::string convertEventToSummary(Event e);
    std::string addReceiptToFrame(std::string subject);
    std::string createConnectionFrame(std::string msg);
    std::string createSubscribeFrame(std::string msg);
    std::string createUnSubscribeFrame(std::string msg);
    std::string createReportFrame(std::string file);
    std::string createSummaryFrame(std::string msg);
    std::string createDisConnectFrame();
    std::string handleReceiptSubOrUnsub(std::string receiptIdFromFrame);
    std::string handleConnected();
    std::string handleMessage(std::string msg);
    std::string handleReceipt(std::string message);
    std::string handleError(std::string message);


public:
    StompProtocol(ConnectionHandler &_connectionHandler);

    std::string processRead(std::string message);
    std::string processWrite(std::string message);

};
