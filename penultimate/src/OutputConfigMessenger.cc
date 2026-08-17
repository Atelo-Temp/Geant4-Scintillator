// User classes
#include "OutputConfigMessenger.hh"
#include "OutputCommands.hh"
#include "OutputConfig.hh"

// G4 lib
#include "G4UIcmdWithABool.hh"

/*
 * Constructor
 * 
 * Defines all commands related to this messenger
 * 
 * Level based output data control ?
 * 1 = write only per-event detections
 * 2 = write per-event detections & photon loss mechanism fractions
 * 3 = same as 2 & photon coordinates
 * 4 = same as 3 & distance travelled, time of flight, reflections
 * 
 * ^ no tbh exposing all flags individually likely best, will get messy else
 * 
 * NOTE: Constructor order is:
 * 
 * Allocate Storage
 * \/
 * Construct members
 * \/
 * Enter cosntructor body
 * \/
 * Construct messenger
 * 
 * NOTE: So calling GetOutputFlags before OutputConfig has finished construction is safe
 */
OutputConfigMessenger::OutputConfigMessenger(OutputConfig& programStateInstance) : fOutputConfig(programStateInstance) {
    // ...
    G4cout << "\n\n>>>>> PROGRAM STATE MESSENGER INSTANTIATING\n\n" << G4endl;
    
    // ...
    CreateBoolCommands(EventCommands.data(), EventCommands.size(), sizeof(EventCommand));
    CreateBoolCommands(StepDetectionCommands.data(), StepDetectionCommands.size(), sizeof(StepDetectionCommand));
    CreateBoolCommands(StepBoundaryAbsorbCommands.data(), StepBoundaryAbsorbCommands.size(), sizeof(StepBoundaryAbsorbCommand));
    CreateBoolCommands(StepBulkAbsorbCommands.data(), StepBulkAbsorbCommands.size(), sizeof(StepBulkAbsorbCommand));
    // ...
    
    G4cout << "\n\n>>>>> PROGRAM STATE MESSENGER INSTANTIATED\n\n" << G4endl;
}

/*
 * Destructor
 * 
 * Deletes all commands defined in constructor
 */
OutputConfigMessenger::~OutputConfigMessenger() {    
    G4cout << "\n\n>>>>> PROGRAM STATE MESSENGER BEING DESTROYED!\n\n" << G4endl;
    
    // ...
    ClearBoolCommands();
    
    G4cout << "\n\n>>>>> PROGRAM STATE MESSENGER DESTROYED!\n\n" << G4endl;
}

/*
 * ...
 */
void OutputConfigMessenger::CreateBoolCommands(void const* commandsArray, size_t length, size_t byte_stride) {
    //     // Iterate through the StateCommands vector
//     for (int i = 0; i < StateCommands.size(); i++) {
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(StateCommands[i].cmdPath, this);
//         
//         // Define usage string and default value
//         cmd->SetGuidance(StateCommands[i].cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         // fCmdMap[cmd] = StateCommands[i];
//         fCmdMap[cmd] = &(StateCommands[i]);
//         // key: G4UIcommand*
//         // val: BoolCommand
//     }

    // ...
    auto const* rawBytePtr = reinterpret_cast<char const*>(commandsArray);
    
    // Iterate through the array
    for (size_t i = 0; i < length; i++) {
        
        // ...
        char const* elementAddress = rawBytePtr + (i * byte_stride);
        
        // ...
        auto const* basePtr = reinterpret_cast<CustomBoolCommand const*>(elementAddress);
        
        // Cache reference to the current object in the array
        CustomBoolCommand const& definition = *basePtr;
        // NOTE: "CustomCommand const definition" or "auto definition" (both without &) would create a copy of the object
        
        // G4cout << "\n >>> CMD PATH: " << definition.cmdPath << " GUIDANCE: " << definition.cmdGuidance << "\n" << G4endl;
        
        // Create a new command, exposed via the cmdPath string
        auto* cmd = new G4UIcmdWithABool(definition.cmdPath, this); // params: theCommandPath, theMessenger
        
        // Define usage string and default value
        cmd->SetGuidance(definition.cmdGuidance); // ..
        cmd->SetDefaultValue(true);
        
        // Create an entry in the map, linking the instantiated command to the command object
        // fOutputCmdMap[cmd] = &definition;
        fOutputCmdMap[cmd] = basePtr;
        // key: G4UIcommand*
        // val: BoolCommand
        
    }
}

