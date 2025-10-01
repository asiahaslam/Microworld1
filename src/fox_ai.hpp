#pragma once

#include"ai.hpp"

using namespace std;

class FoxAI : public AI {
private:
    string goal;
    string exit;
    vector<string> teleporters;
public:
    FoxAI(
        unsigned id,
        unsigned agent_speed,
        mt19937_64* rng,
        string goal,
        string exit,
        vector<string> teleporters
    );
    virtual vector<string> Run(
        Percepts& percepts,
        AgentComm * comms
    );
    virtual string Choice(
        Percepts& percepts
    );
    virtual string findGoal(
        Percepts& percepts
    );
    virtual string nearbyHound(
        Percepts& percepts
    );
    virtual string flee(
        Percepts& percepts,
        string houndLocation
    );
};