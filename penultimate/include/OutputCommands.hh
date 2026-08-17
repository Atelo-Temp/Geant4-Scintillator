#ifndef MyOutputCommands_HH
#define MyOutputCommands_HH

// User lib
#include "OutputConfig.hh"

// C lib
#include <array>
// #include <unordered_map>

// G4 lib
// #include "G4UIcommand.hh"

/*
 * The four umbrella categories of output data
 */
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
    OutputType const type;
};

/*
 * ....
 */
struct EventCommand : public CustomBoolCommand {
    // Pointer to a bool member of EventFlags
    bool EventFlags::* member;
    // NOTE: pointer-to-member
    // const?
    // bool EventFlags::* const member;
};

/*
 * ....
 */
struct StepDetectionCommand : public CustomBoolCommand {
    // Pointer to a bool member of StepDetectionFlags
    bool StepDetectionFlags::* member;
    // NOTE: pointer-to-member
    // const?
};

/*
 * ....
 */
struct StepBoundaryAbsorbCommand : public CustomBoolCommand {
    // Pointer to a bool member of StepBoundaryAbsorbFlags
    bool StepBoundaryAbsorbFlags::* member;
    // NOTE: pointer-to-member
    // const?
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
};

/*
 * Exposed API for controlling simulation output
 * 
 * Compile-time lookup table
 * 
 * NOTE: Each struct essentially states: "this command corresponds to this member of StateFlags"
 */
inline constexpr std::array<EventCommand, 4> EventCommands = {{ // TODO: EventCommandDefinitions
    //////////////
    // Event flags
    //////////////
    
    // Per-event detections
    {
        "fDetection", // name
        "/output/event/detection", // command path
        "Enable per-event detected photons output.", // guidance
        // EventCommand.type,
        // &EventCommand::type,
        OutputType::Event, // umbrella category
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
    }
    // { // NOTE: Not writing per-event boundary absorption counts
    // "fBoundaryAbsorb",
    // "/output/step/...",
    // "Enable ... output.",
        // &StepBoundaryAbsorbFlags::fBoundaryAbsorb
    // },
    // { // NOTE: Not writing per-event bulk absorption counts
    // "fBulkAbsorb",
    // "/output/step/...",
    // "Enable ... output.",
        // &StepBulkAbsorbFlags::fBulkAbsorbNtuple
    // },
}};

/*
 * ....
 */
inline constexpr std::array<StepDetectionCommand, 4> StepDetectionCommands = {{
    ///////////////////////
    // Step detection flags
    ///////////////////////
    
    // ...
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
    }
}};

/*
 * ....
 */
inline constexpr std::array<StepBoundaryAbsorbCommand, 1> StepBoundaryAbsorbCommands = {{
    /////////////////////////////////
    // Step boundary absorption flags
    /////////////////////////////////
    
    {
        "fBoundaryAbsorbCoords",
        "/output/step/boundaryAbsorb/coords",
        "Enable boundary absorption coordinates output.",
        OutputType::StepBoundaryAbsorb,
        &StepBoundaryAbsorbFlags::fBoundaryAbsorbCoordsNtuple
    }
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
