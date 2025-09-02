#include"world.hpp"
#include "defs.hpp"


World::World() 
    : num_goals(0), num_goals_remaining(0)
{}



bool World::LoadConfig(std::string & filename) {
    std::ifstream ifs(filename);
    nlohmann::json j;
    ifs >> j;

    try {
        config.num_foxes = j.at("num_foxes");
        config.num_hounds = j.at("num_hounds");
        config.fox_speed = j.at("fox_speed");
        config.hound_speed = j.at("hound_speed");
        config.fox_sight = j.at("fox_sight");
        config.hound_sight = j.at("hound_sight");
        config.wall = j.at("wall");
        config.open = j.at("open");
        config.exit = j.at("exit");
        config.tele_symbols = j["tele_symbols"].get<std::vector<std::string>>();
        config.tele_dests = j["tele_dests"].get<std::vector<unsigned>>();
        config.goal = j.at("goal");
        config.fox_start = j.at("fox_start");
        config.hound_start = j.at("hound_start");
        config.wall_color = j["wall_color"].get<std::vector<unsigned>>();
        config.open_color = j["open_color"].get<std::vector<unsigned>>();
    } catch(std::exception & e) {
        std::cerr << "Error parsing config.json file" << e.what() << std::endl;
        return false;
    }

    return true;

}

bool World::LoadMap(std::string & filename) {

    std::ifstream ifs("worlds/"+filename);
    if(!ifs.good()) {
        std::cerr << "Failed to open world file: " << filename << std::endl;
        return false;
    }

    std::string line;
    unsigned y = 0;
    while(std::getline(ifs, line)) {
        std::vector<std::string> row;
        for(size_t i = 0, x=0; i < line.size(); i++) {
            if(line[i] == ' ' || line[i] == '\n' || line[i] == '\r') continue;
            else {
                std::string s = line.substr(i, 1);
                row.push_back(s);

                if(s == config.fox_start) {
                    fox_starts.push_back(Vec2(x, y));

                }
                else if (s == config.hound_start) {
                    hound_starts.push_back(Vec2(x, y));
                }
                else if (s == config.goal) {
                    num_goals++;
                }
                else if (std::find(config.tele_symbols.begin(), config.tele_symbols.end(), s) !=
                    config.tele_symbols.end()) {

                    for (size_t t = 0; t < config.tele_symbols.size(); t++) {
                        if (s == config.tele_symbols[t]) {
                            Teleporter tele;
                            tele.id = teleporters.size();
                            tele.location = Vec2(x, y);
                            tele.symbol = config.tele_symbols[t];
                            tele.dest = config.tele_dests[t];
                            teleporters.push_back(tele);
                        }
                    }
                }

                x++;
            }
        }
        map.push_back(row);
        y++;
    }

    num_goals_remaining = num_goals;

    return true;
}

std::string World::GetCell(Vec2 location) {
    return map[location.y][location.x];
}

void World::SetCell(Vec2 location, std::string s) {
    map[location.y][location.x] = s;
}

std::vector<std::string> World::GetCells(
    Vec2 loc, Vec2 heading, int dist,
    std::vector<Vec2>& seen_cells, AgentType agent_type
) {
    std::vector<std::string> v;
    for(int i = 0; i < dist; i++) {
        Vec2 offset = heading * i;
        Vec2 newloc = loc + offset;
        std::string cell = map[newloc.y][newloc.x];

        // Hounds see all specialty cells as being open.
        if (agent_type == AgentType::HOUND &&
            (cell != config.wall && cell != config.open)) {
            cell = config.open;
        }
        // Foxes should see start cells as open.
        else if (agent_type == AgentType::FOX &&
            (cell == config.fox_start || cell == config.hound_start)) {
            cell = config.open;
        }

        v.push_back(cell);
        if(cell == config.wall) {
            break;
        }
        seen_cells.push_back(newloc);
    }
    return v;
}

bool World::CanMoveIntoCell(Vec2 location) {
    if(map[location.y][location.x] == config.wall) {
        return false;
    }
    return true;
}
Result World::UseCell(Vec2 location, Vec2 & result_location) {
    std::string cell = map[location.y][location.x];

    for (size_t i = 0; i < teleporters.size(); i++) {
        if (teleporters[i].symbol == cell) {
            result_location = teleporters[teleporters[i].dest].location;
            return Result::TELEPORT;
        }
    }

    if (cell == config.goal) {
        result_location = location;
        return Result::GOAL_FOUND;
    }
    else if (cell == config.exit) {
        return Result::EXIT;
    }
    

    return Result::NONE;
}

unsigned World::GetNumGoals() const { return num_goals; }

unsigned World::GetNumGoalsRemaining() const {
    return num_goals_remaining;
}
void World::ChangeNumGoalsRemaining(unsigned amt) {
    num_goals_remaining += amt;
}