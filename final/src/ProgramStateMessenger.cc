// User classes
#include "ProgramStateMessenger.hh"
#include "OutputConfig.hh"
#include "ProgramState.hh"
// #include "OutputConfig.hh"

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
 * NOTE: So calling GetStateFlags before ProgramState has finished construction is safe
 */
// ProgramStateMessenger::ProgramStateMessenger() : fProgramState(ProgramState::GetInstance()) {
// ProgramStateMessenger::ProgramStateMessenger() {
// ProgramStateMessenger::ProgramStateMessenger() {
// ProgramStateMessenger::ProgramStateMessenger(ProgramState& programStateInstance) {
ProgramStateMessenger::ProgramStateMessenger(ProgramState& programStateInstance) : fProgramState(programStateInstance) {
    // ...
    // G4cout << "\n\n>>>>>>>>>>>>>>>>>>>>>>> I GOT CONSTRUCTED\n\n" << G4endl;
    G4cout << "\n\n>>>>> PROGRAM STATE MESSENGER INSTANTIATING\n\n" << G4endl;
    
    // fProgramState = ProgramState::GetInstance();
    // fProgramState(ProgramState::GetInstance());
    // fProgramState = ProgramState::GetInstance();
    // fProgramState = programStateInstance;
    
    // CreateCommands(StateCommands);
    
    // CreateCommands(EventCommands);
    // CreateCommands(StepDetectionCommands);
    // CreateCommands(StepBoundaryAbsorbCommands);
    // CreateCommands(StepBulkAbsorbCommands);
    // EventCommands.begin(), EventCommands.size()
    
    // programStateInstance.GetStateFlags();
    
    // CreateCommands(EventCommands.begin(), EventCommands.size());
    // CreateCommands(StepDetectionCommands.begin(), StepDetectionCommands.size());
    // CreateCommands(StepBoundaryAbsorbCommands.begin(), StepBoundaryAbsorbCommands.size());
    // CreateCommands(StepBulkAbsorbCommands.begin(), StepBulkAbsorbCommands.size());
    // NOTE: DONT MIX .begin() with .size(), do .begin() && .end(), OR, .data() && .size()
    
    // ...
    // CreateCommands(EventCommands.data(), EventCommands.size());
    // CreateCommands(StepDetectionCommands.data(), StepDetectionCommands.size());
    // CreateCommands(StepBoundaryAbsorbCommands.data(), StepBoundaryAbsorbCommands.size());
    // CreateCommands(StepBulkAbsorbCommands.data(), StepBulkAbsorbCommands.size());
    
    // CreateCommands(EventCommands.begin(), EventCommands.end());
    // CreateCommands(StepDetectionCommands.begin(), StepDetectionCommands.end());
    // CreateCommands(StepBoundaryAbsorbCommands.begin(), StepBoundaryAbsorbCommands.end());
    // CreateCommands(StepBulkAbsorbCommands.begin(), StepBulkAbsorbCommands.end());
    // ...
    
    // programStateInstance.GetStateFlags().fEventFlags;
    // programStateInstance.GetStateFlags().fStepDetectionFlags;
    // programStateInstance.GetStateFlags().fStepBoundaryAbsorbFlags;
    // programStateInstance.GetStateFlags().fStepBulkAbsorbFlags;
    
    // StateFlags& flags = programStateInstance.GetStateFlags();
    
    // [&flags](bool value) {
    //     flags.fEventFlags = value;
    // };
    
    // CreateCommands(EventCommands.data(), EventCommands.size(), [&flags](bool value) { flags.fEventFlags = value; });
    // CreateCommands(StepDetectionCommands.data(), StepDetectionCommands.size(), [&flags](bool value) { flags.fStepDetectionFlags = value; });
    // CreateCommands(StepBoundaryAbsorbCommands.data(), StepBoundaryAbsorbCommands.size(), [&flags](bool value) { flags.fStepBoundaryAbsorbFlags = value; });
    // CreateCommands(StepBulkAbsorbCommands.data(), StepBulkAbsorbCommands.size(), [&flags](bool value) { flags.fStepBulkAbsorbFlags = value; });
    
    // CreateCommands(EventCommands.data(), EventCommands.size(), [&flags, member](bool value) { flags.fEventFlags.*member = value; });
    // CreateCommands(StepDetectionCommands.data(), StepDetectionCommands.size(), [&flags](bool value) { flags.fStepDetectionFlags.*member = value; });
    // CreateCommands(StepBoundaryAbsorbCommands.data(), StepBoundaryAbsorbCommands.size(), [&flags](bool value) { flags.fStepBoundaryAbsorbFlags.*member = value; });
    // CreateCommands(StepBulkAbsorbCommands.data(), StepBulkAbsorbCommands.size(), [&flags](bool value) { flags.fStepBulkAbsorbFlags.*member = value; });
    
    // CreateCommands(EventCommands.data(), EventCommands.size(), [&flags](bool value, EventCommand::member member) { flags.fEventFlags.*member = value; });
    
    // CreateCommands(EventCommands.data(), EventCommands.size(), [&flags](bool value, bool EventFlags::* member) { flags.fEventFlags.*member = value; });
    
    // CreateCommands(EventCommands.data(), EventCommands.size(), [&flags](bool value, bool EventFlags::* member) { flags.fEventFlags.*member = value; });
    
    // CreateCommands(EventCommands.data(), EventCommands.size(), [&flags]() { return flags.fEventFlags; });
    // CreateCommands(StepDetectionCommands.data(), StepDetectionCommands.size(), [&flags]() { return flags.fStepDetectionFlags; });
    // CreateCommands(StepBoundaryAbsorbCommands.data(), StepBoundaryAbsorbCommands.size(), [&flags]() { return flags.fStepBoundaryAbsorbFlags; });
    // CreateCommands(StepBulkAbsorbCommands.data(), StepBulkAbsorbCommands.size(), [&flags]() { return flags.fStepBulkAbsorbFlags; });
    
    // CreateCommands(EventCommands.data(), EventCommands.size(), [&flags](CustomCommand const* def, bool val) { 
    //     auto const* castedDefinition = static_cast<EventCommand const*>(def);
    //     flags.fEventFlags.*(castedDefinition->member) = val;
    // });
    // CreateCommands(StepDetectionCommands.data(), StepDetectionCommands.size(), [&flags](CustomCommand const* def) { flags.fStepDetectionFlags; });
    // CreateCommands(StepBoundaryAbsorbCommands.data(), StepBoundaryAbsorbCommands.size(), [&flags](CustomCommand const* def) { flags.fStepBoundaryAbsorbFlags; });
    // CreateCommands(StepBulkAbsorbCommands.data(), StepBulkAbsorbCommands.size(), [&flags](CustomCommand const* def) { flags.fStepBulkAbsorbFlags; });
    
    // CreateBoolCommands(EventCommands.begin(), EventCommands.end());
    // CreateBoolCommands(StepDetectionCommands.begin(), StepDetectionCommands.end());
    // CreateBoolCommands(StepBoundaryAbsorbCommands.begin(), StepBoundaryAbsorbCommands.end());
    // CreateBoolCommands(StepBulkAbsorbCommands.begin(), StepBulkAbsorbCommands.end());
    
    // CreateBoolCommands(EventCommands.data(), EventCommands.size());
    // CreateBoolCommands(StepDetectionCommands.data(), StepDetectionCommands.size());
    // CreateBoolCommands(StepBoundaryAbsorbCommands.data(), StepBoundaryAbsorbCommands.size());
    // CreateBoolCommands(StepBulkAbsorbCommands.data(), StepBulkAbsorbCommands.size());
    
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
ProgramStateMessenger::~ProgramStateMessenger() {    
    G4cout << "\n\n>>>>> PROGRAM STATE MESSENGER BEING DESTROYED!\n\n" << G4endl;
    
    // ClearCommands();
    ClearBoolCommands();
    
    G4cout << "\n\n>>>>> PROGRAM STATE MESSENGER DESTROYED!\n\n" << G4endl;
}

/*
 * ...
 */
// template <typename T, std::size_t N>
// template <std::size_t N>
// template <typename T, std::size_t N>
// template <typename T = CustomCommand const*, std::size_t N>
// template <typename T, std::size_t N>
// void ProgramStateMessenger::CreateCommands(int size, std::array<CustomCommand, size>& commandsArray) {
// void ProgramStateMessenger::CreateCommands(const std::array<T, N>& commandsArray) {
// void ProgramStateMessenger::CreateCommands(const std::array<T, N>& commandsArray) {
// void ProgramStateMessenger::CreateCommands(const std::array<T, N>& commandsArray) {
// void ProgramStateMessenger::CreateCommands(CustomCommand const* commandsArray, size_t length, std::function<void(bool)> setter) {
// void ProgramStateMessenger::CreateCommands(CustomCommand const* commandsArray, size_t length) {
// void ProgramStateMessenger::CreateCommands(CustomCommand const* start, CustomCommand const* end) {
// void ProgramStateMessenger::CreateBoolCommands(CustomCommand const* commandsArray, size_t length) {
// void ProgramStateMessenger::CreateBoolCommands(CustomBoolCommand const* commandsArray, size_t length) {
// void ProgramStateMessenger::CreateBoolCommands(std::variant<EventCommand const*, StepDetectionCommand const*, StepBoundaryAbsorbCommand const*, StepBulkAbsorbCommand const*> commandsArray, size_t length) {
// void ProgramStateMessenger::CreateBoolCommands(std::variant<EventCommand const*, StepDetectionCommand const*, StepBoundaryAbsorbCommand const*, StepBulkAbsorbCommand const*> const* commandsArray, size_t length) {
// void ProgramStateMessenger::CreateBoolCommands(std::variant<EventCommand, StepDetectionCommand, StepBoundaryAbsorbCommand, StepBulkAbsorbCommand> const* commandsArray, size_t length) {
void ProgramStateMessenger::CreateBoolCommands(void const* commandsArray, size_t length, size_t byte_stride) {
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
    
    // TODO: Can probably make generic version of this \/\/\/\/\/\/\/\/
    
//     // Iterate through the EventCommands array
//     for (int i = 0; i < EventCommands.size(); i++) {
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(EventCommands[i].cmdPath, this);
//         
//         // Define usage string and default value
//         cmd->SetGuidance(EventCommands[i].cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         // fCmdMap[cmd] = EventCommands[i];
//         fCmdMap[cmd] = &(EventCommands[i]);
//         // key: G4UIcommand*
//         // val: BoolCommand
//     }
//     
//     // Iterate through the StepDetectionCommands array
//     for (int i = 0; i < StepDetectionCommands.size(); i++) {
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(StepDetectionCommands[i].cmdPath, this);
//         
//         // Define usage string and default value
//         cmd->SetGuidance(StepDetectionCommands[i].cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         // fCmdMap[cmd] = StepDetectionCommands[i];
//         fCmdMap[cmd] = &(StepDetectionCommands[i]);
//         // key: G4UIcommand*
//         // val: BoolCommand
//     }
//     
//     // Iterate through the StepBoundaryAbsorbCommands array
//     for (int i = 0; i < StepBoundaryAbsorbCommands.size(); i++) {
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(StepBoundaryAbsorbCommands[i].cmdPath, this);
//         
//         // Define usage string and default value
//         cmd->SetGuidance(StepBoundaryAbsorbCommands[i].cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         // fCmdMap[cmd] = StepBoundaryAbsorbCommands[i];
//         fCmdMap[cmd] = &(StepBoundaryAbsorbCommands[i]);
//         // key: G4UIcommand*
//         // val: BoolCommand
//     }
//     
//     // Iterate through the StepBulkAbsorbCommands array
//     for (int i = 0; i < StepBulkAbsorbCommands.size(); i++) {
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(StepBulkAbsorbCommands[i].cmdPath, this);
//         
//         // Define usage string and default value
//         cmd->SetGuidance(StepBulkAbsorbCommands[i].cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         // fCmdMap[cmd] = StepBulkAbsorbCommands[i];
//         fCmdMap[cmd] = &(StepBulkAbsorbCommands[i]);
//         // key: G4UIcommand*
//         // val: BoolCommand
//     }
    
    // Iterate through the StepBulkAbsorbCommands array
    // for (int i = 0; i < commandsArray.size(); i++) {
    
//     for (size_t i = 0; i < length; i++) {
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(commandsArray[i].cmdPath, this); // params: theCommandPath, theMessenger
//         
//         // Define usage string and default value
//         cmd->SetGuidance(commandsArray[i].cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         // fCmdMap[cmd] = commandsArray[i];
//         fCmdMap[cmd] = &(commandsArray[i]);
//         // key: G4UIcommand*
//         // val: BoolCommand
//     }
    
//     G4cout << "\n >>> CHECK 1\n" << G4endl;
//     
//     G4cout << "\n >>> length = " << length << "\n"<< G4endl;
//     
//     // Iterate through the array
//     for (size_t i = 0; i < length; i++) {
//         G4cout << "\n >>> i = " << i << "\n" << G4endl;
//         
//         // Cache reference to the current object in the array
//         // auto& definition = commandsArray[i];
//         CustomCommand const& definition = commandsArray[i];
//         // NOTE: "CustomCommand const definition" or "auto definition" (both without &) would create a copy of the object
//         
//         G4cout << "\n >>> CMD PATH: " << definition.cmdPath << " GUIDANCE: " << definition.cmdGuidance << "\n" << G4endl;
//         
//         G4cout << "\n >>> CHECK 2\n" << G4endl;
//         
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(definition.cmdPath, this); // params: theCommandPath, theMessenger
//         
//         G4cout << "\n >>> CHECK 3\n" << G4endl;
//         
//         // Define usage string and default value
//         cmd->SetGuidance(definition.cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         G4cout << "\n >>> CHECK 4\n" << G4endl;
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         fCmdMap[cmd] = &definition;
//         // key: G4UIcommand*
//         // val: BoolCommand
//         
//         G4cout << "\n >>> CHECK 5\n" << G4endl;
//     }
    
    
//      G4cout << "\n >>> CHECK 1\n" << G4endl;
//     
//     // G4cout << "\n >>> length = " << length << "\n"<< G4endl;
//     
//     // Iterate through the array
//     for (auto* definition = start; definition != end; definition++) {
//         // G4cout << "\n >>> i = " << i << "\n" << G4endl;
//         
//         G4cout << "\n >>> CMD PATH: " << definition->cmdPath << " GUIDANCE: " << definition->cmdGuidance << "\n" << G4endl;
//         
//         G4cout << "\n >>> CHECK 2\n" << G4endl;
//         
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(definition->cmdPath, this); // params: theCommandPath, theMessenger
//         
//         G4cout << "\n >>> CHECK 3\n" << G4endl;
//         
//         // Define usage string and default value
//         cmd->SetGuidance(definition->cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         G4cout << "\n >>> CHECK 4\n" << G4endl;
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         fCmdMap[cmd] = definition;
//         // key: G4UIcommand*
//         // val: BoolCommand
//         
//         G4cout << "\n >>> CHECK 5\n" << G4endl;
//     }
    
//     G4cout << "\n >>> CHECK 1\n" << G4endl;
//     
//     G4cout << "\n >>> length = " << length << "\n"<< G4endl;
//     
//     // Iterate through the array
//     for (size_t i = 0; i < length; i++) {
//         G4cout << "\n >>> i = " << i << "\n" << G4endl;
//         
//         // Cache reference to the current object in the array
//         // auto& definition = commandsArray[i];
//         // CustomCommand const& definition = commandsArray[i];
//         // CustomBoolCommand const& definition = commandsArray[i];
//         auto const& definition = commandsArray[i];
//         // NOTE: "CustomCommand const definition" or "auto definition" (both without &) would create a copy of the object
//         
//         G4cout << "\n >>> CMD PATH: " << definition.cmdPath << " GUIDANCE: " << definition.cmdGuidance << "\n" << G4endl;
//         
//         G4cout << "\n >>> CHECK 2\n" << G4endl;
//         
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(definition.cmdPath, this); // params: theCommandPath, theMessenger
//         
//         G4cout << "\n >>> CHECK 3\n" << G4endl;
//         
//         // Define usage string and default value
//         cmd->SetGuidance(definition.cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         G4cout << "\n >>> CHECK 4\n" << G4endl;
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         fCmdMap[cmd] = &definition;
//         // key: G4UIcommand*
//         // val: BoolCommand
//         
//         G4cout << "\n >>> CHECK 5\n" << G4endl;
//     }
    
    
//     G4cout << "\n >>> CHECK 1\n" << G4endl;
//     
//     G4cout << "\n >>> length = " << length << "\n"<< G4endl;
//     
//     // ...
//     auto const* rawBytePtr = reinterpret_cast<char const*>(commandsArray);
//     
//     // Iterate through the array
//     for (size_t i = 0; i < length; i++) {
//         G4cout << "\n >>> i = " << i << "\n" << G4endl;
//         
//         // ...
//         char const* elementAddress = rawBytePtr + (i * byte_stride);
//         
//         auto const* basePtr = reinterpret_cast<CustomBoolCommand const*>(elementAddress);
//         
//         // Cache reference to the current object in the array
//         // auto& definition = commandsArray[i];
//         // CustomCommand const& definition = commandsArray[i];
//         // CustomBoolCommand const& definition = commandsArray[i];
//         // auto const& definition = commandsArray[i];
//         CustomBoolCommand const& definition = *basePtr;
//         // NOTE: "CustomCommand const definition" or "auto definition" (both without &) would create a copy of the object
//         
//         G4cout << "\n >>> CMD PATH: " << definition.cmdPath << " GUIDANCE: " << definition.cmdGuidance << "\n" << G4endl;
//         
//         G4cout << "\n >>> CHECK 2\n" << G4endl;
//         
//         // Create a new command, exposed via the cmdPath string
//         auto* cmd = new G4UIcmdWithABool(definition.cmdPath, this); // params: theCommandPath, theMessenger
//         
//         G4cout << "\n >>> CHECK 3\n" << G4endl;
//         
//         // Define usage string and default value
//         cmd->SetGuidance(definition.cmdGuidance); // ..
//         cmd->SetDefaultValue(true);
//         
//         G4cout << "\n >>> CHECK 4\n" << G4endl;
//         
//         // Create an entry in the map, linking the instantiated command to the command object
//         fCmdMap[cmd] = &definition;
//         // key: G4UIcommand*
//         // val: BoolCommand
//         
//         G4cout << "\n >>> CHECK 5\n" << G4endl;
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
        
        // G4cout << "\n\nAVAILABLE : " << cmd->IsAvailable() << G4endl;
        // G4cout << "\n\nMESSENGER : " << cmd->GetMessenger() << G4endl;
        // G4cout << "\n\nMESSENGER : " << this << G4endl;
        
        // Define usage string and default value
        cmd->SetGuidance(definition.cmdGuidance); // ..
        cmd->SetDefaultValue(true);
        
        // Create an entry in the map, linking the instantiated command to the command object
        // fCmdMap[cmd] = &definition;
        fCmdMap[cmd] = basePtr;
        // key: G4UIcommand*
        // val: BoolCommand
        
    }
}