/*
 * ...
 */
void OutputConfigMessenger::ClearBoolCommands() {
    // Iterate through the key-value pairs in the map
    for (auto entry = fOutputCmdMap.begin(); entry != fOutputCmdMap.end(); entry++) {
        // entry->first; // key (G4UIcommand*)
        // entry->second; // value (CustomBoolCommand const*)
        
        // Free the heap allocated memory
        delete entry->first; // delete command
    }
}

/*
 * Converts the string newVal to value(s) of the type(s) of parameter(s)
 * 
 * Updates the boolean value of the OutputFlags object
 */
void OutputConfigMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue) {
    // ...    
    G4cout << "\n\n>>> SETTING COMMAND\n\n" << G4endl;
    
    // Search for the command in the map (using cmd as key)
    auto found = fOutputCmdMap.find(cmd);
    // NOTE: find() returns [key, value] pair
    
    // If the passed command is found in the map populated during class construction
    if (found != fOutputCmdMap.end()) {
        HandleOutputCmd(cmd, newValue, found->second);
    }
}

/*
 * ...
 */
void OutputConfigMessenger::HandleOutputCmd(G4UIcommand* cmd, G4String newValue, CustomBoolCommand const* definition) {
    // Get mutable reference to OutputFlags object (as we need to update a bool)
    OutputFlags& stateFlags = fOutputConfig.GetOutputFlags();
    
    // Get a readonly reference to the BoolCommand object associated with this command
    // auto const& definition = found->second;
    // CustomCommand const* definition = found->second;
    // CustomBoolCommand const* definition = found->second;
    
    // Since "cmd" was found in fCmdMap, we know its of type G4UIcmdWithABool
    auto const* casted = static_cast<G4UIcmdWithABool*>(cmd);
    
    // Convert the string ("true" | "false") to a boolean
    G4bool const value = casted->GetNewBoolValue(newValue);
    
    // Update OutputFlags object member with the new bool value
    // stateFlags.*(definition.member) = value;
    // stateFlags.*(definition->member) = value;
    // NOTE: Takes flags, and accesses whichever member "member" points to,
    // i.e., effectively doing flags.fDetectionNtuple
    // NOTE: Essentially a compiler-checked version of dynamic property access
    
    // ...
    switch (definition->type) {
        case OutputType::Event: {
            auto const* castedDefinition = static_cast<EventCommand const*>(definition);
            stateFlags.fEventFlags.*(castedDefinition->member) = value;
            break;
        }
        case OutputType::StepDetection: {
            auto const* castedDefinition = static_cast<StepDetectionCommand const*>(definition);
            stateFlags.fStepDetectionFlags.*(castedDefinition->member) = value;
            break;
        }
        case OutputType::StepBoundaryAbsorb: {
            auto const* castedDefinition = static_cast<StepBoundaryAbsorbCommand const*>(definition);
            stateFlags.fStepBoundaryAbsorbFlags.*(castedDefinition->member) = value;
            break;
        }
        case OutputType::StepBulkAbsorb: {
            auto const* castedDefinition = static_cast<StepBulkAbsorbCommand const*>(definition);
            stateFlags.fStepBulkAbsorbFlags.*(castedDefinition->member) = value;
            break;
        }
        default: {
            G4cout << "\nINVALID USAGE\n" << G4endl;
            break;
        }
    }
}
