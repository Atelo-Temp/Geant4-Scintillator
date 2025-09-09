#include "AnalysisManager.hh"

#include "G4AnalysisManager.hh"

// Define the constructor
AnalysisManager::AnalysisManager() {
   CreateHistogram(); 
}

// Private method to initialise histogram, call internally at construction
void AnalysisManager::CreateHistogram() {
    // Get a pointer to the singleton analysis manager via the static method
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Creat histogram (one dimension to store deposited energy)
    analysisManager->CreateH1(
        "Photons", // Name
        "Optical Photons", // Title
        256, // NOTE: Seems inadequate for optical photons
        // 512, // 
        // 1024, // Number of bins
        // 0., // Lower bounds
        // 1. * eV, // Lower bounds
        // 1. * MeV // Upper bounds
        1., // Lower bounds
        // 38000. // Upper bounds (NOTE: This is far too much, 5k would be fine due to how few of those generated actually detect in photocathode)
        5000. // Upper bounds
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
