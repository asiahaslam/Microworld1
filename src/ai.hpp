#pragma once

#include<algorithm>
#include<string>
#include<random>
#include<map>
#include<cstdlib>
#include<iostream>
#include"percepts.hpp"
#include"comm.hpp"

class AI {
protected:
    // Necessary, do not delete.
    unsigned id;
    unsigned agent_speed;
    std::mt19937_64* rng;
public:
    AI();
    AI(
        unsigned id, 
        unsigned agent_speed,
        std::mt19937_64* rng
    );
    virtual std::vector<std::string> Run(
        Percepts & percepts,
        AgentComm * comms
    );
};



