// ..
#include "RunAction.hh"
#include "AnalysisManager.hh"

#include "G4AnalysisManager.hh"

// ..

// Define the class constructor
RunAction::RunAction() {
    auto fAnalysis = new AnalysisManager();
}

RunAction::~RunAction() {
    delete fAnalysis;
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
    
    // TODO: May wanna just move this ^ to method in AnalysisManager
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
