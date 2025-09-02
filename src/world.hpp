#pragma once

#include<cstdio>
#include<fstream>
#include<iostream>
#include<vector>
#include<exception>
#include<algorithm>
#include"json.hpp"
#include"defs.hpp"
#include"vec2.hpp"
#include"percepts.hpp"
#include"defs.hpp"


struct Teleporter {
    unsigned id;
    unsigned dest;
    std::string symbol;
    Vec2 location;
};


struct World {


    WorldConfig config;
    wmap map;
    std::vector<Vec2> fox_starts;
    std::vector<Vec2> hound_starts;
    std::vector<Teleporter> teleporters;
    unsigned num_goals;
    unsigned num_goals_remaining;

    World();
    bool LoadConfig(std::string & filename);
    bool LoadMap(std::string & filename);

    std::string GetCell(Vec2 location);
    void SetCell(Vec2 location, std::string s);
    std::vector<std::string> GetCells(
        Vec2 loc, Vec2 heading, int dist, 
        std::vector<Vec2> & seen_cells, AgentType agent_type
    );
    bool CanMoveIntoCell(Vec2 location);
    Result UseCell(Vec2 location, Vec2 & result_location);
    unsigned GetNumGoals() const;
    unsigned GetNumGoalsRemaining() const;
    void ChangeNumGoalsRemaining(unsigned amt);
};