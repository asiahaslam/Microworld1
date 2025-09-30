#include"hound_ai.hpp"

using namespace std;

/***************************************************************
HOUND AI CLASS DEFINITION
*/

HoundAI::HoundAI(
    unsigned id,
    unsigned agent_speed,
    mt19937_64* rng
)
    : AI(id, agent_speed, rng)
{
}

vector<string> HoundAI::Run(
    Percepts& percepts,
    AgentComm * comms
) {

    // DO NOT LEAVE THIS CODE (COMMENTED OUT OR IN) IN
    // YOUR SUBMISSION

    cout << "\n=========================\n";
    cout << "HOUND ID: " << id << endl;
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
        for (size_t i = 0; i < comms->bark.size(); i++) {
            cout << "HOUND " << i << " Bark: " << comms->bark[i]
                << " Dir: " << comms->direction[i].to_string()
                << endl;
        }
    }

    /***************************************************************
    YOUR HOUND AI CODE GOES HERE.
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

    // If the AI's id is 0, it sets its message to 9.
    // This demonstrates how you can pass messages.
    if (id == 0) {
        comms->bark[0] = 9;
    }

    vector<string> cmds;

    vector<string> arr = { "F", "L", "R" };

    for (unsigned i = 0; i < agent_speed; i++) {
        shuffle(arr.begin(), arr.end(), *rng);
        cmds.push_back(arr[0]);
    }
    return cmds;
}