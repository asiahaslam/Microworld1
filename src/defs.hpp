#pragma once

#include<vector>
#include<string>
#include<array>
#include"vec2.hpp"
#include"comm.hpp"

inline const float CELL_SIZE = 24.0f;

using wmap = std::vector<std::vector<std::string>>;

struct WorldConfig {
    unsigned num_foxes;
    unsigned num_hounds;
    unsigned fox_speed;
    unsigned hound_speed;
    std::array<unsigned, 4> fox_sight;
    std::array<unsigned, 4> hound_sight;
    std::string wall;
    std::string open;
    std::string exit;
    std::vector<std::string> tele_symbols;
    std::vector<unsigned> tele_dests;
    std::string fox_start;
    std::string hound_start;
    std::string goal;
    std::vector<unsigned> wall_color;
    std::vector<unsigned> open_color;
};

struct AgentSight {
    unsigned forward;
    unsigned behind;
    unsigned left;
    unsigned right;
};



inline const Vec2 NORTH(0, -1);
inline const Vec2 SOUTH(0, 1);
inline const Vec2 EAST(1, 0);
inline const Vec2 WEST(-1, 0);

inline const std::vector<std::string> VALID_CMDS = {
    "F", "B",
    "L", "R",
    "U"
};

enum class Result {
    NONE,
    TELEPORT,
    GOAL_FOUND,
    CAUGHT_FOX,
    EXIT
};



