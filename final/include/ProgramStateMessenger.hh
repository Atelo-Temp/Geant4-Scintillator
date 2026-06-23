#ifndef MyProgramStateMessenger_HH
#define MyProgramStateMessenger_HH

// User classes
#include "ProgramState.hh" // TODO:  Maybe extract StateFlags, BoolCommand, and StateCommands

// C lib
#include <unordered_map>
#include <array>

// G4 lib
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
// #include "G4UIcmdWithABool.hh"
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
 * ....
 */
// struct BoolCommand {
//     // Name of the command
//     G4String name; // NOTE: Unused property currently, potentially remove
//     
//     // The command path exposed to the UI
//     G4String cmdPath;
//     
//     // Guidance message for explaining endpoint usage
//     G4String cmdGuidance;
//     
//     // Pointer to a bool member of StateFlags
//     bool StateFlags::* member;
//     // NOTE: pointer-to-member
// };

/*
 * ....
 */
struct BoolCommand {
    // Name of the command
    char const* name; // NOTE: Unused property currently, potentially remove
    
    // The command path exposed to the UI
    char const* cmdPath;
    
    // Guidance message for explaining endpoint usage
    char const* cmdGuidance;
    
    // Pointer to a bool member of StateFlags
    bool StateFlags::* member;
    // NOTE: pointer-to-member
    // const?
    
    // constexpr BoolCommand() {};
};

/*
 * Exposed API for controlling simulation output
 * 
 * Compile-time lookup table
 * 
 * NOTE: Each struct essentially states: "this command corresponds to this member of StateFlags"
 */
// std::vector<BoolCommand> const StateCommands = {
// inline constexpr std::array<BoolCommand, 11> StateCommands = {
// constexpr BoolCommand StateCommands[] = {
// constexpr std::array<BoolCommand, 11> StateCommands = {{
inline constexpr std::array<BoolCommand, 11> StateCommands = {{
    //////////////
    // Event flags
    //////////////
    
    // Per-event detections
    {
        "fDetection",
        "/output/event/detection",
        "Enable per-event detected photons output.",
        &StateFlags::fDetectionNtuple // NOTE: Create the pointer-to-member
        // NOTE: Pointer to the fDetectionNtuple field inside StateFlags, havent got an actual object yet,
        // so it cant be the address of a particular bool - its just describing which member of the struct
        // NOTE: This pointer does not contain a memory address, since any object of type StateFlags could
        // be used for assignment, it only contains some compiler representation of "member #1 of StateFlags"
    },
    // Per-event detections fraction
    {
        "fDetectionFraction",
        "/output/event/detectionFraction",
        "Enable per-event photons detected fraction output.",
        &StateFlags::fDetectionFractionNtuple
    },
    // Per-event boundary absorptions fraction
    {
        "fBoundaryAbsorbFraction",
        "/output/event/boundaryAbsorbFraction",
        "Enable per-event photons absorbed at boundary fraction output.",
        &StateFlags::fBoundaryAbsorbFractionNtuple
    },
    // Per-event bulk absorptions fraction
    {
        "fBulkAbsorbFraction",
        "/output/event/bulkAbsorbFraction",
        "Enable per-event photons bulk absorbed fraction output.",
        &StateFlags::fBulkAbsorbFractionNtuple
    },
    
    ///////////////////////
    // Step detection flags
    ///////////////////////
    
    {
        "fDetectionCoords",
        "/output/step/detectionCoords",
        "Enable detection coordinates output.",
        &StateFlags::fDetectionCoordsNtuple
    },
    {
        "fDetectionDistance",
        "/output/step/detectionDistance",
        "Enable distance travelled by photon before detection output.",
        &StateFlags::fDetectionDistanceNtuple
    },
    {
        "fDetectionTimeOfFlight",
        "/output/step/detectionTimeOfFlight",
        "Enable time duration of travel by photon before detection output.",
        &StateFlags::fDetectionTimeOfFlightNtuple
    },
    {
        "fDetectionReflections",
        "/output/step/detectionReflections",
        "Enable number of reflections before detection output.",
        &StateFlags::fDetectionReflectionsNtuple
    },
    
    /////////////////////////////////
    // Step boundary absorption flags
    /////////////////////////////////
    
    // { // NOTE: Not writing per-event boundary absorption counts
    // "fBoundaryAbsorb",
    // "/output/step/...",
    // "Enable ... output.",
        // &StateFlags::fBoundaryAbsorb
    // },
    {
        "fBoundaryAbsorbCoords",
        "/output/step/boundaryAbsorbCoords",
        "Enable boundary absorption coordinates output.",
        &StateFlags::fBoundaryAbsorbCoordsNtuple
    },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbDistance",
    // "/output/step/...",
    // "Enable ... output.",
        // &StateFlags::fBoundaryAbsorbDistanceNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbTimeOfFlight",
    // "/output/step/...",
    // "Enable ... output.",
        // &StateFlags::fBoundaryAbsorbTimeOfFlightNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbReflections",
    // "/output/step/...",
    // "Enable ... output.",
        // &StateFlags::fBoundaryAbsorbReflectionsNtuple
    // },
    
    /////////////////////////////
    // Step bulk absorption flags
    /////////////////////////////
    
    // { // NOTE: Not writing per-event bulk absorption counts
    // "fBulkAbsorb",
    // "/output/step/...",
    // "Enable ... output.",
        // &StateFlags::fBulkAbsorbNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBulkAbsorbCoords",
    // "/output/step/...",
    // "Enable ... output.",
        // &StateFlags::fBulkAbsorbCoordsNtuple
    // },
    {
        "fBulkAbsorbDistance",
        "/output/step/bulkAbsorbDistance",
        "Enable distance travelled by photon before bulk absorption output.",
        &StateFlags::fBulkAbsorbDistanceNtuple
    },
    // { // NOTE: Not yet implemented
    // "fBulkAbsorbTimeOfFlight",
    // "/output/step/...",
    // "Enable ... output.",
        // &StateFlags::fBulkAbsorbTimeOfFlightNtuple
    // },
    {
        "fBulkAbsorbReflections",
        "/output/step/bulkAbsorbReflections",
        "Enable number of reflections before bulk absorption output.",
        &StateFlags::fBulkAbsorbReflectionsNtuple
    }
}};

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
        // ProgramState& fProgramState;
        ProgramState* fProgramState;
        
        // Commands (cmd pointer, cmd path, guidance msg)
        // StateCommands fStateCmds;
        // std::vector<BoolCommand> fStateCmds;
        // constexpr BoolCommand[] fStateCmds = StateCommands;
        // BoolCommand fStateCmds[11] = StateCommands;
        // auto fStateCmds = StateCommands;
        // std::array<BoolCommand, 11> fStateCommands = StateCommands;
        
        // ...
        // std::unordered_map<G4UIcommand*, G4String> cmdMap;
        std::unordered_map<G4UIcommand*, BoolCommand> fCmdMap;
};

#endif
