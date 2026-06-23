#ifndef MyProgramStateMessenger_HH
#define MyProgramStateMessenger_HH

// User classes
#include "ProgramState.hh"

// C lib
#include <unordered_map>

// G4 lib
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithABool.hh"
// #include "G4UIcmdWithAnInteger.hh"

// Forward declarations
// class ProgramState;
// NOTE: ProgramState class only holds a pointer to a ProgramStateMessenger instance on the heap,
// since a pointer has a fixed memory size, there is no need to import here, import in ".cc" file
// NOTE: Since both ProgramState and ProgramStateMessenger hold references to oneanother, importing
// would also create circular dependencies

/*
 * ...
 * 
 * TODO: This repetition of StateFlags feels rough, is there better way?
 * 
 * TODO: Metadata list (implement after explicit version)
 */
// struct StateCommands {
//     // Event flags
//     G4UIcmdWithABool* fDetectionOutput = nullptr; // Per-event detections
//     G4UIcmdWithABool* fDetectionFractionOutput = nullptr; // Per-event detections fraction
//     G4UIcmdWithABool* fBoundaryAbsorbFractionOutput = nullptr; // Per-event boundary absorptions fraction
//     G4UIcmdWithABool* fBulkAbsorbFractionOutput = nullptr; // Per-event bulk absorptions fraction
//     
//     // Step detection flags
//     G4UIcmdWithABool* fDetectionCoordsOutput = nullptr;
//     G4UIcmdWithABool* fDetectionDistanceOutput = nullptr;
//     G4UIcmdWithABool* fDetectionTimeOfFlightOutput = nullptr;
//     G4UIcmdWithABool* fDetectionReflectionsOutput = nullptr;
//     
//     // Step boundary absorption flags
//     // G4UIcmdWithABool* fBoundaryAbsorbOutput = nullptr; // NOTE: Not writing per-event boundary absorption counts
//     G4UIcmdWithABool* fBoundaryAbsorbCoordsOutput = nullptr;
//     // G4UIcmdWithABool* fBoundaryAbsorbDistanceOutput = nullptr; // NOTE: Not yet implemented
//     // G4UIcmdWithABool* fBoundaryAbsorbTimeOfFlightOutput = nullptr; // NOTE: Not yet implemented
//     // G4UIcmdWithABool* fBoundaryAbsorbReflectionsOutput = nullptr;  // NOTE: Not yet implemented
//     
//     // Step bulk absorption flags
//     // G4UIcmdWithABool* fBulkAbsorbOutput = nullptr; // NOTE: Not writing per-event bulk absorption counts
//     // G4UIcmdWithABool* fBulkAbsorbCoordsOutput = nullptr; // NOTE: Not yet implemented
//     G4UIcmdWithABool* fBulkAbsorbDistanceOutput = nullptr;
//     // G4UIcmdWithABool* fBulkAbsorbTimeOfFlightOutput = nullptr; // NOTE: Not yet implemented
//     G4UIcmdWithABool* fBulkAbsorbReflectionsOutput = nullptr;
// };

/*
 * ...
 */
// struct BoolCommand {
//     G4UIcmdWithABool* command;
//     G4String cmdPath;
//     G4String cmdGuidance;
// };

/*
 * ...
 */
// struct StateCommands {
//     //////////////
//     // Event flags
//     //////////////
//     
//     // Per-event detections
//     BoolCommand fDetection {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     // Per-event detections fraction
//     BoolCommand fDetectionFraction {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     // Per-event boundary absorptions fraction
//     BoolCommand fBoundaryAbsorbFraction {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     // Per-event bulk absorptions fraction
//     BoolCommand fBulkAbsorbFraction {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     
//     ///////////////////////
//     // Step detection flags
//     ///////////////////////
//     
//     BoolCommand fDetectionCoords {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     BoolCommand fDetectionDistance {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     BoolCommand fDetectionTimeOfFlight {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     BoolCommand fDetectionReflections {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     
//     /////////////////////////////////
//     // Step boundary absorption flags
//     /////////////////////////////////
//     
//     // BoolCommand fBoundaryAbsorb { // NOTE: Not writing per-event boundary absorption counts
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // };
//     BoolCommand fBoundaryAbsorbCoords {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     // BoolCommand fBoundaryAbsorbDistance { // NOTE: Not yet implemented
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // };
//     // BoolCommand fBoundaryAbsorbTimeOfFlight { // NOTE: Not yet implemented
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // };
//     // BoolCommand fBoundaryAbsorbReflections { // NOTE: Not yet implemented
//     // nullptr, 
//     // "/output/...",
//     // "Enable ... output."
//     // };
//     
//     /////////////////////////////
//     // Step bulk absorption flags
//     /////////////////////////////
//     
//     // BoolCommand fBulkAbsorb { // NOTE: Not writing per-event bulk absorption counts
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // };
//     // BoolCommand fBulkAbsorbCoords { // NOTE: Not yet implemented
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // };
//     BoolCommand fBulkAbsorbDistance {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
//     // BoolCommand fBulkAbsorbTimeOfFlight { // NOTE: Not yet implemented
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // };
//     BoolCommand fBulkAbsorbReflections {
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     };
// };

