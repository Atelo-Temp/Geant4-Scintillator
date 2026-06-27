#ifndef MyOutputConfig_HH
#define MyOutputConfig_HH

// User lib
#include "ProgramState.hh"

// C lib
#include <array>
// #include <unordered_map>
// #include "G4UIcommand.hh"

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
// struct BoolCommand {
//     // Name of the command
//     char const* name; // NOTE: Unused property currently, potentially remove
//     
//     // The command path exposed to the UI
//     char const* cmdPath;
//     
//     // Guidance message for explaining endpoint usage
//     char const* cmdGuidance;
//     
//     // Pointer to a bool member of StateFlags
//     bool StateFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
//     
//     // constexpr BoolCommand() {};
// };

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
// inline constexpr std::array<BoolCommand, 11> StateCommands = {{
//     //////////////
//     // Event flags
//     //////////////
//     
//     // Per-event detections
//     {
//         "fDetection",
//         "/output/event/detection",
//         "Enable per-event detected photons output.",
//         &StateFlags::fDetectionNtuple // NOTE: Create the pointer-to-member
//         // NOTE: Pointer to the fDetectionNtuple field inside StateFlags, havent got an actual object yet,
//         // so it cant be the address of a particular bool - its just describing which member of the struct
//         // NOTE: This pointer does not contain a memory address, since any object of type StateFlags could
//         // be used for assignment, it only contains some compiler representation of "member #1 of StateFlags"
//     },
//     // Per-event detections fraction
//     {
//         "fDetectionFraction",
//         "/output/event/detectionFraction",
//         "Enable per-event photons detected fraction output.",
//         &StateFlags::fDetectionFractionNtuple
//     },
//     // Per-event boundary absorptions fraction
//     {
//         "fBoundaryAbsorbFraction",
//         "/output/event/boundaryAbsorbFraction",
//         "Enable per-event photons absorbed at boundary fraction output.",
//         &StateFlags::fBoundaryAbsorbFractionNtuple
//     },
//     // Per-event bulk absorptions fraction
//     {
//         "fBulkAbsorbFraction",
//         "/output/event/bulkAbsorbFraction",
//         "Enable per-event photons bulk absorbed fraction output.",
//         &StateFlags::fBulkAbsorbFractionNtuple
//     },
//     
//     ///////////////////////
//     // Step detection flags
//     ///////////////////////
//     
//     {
//         "fDetectionCoords",
//         "/output/step/detectionCoords",
//         "Enable detection coordinates output.",
//         &StateFlags::fDetectionCoordsNtuple
//     },
//     {
//         "fDetectionDistance",
//         "/output/step/detectionDistance",
//         "Enable distance travelled by photon before detection output.",
//         &StateFlags::fDetectionDistanceNtuple
//     },
//     {
//         "fDetectionTimeOfFlight",
//         "/output/step/detectionTimeOfFlight",
//         "Enable time duration of travel by photon before detection output.",
//         &StateFlags::fDetectionTimeOfFlightNtuple
//     },
//     {
//         "fDetectionReflections",
//         "/output/step/detectionReflections",
//         "Enable number of reflections before detection output.",
//         &StateFlags::fDetectionReflectionsNtuple
//     },
//     
//     /////////////////////////////////
//     // Step boundary absorption flags
//     /////////////////////////////////
//     
//     // { // NOTE: Not writing per-event boundary absorption counts
//     // "fBoundaryAbsorb",
//     // "/output/step/...",
//     // "Enable ... output.",
//         // &StateFlags::fBoundaryAbsorb
//     // },
//     {
//         "fBoundaryAbsorbCoords",
//         "/output/step/boundaryAbsorbCoords",
//         "Enable boundary absorption coordinates output.",
//         &StateFlags::fBoundaryAbsorbCoordsNtuple
//     },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbDistance",
//     // "/output/step/...",
//     // "Enable ... output.",
//         // &StateFlags::fBoundaryAbsorbDistanceNtuple
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbTimeOfFlight",
//     // "/output/step/...",
//     // "Enable ... output.",
//         // &StateFlags::fBoundaryAbsorbTimeOfFlightNtuple
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBoundaryAbsorbReflections",
//     // "/output/step/...",
//     // "Enable ... output.",
//         // &StateFlags::fBoundaryAbsorbReflectionsNtuple
//     // },
//     
//     /////////////////////////////
//     // Step bulk absorption flags
//     /////////////////////////////
//     
//     // { // NOTE: Not writing per-event bulk absorption counts
//     // "fBulkAbsorb",
//     // "/output/step/...",
//     // "Enable ... output.",
//         // &StateFlags::fBulkAbsorbNtuple
//     // },
//     // { // NOTE: Not yet implemented
//     // "fBulkAbsorbCoords",
//     // "/output/step/...",
//     // "Enable ... output.",
//         // &StateFlags::fBulkAbsorbCoordsNtuple
//     // },
//     {
//         "fBulkAbsorbDistance",
//         "/output/step/bulkAbsorbDistance",
//         "Enable distance travelled by photon before bulk absorption output.",
//         &StateFlags::fBulkAbsorbDistanceNtuple
//     },
//     // { // NOTE: Not yet implemented
//     // "fBulkAbsorbTimeOfFlight",
//     // "/output/step/...",
//     // "Enable ... output.",
//         // &StateFlags::fBulkAbsorbTimeOfFlightNtuple
//     // },
//     {
//         "fBulkAbsorbReflections",
//         "/output/step/bulkAbsorbReflections",
//         "Enable number of reflections before bulk absorption output.",
//         &StateFlags::fBulkAbsorbReflectionsNtuple
//     }
// }};

