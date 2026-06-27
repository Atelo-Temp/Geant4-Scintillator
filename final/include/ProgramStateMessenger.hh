#ifndef MyProgramStateMessenger_HH
#define MyProgramStateMessenger_HH

// User classes
// #include "ProgramState.hh" // TODO:  Maybe extract StateFlags, BoolCommand, and StateCommands
#include "OutputConfig.hh"

// C lib
#include <unordered_map>
// #include <variant> // TEST
// #include <functional>

// G4 lib
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
// #include "G4UIcmdWithABool.hh"
// #include "G4UIcmdWithAnInteger.hh"

// Forward declarations
class ProgramState;
// NOTE: ProgramState class only holds a pointer to a ProgramStateMessenger instance on the heap,
// since a pointer has a fixed memory size, there is no need to import here, import in ".cc" file
// NOTE: Since both ProgramState and ProgramStateMessenger hold references to oneanother, importing
// would also create circular dependencies
// struct BoolCommand;

/*
 * Output data messenger
 * 
 * Messenger responsible for:
 * - Creating and deleting commands
 * - Exposing state flags to UI
 * - Modifiying program StateFlags (on recieving UI command to do so)
 * 
 * Also takes care of delivering commands to the destination class, and provides
 * the current value(s) for the parameter(s)
 * 
 * NOTE: Extends base class "G4UImessenger"
 */
class ProgramStateMessenger : public G4UImessenger {
    public:
        // Constructor - should define all commands related to this messenger
        ProgramStateMessenger(ProgramState& programStateInstance);
        
        // Destructor - all commands defined in constructor must be deleted
        ~ProgramStateMessenger() override;
        
//         // Instantiate all commands
//         // template <typename T, std::size_t N>
//         // template <CustomCommand const* T, std::size_t N>
//         // template <typename T = CustomCommand const*, std::size_t N>
//         // void CreateCommands(const std::array<T, N>& commandsArray);
//         // void CreateCommands(const std::array<CustomCommand*, T> commandsArray);
//         // void CreateCommands(CustomCommand const* commandsArray, size_t length);
//         // void CreateCommands(CustomCommand const* commandsArray, size_t length, std::function<void(bool)> setter);
//         // void CreateCommands(CustomCommand const* commandsArray, size_t length, std::function<void(CustomCommand const* def, bool val)> setter);
//         // void CreateCommands(CustomCommand const* start, CustomCommand const* end);
//         // void CreateBoolCommands(CustomCommand const* commandsArray, size_t length);
//         // void CreateBoolCommands(CustomBoolCommand const* commandsArray, size_t length);
//         // void CreateBoolCommands(std::variant<EventCommand const*, StepDetectionCommand const*, StepBoundaryAbsorbCommand const*, StepBulkAbsorbCommand const*> commandsArray, size_t length);
//         // void CreateBoolCommands(std::variant<EventCommand const*, StepDetectionCommand const*, StepBoundaryAbsorbCommand const*, StepBulkAbsorbCommand const*> const* commandsArray, size_t length);
//         // void CreateBoolCommands(std::variant<EventCommand, StepDetectionCommand, StepBoundaryAbsorbCommand, StepBulkAbsorbCommand> const* commandsArray, size_t length);
//         void CreateBoolCommands(void const* commandsArray, size_t length, size_t byte_stride);
//         
//         // Deconstruct all created commands
//         // void ClearCommands();
//         void ClearBoolCommands();
        
        // Converts the string newVal to value(s) of the type(s) of parameter(s)
        void SetNewValue(G4UIcommand* cmd, G4String newVal) override;
    
    private:
        // Instantiate all commands
        void CreateBoolCommands(void const* commandsArray, size_t length, size_t byte_stride);
        
        // Deconstruct all created commands
        void ClearBoolCommands();
        
        // Maintain reference to program state singleton instance
        ProgramState& fProgramState;
        // ProgramState* fProgramState;
        
        // Commands (cmd pointer, cmd path, guidance msg)
        // StateCommands fStateCmds;
        // std::vector<BoolCommand> fStateCmds;
        // constexpr BoolCommand[] fStateCmds = StateCommands;
        // BoolCommand fStateCmds[11] = StateCommands;
        // auto fStateCmds = StateCommands;
        // std::array<BoolCommand, 11> fStateCommands = StateCommands;
        
        // Map
        // std::unordered_map<G4UIcommand*, BoolCommand> fCmdMap;
        // std::unordered_map<G4UIcommand*, BoolCommand&> fCmdMap;
        // std::unordered_map<G4UIcommand*, BoolCommand const*> fCmdMap;
        // CommandMap fCmdMap;
        // std::unordered_map<G4UIcommand*, CustomCommand const*> fCmdMap;
        // std::unordered_map<G4UIcommand*, std::variant<EventCommand const*, StepDetectionCommand const*, StepBoundaryAbsorbCommand const*, StepBulkAbsorbCommand const*>> fCmdMap;
        std::unordered_map<G4UIcommand*, CustomBoolCommand const*> fCmdMap;
};

#endif
