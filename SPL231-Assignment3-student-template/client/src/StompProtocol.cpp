#include "../include/StompProtocol.h"
#include <iostream>
#include <fstream>
using namespace std;

StompProtocol::StompProtocol(ConnectionHandler &_connectionHandler):subId(0), receiptId(0),connectionHandler(_connectionHandler), mapSubIdToTopic(),mapUserToUSummary(),mapReceiptIdToMsg(),mapReceiptIdToSubId(){}

string StompProtocol::convertEventToMessage(Event e){
	string output = "event name: "+e.get_name()+"\ntime: "+std::to_string(e.get_time())+"\ngeneral game updates:\n";
	map<string,string> map = e.get_game_updates();
	if(map.size()>0){
		for(std::map<string,string>::iterator it=map.begin();it!=map.end();it++){
			output += "	" + it->first + ": " + it->second + "\n";
		}
	}
	output += "team a updates: \n";
	map = e.get_team_a_updates();
	if(map.size()>0){
		for(std::map<string,string>::iterator it=map.begin();it!=map.end();++it){
			output += "	" + it->first + ": " + it->second + "\n";
		}
	}
	output += "team b updates: \n";
	map = e.get_team_b_updates();
	if(map.size()>0){
		for(std::map<string,string>::iterator it=map.begin();it!=map.end();++it){
			output += "	" + it->first + ": " + it->second + "\n";
		}
	}
	output += "description:\n" + e.get_discription()+"\n";
	return output;
}

string StompProtocol::convertEventToSummary(Event e){
	string output = "";
	output = "Game stats:\nGeneral stats:\n";
	for(auto stat:e.get_game_updates()){
		output += stat.first+": "+stat.second+"\n";
	}
	output += e.get_team_a_name()+" stats:\n";
	for(auto stat:e.get_team_a_updates()){
		output += stat.first+": "+stat.second+"\n";
	}
	output += e.get_team_b_name()+" stats:\n";
	for(auto stat:e.get_team_b_updates()){
		output += stat.first+": "+stat.second+"\n";
	}
	output += "Game events reports:\n" + e.get_discription();
	return output;
}

string StompProtocol::createConnectionFrame(string msg){
	if(approvedUser!=""){
		std::cout << "The client is already logged in, log out before trying again" << std::endl;
		return"";
	}
	string output="CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\n";
	istringstream sd(msg);
	string del;
	int i=0;
	while(getline(sd, del, ' ')) {
		if(i==2){
			requestedUser = del;
			output+="login:"+requestedUser+"\n";
		}
		else if(i==3){
			output+="passcode:"+del+"\n";
		}
		i++;
	}
	return output+"\n^@";
}

string StompProtocol::createSubscribeFrame(string msg){
	string output="SUBSCRIBE\ndestination:/";
	istringstream sd(msg);
	string del;
	int i=0;
	while(getline(sd, del, ' ')) {
		if(i==1){
			output+=del;
			mapSubIdToTopic[del]=subId;
		}
		i++;
	}
	output+="\nid:"+std::to_string(subId);
	output+=addReceiptToFrame("SUBSCRIBE");
	mapReceiptIdToSubId[std::to_string(receiptId)] = std::to_string(subId);
    subId++;
	return output;
}

string StompProtocol::createUnSubscribeFrame(string msg){
	string output="UNSUBSCRIBE\nid:";
	istringstream sd(msg);
	string del;
	int i=0,id=-1;
	while(getline(sd, del, ' ')) {
		if(i==1){
			id = mapSubIdToTopic[del];
			output+=std::to_string(id);		
		}
		i++;
	}
	output += addReceiptToFrame("UNSUBSCRIBE");
	mapReceiptIdToSubId[std::to_string(receiptId)] = std::to_string(id);
	return output;
}

string StompProtocol::addReceiptToFrame(string subject){
	string output = "\nreceipt:"+std::to_string(receiptId)+"\n"+"^@";
	mapReceiptIdToMsg[std::to_string(receiptId)]=subject;
	return output;
}

string StompProtocol::createReportFrame(string file){
	if(approvedUser==""){
		std::cout << "error- user isn't connect" << std::endl;
		return "";
	}
	names_and_events nae = parseEventsFile(file);
	string teamAName = nae.team_a_name;
	string teamBName = nae.team_b_name;
	string gameName = teamAName+"_"+teamBName;
	vector<Event> events = nae.events;
	string output;
	int i=0;
	for(Event e : events){
		output = "SEND\ndestination:/"+gameName+"\n\n";
		output += "user:"+approvedUser+"\n";
		if(i==0){
			output +="team a: "+teamAName+"\nteam b: "+teamBName+"\n";
		}
		output += convertEventToMessage(e);
		output += "\n^@";
		if (!connectionHandler.sendLine(output)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            return "bye";
			//change some field for write task loop?
        }

		i++;
	}
	return "";
}

