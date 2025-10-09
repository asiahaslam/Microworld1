#include"fox_ai.hpp"

using namespace std;

/***************************************************************
FOX AI CLASS DEFINITION
*/

FoxAI::FoxAI(
    unsigned id,
    unsigned agent_speed,
    mt19937_64* rng,
    string goal,
    string exit,
    vector<string> teleporters)
    :AI(id, agent_speed, rng), goal(goal), exit(exit), teleporters(teleporters)
{
}

string FoxAI::nearbyHound(
    Percepts& percepts
) {
    string location = "";

    bool foundHound = false;

    for (size_t i = 0; i < percepts.sightings.size(); i++) {
        if (percepts.sightings[i].type == AgentType::HOUND) {
            location += percepts.sightings[i].direction;
            location += percepts.sightings[i].distance;
            foundHound = true;
            break;
        }
    }

    if (!foundHound) location = "N";

    // return whether there is a hound close by
    // if no return N. if yes return distance and direction
    return location;
}

vector<string> FoxAI::Flee(
    Percepts& percepts,
    string houndLocation
) {
    // NEED more logic here so it doesn't get stuck against walls!!
    vector<string> commands;
    switch (houndLocation[0]) { 
        case 'L':
            commands.push_back("R");
            commands.push_back("F");
            break;
        case 'R':
            commands.push_back("L");
            commands.push_back("F");
            break;
        default:
            commands.push_back("B");
            commands.push_back("B");
            break;
    }
    // decide where to flee to. avoid walls. maybe do exit or teleport
    // go away from hound's location
    // start out by just going in opposite direction from hound
    return commands;
}

vector<string> FoxAI::findGoal(
    Percepts& percepts
) {
    vector<string> commands;

    bool foundGoal = false;
    // return string of commands. if no goal found return "N"

    if (percepts.current[0] == goal) {
        commands.push_back("U");
        foundGoal = true;
    }

    // if no goal found forward go left. then right. then if not do random.

    if (!foundGoal) {
        for (size_t i = 0; i < percepts.forward.size(); i++) {
            if (percepts.forward[i] == goal) {
                commands.push_back("F");
                if (i == 0) {
                    commands.push_back("U");
                }
                else {
                    commands.push_back("F");
                }
                foundGoal = true;
            }
        }
    }

    if (!foundGoal) {
        for (size_t i = 0; i < percepts.left.size(); i++) {
            if (percepts.left[i] == goal) {
                commands.push_back("L");
                commands.push_back("F");
                foundGoal = true;
            }
        }
    }

    if (!foundGoal) {
        for (size_t i = 0; i < percepts.right.size(); i++) {
            if (percepts.right[i] == goal) {
                commands.push_back("R");
                commands.push_back("F");
                foundGoal = true;
            }
        }
    }

    if (!foundGoal) {
        commands.push_back("N");
    }

    return commands;
}

vector<string> FoxAI::Explore(
    Percepts& percepts
) {
    // use memory, percepts, location on map to decide where to go
    vector<string> commands;
    vector<string> arr = { "F", "B", "L", "R" };
        /* while(commands.size() < 2) {        
            shuffle(arr.begin(), arr.end(), *rng);
            commands.push_back(arr[0]);
        } */
        

    bool f, ff, l, ll, r, rr = false; // variables to hold nearby walls
    if (percepts.forward[0] == "w") f = true;
    else if (percepts.forward[1] == "w") ff = true;
    if (percepts.left[0] == "w") l = true;
    else if (percepts.left[1] == "w") ll = true;
    if (percepts.right[0] == "w") r = true;
    else if (percepts.right[1] == "w") rr = true;

    if (f) { // wall front
        if (l) { // wall front and left
            if (r) { // wall front, left, and right
                commands.push_back("B");
                commands.push_back("B");
            }
            else { // wall in front and left
                // RF and BR
                vector<int> arr = { 0, 1 };  
                shuffle(arr.begin(), arr.end(), *rng);
                if (arr[0] == 0) {
                    commands.push_back("R");
                    commands.push_back("F");
                }
                else {
                    commands.push_back("B");
                    commands.push_back("R");
                }
            }
        }
        else { // wall front but not left
            if (r) { // wall front and right but not left
                // can go LF or BL
                vector<int> arr = { 0, 1 };  
                shuffle(arr.begin(), arr.end(), *rng);
                if (arr[0] == 0) {
                    commands.push_back("L");
                    commands.push_back("F");
                }
                else {
                    commands.push_back("B");
                    commands.push_back("L");
                }
            }
            else { // wall front but not right or left
                vector<string> arr = { "B", "L", "R" };   
                while(commands.size() < 2) {        
                    shuffle(arr.begin(), arr.end(), *rng);
                    commands.push_back(arr[0]);
                }
            }     
        } 
    }
    else { // no wall in front
        if (ff) {
            commands.push_back("F");
            vector<string> arr = { "L", "R" };      
            shuffle(arr.begin(), arr.end(), *rng);
            commands.push_back(arr[0]);
        }
        else { // no wall in front or 2 in front
            if (l) { // wall left but not front or 2 in front
                if (r) { // wall left and right
                    commands.push_back("F");
                    commands.push_back("F");
                }
                else { // wall only to left
                    vector<string> arr = { "F", "B", "R" };
                    while(commands.size() < 2) {        
                        shuffle(arr.begin(), arr.end(), *rng);
                        commands.push_back(arr[0]);
                    }
                }
            }
            else { // no wall in front or left
                if (r) { // wall only right
                    vector<string> arr = { "F", "B", "L" };
                    while(commands.size() < 2) {        
                        shuffle(arr.begin(), arr.end(), *rng);
                        commands.push_back(arr[0]);
                    }
                }
                else { // no walls front, left, or right
                    vector<string> arr = { "F", "B", "L", "R" };
                    while(commands.size() < 2) {        
                        shuffle(arr.begin(), arr.end(), *rng);
                        commands.push_back(arr[0]);
                    }
                }
            }
        }
        
        /* if (ff) { // wall 2 in front    
            if (l) { // wall 2 in front and left
                if (r) { // wall 2 in front, left, right
                    commands.push_back("B");
                    commands.push_back("B");
                }
                else { // wall 2 in front, left, and not right
                    commands.push_back("F");
                }
            }
            else { // wall 2 in front and not left
                commands.push_back("F");
            }
        }
        else {
            commands.push_back("F");
            commands.push_back("F");
        } */
    }
    return commands;
}

