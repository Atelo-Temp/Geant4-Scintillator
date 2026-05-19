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
    
    // Create nTuples to store more in root file
    // analysisManager->CreateNtuple("Photons", "Photons"); // name, title
    // analysisManager->CreateNtuple("Photons Data", "Photon Coordinates"); // name, title
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
    // analysisManager->FinishNtuple(0);
    analysisManager->FinishNtuple(); // dont need to pass 0 here, automatically finishes ID = 0
    
    // TEST \/\/\/\/\/\/\/\/\/
    // Create Ntuple ID = 1
    // analysisManager->CreateNtuple("Optical Data", "Photon Counts Per Event");
    analysisManager->CreateNtuple("EventData", "Photon Counts Per Event");
    analysisManager->CreateNtupleIColumn("NumPhotons"); // Column 0: integer count
    analysisManager->FinishNtuple(); // automatically finishes ID = 1
    // NOTE: Second call to createNtuple automatically assigns ID = 1
}
