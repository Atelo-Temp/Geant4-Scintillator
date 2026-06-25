/*
 * Mandatory user initialisation class (no default behaviour provided)
 * 
 * NOTE: G4RunManager will check for existance of mandatory classes when Initialize() and BeamOn() are invoked
 *
 * Derived from abstract base class: G4VUserActionInitialization
 *
 * Requires the user to define:
 * - User action classes that are invoked during the simulation
 *
 * Which includes one mandatory user action to define the primary particles
 *
 * The particle generation defined will be handled by the "build" method
*/

// User classes
#include "ActionInitialization.hh"
#include "PrimaryGenerator.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
// #include "TrackingAction.hh"

// When specifying default in header file, dont need to define these
// ActionInitialization::ActionInitialization() {}
// ActionInitialization::~ActionInitialization() {}

/*
 * Multithreaded ...
 * 
 * Because the histogram is created in the master thread (output0.root),
 * whereas thread wise the output will be output0_TO.root, output0_T1.root, etc,
 * where the nTuples will be stored
*/
void ActionInitialization::BuildForMaster() const {
    // Instantiate the run handler (start/end of run handlers for histogramming)
    SetUserAction(new RunAction());
}

/*
 * Implementation of build method, which will be called by "runManager->Initialise()"
 * 
 * Should include at least one mandatory user action class (G4VUserPrimaryGeneratorAction).
 * This mandatory class creates an instance of a primary particle generator.
 * User action classes are used during the run, and defined in this class.
*/
void ActionInitialization::Build() const {
    // Instatiate the particle generator
    SetUserAction(new PrimaryGenerator());
    
    // Instantiate the run handler (start/end of run handlers for data output/run timings)
    auto runHandler = new RunAction();
    SetUserAction(runHandler);
    
    // Event handler (start/end of event handlers for photon counting, and passing data to run handler)
    auto eventHandler = new EventAction(runHandler); // TODO: RunAction unused in EventAction
    SetUserAction(eventHandler);
    
    // Track handler (start/end of track handlers for custom user information object assignment to particles)
    // SetUserAction(new TrackingAction()); // NOTE: DISABLED
    
    // Step handler (pre/post step photon processing, passes intra event data up to event handler)
    SetUserAction(new SteppingAction(eventHandler));
}
