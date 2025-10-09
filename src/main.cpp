/***
*
* Microworld 1 - Foxes and Hounds
* main.cpp
* zax
* adopted from a previous microworld in July 2025
* 
* HISTORY
* 
* Version 1.0
* - Initial version
*/

#include<SFML/Graphics.hpp>
#include <algorithm>
#include<cstring>
#include <stdexcept>
#include<string>
#include<random>
#include<chrono>
#include<map>
#include"defs.hpp"
#include"display.hpp"
#include"world.hpp"
#include"agent.hpp"
#include"percepts.hpp"

/* The main sim loop. */
void loop(
    World & world, 
    std::vector<Agent> & foxes, 
    std::vector<Agent> & hounds,
    int update_delay, int max_turns);

/* Checks whether the locations vector contains
    any animals. If so it adds to the return vector. */
std::vector<Sighting>  ContainsAnimals(
    std::string direction,
    int self_id,
    AgentType self_type,
    int offset,
    std::vector<Vec2> locations,
    std::vector<Agent>& foxes,
    std::vector<Agent>& hounds
    );

/* Checks if a moving animal collides with other animals. */
void CheckAgentCollisionAndAssignPoints(
    Agent & agent,
    Vec2 location,
    std::vector<Agent>& foxes,
    std::vector<Agent>& hounds
);

double GetScentForAgent(
    Agent & agent,
    std::vector<Agent>& others
);

/* Called when the game ends. */
void game_over(
    AgentType winner,
    World& world,
    std::string reason,
    std::vector<Agent> & foxes,
    std::vector<Agent> & hounds);


inline std::string config_filename = "config.json";
inline unsigned screen_width = 1000;
inline unsigned screen_height = 850;

int main(int argc, char **argv) {

    /**************************************************************
    VARS
    */
    bool RUN_SFML_TEST = false;
    bool DEBUG = false;
    bool RANDOMIZE_STARTS = false;
    bool RANDOMIZE_HEADINGS = false;
    screen_width = 1000;
    screen_height = 850;
    std::random_device rd;
    std::mt19937_64 rng(rd());
    int update_delay = 1000;
    int max_turns = 1000;

    /**************************************************************
    SET UP THE WORLD
    */
    std::string world_filename = "worldA";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-w") == 0) {
            if(argc > i+1) {
                world_filename = std::string(argv[i+1]);
            } else {
                std::cerr << "Missing filename after -w\n";
                return 1;
            }
        } else if(strcmp(argv[i], "-rs") == 0) {
            RANDOMIZE_STARTS = true;
        } else if(strcmp(argv[i], "-rh") == 0) {
            RANDOMIZE_HEADINGS = true;
        } else if(strcmp(argv[i], "-rsh") == 0 || strcmp(argv[i], "-rhs") == 0) {
            RANDOMIZE_HEADINGS = true;
            RANDOMIZE_STARTS = true;
        }
        else if (strcmp(argv[i], "-s") == 0) {
            if (argc > i + 1) {
                try {
                    unsigned long seed = std::stol(argv[i + 1]);
                    rng.seed(seed);
                }
                catch (std::invalid_argument& e) {
                    std::cerr << "Invalid arg for -s" << argv[i + 1] << std::endl
                        << e.what() << std::endl;
                    return 1;
                }
            }
        }
        else if(strcmp(argv[i], "-d") == 0) {
            if(argc > i+1) {
                try {
                    update_delay = std::stol(argv[i+1]);
                } catch(std::invalid_argument & e) {
                    std::cerr << "Invalid arg for -d" << argv[i+1] << std::endl 
                        << e.what() << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Missing value for -d" << std::endl;
                return 1;
            }
        } else if(strcmp(argv[i], "-t") == 0) {
            if(argc > i+1) {
                try {
                    max_turns = std::stoi(argv[i+1]);
                } catch(std::invalid_argument & e) {
                    std::cerr << "Invalid value for -t" << argv[i+1] << std::endl
                        << e.what() <<  std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Missing value for -t" << std::endl;
                return 1;
            }
        }
    }

    
    
    World world;
    
    if(!world.LoadConfig(config_filename)) {
        return 1;
    }
    if(!world.LoadMap(world_filename)) {
        return 1;
    }

    /**************************************************************
    SET UP THE AGENTS
    */
    if(RANDOMIZE_STARTS) {
        std::shuffle(
            world.fox_starts.begin(), 
            world.fox_starts.end(),
            rng
        );
        std::shuffle(
            world.hound_starts.begin(),
            world.hound_starts.end(),
            rng
        );
    }

    std::vector<Vec2> headings = {
        Vec2(0,-1),
        Vec2(1,0),
        Vec2(0,1),
        Vec2(-1, 0)
    };

    AgentSight fox_sight;
    fox_sight.forward = world.config.fox_sight[0];
    fox_sight.left = world.config.fox_sight[1];
    fox_sight.right = world.config.fox_sight[2];
    fox_sight.behind = world.config.fox_sight[3];

    AgentSight hound_sight;
    hound_sight.forward = world.config.hound_sight[0];
    hound_sight.left = world.config.hound_sight[1];
    hound_sight.right = world.config.hound_sight[2];
    hound_sight.behind = world.config.hound_sight[3];

    std::vector<Agent> foxes;
    std::vector<Agent> hounds;
    for(unsigned i = 0; i < world.config.num_foxes; i++) {

        if(RANDOMIZE_HEADINGS) {
            std::shuffle(
                headings.begin(), headings.end(), rng
            );
        }

        Agent a(
            world.fox_starts[i%world.fox_starts.size()],
            headings[0],
            fox_sight,
            i,
            &rng,
            world.config.fox_speed,
            AgentType::FOX,
            world.config.goal,
            world.config.exit,
            world.config.tele_symbols
        );

        foxes.push_back(std::move(a));
    }

    

    for (unsigned i = 0; i < world.config.num_hounds; i++) {

        if (RANDOMIZE_HEADINGS) {
            std::shuffle(
                headings.begin(), headings.end(), rng
            );
        }

        Agent a(
            world.hound_starts[i%world.hound_starts.size()],
            headings[0],
            hound_sight,
            i,
            &rng,
            world.config.hound_speed,
            AgentType::HOUND,
            world.config.goal,
            world.config.exit,
            world.config.tele_symbols
        );

        hounds.push_back(std::move(a));
    }
    
    /***************************************************************
    SET UP DISPLAY
    */
    if(!dspl::InitDisplay(screen_width, screen_height, world.config)) {
        return 1;
    }
    
    /***************************************************************
    RUN SIMULATION
    */
    loop(world, foxes, hounds, update_delay, max_turns);


}