// /*
//  * ....
//  */
// struct CustomCommand {
//     // Name of the command
//     char const* name; // NOTE: Unused property currently, potentially remove
//     
//     // The command path exposed to the UI
//     char const* cmdPath;
//     
//     // Guidance message for explaining endpoint usage
//     char const* cmdGuidance;
// };
// 
// /*
//  * ....
//  */
// struct EventCommand : public CustomCommand {
//     // Pointer to a bool member of EventFlags
//     bool EventFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
// };
// 
// /*
//  * ....
//  */
// struct StepDetectionCommand : public CustomCommand {
//     // Pointer to a bool member of StepDetectionFlags
//     bool StepDetectionFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
// };
// 
// /*
//  * ....
//  */
// struct StepBoundaryAbsorbCommand : public CustomCommand {
//     // Pointer to a bool member of StepBoundaryAbsorbFlags
//     bool StepBoundaryAbsorbFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
// };
// 
// /*
//  * ....
//  */
// struct StepBulkAbsorbCommand : public CustomCommand {
//     // Pointer to a bool member of StepBulkAbsorbFlags
//     bool StepBulkAbsorbFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
//     
//     // StateFlags::* a = &StateFlags::StepBulkAbsorbFlags;
//     
//     // StateFlags::StepBulkAbsorbFlags StateFlags::* ptr_to_inner;
//     // StateFlags::fStepBulkAbsorbFlags StateFlags::* ptr_to_inner;
// };

/*
 * ....
 */
// template <typename Derived>
// struct CustomCommand {
//     // Name of the command
//     char const* name; // NOTE: Unused property currently, potentially remove
//     
//     // The command path exposed to the UI
//     char const* cmdPath;
//     
//     // Guidance message for explaining endpoint usage
//     char const* cmdGuidance;
//     
//     bool Derived::* member;
// };
// 
// /*
//  * ....
//  */
// // struct EventCommand : public CustomCommand {
// struct EventCommand : public CustomCommand<EventFlags> {
//     // Pointer to a bool member of EventFlags
//     // bool EventFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
// };
// 
// /*
//  * ....
//  */
// // struct StepDetectionCommand : public CustomCommand {
// struct StepDetectionCommand : public CustomCommand<StepDetectionFlags> {
//     // Pointer to a bool member of StepDetectionFlags
//     bool StepDetectionFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
// };
// 
// /*
//  * ....
//  */
// // struct StepBoundaryAbsorbCommand : public CustomCommand {
// struct StepBoundaryAbsorbCommand : public CustomCommand<StepBoundaryAbsorbFlags> {
//     // Pointer to a bool member of StepBoundaryAbsorbFlags
//     bool StepBoundaryAbsorbFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
// };
// 
// /*
//  * ....
//  */
// struct StepBulkAbsorbCommand : public CustomCommand<StepBulkAbsorbFlags> {
//     // Pointer to a bool member of StepBulkAbsorbFlags
//     bool StepBulkAbsorbFlags::* member;
//     // NOTE: pointer-to-member
//     // const?
// };


