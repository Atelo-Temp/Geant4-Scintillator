#ifndef MyProgramStateMessenger_HH
#define MyProgramStateMessenger_HH

// G4 lib
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"

// Forward declarations
class ProgramState;
// NOTE: ProgramState class only holds a pointer to a ProgramStateMessenger instance on the heap,
// since a pointer has a fixed memory size, there is no need to import here, import in ".cc" file
// NOTE: Since both ProgramState and ProgramStateMessenger hold references to oneanother, importing
// would also create circular dependencies

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
        // NOTE: This can be one way communication, ProgramState doesnt need
        // pointer to this class
        // ^^ not true, ProgramStateMessenger needs to be instantiated, and it makes sense to do so
        // in ProgramState constructor, since the two are linked
        
        // ...
        G4UIcmdWithABool* fCmd1 = nullptr;
        G4UIcmdWithABool* fCmd2 = nullptr;
        G4UIcmdWithABool* fCmd3 = nullptr;
};

#endif
