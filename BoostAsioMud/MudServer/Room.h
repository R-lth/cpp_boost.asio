#pragma once

#include <map>
#include <string>

struct Room {
    int id;
    std::string name;
    std::string description;
    std::map<std::string, int> exits;
};