/*
 * ...
 */
// enum class CmdTypeAlias {
enum class OutputType {
    Event,
    StepDetection,
    StepBoundaryAbsorb,
    StepBulkAbsorb
};

/*
 * Interface required to create a "G4UIcmdWithABool"
 */
struct CustomBoolCommand {
    // Name of the command
    char const* name; // NOTE: Unused property currently, potentially remove
    
    // The command path exposed to the UI
    char const* cmdPath;
    
    // Guidance message for explaining endpoint usage
    char const* cmdGuidance;
    
    // TEST
    // char const* type;
    // TypeAlias type;
    OutputType const type;
};

/*
 * ....
 */
struct EventCommand : public CustomBoolCommand {
    // OutputType const type = OutputType::Event;
    
    // Pointer to a bool member of EventFlags
    bool EventFlags::* member;
    // NOTE: pointer-to-member
    // const?
    
    // ...
    // char const* type = "Event";
    // TypeAlias type = TypeAlias::Event;
    // OutputType const type = OutputType::Event;
};

/*
 * ....
 */
struct StepDetectionCommand : public CustomBoolCommand {
    // Pointer to a bool member of StepDetectionFlags
    bool StepDetectionFlags::* member;
    // NOTE: pointer-to-member
    // const?
    
    // ...
    // TypeAlias type = TypeAlias::StepDetection;
    // OutputType const type = OutputType::StepDetection;
};

/*
 * ....
 */
struct StepBoundaryAbsorbCommand : public CustomBoolCommand {
    // Pointer to a bool member of StepBoundaryAbsorbFlags
    bool StepBoundaryAbsorbFlags::* member;
    // NOTE: pointer-to-member
    // const?
    
    // ...
    // TypeAlias type = TypeAlias::StepBoundaryAbsorb;
    // OutputType const type = OutputType::StepBoundaryAbsorb;
};

/*
 * ....
 */
struct StepBulkAbsorbCommand : public CustomBoolCommand {
    // Pointer to a bool member of StepBulkAbsorbFlags
    bool StepBulkAbsorbFlags::* member;
    // NOTE: pointer-to-member
    // const?
    
    // StateFlags::* a = &StateFlags::StepBulkAbsorbFlags;
    
    // StateFlags::StepBulkAbsorbFlags StateFlags::* ptr_to_inner;
    // StateFlags::fStepBulkAbsorbFlags StateFlags::* ptr_to_inner;
    
    // ...
    // TypeAlias type = TypeAlias::StepBulkAbsorb;
    // OutputType const type = OutputType::StepBulkAbsorb;
};

/*
 * Exposed API for controlling simulation output
 * 
 * Compile-time lookup table
 * 
 * NOTE: Each struct essentially states: "this command corresponds to this member of StateFlags"
 */
inline constexpr std::array<EventCommand, 4> EventCommands = {{
    //////////////
    // Event flags
    //////////////
    
    // Per-event detections
    {
        "fDetection",
        "/output/event/detection",
        "Enable per-event detected photons output.",
        // EventCommand.type,
        // &EventCommand::type,
        OutputType::Event,
        &EventFlags::fDetectionNtuple // NOTE: Create the pointer-to-member
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
        OutputType::Event,
        &EventFlags::fDetectionFractionNtuple
    },
    // Per-event boundary absorptions fraction
    {
        "fBoundaryAbsorbFraction",
        "/output/event/boundaryAbsorbFraction",
        "Enable per-event photons absorbed at boundary fraction output.",
        OutputType::Event,
        &EventFlags::fBoundaryAbsorbFractionNtuple
    },
    // Per-event bulk absorptions fraction
    {
        "fBulkAbsorbFraction",
        "/output/event/bulkAbsorbFraction",
        "Enable per-event photons bulk absorbed fraction output.",
        OutputType::Event,
        &EventFlags::fBulkAbsorbFractionNtuple
    },
}};

/*
 * ....
 */
