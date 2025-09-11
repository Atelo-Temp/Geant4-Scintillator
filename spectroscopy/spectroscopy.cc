// #include "DetectorConstruction.hh"
#include "DetectorConstruction.hh"  // When leaving it as named here, causes errors (only in vscode), in scintillator/ no errors ...
// Is it due to scintillator "using namespace xyz;"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

// #include "G4MTRunManager.hh"
// #include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

#include "G4HadronicParameters.hh"

// using namespace GEOMETRY;

int main(int argc, char** argv) {
    // Detect interactive mode (if no arguments) and define UI session
    G4UIExecutive* ui = nullptr;

    // For interactive mode "./myProgram" (no further args), in batch mode argc > 1
    if (argc == 1) {
        // Instantiate the UI
        ui = new G4UIExecutive(argc, argv);
    }

    // Construct the default run manager
    auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
    // NOTE: Use auto when initialising classes to avoid duplicating type name

    // Identify multi-threaded environment or not
    // #ifdef G4MULTITHREADED
    //     auto *runManager = new G4MTRunManager(); // G4RunManagerFactory::GetMTMasterRunManager() ?
    // #else
    //     auto *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
    // #endif
    
    // Default time threshold for radioactive decay is 1 year, set it higher for longer lived isotopes
    G4HadronicParameters::Instance()->SetTimeThresholdForRadioactiveDecay(1.E+60 * CLHEP::year);
    // NOTE: nuclides with sampled lifetime longer than this threshold would otherwise be killed
    
    // Mandatory initialisation classes
    //
    // Register geometry
    runManager->SetUserInitialization(new DetectorConstruction());

    // Register physics list
    runManager->SetUserInitialization(new PhysicsList());

    // User action initialisation
    runManager->SetUserInitialization(new ActionInitialization());
 
    // Initialise visualisation with the default graphics system
    auto visManager = new G4VisExecutive(argc, argv);
    // Constructors can also take optional arguments:
    // - a graphics system of choice, eg. "OGL"
    // - and a verbosity argument - see /vis/verbose guidance.
    // auto visManager = new G4VisExecutive(argc, argv, "OGL", "Quiet");
    // auto visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();

    // Get the pointer to the UI manager object (static method)
    auto UImanager = G4UImanager::GetUIpointer();

    // Process supplied macro file, or start UI session (if pointer exists || null)
    if (!ui) {
        // Run in batch mode (execute the supplied macro file)
        G4String command = "/control/execute ";

        // Get the filename from the second command line argument (i.e. ./myProgram myMacro.mac)
        G4String fileName = argv[1];

        // Execute the macro file
        UImanager->ApplyCommand(command + fileName);
    } else {
        // Run in interactive mode (execute visualisation macro)
        // UImanager->ApplyCommand("/control/execute init_vis.mac");
        UImanager->ApplyCommand("/control/execute vis.mac");

        // Start UI mode (interactive session)
        ui->SessionStart();

        // Clean up
        delete ui;
    }

    // Job termination
    delete visManager;
    delete runManager;

    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted
    // in the main() program !

    // Optional really (0 = success, non-zero = error)
    return 0;
}
