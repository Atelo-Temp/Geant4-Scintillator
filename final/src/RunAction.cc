// User classes
#include "RunAction.hh"
#include "AnalysisManager.hh"
#include <CLHEP/Random/Random.h>
#include <chrono>

// G4 lib
#include "G4AnalysisManager.hh"

// Define the class constructor
RunAction::RunAction() {
    // auto fAnalysis = new AnalysisManager(); // NOTE: Shouldnt have auto here!
    fAnalysis = new AnalysisManager();
    
    // Enable merging of Ntuples which are spread across the threads into one outfile
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetNtupleMerging(true);
    // NOTE: See README.md for more info
    
    // TEST
    fStartTime = GenerateTimestamp();
    
    // Passing a time-dependent metric such as the computers system clock ensures
    // that every simulation run produces entirely unique and unpredictable results
    // CLHEP::HepRandom::setTheSeed(fStartTime); 
    // NOTE: ^ UNCOMMENT ME TO RANDOMISE SIMULATION RUNS
    
    // Write to G4 stdout at end of run with id
    G4cout << "Starting Run At: " << fStartTime << G4endl;
    // TODO: ^ maybe move this to BeginOfRunAction
}

// Class destructor frees up local storage
RunAction::~RunAction() {
    delete fAnalysis;
    
    // This code executes only on the master thread
    // NOTE: A run action class is instantiated both thread-local and global,
    // this saves calculating run time for each thread
    // if (isMaster) {
    
    // This code wont execute on the master thread, only on worker threads
    if (!isMaster) {
        // TEST: May wanna do this in EndOfRunAction
        HandleRuntime(); 
        // was exploring it here to see if it avoided multiple thread execution of this,
        // but it doesnt, so may as well do in EndOfRunAction
    }
}

// Define the start of run event handler (Takes run object as a parameter)
// NOTE: This method is invoked at the beginning of the BeamOn() method, but after
// confirmation of the conditions of the G4 kernel
void RunAction::BeginOfRunAction(const G4Run* run) {
    // TODO: May wanna just move all the code below \/ \/ to a method in AnalysisManager,
    // as im instantiating fAnalysis in constructor anyways ...
    
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
    // TODO: Again this all seems suitable for a dedicated method in AnalysisManager \/
    // except the "finishing run" console log (and run ID grab for said log)
    
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

// Get a millisecond based timestamp for runtime tracking and random seeding
long RunAction::GenerateTimestamp() {
//     // Capture the current system date/time (for generating a unique seed)
//     std::chrono::time_point time = std::chrono::system_clock::now();
//     
//     // Convert the "time_point" to an OS dependent epoch timestamp
//     std::chrono::duration epoch = time.time_since_epoch();
//     
//     // Convert the OS dependent timestamp into a unix epoch millisecond timestamp
//     std::chrono::milliseconds casted = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    
    
    /// ...
//     // Capture the current system date/time (for generating a unique seed)
//     auto time = std::chrono::system_clock::now();
//     
//     // Convert the "time_point" to an OS dependent epoch timestamp
//     auto epoch = time.time_since_epoch();
//     
//     // Convert the OS dependent timestamp into a unix epoch millisecond timestamp
//     // auto casted = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
//     auto casted = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
//     
//     // Convert the "duration" type to a long (suitable for seeding)
//     long timestamp = epoch.count();
    
    
    /// ....
    
    // Capture the current system date/time (for generating a unique seed)
    auto time = std::chrono::system_clock::now();
    
    // Convert the OS dependent timestamp into a unix epoch millisecond timestamp
    auto casted = std::chrono::time_point_cast<std::chrono::milliseconds>(time);
    
    // Convert the "time_point" to an OS dependent epoch timestamp
    auto epoch = casted.time_since_epoch();
    
    // Convert the "duration" type to a long (suitable for seeding)
    long timestamp = epoch.count();
    
    // ...
    return timestamp;
    
    // TODO: Store start timestamp as class property, get timestamp at end of run,
    // subtract start from end, convert stamp to hrs/mins/seconds
    // NOTE: May wanna extract timestamp handling out to dedicated method for reuse at end of run
    // NOTE: May also want to move the timestamp handling/start time printing
    // as it gets repeated for every single thread currently
    // (maybe at class construction, will need to play around and check best spot, can still do
    // seed here though, as will be able to access start timestamp via class property)
}

// Get timestamp at end of run, subtract start from end, convert stamp to hrs/mins/seconds
void RunAction::HandleRuntime() {
    long const endTime = GenerateTimestamp(); // get millisecond timestamp
    
    long const runDurationMS = endTime - fStartTime; // run duration in milliseconds
    
    long const runDurationS = runDurationMS / 1000; // milliseconds to seconds
    
    long const runDurationM = runDurationS / 60; // seconds to minutes
    
    long const runDurationH = runDurationM / 60; // minutes to hours
    
    long const remainderS = runDurationS % 60; // remainder seconds not divisible by 60
    
    // TEST ...
    // G4cout << "Finishing Run At: " << G4endl;
    // G4cout << "Run Duration: " << runDurationM << G4endl;
    // G4cout << "Run Duration: " << runDurationM << "m" << remainderS << "s" << G4endl;
    G4cout << "Run Duration: " << runDurationH << "h " << runDurationM << "m " << remainderS << "s" << G4endl;
}
