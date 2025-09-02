#pragma once

#include<vector>
#include<string>

enum class AgentType {
    FOX,
    HOUND,
    NONE
};

struct Sighting {
    AgentType type;
    std::string direction;
    unsigned distance;
};

struct Percepts {
    std::vector<std::string> current;
    std::vector<std::string> forward;
    std::vector<std::string> backward;
    std::vector<std::string> left;
    std::vector<std::string> right;
    std::vector<Sighting> sightings;
    double scent;
};