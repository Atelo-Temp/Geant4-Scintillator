// Mandatory user initialisation class (no default behaviour provided)
// NOTE: G4RunManager will check for existance of mondatory classes when Initialize() and BeamOn() are invoked

// Derived from abstract base class: G4VUserActionInitialization

// Requires the user to define:
// - User action classes that are invoked during the simulation
// ^ Which includes one mandatory user action to define the primary particles

// Simplest example of ActionInitialization() required to build a simulation program

#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

// User action classes are used during the run, and defined in this class
void ActionInitialization::Build() const {
    // Should include at least one mandatory user action class (G4VUserPrimaryGeneratorAction)
    // This mandatory class creates an instance of a primary particle generator
    SetUserAction(new PrimaryGeneratorAction);
    // NOTE: PrimaryGeneratorAction is derived from G4VUserPrimaryGeneratorAction
    // In this class the initial state of the primary event must be described
}

// NOTE: This class has a public virtual method named

// Is this overriding the Build() method in the abstract base class ?
// And Build() is called during Initialize() to SetUserAction