// /*
//  * ....
//  */
// struct BoolCommand {
//     G4String name;
//     G4UIcmdWithABool* command;
//     G4String cmdPath;
//     G4String cmdGuidance;
// };
// 
// /*
//  * ...
//  */
// std::vector<BoolCommand> StateCommands = {
//     //////////////
//     // Event flags
//     //////////////
//     
//     // Per-event detections
//     {
//         "fDetection",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     // Per-event detections fraction
//     {
//         "fDetectionFraction",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     // Per-event boundary absorptions fraction
//     {
//         "fBoundaryAbsorbFraction",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     // Per-event bulk absorptions fraction
//     {
//         "fBulkAbsorbFraction",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     
//     ///////////////////////
//     // Step detection flags
//     ///////////////////////
//     
//     {
//         "fDetectionCoords",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     {
//         "fDetectionDistance",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     {
//         "fDetectionTimeOfFlight",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     {
//         "fDetectionReflections",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     
//     /////////////////////////////////
//     // Step boundary absorption flags
//     /////////////////////////////////
//     
//     // { // NOTE: Not writing per-event boundary absorption counts
//     // "fBoundaryAbsorb",
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     {
//         "fBoundaryAbsorbCoords",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbDistance",
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbTimeOfFlight",
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbReflections",
//     // nullptr, 
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     
//     /////////////////////////////
//     // Step bulk absorption flags
//     /////////////////////////////
//     
//     // { // NOTE: Not writing per-event bulk absorption counts
//     // "fBulkAbsorb",
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBulkAbsorbCoords",
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     {
//         "fBulkAbsorbDistance",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     },
//     // { // NOTE: Not yet implemented
//     // "fBulkAbsorbTimeOfFlight",
//     // nullptr,
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     {
//         "fBulkAbsorbReflections",
//         nullptr,
//         "/output/",
//         "Enable ... output."
//     }
// };


// /*
//  * ....
//  */
// struct BoolCommand {
//     G4String name;
//     G4String cmdPath;
//     G4String cmdGuidance;
// };
// 
// /*
//  * ...
//  */
// std::vector<BoolCommand> StateCommands = {
// // constexpr BoolCommand StateCommands[] = {
//     //////////////
//     // Event flags
//     //////////////
//     
//     // Per-event detections
//     {
//         "fDetection",
//         "/output/...",
//         "Enable ... output."
//     },
//     // Per-event detections fraction
//     {
//         "fDetectionFraction",
//         "/output/...",
//         "Enable ... output."
//     },
//     // Per-event boundary absorptions fraction
//     {
//         "fBoundaryAbsorbFraction",
//         "/output/...",
//         "Enable ... output."
//     },
//     // Per-event bulk absorptions fraction
//     {
//         "fBulkAbsorbFraction",
//         "/output/...",
//         "Enable ... output."
//     },
//     
//     ///////////////////////
//     // Step detection flags
//     ///////////////////////
//     
//     {
//         "fDetectionCoords",
//         "/output/...",
//         "Enable ... output."
//     },
//     {
//         "fDetectionDistance",
//         "/output/...",
//         "Enable ... output."
//     },
//     {
//         "fDetectionTimeOfFlight",
//         "/output/...",
//         "Enable ... output."
//     },
//     {
//         "fDetectionReflections",
//         "/output/...",
//         "Enable ... output."
//     },
//     
//     /////////////////////////////////
//     // Step boundary absorption flags
//     /////////////////////////////////
//     
//     // { // NOTE: Not writing per-event boundary absorption counts
//     // "fBoundaryAbsorb",
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     {
//         "fBoundaryAbsorbCoords",
//         "/output/...",
//         "Enable ... output."
//     },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbDistance",
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbTimeOfFlight",
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbReflections",
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     
//     /////////////////////////////
//     // Step bulk absorption flags
//     /////////////////////////////
//     
//     // { // NOTE: Not writing per-event bulk absorption counts
//     // "fBulkAbsorb",
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBulkAbsorbCoords",
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     {
//         "fBulkAbsorbDistance",
//         "/output/...",
//         "Enable ... output."
//     },
//     // { // NOTE: Not yet implemented
//     // "fBulkAbsorbTimeOfFlight",
//     // "/output/...",
//     // "Enable ... output."
//     // },
//     {
//         "fBulkAbsorbReflections",
//         "/output/...",
//         "Enable ... output."
//     }
// };

/*
 * ....
 */
struct BoolCommand {
    G4String name;
    G4String cmdPath;
    G4String cmdGuidance;
    bool StateFlags::* member;
};

/*
 * ...
 */
std::vector<BoolCommand> StateCommands = {
// constexpr BoolCommand StateCommands[] = {
    //////////////
    // Event flags
    //////////////
    
    // Per-event detections
    {
        "fDetection",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fDetectionNtuple
    },
    // Per-event detections fraction
    {
        "fDetectionFraction",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fDetectionFractionNtuple
    },
    // Per-event boundary absorptions fraction
    {
        "fBoundaryAbsorbFraction",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fBoundaryAbsorbFractionNtuple
    },
    // Per-event bulk absorptions fraction
    {
        "fBulkAbsorbFraction",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fBulkAbsorbFractionNtuple
    },
    
    ///////////////////////
    // Step detection flags
    ///////////////////////
    
    {
        "fDetectionCoords",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fDetectionCoordsNtuple
    },
    {
        "fDetectionDistance",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fDetectionDistanceNtuple
    },
    {
        "fDetectionTimeOfFlight",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fDetectionTimeOfFlightNtuple
    },
    {
        "fDetectionReflections",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fDetectionReflectionsNtuple
    },
    
    /////////////////////////////////
    // Step boundary absorption flags
    /////////////////////////////////
    
    // { // NOTE: Not writing per-event boundary absorption counts
    // "fBoundaryAbsorb",
    // "/output/...",
    // "Enable ... output.",
        // &StateFlags::fBoundaryAbsorb
    // },
    {
        "fBoundaryAbsorbCoords",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fBoundaryAbsorbCoordsNtuple
    },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbDistance",
    // "/output/...",
    // "Enable ... output.",
        // &StateFlags::fBoundaryAbsorbDistanceNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbTimeOfFlight",
    // "/output/...",
    // "Enable ... output.",
        // &StateFlags::fBoundaryAbsorbTimeOfFlightNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbReflections",
    // "/output/...",
    // "Enable ... output.",
        // &StateFlags::fBoundaryAbsorbReflectionsNtuple
    // },
    
    /////////////////////////////
    // Step bulk absorption flags
    /////////////////////////////
    
    // { // NOTE: Not writing per-event bulk absorption counts
    // "fBulkAbsorb",
    // "/output/...",
    // "Enable ... output.",
        // &StateFlags::fBulkAbsorbNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBulkAbsorbCoords",
    // "/output/...",
    // "Enable ... output.",
        // &StateFlags::fBulkAbsorbCoordsNtuple
    // },
    {
        "fBulkAbsorbDistance",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fBulkAbsorbDistanceNtuple
    },
    // { // NOTE: Not yet implemented
    // "fBulkAbsorbTimeOfFlight",
    // "/output/...",
    // "Enable ... output.",
        // &StateFlags::fBulkAbsorbTimeOfFlightNtuple
    // },
    {
        "fBulkAbsorbReflections",
        "/output/...",
        "Enable ... output.",
        &StateFlags::fBulkAbsorbReflectionsNtuple
    }
};

/*
 * ...
 * 
 * Messenger has the responsibility of creating and deleting commands
 * 
 * Also takes care of delivering commands to the destination class, and provides
 * the current value(s) for the parameter(s)
 * 
 * NOTE: Extends base class "G4UImessenger"
 */
class ProgramStateMessenger : public G4UImessenger {
    public:
        // Constructor - should define all commands related to this messenger
        ProgramStateMessenger();
        
        // Destructor - all commands defined in constructor must be deleted
        ~ProgramStateMessenger() override;
        
        // Converts the string newVal to value(s) of the type(s) of parameter(s)
        void SetNewValue(G4UIcommand* cmd, G4String newVal) override;
    
    private:
        // Maintain reference to program state singleton
        ProgramState& fProgramState;
        
        // Commands (cmd pointer, cmd path, guidance msg)
        // StateCommands fStateCmds;
        std::vector<BoolCommand> fStateCmds;
        
        // ...
        // std::unordered_map<G4UIcommand*, G4String> cmdMap;
        std::unordered_map<G4UIcommand*, BoolCommand> fCmdMap;
};

#endif