void loop(

    /******************************************************
        VARS
    *******************************************************/
    World& world,
    std::vector<Agent>& foxes,
    std::vector<Agent>& hounds,
    int update_delay, int max_turns) {

    bool finished = false;
    AgentType winner = AgentType::FOX;
    std::string finish_reason = "NONE";
    int turn = 1;

    unsigned remaining_goals = world.GetNumGoals();
    std::vector<int> fox_points;
    std::vector<int> hound_points;
    for(size_t i = 0; i < foxes.size(); i++) {
        fox_points.push_back(0);
    }
    for (size_t i = 0; i < hounds.size(); i++) {
        hound_points.push_back(0);
    }

    auto last_update = std::chrono::high_resolution_clock::now();

    // Init hound comms. Fox comms are not persistent.
    AgentComm comms;
    for(size_t i = 0; i < hounds.size(); i++) {
        comms.bark.push_back(0);
        comms.direction.push_back(Vec2());
    }

    bool paused = false;

    /******************************************************
        MAIN LOOP
    *******************************************************/
    while(dspl::IsOpen()) {

        // Handle events
        while(const std::optional<sf::Event> event = dspl::PollEvent()) {
            if(event->is<sf::Event::KeyPressed>()) {
                const auto *key = event->getIf<sf::Event::KeyPressed>();
                if (key->code == sf::Keyboard::Key::Escape) dspl::Close();
                else if (key->code == sf::Keyboard::Key::P) paused = !paused;
            }
        }

        if (!paused) {

            // If time to run AI updates.
            auto cur_time = std::chrono::high_resolution_clock::now();
            if (!finished && std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - last_update).count() > update_delay) {

                // Run all foxes first
                for (size_t i = 0; i < foxes.size(); i++) {

                    // Gather information
                    if (foxes[i].GetAgentState() != AgentState::Active) continue;
                    AgentSight sight = foxes[i].GetSight();
                    Vec2 location = foxes[i].GetLoc();
                    Vec2 forward = foxes[i].GetHeading();
                    unsigned speed = foxes[i].GetSpeed();
                    Vec2 left = Vec2(forward.y, -forward.x);
                    Vec2 right = Vec2(-forward.y, forward.x);
                    Vec2 back = Vec2(-forward.x, -forward.y);
                    Percepts percepts;
                    std::vector<Vec2> seen_cells;
                    std::vector<Sighting> temp;
                    AgentType type = foxes[i].GetType();
                    int id = foxes[i].GetID();

                    // Process and find all percepts
                    percepts.current = world.GetCells(location, forward, 1, seen_cells, AgentType::FOX);
                    temp = ContainsAnimals(
                        "X", id, type, 0, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.forward = world.GetCells(location + forward, forward, sight.forward, seen_cells, AgentType::FOX);
                    temp = ContainsAnimals(
                        "F", id, type, 1, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.backward = world.GetCells(location + back, back, sight.behind, seen_cells, AgentType::FOX);
                    temp = ContainsAnimals(
                        "B", id, type, 1, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.left = world.GetCells(location + left, left, sight.left, seen_cells, AgentType::FOX);
                    temp = ContainsAnimals(
                        "L", id, type, 1, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.right = world.GetCells(location + right, right, sight.right, seen_cells, AgentType::FOX);
                    temp = ContainsAnimals(
                        "R", id, type, 1, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.scent = 0.0;

                    // Call the AI
                    std::vector<std::string> cmds = foxes[i].RunAI(percepts, nullptr);

                    // Handle AI commands.
                    unsigned s = 0;
                    while (s < foxes[i].GetSpeed() && s < cmds.size()) {
                        std::string cmd = cmds[s];
                        location = foxes[i].GetLoc();
                        forward = foxes[i].GetHeading();
                        left = Vec2(forward.y, -forward.x);
                        right = Vec2(-forward.y, forward.x);
                        back = Vec2(-forward.x, -forward.y);
                        // Move forward
                        if (cmd == "F") {
                            location = location + forward;
                            if (world.CanMoveIntoCell(location)) {
                                foxes[i].SetLoc(location);
                                CheckAgentCollisionAndAssignPoints(foxes[i], location, foxes, hounds);
                            }
                            else {
                                break;
                            }
                        }

                        // Move backward
                        else if (cmd == "B") {
                            location = location + back;
                            if (world.CanMoveIntoCell(location)) {
                                foxes[i].SetLoc(location);
                                CheckAgentCollisionAndAssignPoints(foxes[i], location, foxes, hounds);
                            }
                            else {
                                break;
                            }
                        }
                        // Two turn commands
                        else if (cmd == "L") {
                            foxes[i].SetHeading(left);
                        }
                        else if (cmd == "R") {
                            foxes[i].SetHeading(right);
                        }
                        // Use command
                        else if (cmd == "U") {
                            Vec2 result_location;
                            Result res = world.UseCell(location, result_location);
                            if (res == Result::TELEPORT) {
                                foxes[i].SetLoc(result_location);
                            }
                            else if (res == Result::GOAL_FOUND) {
                                world.SetCell(location, world.config.open);
                                world.ChangeNumGoalsRemaining(-1);
                                foxes[i].AddPoints(1);
                                remaining_goals--;
                                if (remaining_goals == 0) {
                                    finished = true;
                                    finish_reason = "ALL GOALS CAPTURED";
                                    winner = AgentType::FOX;
                                }
                            }
                            else if (res == Result::EXIT) {
                                foxes[i].SetAgentState(AgentState::Escaped);
                            }
                        }
                        s++;
                    }
                }

                // Check if the game is over due to fox movement.
                if (finished) break;

                // Run all hounds.
                for (size_t i = 0; i < hounds.size(); i++) {
                    if (hounds[i].GetAgentState() != AgentState::Active) continue;
                    AgentSight sight = hounds[i].GetSight();
                    Vec2 location = hounds[i].GetLoc();
                    Vec2 forward = hounds[i].GetHeading();
                    unsigned speed = hounds[i].GetSpeed();
                    Vec2 left = Vec2(forward.y, -forward.x);
                    Vec2 right = Vec2(-forward.y, forward.x);
                    Vec2 back = Vec2(-forward.x, -forward.y);
                    Percepts percepts;
                    std::vector<Vec2> seen_cells;
                    std::vector<Sighting> temp;
                    AgentType type = hounds[i].GetType();
                    int id = hounds[i].GetID();

                    // Update agent comms direction vectors.
                    comms.direction.clear();
                    for(size_t k = 0; k < hounds.size(); k++) {
                        Vec2 d = hounds[k].GetLoc() - hounds[i].GetLoc();

                        // Adjust for the heading (make it relative).
                        if(hounds[i].GetHeading() == SOUTH) {
                            d = d * Vec2(-1,-1);
                        } else if(hounds[i].GetHeading() == EAST) {
                            d.SwapXY();
                            d = d * Vec2(1, -1);
                        } else if(hounds[i].GetHeading() == WEST) {
                            d.SwapXY();
                            d = d * Vec2(-1, 1);
                        }
                        comms.direction.push_back(d);
                    }

                    percepts.current = world.GetCells(location, forward, 1, seen_cells, AgentType::HOUND);
                    temp = ContainsAnimals(
                        "X", id, type, 0, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.forward = world.GetCells(location + forward, forward, sight.forward, seen_cells, AgentType::HOUND);
                    temp = ContainsAnimals(
                        "F", id, type, 1, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.backward = world.GetCells(location + back, back, sight.behind, seen_cells, AgentType::HOUND);
                    temp = ContainsAnimals(
                        "B", id, type, 1, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.left = world.GetCells(location + left, left, sight.left, seen_cells, AgentType::HOUND);
                    temp = ContainsAnimals(
                        "L", id, type, 1, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.right = world.GetCells(location + right, right, sight.right, seen_cells, AgentType::HOUND);
                    temp = ContainsAnimals(
                        "R", id, type, 1, seen_cells, foxes, hounds
                    );
                    std::copy(temp.begin(), temp.end(), std::back_inserter(percepts.sightings));
                    seen_cells.clear();
                    temp.clear();

                    percepts.scent = GetScentForAgent(hounds[i], foxes);


                    std::vector<std::string> cmds = hounds[i].RunAI(percepts, &comms);
                    unsigned s = 0;
                    while (s < hounds[i].GetSpeed() && s < cmds.size()) {
                        std::string cmd = cmds[s];
                        Vec2 location = hounds[i].GetLoc();
                        forward = hounds[i].GetHeading();
                        left = Vec2(forward.y, -forward.x);
                        right = Vec2(-forward.y, forward.x);
                        back = Vec2(-forward.x, -forward.y);
                        if (cmd == "F") {
                            location = location + forward;
                            if (world.CanMoveIntoCell(location)) {
                                hounds[i].SetLoc(location);
                                CheckAgentCollisionAndAssignPoints(hounds[i], location, foxes, hounds);
                            }
                            else break;
                        }
                        else if (cmd == "B") {
                            // HOUNDS CANNOT MOVE BACKWARD
                            // Do nothing
                        }
                        else if (cmd == "L") {
                            hounds[i].SetHeading(left);
                        }
                        else if (cmd == "R") {
                            hounds[i].SetHeading(right);
                        }
                        else if (cmd == "U") {
                            // HOUNDS CANNOT USE ANYTHING
                            // Do nothing
                        }
                        s++;
                    }
                }

                last_update = std::chrono::high_resolution_clock::now();
                turn++;
            }

        }

        /**********************************************************
            DRAWING CODE
        ***********************************************************/
        dspl::Clear();

        dspl::DrawMap(world.map,world.config);
        for(std::vector<Agent>::iterator it = foxes.begin();
            it != foxes.end(); it++) {
        
            if(it->GetAgentState() != AgentState::Active) continue;
            dspl::DrawAgent(it->GetLoc(), it->GetHeading(), "F", sf::Color::Black);
        }
        for (std::vector<Agent>::iterator it = hounds.begin();
            it != hounds.end(); it++) {

            if (it->GetAgentState() != AgentState::Active) continue;
            dspl::DrawAgent(it->GetLoc(), it->GetHeading(), "H", sf::Color::Black);
        }
        
        dspl::DrawMsg("Turn: "+std::to_string(turn), 10, 700, 20, sf::Color::Yellow);

        if (paused) {
            dspl::DrawMsg("SIM IS PAUSED", 300, 700, 20, sf::Color::Red);
        }

        // Make several checks to see if the sim has finished.
        // First, are we at max_turns?
        if(!finished && turn >= max_turns) {
            finished = true;
            winner = AgentType::NONE;
            finish_reason = "MAX TURNS REACHED";
        }

        // Second, are any foxes left in the environment?
        if (!finished) {
            bool one_alive = false;
            for (size_t i = 0; i < foxes.size(); i++) {
                if (foxes[i].GetAgentState() == AgentState::Active) {
                    one_alive = true;
                    break;
                }
            }
            if (!one_alive) {
                finished = true;
                winner = AgentType::HOUND;
                finish_reason = "NO MORE FOXES";
            }
        }

        // If the sim is over...
        if (finished) {
            game_over(winner, world, finish_reason, foxes, hounds);
        }

        dspl::Display();
    }
}



std::vector<Sighting> ContainsAnimals(
    std::string direction,
    int self_id,
    AgentType self_type,
    int offset,
    std::vector<Vec2> locations,
    std::vector<Agent>& foxes,
    std::vector<Agent>& hounds
    
) {
    std::vector<Sighting> sightings;
    for (size_t i = 0; i < locations.size(); i++) {
        for (std::vector<Agent>::iterator it = foxes.begin(); it != foxes.end(); it++) {
            if (it->GetType() == self_type && it->GetID() == self_id) continue;
            else if (locations[i] == it->GetLoc()) {
                Sighting s;
                s.direction = direction;
                s.distance = static_cast<unsigned>(i + offset);
                s.type = AgentType::FOX;
                sightings.push_back(s);
            }
        }
        for (std::vector<Agent>::iterator it = hounds.begin(); it != hounds.end(); it++) {
            if (it->GetType() == self_type && it->GetID() == self_id) continue;
            else if (locations[i] == it->GetLoc()) {
                Sighting s;
                s.direction = direction;
                s.distance = static_cast<unsigned>(i + offset);
                s.type = AgentType::HOUND;
                sightings.push_back(s);
            }
        }
    }
    return sightings;
}

void CheckAgentCollisionAndAssignPoints(
    Agent & agent,
    Vec2 location,
    std::vector<Agent>& foxes,
    std::vector<Agent>& hounds
) {
    if (agent.GetType() == AgentType::FOX) {
        for (std::vector<Agent>::iterator it = hounds.begin(); it != hounds.end(); it++) {
            if (it->GetLoc() == location) {
                agent.SetAgentState(AgentState::Dead);
                it->AddPoints(agent.GetPoints());
                agent.SetPoints(0);
                break;  // need to break so the fox isn't counted twice
                        // if there's more than one hound in the cell.
            }
        }
    }
    else if (agent.GetType() == AgentType::HOUND) {
        for (std::vector<Agent>::iterator it = foxes.begin(); it != foxes.end(); it++) {
            if (it->GetLoc() == location) {
                it->SetAgentState(AgentState::Dead);
                agent.AddPoints(it->GetPoints());
                it->SetPoints(0);
            }
        }
    }
}

double GetScentForAgent(
    Agent& agent,
    std::vector<Agent>& others
) {
    double min_distance = std::numeric_limits<double>::max();
    for (size_t i = 0; i < others.size(); i++) {
        double dist = agent.GetLoc().Distance(others[i].GetLoc());
        if (dist < min_distance) {
            min_distance = dist;
        }
    }
    return min_distance;
}

void game_over(
    AgentType winner,
    World& world,
    std::string reason,
    std::vector<Agent>& foxes,
    std::vector<Agent>& hounds) {

    dspl::DrawMsg(reason, 200,700,20);

    float x = 100.0f;
    float y = 730.f;
    int total = 0;
    std::string msg;
    for(size_t i = 0; i < foxes.size(); i++) {
        int points = foxes[i].GetPoints();
        // if (foxes[i].GetAgentState() == AgentState::Escaped) {
        //     points = foxes[i].GetPoints();
        // }
        msg = ("Fox "+std::to_string(i)+": "+std::to_string(points));
        dspl::DrawMsg(msg, x, y, 15, sf::Color::White);
        total += points;
        y+=20.f;
        if(y + 20.0 > screen_height) {
            x += 150.0f;
            y = 730.f;
        }
    }
    msg = "Total: " + std::to_string(total);
    dspl::DrawMsg(msg, x, y, 15, sf::Color::White);

    x = 400.0f;
    y = 730.f;
    total = world.GetNumGoalsRemaining();
    msg = "Goals Left: " + std::to_string(total);
    dspl::DrawMsg(msg, x, y, 15, sf::Color::White);
    y += 20.f;
    for (size_t i = 0; i < hounds.size(); i++) {
        msg = "Hound " + std::to_string(i) + ": " + std::to_string(hounds[i].GetPoints());
        dspl::DrawMsg(msg, x, y, 15, sf::Color::White);
        total += hounds[i].GetPoints();
        y += 20.f;
        if(y + 20.0 > screen_height) {
            x += 150.f;
            y = 730.f;
        }
    }
    msg = "Total: " + std::to_string(total);
    dspl::DrawMsg(msg, x, y, 15, sf::Color::White);
}
