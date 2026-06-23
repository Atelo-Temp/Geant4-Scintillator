// User classes
#include "ProgramStateMessenger.hh"
#include "ProgramState.hh"

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
 */
ProgramStateMessenger::ProgramStateMessenger() : fProgramState(ProgramState::GetInstance()) {
    // fX = new G4UIcmdWithABool("/.../", this);
    // auto fX = new G4UIcmdWithABool("/prefix/myCmd", this);
    
    // auto intCMD = new G4UIcmdWithAnInteger("/output/myCmd", this);
    // intCMD->SetGuidance("Set the verbosity of the event data."); // ..
    // intCMD->SetParameterName("verbose", true); // name, omittable, current as default
    // intCMD->SetDefaultValue(1);
    
    // ...
    // auto boolCMD = new G4UIcmdWithABool("/output/myCmd", this);
    // boolCMD->SetGuidance("Set the verbosity of the event data."); // ..
    // boolCMD->SetParameterName("verbose", true); // name, omittable, current as default
    // boolCMD->SetDefaultValue(true);
    
    // ...
    // auto fBoundaryAbsorbCoordsCMD = new G4UIcmdWithABool(fStateCmds.fBoundaryAbsorbCoords.cmdPath, this);
    // fBoundaryAbsorbCoordsCMD->SetGuidance(fStateCmds.fBoundaryAbsorbCoords.cmdGuidance); // ..
    // fBoundaryAbsorbCoordsCMD->SetDefaultValue(true);
    // fStateCmds.fBoundaryAbsorbCoords.command = fBoundaryAbsorbCoordsCMD;
    
    // ...
    // for (int i = 0; i < fStateCmds.size(); i++) {
    //     auto* cmd = new G4UIcmdWithABool(fStateCmds[i].cmdPath, this);
    //     cmd->SetGuidance(fStateCmds[i].cmdGuidance); // ..
    //     cmd->SetDefaultValue(true);
    //     fStateCmds[i].command = cmd;
    // }
    
    // Iterate through the StateCommands vector
    for (int i = 0; i < fStateCmds.size(); i++) {
        // Create a new command, exposed via the cmdPath string
        auto* cmd = new G4UIcmdWithABool(fStateCmds[i].cmdPath, this);
        
        // Define usage string and default value
        cmd->SetGuidance(fStateCmds[i].cmdGuidance); // ..
        cmd->SetDefaultValue(true);
        
        // Create an entry in the map
        fCmdMap[cmd] = fStateCmds[i];
        // key: G4UIcommand*
        // val: BoolCommand
    }
}

/*
 * Destructor
 * 
 * Deletes all commands defined in constructor
 */
ProgramStateMessenger::~ProgramStateMessenger() {
    // delete fX;
    // delete ...;
    // delete ...;
    
    // delete fCmd1; // temp, rename
    // delete fCmd2; // temp, rename
    
    // delete fStateCmds.fBoundaryAbsorbCoords.command;
    
    // for (int i = 0; i < fStateCmds.size(); i++) {
    //     delete fStateCmds[i].command;
    // }
    
    // Iterate through the key-value pairs in the map
    for (auto i = fCmdMap.begin(); i != fCmdMap.end(); i++) {
        // i->first; // key (G4UIcommand)
        // i->second; // value (BoolCommand)
        
        // Free the heap allocated memory
        delete i->first;
    }
}

/*
 * Converts the string newVal to value(s) of the type(s) of parameter(s)
 * 
 * Updates the boolean value of the StateFlags object
 */
void ProgramStateMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue) {
    // ...
    
    // if (cmd == fCmd1) {
    // if (cmd == fStateCmds.fBoundaryAbsorbCoords.command) {
    //     // fProgramState->stateFlags.fBoundaryAbsorbCoordsNtuple = false;
    //     // fProgramState->stateFlags.fBoundaryAbsorbCoordsNtuple = fCmd1->GetNewIntValue(newValue);
    //     // fProgramState->stateFlags.fBoundaryAbsorbCoordsNtuple = fCmd1->GetNewBoolValue(newValue);
    //     // fProgramState->GetStateFlags().fBoundaryAbsorbCoordsNtuple = fCmd1->GetNewBoolValue(newValue);
    //     // fProgramState.GetStateFlags().fBoundaryAbsorbCoordsNtuple = fCmd1->GetNewBoolValue(newValue);
    //     fProgramState.GetStateFlags().fBoundaryAbsorbCoordsNtuple = fStateCmds.fBoundaryAbsorbCoords.command->GetNewBoolValue(newValue);
    // }
    // else if (cmd == fCmd2) {
    //     // fProgramState->stateFlags.fDetectionCoordsNtuple = false;
    //     // fProgramState->stateFlags.fDetectionCoordsNtuple = fCmd2->GetNewBoolValue(newValue);
    //     // fProgramState->GetStateFlags().fDetectionCoordsNtuple = fCmd2->GetNewBoolValue(newValue);
    //     fProgramState.GetStateFlags().fDetectionCoordsNtuple = fCmd2->GetNewBoolValue(newValue);
    // }
    // etc ...
    
    // ...
    // if (fCmdMap.count(cmd)) {
        // fProgramState.GetStateFlags().fBoundaryAbsorbCoordsNtuple = fCmdMap.at(cmd).command->GetNewBoolValue(newValue);
        // fProgramState.GetStateFlags(fCmdMap.at(cmd).name) = fCmdMap.at(cmd).command->GetNewBoolValue(newValue);
        // add name to state flags?
    // }
    
    // ...
    auto found = fCmdMap.find(cmd);
    
    // ...
    if (found != fCmdMap.end()) {
        // ...
        auto& flags = fProgramState.GetStateFlags();
        
        // ...
        auto const& definition = found->second;
        
        // ...
        flags.*(definition.member) = static_cast<G4UIcmdWithABool*>(cmd)->GetNewBoolValue(newValue);
    }
}
