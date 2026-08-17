#ifndef MyOutputConfigMessenger_HH
#define MyOutputConfigMessenger_HH

// User classes
#include "OutputCommands.hh"

// C lib
#include <unordered_map>

// G4 lib
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
// #include "G4UIcmdWithABool.hh"
// #include "G4UIcmdWithAnInteger.hh"

// Forward declarations
class OutputConfig; // TODO:  Maybe extract StateFlags, BoolCommand, and StateCommands
// NOTE: OutputConfig class only holds a pointer to a OutputConfigMessenger instance on the heap,
// since a pointer has a fixed memory size, there is no need to import here, import in ".cc" file
// NOTE: Since both OutputConfig and OutputConfigMessenger hold references to oneanother, importing
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
class OutputConfigMessenger : public G4UImessenger {
    public:
        // Constructor - should define all commands related to this messenger
        OutputConfigMessenger(OutputConfig& programStateInstance);
        
        // Destructor - all commands defined in constructor must be deleted
        ~OutputConfigMessenger() override;
        
        // Converts the string newVal to value(s) of the type(s) of parameter(s)
        void SetNewValue(G4UIcommand* cmd, G4String newVal) override;
    
    private:
        // Instantiate all commands
        void CreateBoolCommands(void const* commandsArray, size_t length, size_t byte_stride);
        
        // Deconstruct all created commands
        void ClearBoolCommands();
        
        void HandleOutputCmd(G4UIcommand* cmd, G4String newValue, CustomBoolCommand const* definition);
        
        // Maintain reference to program state singleton instance
        OutputConfig& fOutputConfig;
        // OutputConfig* fOutputConfig;
        
        // Commands (cmd pointer, cmd path, guidance msg)
        std::unordered_map<G4UIcommand*, CustomBoolCommand const*> fOutputCmdMap;
};

#endif
