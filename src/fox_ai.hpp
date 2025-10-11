#pragma once

#include "ai.hpp"
#include <vector>
#include <string>
#include <random>
#include <iostream>

using namespace std;

enum mapItems {
    g, e, t, w, o, h, m, i
    // goal, exit, teleport, wall, open, hound, mystery, unmapped
};

class FoxAI : public AI {
private:
    string goal;
    string exit;
    vector<string> teleporters;

public:
    vector<vector<mapItems>> map;  

    FoxAI(
        unsigned id,
        unsigned agent_speed,
        mt19937_64* rng,
        string goal,
        string exit,
        vector<string> teleporters
    );

    virtual vector<string> Run(Percepts& percepts, AgentComm* comms);
    virtual vector<string> Choice(Percepts& percepts);
    virtual vector<string> findGoal(Percepts& percepts);
    virtual string nearbyHound(Percepts& percepts);
    virtual vector<string> Flee(Percepts& percepts, string houndLocation);
    virtual vector<string> Explore(Percepts& percepts);
    virtual void updateMap(int i, int j, mapItems item);

    void printMap() const;  // ✅ add this declaration
};
