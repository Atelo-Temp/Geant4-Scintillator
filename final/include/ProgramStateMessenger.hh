#ifndef MyProgramStateMessenger_HH
#define MyProgramStateMessenger_HH

// User classes
#include "OutputConfig.hh"

// C lib
#include <unordered_map>

// G4 lib
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
// #include "G4UIcmdWithABool.hh"
// #include "G4UIcmdWithAnInteger.hh"

// Forward declarations
class ProgramState; // TODO:  Maybe extract StateFlags, BoolCommand, and StateCommands
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
        
        // Converts the string newVal to value(s) of the type(s) of parameter(s)
        void SetNewValue(G4UIcommand* cmd, G4String newVal) override;
    
    private:
        // Instantiate all commands
        void CreateBoolCommands(void const* commandsArray, size_t length, size_t byte_stride);
        
        // Deconstruct all created commands
        void ClearBoolCommands();
        
        void HandleOutputCmd(G4UIcommand* cmd, G4String newValue, CustomBoolCommand const* definition);
        
        // Maintain reference to program state singleton instance
        ProgramState& fProgramState;
        // ProgramState* fProgramState;
        
        // Commands (cmd pointer, cmd path, guidance msg)
        std::unordered_map<G4UIcommand*, CustomBoolCommand const*> fOutputCmdMap;
};

#endif
