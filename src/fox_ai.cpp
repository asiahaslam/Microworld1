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

vector<string> FoxAI::Run(
    Percepts& percepts,
    AgentComm * comms
) {

    // DO NOT LEAVE THIS CODE (COMMENTED OUT OR IN) IN
    // YOUR SUBMISSION

    /*cout << "\n=========================\n";
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
    cout << "COMMS:\n";
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

    vector<string> cmds;

    if (percepts.current[0] == goal) cmds.push_back("U");
    else if (percepts.current[0] == exit) cmds.push_back("U");
    else if (find(teleporters.begin(), teleporters.end(), percepts.current[0]) != teleporters.end()) {
        cmds.push_back("U");
    }
    vector<string> arr = { "F", "B", "L", "R" };
    while(cmds.size() < 2) {        
	shuffle(arr.begin(), arr.end(), *rng);
        cmds.push_back(arr[0]);
    }
    
    return cmds;
}