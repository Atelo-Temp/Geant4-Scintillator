/*
 * Handles everything done at the beginning and end of a run:
 *
 * - At construction, when runManager initialises "ActionInitialisation", and subsequently this class,
 * a histogram will be created, as well as an nTuple for storing data from a run
 * 
 * - At the start of a run, an outfile will be created and opened, using run id for the filename
 * 
 * - At the end of a run, data will be written to the open outfile, then the file closed, with confirmation via G4 stdout
*/

#include "MyRunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
// #include "G4UnitsTable.hh"
// #include "G4Run.hh" // this is used but not 

// Define the class constructor
RunAction::RunAction() {
    // Get a pointer to the singleton analysis manager via the static method
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Creat histogram (one dimension to store deposited energy)
    analysisManager->CreateH1(
        "eDep", // Name
        "Energy Deposit", // Title
        100, // Number of bins
        0., // Lower bounds
        1. * MeV // Upper bounds
    );
    // NOTE: 100 bins from 0->1 MeV
    
    // Create nTuples to store more in root file
    analysisManager->CreateNtuple("Photons", "Photons"); // name, title
    
    // Define columns and rows iniside the tuple
    analysisManager->CreateNtupleIColumn("iEvent"); // I = integer (event number)
    analysisManager->CreateNtupleDColumn("fX"); // D = double (float maybe fine, but double gives increased precision) // store x position (may not need for now but in future may be useful)
    analysisManager->CreateNtupleDColumn("fY"); // y position of the photon
    analysisManager->CreateNtupleDColumn("fZ"); // y position of the photon
    analysisManager->CreateNtupleDColumn("fGlobalTime"); // store the global time at the start of each event
    analysisManager->CreateNtupleDColumn("fWlen"); // wavelength
    // NOTE: Could also define momentum, energy, etc ...
    
    // Mark the definition of the tuple columns as completed
    analysisManager->FinishNtuple(0);
}

// Define the start of run event handler (Takes run object as a parameter)
void RunAction::BeginOfRunAction(const G4Run* run) {
    // Get a pointer to the singleton analysis manager
    auto analysisManager = G4AnalysisManager::Instance();
    // NOTE: Couldnt the pointer just be stored in the class?
    // But then youre storing a pointer to it in two places, maybe wasteful ?
    
    // Access the ID for the run object (integer)
    G4int runID = run->GetRunID();
    
    // Convert ID integer to string (To include run ID in the filename)
    std::stringstream strRunID; // String stream object
    strRunID << runID; // Pipe the run id into the stream
    // NOTE: Must convert int to string in C++ (not always necessary in other languages)
    
    // Define the outfile name (converting string stream object into a string)
    G4String fileName = "output" + strRunID.str() + ".root"; // append file type
    // NOTE: Could overwrite output.root on each run, but this is a better option
    
    // NOTE: Alternatively std lib may be used to convert to string
    // std::string str = std::to_string(runID);
    
    // Create and open the file with the supplied name
    analysisManager->OpenFile(fileName);
}

// Define the end of run event handler (Also takes run object)
void RunAction::EndOfRunAction(const G4Run* run) {
    // Get a pointer to the singleton analysis manager
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Write data (nTuples & histogram) to the open outfile at the end of each run
    analysisManager->Write();
    
    // Ensure the the outfile is closed after writing data
    analysisManager->CloseFile();
    
    // Grab the run id from the ruin object again for G4 stdout feedback
    G4int runID = run->GetRunID();
    
    // Write to G4 stdout at end of run with id
    G4cout << "Finishing Run: " << runID << G4endl;
}
