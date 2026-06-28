// User classes
#include "RunAction.hh"
#include "RunAnalysis.hh"
#include "Timer.hh"

// G4 lib
#include "G4AnalysisManager.hh"
// #include "G4RunManager.hh"

/*
 * Constructor
 * 
 * NOTE: A run action class is instantiated both thread-local and global,
 * hence the constructor & destructor will be called on the master thread,
 * and every worker thread (i.e. 3 threads = 4 constructions/destructions)
 */
RunAction::RunAction() {
    // Instantiate the analysis handler and store a pointer to it in class property
    fAnalysis = new RunAnalysis(); // NOTE: Shouldnt have auto here!
    
    // Enable merging of Ntuples which are spread across the threads into one outfile
    G4GenericAnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetNtupleMerging(true); // <<<<<<<<<<<<<< Uncomment me
    // NOTE: See README.md for more info
    
    G4cout << "\n\n>>>>> RUN ACTION INSTANTIATED\n\n" << G4endl;
}

/*
 * Destructor frees up local storage (deallocating from the heap)
 */
RunAction::~RunAction() {
    delete fAnalysis;
}
 
/*
 * Define the start of run event handler (Takes run object as a parameter)
 * NOTE: This method is invoked at the beginning of the BeamOn() method, but after
 * confirmation of the conditions of the G4 kernel
 * 
 * TODO: Extract marked code below to fRunAnalysis->CreateOutfile()
 * 
 * TODO: call fRunAnalysis->CreateDataStructures() here, instead of at construction
 * ^ needs to be delayed to allow macro to set output flags
 * 
 * TODO: How do things behave with ntuple and file creation on worker threads ?
 */
void RunAction::BeginOfRunAction(const G4Run* run) {
    // Inform the runManager to save random number seed (for reproducibility at later date)
    // G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  
    // TODO: May wanna just move all the code below \/ \/ to a method in AnalysisManager,
    // as im instantiating fAnalysis in constructor anyways ...
    
    // Get a pointer to the singleton analysis manager
    G4GenericAnalysisManager* analysisManager = G4AnalysisManager::Instance();
    // NOTE: Couldnt the pointer just be stored in the class?
    // But then youre storing a pointer to it in two places, maybe wasteful ?
    
    // Access the ID for the run object (integer)
    G4int const runID = run->GetRunID();
    
    // Convert ID integer to string (To include run ID in the filename)
    std::stringstream strRunID; // String stream object
    strRunID << runID; // Pipe the run id into the stream
    // NOTE: Must convert int to string in C++ (not always necessary in other languages)
    
    // NOTE: Alternatively std lib may be used to convert to string
    // std::string str = std::to_string(runID);
    
    // Define the outfile name (converting string stream object into a string)
    G4String const fileName = "output" + strRunID.str() + ".root"; // append file type
    // NOTE: Could overwrite output.root on each run, but this is a better option
    
    // Create and open the file with the supplied name
    analysisManager->OpenFile(fileName);
    
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    
    
    // TEST - Delay creation of data structures until start of run and outfile successfully opened
    fAnalysis->InitialiseDataStructures();
    // NOTE: Instead of doing so at construction
    // TEST
    
    
    // This code wont execute on the master thread, only on worker threads
    // NOTE: if (isMaster) enclosed code would execute only on the master thread
    if (!isMaster) {
        // Instantiate the timer (on the heap so it outlives enclosure)
        fTimer = new Timer();
        
        // Start the timer for this run
        long const startTime = fTimer->StartTimer();
        
        // Write to G4 stdout at end of run with id
        G4cout << "Starting Run At: " << startTime << G4endl;
        // NOTE: May move this stdout output to timer, not sure
    }
}

/*
 * Define the end of run event handler (Also takes run object)
 * 
 * TODO: Extract marked code below to fRunAnalysis->WriteOutfile()
 */
void RunAction::EndOfRunAction(const G4Run* run) {
    // TODO: Again this all seems suitable for a dedicated method in AnalysisManager \/\/\/
    // except the "finishing run" console log (and run ID grab for said log)
    
     // Get a pointer to the singleton analysis manager
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Write data (nTuples & histogram) to the open outfile at the end of each run
    analysisManager->Write();
    
    // Ensure the the outfile is closed after writing data
    analysisManager->CloseFile();
    
    // Grab the run id from the ruin object again for G4 stdout feedback
    G4int const runID = run->GetRunID();
    
    // Write to G4 stdout at end of run with id
    G4cout << "Finishing Run: " << runID << G4endl;
    
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    
    // This code wont execute on the master thread, only on worker threads
    if (!isMaster && (fTimer != nullptr)) {
        // End the run timer and calculate run time (prints to g4cout)
        fTimer->EndTimer();
        
        // Free allocated memory
        delete fTimer;
    }
}
