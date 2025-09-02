#pragma once

#include"ai.hpp"

class HoundAI : public AI {
private:
public:
    HoundAI(
        unsigned id,
        unsigned agent_speed,
        std::mt19937_64* rng
    );
    virtual std::vector<std::string> Run(
        Percepts& percepts,
        AgentComm * comms
    );
};