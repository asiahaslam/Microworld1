#pragma once

#include"ai.hpp"

using namespace std;

class HoundAI : public AI {
private:
public:
    HoundAI(
        unsigned id,
        unsigned agent_speed,
        mt19937_64* rng
    );
    virtual vector<string> Run(
        Percepts& percepts,
        AgentComm * comms
    );
};