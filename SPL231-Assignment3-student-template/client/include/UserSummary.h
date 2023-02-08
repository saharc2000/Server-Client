#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include "../include/event.h"

class UserSummary
{
private:
    std::string user;
    std::unordered_map<std::string,Event> mapGameToSummary;
public:
    UserSummary();
    UserSummary(std::string _user,std::unordered_map<std::string,Event> mapGameToSummary);
    void addToSummary(std::string gameName,Event e);
    Event getSummary(std::string gameName);
    std::string getUser();
};