string StompProtocol::createSummaryFrame(string msg){
	istringstream sd(msg);
	string del;
	string gameName,userName,file;
	int i=0;
	while(getline(sd, del, ' ')) {
		if(i==1) gameName = del;
		else if(i==2) userName = del;
		else if(i==3) file = del;
		i++;
	} 
	Event e = mapUserToUSummary[userName].getSummary(gameName);
	string summary = e.get_team_a_name()+" vs "+e.get_team_b_name()+"\n";
	summary += convertEventToSummary(e);
	const string output_path = file;
    std::ofstream outputFile(output_path);
    outputFile << summary <<endl;
	return "";
}

string StompProtocol::createDisConnectFrame(){
	if(approvedUser==""){
		std::cout << "error- user isn't connected" << std::endl;
		return "";
	}
	approvedUser = "";
	string output="DISCONNECT";
	output+=addReceiptToFrame("DISCONNECT");
	return output;
}

string StompProtocol::handleConnected(){
	approvedUser = requestedUser;
	requestedUser = "";
	return "Login Successful";
}

string StompProtocol::handleMessage(string msg){
	istringstream sd(msg);
	string del;
	string gameName,userName;
	int pos;
	int i=0;
	while(getline(sd, del, '\n'),i<6) {
		if(i==3){
			pos = del.find("/");
			gameName = del.substr(pos+1,del.length()-1);
		} 
		else if(i==5) {
			pos = del.find(":");
			userName = del.substr(pos+1,del.length()-1);
		}
		i++;
	}
	int pos2 = msg.find("team a");
	int pos3 = msg.find("event name");
	if(pos2>pos3)msg = msg.erase(0,pos3);
	else msg = msg.erase(0,pos2);
	Event e(msg);
	if(mapUserToUSummary.find(userName)==mapUserToUSummary.end()){
		std::unordered_map<string,Event> mapGameToSummary;
		mapGameToSummary[gameName] = e;
		UserSummary us(userName,mapGameToSummary);
		mapUserToUSummary[userName] = us;
	}
	else{ 
		mapUserToUSummary[userName].addToSummary(gameName,e);
	}
	return "user: "+userName+"\n"+ msg;
}
string StompProtocol::handleReceiptSubOrUnsub(string receiptIdFromFrame){
	string& subscriptionId = mapReceiptIdToSubId.at(receiptIdFromFrame);
	string topic = "";
	for (auto it=mapSubIdToTopic.begin(); it!=mapSubIdToTopic.end(); ++it){
		string x = std::to_string(it->second);
		if (x==subscriptionId)
			topic = it->first;
	}
	mapReceiptIdToSubId.erase(receiptIdFromFrame);
	return topic;
}

string StompProtocol::handleReceipt(string message){
	istringstream sd(message);
	string del;

	int pos = message.find("id");
	string receiptIdFromFrame = message.substr(pos+3,message.length()-3);
	pos = receiptIdFromFrame.find("\n");
	receiptIdFromFrame = receiptIdFromFrame.substr(0,pos);
	
	const string& subForReceipt = mapReceiptIdToMsg.at(receiptIdFromFrame);

	
	string response = "";

	if(subForReceipt.compare("SUBSCRIBE")==0){
		response = "Joined channel " + handleReceiptSubOrUnsub(receiptIdFromFrame);	
	}
	else if(subForReceipt.compare("UNSUBSCRIBE")==0){
		response = "Exited channel " + handleReceiptSubOrUnsub(receiptIdFromFrame);
	}
	else if(subForReceipt.compare("DISCONNECT")==0){
		approvedUser="";
		response = "bye";
	}
	mapReceiptIdToMsg.erase(receiptIdFromFrame);
	return response;
}

string StompProtocol::handleError(string message){
	int headerPos = message.find("message:");
	string header = message.substr(headerPos+9, message.length());
	int headeEndPos = header.find("\n");
	header = header.substr(0, headeEndPos);
	int bodyMsgPos = message.find("The message:");
	return "bye" + header + message.substr(bodyMsgPos+12, message.length());
}

string StompProtocol::processRead(string message){

	int pos = message.find("\n");
	string command = message.substr(0,pos);

	if(command.compare("CONNECTED")==0){
		return handleConnected();
	}
	else if(command.compare("MESSAGE")==0){
		return handleMessage(message);
	}
	else if(command.compare("ERROR")==0){
		return handleError(message);
	}
	else if(command.compare("RECEIPT")==0){
		return handleReceipt(message);
	}
	
	return "";
}

string StompProtocol::processWrite(string message){
    int pos = message.find(" ");
    string command = message.substr(0,pos);
    string frame="";
    if(command.compare("login")==0){
        frame = createConnectionFrame(message);
    }
    else if(command.compare("join")==0){
        frame = createSubscribeFrame(message);
    }
    else if(command.compare("exit")==0){
        frame = createUnSubscribeFrame(message);
    }
	else if(command.compare("report")==0){
		frame = createReportFrame(message.substr(pos+1,message.length()-1));
	}
	else if(command.compare("summary")==0){
		frame = createSummaryFrame(message);
	}
	else if(command.compare("logout")==0){
        frame = createDisConnectFrame();
    }
	receiptId++;
    return frame;
}