/*
 * ...
 */
// void ProgramStateMessenger::ClearCommands() {
void ProgramStateMessenger::ClearBoolCommands() {
    // Iterate through the key-value pairs in the map
    for (auto entry = fCmdMap.begin(); entry != fCmdMap.end(); entry++) {
        // entry->first; // key (G4UIcommand*)
        // entry->second; // value (CustomBoolCommand const*)
        
        // Free the heap allocated memory
        delete entry->first; // delete command
    }
}

/*
 * Converts the string newVal to value(s) of the type(s) of parameter(s)
 * 
 * Updates the boolean value of the StateFlags object
 */
void ProgramStateMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue) {
    // ...
    
    // if (!fProgramState) fProgramState = ProgramState::GetInstance();
    // fProgramState = &ProgramState::GetInstance();
    
    G4cout << "\n\n>>> SETTING COMMAND\n\n" << G4endl;
    
    // Search for the command in the map (using cmd as key)
    auto found = fCmdMap.find(cmd);
    // NOTE: find() returns [key, value] pair
    
    // If the passed command is found in the map populated during class construction
    if (found != fCmdMap.end()) {
        // Get mutable reference to StateFlags object (as we need to update a bool)
        // auto& stateFlags = fProgramState.GetStateFlags();
        // auto& stateFlags = fProgramState->GetStateFlags();
        StateFlags& stateFlags = fProgramState.GetStateFlags();
        
        // Get a readonly reference to the BoolCommand object associated with this command
        // auto const& definition = found->second;
        // CustomCommand const* definition = found->second;
        CustomBoolCommand const* definition = found->second;
        
        // Since "cmd" was found in fCmdMap, we know its of type G4UIcmdWithABool
        auto const* casted = static_cast<G4UIcmdWithABool*>(cmd);
        
        // Convert the string ("true" | "false") to a boolean
        G4bool const value = casted->GetNewBoolValue(newValue);
        
        // Update StateFlags object member with the new bool value
        // stateFlags.*(definition.member) = value;
        // stateFlags.*(definition->member) = value;
        // NOTE: Takes flags, and accesses whichever member "member" points to,
        // i.e., effectively doing flags.fDetectionNtuple
        // NOTE: Essentially a compiler-checked version of dynamic property access
        
        // stateFlags.fEventFlags.*(definition->member) = value;
        // stateFlags.fStepDetectionFlags.*(definition->member) = value;
        // stateFlags.fStepBoundaryAbsorbFlags.*(definition->member) = value;
        // stateFlags.fStepBulkAbsorbFlags.*(definition->member) = value;
        
//         if (definition->type == "event") {
//             auto const* castedDefinition = static_cast<EventCommand const*>(definition);
//             stateFlags.fEventFlags.*(castedDefinition->member) = value;
//         }
        
        // switch (definition->type == TypeAlias::Event) {
        //     auto const* castedDefinition = static_cast<EventCommand const*>(definition);
        //     stateFlags.fEventFlags.*(castedDefinition->member) = value;
        // }
        
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
}
