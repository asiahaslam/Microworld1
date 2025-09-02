#pragma once

#include<vector>
#include"vec2.hpp"

struct AgentComm {
    std::vector<unsigned> bark;
    std::vector<Vec2> direction;
};