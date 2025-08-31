// Handles the primary generator (particle gun)
// Mandatory for run manager initialisation

#include "MyActionInitialization.hh"

// The particle generation defined will be handled by the "build" method
#include "MyPrimaryGenerator.hh"

// Class implementation

// When specifying default in header file, dont need to define these
// ActionInitialization::ActionInitialization() {}
// ActionInitialization::~ActionInitialization() {}

// Multithreaded ...
void ActionInitialization::BuildForMaster() const {}

// Implement build method, which will be called by "runManager->Initialise()"
void ActionInitialization::Build() const {
    // Instantiate the particle generator
    // auto generator = new PrimaryGenerator();

    // NOTE: What exactly does this function to ...
    // SetUserAction(generator);

    // Instatiate the particle generator
    SetUserAction(new PrimaryGenerator());
}
