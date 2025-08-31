// #include "DetectorConstruction.hh"
#include "MyDetectorConstruction.hh"  // When leaving it as named here, causes errors (only in vscode), in scintillator/ no errors ...
// Is it due to scintillator "using namespace xyz;"
#include "MyPhysicsList.hh"
#include "MyActionInitialization.hh"

// #include "G4MTRunManager.hh"
// #include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

// using namespace GEOMETRY;

int main(int argc, char** argv) {
    // Instantiate the UI ...
    // G4UIExecutive *ui = new G4UIExecutive(argc, argv);
    // NOTE: Use auto when initialising classes to avoid duplicating type name
    auto* ui = new G4UIExecutive(argc, argv);
    // NOTE: This can be no pointer i think (see supplied example B1)

    // Construct the default run manager
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
    // NOTE: Likewwise can be "runManager" not "*runManager" (see supplied example B1)

    // Identify multi-threaded environment or not
    // #ifdef G4MULTITHREADED
    //     auto *runManager = new G4MTRunManager(); // G4RunManagerFactory::GetMTMasterRunManager() ?
    // #else
    //     auto *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
    // #endif

    // Mandatory initialisation classes
    //
    // Register geometry
    runManager->SetUserInitialization(new DetectorConstruction());

    // Register physics list
    runManager->SetUserInitialization(new PhysicsList());

    // ...
    runManager->SetUserInitialization(new ActionInitialization());

    // Initialise visualisation with the default graphics system
    auto visManager = new G4VisExecutive(argc, argv);  // NOTE: WHY ON EARTH DOES THE PREVIOUS TWO NEED TO BE POINTERS BUT NOT THIS ONE LOL
    // Constructors can also take optional arguments:
    // - a graphics system of choice, eg. "OGL"
    // - and a verbosity argument - see /vis/verbose guidance.
    // auto visManager = new G4VisExecutive(argc, argv, "OGL", "Quiet");
    // auto visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();
    // NOTE: HAVE SEEN visManager* also used though

    // Get the pointer to the UI manager
    auto UImanager = G4UImanager::GetUIpointer();  // is this a static method i guess ? no construction needed
    // NOTE: I think this can be *UImanager too ??

    // ...
    // UImanager->ApplyCommand("/control/execute init_vis.mac");
    UImanager->ApplyCommand("/control/execute vis.mac");

    // Start UI mode (interactive session)
    ui->SessionStart();

    // Clean up
    delete ui;
    delete visManager;
    delete runManager;

    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted
    // in the main() program !

    // Optional really (0 = success, non-zero = error)
    return 0;
}
