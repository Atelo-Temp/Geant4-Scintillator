// User classes
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"
// #include "Timer.hh"

// G4 lib
// #include "G4MTRunManager.hh"
// #include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4HadronicParameters.hh" // decay time threshold
// #include "CLHEP/Random/Random.h" // random seeding

/*
 * Main entry point
 * 
 * Instantiates UI (if in interactive mode)
 * 
 * Instantiates run manager
 * 
 * Registers mandatory initialisation classes
 */
int main(int argc, char** argv) {
    // Detect interactive mode (if no arguments) and define UI session
    G4UIExecutive* ui = nullptr;

    // For interactive mode "./myProgram" (no further args), in batch mode argc > 1
    if (argc == 1) {
        // Instantiate the UI
        ui = new G4UIExecutive(argc, argv);
    }
    
    // Get a timestamp for random seeding
    // auto timer = new Timer();
    // long timestamp = timer->GetTimestamp();
    
    // Passing a time-dependent metric such as the computers system clock ensures
    // that every simulation run produces entirely unique and unpredictable results
    // CLHEP::HepRandom::setTheSeed(timestamp); // NOTE: Option 1
    // G4Random::setTheSeed(timestamp); // NOTE: Option 2 - from: OpNovice.cc main()
    // NOTE: ^ UNCOMMENT ME TO RANDOMISE SIMULATION RUNS

    // Construct the default run manager (NOTE: Factory will auto select MT or Serial based on G4 build)
    auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
    // NOTE: Use auto when initialising classes to avoid duplicating type name

    // Identify multi-threaded environment or not
    // #ifdef G4MULTITHREADED
        // auto *runManager = new G4MTRunManager(); // G4RunManagerFactory::GetMTMasterRunManager() ?
        // std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>> MULTITHREADING\n\n";
    // #else
        // auto *runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
        // std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>> NO MULTITHREADING\n\n";
    // #endif
    
    // Default time threshold for radioactive decay is 1 year, set it higher for longer lived isotopes
    G4HadronicParameters::Instance()->SetTimeThresholdForRadioactiveDecay(1.E+60 * CLHEP::year);
    // NOTE: nuclides with sampled lifetime longer than this threshold would otherwise be killed
    // Set a very high time threshold to allow all decays to happen
    
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
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

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
