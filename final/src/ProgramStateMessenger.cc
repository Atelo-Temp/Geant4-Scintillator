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
 * ^ no tbh exposive all flags individually likely best, will get messy else
 */
ProgramStateMessenger::ProgramStateMessenger() : fProgramState(ProgramState::GetInstance()) {
    // fX = new G4UIcmdWithABool("/.../", this);
    // auto fX = new G4UIcmdWithABool("/prefix/myCmd", this);
    
    auto fCmd1 = new G4UIcmdWithAnInteger("/output/myCmd", this);
    fCmd1->SetGuidance("Set the verbosity of the event data."); // ..
    fCmd1->SetParameterName("verbose", true); // name, omittable, current as default
    fCmd1->SetDefaultValue(1);
    
    // ...
    auto fCmd2 = new G4UIcmdWithABool("/output/myCmd", this);
    fCmd2->SetGuidance("Set the verbosity of the event data."); // ..
    fCmd2->SetParameterName("verbose", true); // name, omittable, current as default
    fCmd2->SetDefaultValue(true);
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
    
    delete fCmd1; // temp, rename
    delete fCmd2; // temp, rename
}

/*
 * Converts the string newVal to value(s) of the type(s) of parameter(s)
 */
void ProgramStateMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue) {
    // ...
    
    if (cmd == fCmd1) {
        // fProgramState->stateFlags.fBoundaryAbsorbCoordsNtuple = false;
        // fProgramState->stateFlags.fBoundaryAbsorbCoordsNtuple = fCmd1->GetNewIntValue(newValue);
        // fProgramState->stateFlags.fBoundaryAbsorbCoordsNtuple = fCmd1->GetNewBoolValue(newValue);
        // fProgramState->GetStateFlags().fBoundaryAbsorbCoordsNtuple = fCmd1->GetNewBoolValue(newValue);
        fProgramState.GetStateFlags().fBoundaryAbsorbCoordsNtuple = fCmd1->GetNewBoolValue(newValue);
    }
    else if (cmd == fCmd2) {
        // fProgramState->stateFlags.fDetectionCoordsNtuple = false;
        // fProgramState->stateFlags.fDetectionCoordsNtuple = fCmd2->GetNewBoolValue(newValue);
        // fProgramState->GetStateFlags().fDetectionCoordsNtuple = fCmd2->GetNewBoolValue(newValue);
        fProgramState.GetStateFlags().fDetectionCoordsNtuple = fCmd2->GetNewBoolValue(newValue);
    }
    // etc ...
    
    // ...
}
