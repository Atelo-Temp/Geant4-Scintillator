/*
 * Mandatory user initialisation class (no default behaviour provided)
 * NOTE: G4RunManager will check for existance of mondatory classes when Initialize() and BeamOn() are invoked
 *
 * Derived from abstract base class: G4VUserActionInitialization
 *
 * Requires the user to define:
 * - User action classes that are invoked during the simulation
 *
 * Which includes one mandatory user action to define the primary particles
 *
 * Simplest example of ActionInitialization() required to build a simulation program:
*/

#include "ActionInitialization.hh"

// The particle generation defined will be handled by the "build" method
#include "PrimaryGenerator.hh"
// NOTE: PrimaryGenerator is derived from G4VUserPrimaryGeneratorAction
// In this class the initial state of the primary event must be described

// #include "RunAction.hh"

// Class implementation - handles the primary generator (particle gun)

// When specifying default in header file, dont need to define these
// ActionInitialization::ActionInitialization() {}
// ActionInitialization::~ActionInitialization() {}

/*
 * Multithreaded ...
 * Because the histogram is created in the master thread (output0.root),
 * whereas thread wise the output will be output0_TO.root, output0_T1.root, etc,
 * where the nTuples will be stored
*/
void ActionInitialization::BuildForMaster() const {
    // Instantiate the run handler (start/end of run handlers for histogramming)
    // SetUserAction(new RunAction());
}

/*
 * Implementation of build method, which will be called by "runManager->Initialise()"
 * 
 * Should include at least one mandatory user action class (G4VUserPrimaryGeneratorAction).
 * This mandatory class creates an instance of a primary particle generator.
 * User action classes are used during the run, and defined in this class.
*/
void ActionInitialization::Build() const {
    // Instantiate the particle generator
    // auto generator = new PrimaryGenerator();

    // NOTE: What exactly does this function to ...
    // SetUserAction(generator);

    // Instatiate the particle generator
    SetUserAction(new PrimaryGenerator());
    
    // Instantiate the run handler (start/end of run handlers for histogramming)
    // SetUserAction(new RunAction());
}
