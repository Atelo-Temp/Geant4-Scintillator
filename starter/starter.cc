// Simplest example of main() required to build a simulation program
// https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/gettingStarted.html

// Implemented by two classes from the Geant4 toolkit, G4RunManager and G4UImanager

// As well as three user defined classes; DetectorConstruction, PhysicsList, ActionInitialization
// ^ Derived from toolkit classes

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
// #include "ActionInitialization01.hh" // no 01?
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"

int main() {
    // Construct the default run manager (instantiates a G4RunManager object)
    auto runManager = G4RunManagerFactory::CreateRunManager(); // "::" Accesses imported namespace (like "import x" in js, x.someMethod())
    // NOTE: Creating an instance of the G4RunManager class must always be first step in main() !
    // This manager controls the flow of the program and manages the event loop(s) within a run

    // Set mandatory initialisation classes (before Initialize())
    //
    // Tell the manager how the detector should be constructed (following user defined class)
    runManager->SetUserInitialization(new DetectorConstruction); // "->" is equivalent to (runManager*).SetUserInitialization
    //  runManager.SetUserInitialization(new ActionInitialization()); // NOTE: Type G4RunManager* is a pointer
    //
    // Specify all the particles and all the physics processes to be simulated
    runManager->SetUserInitialization(new PhysicsList);

    // Set mandatory user action class
    //
    // Define how the primary particle(s) in an event should be produced
    runManager->SetUserInitialization(new ActionInitialization); // 01 ??

    // Initialise G4 kernel (with chosen settings)
    runManager->Initialize();
    // NOTE: Performs detector construction, creates physics processes, calculates cross sections, sets up run, etc

    // Get the pointer to the UI manager and set verbosities
    G4UImanager* UIManager = G4UImanager::GetUIpointer();
    UIManager->ApplyCommand("/run/verbose 1");
    UIManager->ApplyCommand("/event/verbose 1");
    UIManager->ApplyCommand("/tracking/verbose 1");
    // NOTE: One of the other manager classes instantiated from CreateRunManager() is the UI manager
    // In main() a pointer to the interface manager must be obtained to issue commands to the program
    // Here, ApplyCommand() is called three times to print information at the; run, event, and tracking levels of simulation
    // Full list of commands can be found in "Built-in Commands" in the Users Guides

    // Start a run
    int numberOfEvent = 3;
    runManager->BeamOn(numberOfEvent);
    // NOTE: Begins run of three sequentially processed events
    // Each call to BeamOn() invokes a seperate run (i.e. 1 call = 1 run of 3 events here)
    // Detector setup and physics processes cannot be altered during a run, but can be between runs

    // Job termination
    delete runManager;
    return 0;
}

// NOTE: There is an extension to this with interactive terminal, may alter this or create new main to keep seperate
