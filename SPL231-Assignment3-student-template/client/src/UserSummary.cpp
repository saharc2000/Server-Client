#include "../include/UserSummary.h"
#include<sstream>

using namespace std;

UserSummary::UserSummary():user(), mapGameToSummary(){}
UserSummary::UserSummary(string _user,std::unordered_map<std::string,Event> _mapGameToSummary):user(_user),mapGameToSummary(_mapGameToSummary) {
}

string UserSummary::getUser(){
    return user;
}

Event UserSummary::getSummary(string gameName){
    return mapGameToSummary.at(gameName);
}

void UserSummary::addToSummary(string gameName,Event e){
    auto it = mapGameToSummary.find(gameName);
    if(it!=mapGameToSummary.end()){
        mapGameToSummary[gameName].setEvent(e);
    }
    else{
        mapGameToSummary[gameName] = e;
    }
}

