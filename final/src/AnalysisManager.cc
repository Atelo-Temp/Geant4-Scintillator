// Class interface
#include "AnalysisManager.hh"

// G4 lib
#include "G4AnalysisManager.hh"

// Define the constructor
AnalysisManager::AnalysisManager() {
   CreateHistogram(); 
}

// Private method to initialise histogram, call internally at construction
void AnalysisManager::CreateHistogram() {
    // Get a pointer to the singleton analysis manager via the static method
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Create histogram (one dimension to store deposited energy)
    analysisManager->CreateH1(
        "PhotonsSpectrum", // Name
        "Optical Photons", // Title
        1024, // Number of bins
        0., // Lower bounds (NOTE: EventAction now only writes to histo when photons > 0)
        5000. // Upper bounds (TODO: This needs to be dynamic ... Different sources will produce different amount of optical photons)
        // TODO: 1024. // Upper bounds (need to adjust EventAction to calculate channel instead of photons)
        // G4String& unitName = "none"
    );
    // NOTE: 100 bins from 0->1 MeV
    
    ////////////////
    // PHOTON COORDS
    ////////////////
    
    // Create nTuples to store more in root file
    analysisManager->CreateNtuple("StepData", "Photon Coordinates"); // name, title
    
    // Define columns and rows iniside the tuple
    // analysisManager->CreateNtupleIColumn("iEvent"); // I = integer (event number)
    analysisManager->CreateNtupleDColumn("fX"); // D = double (float maybe fine, but double gives increased precision) // store x position (may not need for now but in future may be useful)
    analysisManager->CreateNtupleDColumn("fY"); // y position of the photon
    analysisManager->CreateNtupleDColumn("fZ"); // y position of the photon
    // analysisManager->CreateNtupleDColumn("fGlobalTime"); // store the global time at the start of each event
    // analysisManager->CreateNtupleDColumn("fWlen"); // wavelength
    // NOTE: Could also define momentum, energy, etc ...
    
    // Absorption positions
    analysisManager->CreateNtupleDColumn("aX"); // D = double (float maybe fine, but double gives increased precision) // store x position (may not need for now but in future may be useful)
    analysisManager->CreateNtupleDColumn("aY"); // y position of the photon
    analysisManager->CreateNtupleDColumn("aZ"); // y position of the photon
    
    // Mark the definition of the tuple columns as completed
    analysisManager->FinishNtuple(); // === analysisManager->FinishNtuple(0);
    // NOTE: Dont need to pass 0 here, automatically finishes ID = 0
    
    ////////////////
    // PHOTON COUNTS
    ////////////////
    
    // Store per-event photon detection data in an Ntuple
    analysisManager->CreateNtuple("EventData", "Photon Counts Per Event"); // Create Ntuple ID = 1
    analysisManager->CreateNtupleIColumn("NumPhotons"); // Column 0: integer count
    analysisManager->FinishNtuple(); // automatically finishes ID = 1
    // NOTE: Second call to createNtuple automatically assigns ID = 1
    
    ////////////////////////////////
    // DETECTED PHOTON TRACK LENGTHS
    ////////////////////////////////
    
    /// TEST TEST TEST
    analysisManager->CreateNtuple("TrackData", "Detected Photon Track Length");
    analysisManager->CreateNtupleDColumn("Distance");
    analysisManager->FinishNtuple(); // ID = 2
    // NOTE: Could just add this to Ntuple(1), as its still per event data, just do another column 
    // ^^ no, its not per event data, need the distance travelled by every photon detected,
    // which needs to come from stepping action
}

// TODO: Extract file handling logic from RunAction
// void AnalysisManager::CreateFile() {} // creates root outfile and opens it
// void AnalysisManager::WriteAndClose() {} // writes stored data to outfile and closes it
