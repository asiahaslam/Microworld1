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
    virtual vector<string> Choice(
        Percepts& percepts
    );
    virtual vector<string> findGoal(
        Percepts& percepts
    );
    virtual string nearbyHound(
        Percepts& percepts
    );
    virtual vector<string> Flee(
        Percepts& percepts,
        string houndLocation
    );
    virtual vector<string> Explore(
        Percepts& percepts
    );
};