vector<string> FoxAI::Choice(
    Percepts& percepts
) {
    /* vector<string> goToGoal = findGoal(percepts);

        if (goToGoal[0] == "N") {
            // run function to explore area
            return Explore(percepts);
        }
        else {
            // send commands to go toward goal
            return goToGoal;
        } */

    string houndLocation = nearbyHound(percepts);

    // if no hound visible, explore the area
    if (houndLocation == "N") {
        vector<string> goToGoal = findGoal(percepts);

        if (goToGoal[0] == "N") {
            // run function to explore area
            return Explore(percepts);
        }
        else {
            // send commands to go toward goal
            return goToGoal;
        }
    }
    // if hound is not too close, get nearby goals
    else if (houndLocation[1] > 5) {
        vector<string> goToGoal = findGoal(percepts);

        if (goToGoal[0] == "N") {
            // if no goal, run away from hound
            return Flee(percepts, houndLocation);
        }
        else {
            // if there is a goal closeby, get the goal
            return goToGoal;
        }
    }
    // if hound is close, run away
    else {
        return Flee(percepts, houndLocation);
    }
}

vector<string> FoxAI::Run(
    Percepts& percepts,
    AgentComm * comms
) {

    // DO NOT LEAVE THIS CODE (COMMENTED OUT OR IN) IN
    // YOUR SUBMISSION

    cout << "\n=========================\n";
    cout << "FOX ID: " << id << endl;
    cout << "CURRENT: ";
    for (size_t i = 0; i < percepts.current.size(); i++) {
        cout << percepts.current[i] << " ";
    }
    cout << "\nFORWARD: ";
    for (size_t i = 0; i < percepts.forward.size(); i++) {
        cout << percepts.forward[i] << " ";
    }
    cout << "\nBACKWARD: ";
    for (size_t i = 0; i < percepts.backward.size(); i++) {
        cout << percepts.backward[i] << " ";
    }
    cout << "\nLEFT: ";
    for (size_t i = 0; i < percepts.left.size(); i++) {
        cout << percepts.left[i] << " ";
    }
    cout << "\nRIGHT: ";
    for (size_t i = 0; i < percepts.right.size(); i++) {
        cout << percepts.right[i] << " ";
    }
    cout << "\nSIGHTINGS:\n";
    for (size_t i = 0; i < percepts.sightings.size(); i++) {
        if (percepts.sightings[i].type == AgentType::FOX) cout << "   FOX ";
        else cout << "  HOUND ";
        cout << "Dir " << percepts.sightings[i].direction << " ";
        cout << "Dis " << percepts.sightings[i].distance << "\n";
    }
    cout << "SCENT: " << percepts.scent << endl;
    /*cout << "COMMS:\n";
    if (comms == nullptr) cout << "No communication.\n";
    else {
        for (size_t i = 0; i < comms->size(); i++) {
            cout << "HOUND " << i << " a: " << (*comms)[i].a
                << " b: " << (*comms)[i].b
                << " c: " << (*comms)[i].c
                << endl;
        }
    }*/


    /***************************************************************
    YOUR FOX AI CODE GOES HERE.
    Notes:
    - you may add other member functions to this class.
    - you may add member data to this class.
    - you may develop new classes to help or supplement this one.
    - you may NOT alter the function definition here or where
        it is called by the simulation.
    - you may not alter ANY simulation code other than this AI file.
    */


    // The following is a purely random solution to give you an idea
    // how percepts and commands work. You should not include the
    // following in your solution.

    vector<string> cmds = Choice(percepts);

    for (int i = 0; i < cmds.size(); i++) {
        cout << cmds[i] << endl;
    }

    /* if (percepts.current[0] == goal) cmds.push_back("U");
    else if (percepts.current[0] == exit) cmds.push_back("U");
    else if (find(teleporters.begin(), teleporters.end(), percepts.current[0]) != teleporters.end()) {
        cmds.push_back("U");
    }
    vector<string> arr = { "F", "B", "L", "R" };
    while(cmds.size() < 2) {        
	shuffle(arr.begin(), arr.end(), *rng);
        cmds.push_back(arr[0]);
    } */

    /* string commands = Choice(percepts); */

    /* string command1 = "";
    command1 += commands[0];
    cout << command1 << endl;
    string command2 = "";
    command2 += commands[1];
    cout << command2 << endl;

    cmds.push_back(command1);
    cmds.push_back(command2); */
    
    return cmds;
}   