inline constexpr std::array<StepDetectionCommand, 4> StepDetectionCommands = {{
    ///////////////////////
    // Step detection flags
    ///////////////////////
    
    {
        "fDetectionCoords",
        "/output/step/detection/coords",
        "Enable detection coordinates output.",
        OutputType::StepDetection,
        &StepDetectionFlags::fDetectionCoordsNtuple
    },
    {
        "fDetectionDistance",
        "/output/step/detection/distance",
        "Enable distance travelled by photon before detection output.",
        OutputType::StepDetection,
        &StepDetectionFlags::fDetectionDistanceNtuple
    },
    {
        "fDetectionTimeOfFlight",
        "/output/step/detection/timeOfFlight",
        "Enable time duration of travel by photon before detection output.",
        OutputType::StepDetection,
        &StepDetectionFlags::fDetectionTimeOfFlightNtuple
    },
    {
        "fDetectionReflections",
        "/output/step/detection/reflections",
        "Enable number of reflections before detection output.",
        OutputType::StepDetection,
        &StepDetectionFlags::fDetectionReflectionsNtuple
    },
}};

/*
 * ....
 */
inline constexpr std::array<StepBoundaryAbsorbCommand, 1> StepBoundaryAbsorbCommands = {{
    /////////////////////////////////
    // Step boundary absorption flags
    /////////////////////////////////
    
    // { // NOTE: Not writing per-event boundary absorption counts
    // "fBoundaryAbsorb",
    // "/output/step/...",
    // "Enable ... output.",
        // &StepBoundaryAbsorbFlags::fBoundaryAbsorb
    // },
    {
        "fBoundaryAbsorbCoords",
        "/output/step/boundaryAbsorb/coords",
        "Enable boundary absorption coordinates output.",
        OutputType::StepBoundaryAbsorb,
        &StepBoundaryAbsorbFlags::fBoundaryAbsorbCoordsNtuple
    },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbDistance",
    // "/output/step/...",
    // "Enable ... output.",
        // &StepBoundaryAbsorbFlags::fBoundaryAbsorbDistanceNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbTimeOfFlight",
    // "/output/step/...",
    // "Enable ... output.",
        // &StepBoundaryAbsorbFlags::fBoundaryAbsorbTimeOfFlightNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBoundaryAbsorbReflections",
    // "/output/step/...",
    // "Enable ... output.",
        // &StepBoundaryAbsorbFlags::fBoundaryAbsorbReflectionsNtuple
    // },
}};

/*
 * ....
 */
inline constexpr std::array<StepBulkAbsorbCommand, 2> StepBulkAbsorbCommands = {{
    /////////////////////////////
    // Step bulk absorption flags
    /////////////////////////////
    
    // { // NOTE: Not writing per-event bulk absorption counts
    // "fBulkAbsorb",
    // "/output/step/...",
    // "Enable ... output.",
        // &StepBulkAbsorbFlags::fBulkAbsorbNtuple
    // },
    // { // NOTE: Not yet implemented
    // "fBulkAbsorbCoords",
    // "/output/step/bulkAbsorb/coords",
    // "Enable ... output.",
        // &StepBulkAbsorbFlags::fBulkAbsorbCoordsNtuple
    // },
    {
        "fBulkAbsorbDistance",
        "/output/step/bulkAbsorb/distance",
        "Enable distance travelled by photon before bulk absorption output.",
        OutputType::StepBulkAbsorb,
        &StepBulkAbsorbFlags::fBulkAbsorbDistanceNtuple
    },
    // { // NOTE: Not yet implemented
    // "fBulkAbsorbTimeOfFlight",
    // "/output/step/...",
    // "Enable ... output.",
        // &StepBulkAbsorbFlags::fBulkAbsorbTimeOfFlightNtuple
    // },
    {
        "fBulkAbsorbReflections",
        "/output/step/bulkAbsorb/reflections",
        "Enable number of reflections before bulk absorption output.",
        OutputType::StepBulkAbsorb,
        &StepBulkAbsorbFlags::fBulkAbsorbReflectionsNtuple
    }
}};


// typedef std::unordered_map<G4UIcommand*, BoolCommand&> afaasf;
// using afsfaf = std::unordered_map<G4UIcommand*, BoolCommand&>;

#endif
