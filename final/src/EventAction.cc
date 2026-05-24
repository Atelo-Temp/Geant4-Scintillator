// User classes
#include "EventAction.hh" // class interface

// G4 Lib
#include "G4Event.hh" // event object
#include "G4ios.hh" // for G4cout
#include "G4AnalysisManager.hh" // histogramming

// Electronics smearing (simulating the PMT)
#include <Randomize.hh>

// TODO: If needed
// struct LostPhotonMap {
//     std::string location;
//     int frequency;
// };

// Define the constructor
EventAction::EventAction(RunAction* runAction) { fRunAction = runAction; }
// EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {} 
// NOTE: ^^ I think: "fRunAction(runAction)", does same thing as: "fRunAction = runAction"

// ...
void EventAction::BeginOfEventAction(const G4Event*) {
    // Reset counters between events
    fTotalPhotons = 0;
    fDetectedPhotons = 0;
    fAbsorbedPhotons = 0;
    
    // TEST:
    fLostPhotons = 0; // NoRINDEX
    fBulkAbsorb = 0;
    fKilled = 0;
}

// ...
void EventAction::EndOfEventAction(const G4Event*) {
    // Log particle information
    Debug();
    // NOTE: Disable this if running in batch mode
    
    /////////////////
    // HISTOGRAMMING:
    /////////////////
    
    // Get a pointer to the analysis manager instance
    auto analysisManager = G4AnalysisManager::Instance();
    // TODO: Seems awfully inefficient to get a pointer every event ?
    
    // Only write to histo when non-zero optical photons detected at the photocathode
    if (fDetectedPhotons > 0) {
        // Fill the histogram (add a count to the appropriate energy bin)
        analysisManager->FillH1(0, fDetectedPhotons); // (id, value) only one histo, so id = 0
        // NOTE: On full 662 keV energy deposited will add to counts for that bin, etc
        
        // TEST \/\/\/\/\/\/\/\/\/
        // ...
        analysisManager->FillNtupleIColumn(1, 0, fDetectedPhotons); // ntuple ID, column ID, fill value
        // NOTE: Only 1 column, hence column ID = 0
        analysisManager->AddNtupleRow(1); // Saves row for Ntuple ID = 1
        // ...
    }
    // NOTE: If using multiple histograms for any reason (i.e. two detectors)
    // make sure to mark appropriate ID
}

// NOTE: fDetectedPhotons should be an int (likewise with fTotal, and fAbsorbed)...
// void EventAction::ConvertToChannel(G4double detectedPhotons) {
//     /* NOTE: Can either convert directly from fDetectedPhotons to channel number (0-1024),
//      * or convert fDetectedPhotons to an initial voltage (will be small), then apply
//      * PMT gain (which may fluctuate by n=4 to n=5 electrons per stage, for example),
//      * and finally 
//     */
//     
//     // NOTE: Converting fDetectedPhotons to voltage, then introducing a gain that fluctuates
//     
//     // Apply electronic noise
//     // G4double smearedPhotons = G4RandGauss::shoot(fDetectedPhotons, sigma);
//     
//     int channel;
// }

// ..
void EventAction::CountPhoton() { fTotalPhotons += 1; }

// ..
void EventAction::CountDetectedPhoton() { fDetectedPhotons += 1; }

// ..
void EventAction::CountAbsorbedPhoton() { fAbsorbedPhotons += 1; }

// TEST: ..
void EventAction::CountLostPhoton() { fLostPhotons += 1; }
// void EventAction::CountLostPhoton(std::string medium) { fLostPhotons += 1; } // TODO: Add medium where each of these things occured (same for absorption, etc)
void EventAction::CountBulkAbsorption() { fBulkAbsorb += 1; }
void EventAction::CountKill() { fKilled += 1; }

// ...
void EventAction::Debug() {
        // Print to stdout
    G4cout << G4endl << "Optical Photons Generated: " << fTotalPhotons << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected: " << fDetectedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected: " << (1. * fDetectedPhotons / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Absorbed: " << fAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Absorbed: " << (1. * fAbsorbedPhotons / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Absorbed: " << fDetectedPhotons + fAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected OR Absorbed: " << ((1. * fDetectedPhotons + fAbsorbedPhotons) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Lost: " << fLostPhotons << G4endl;
    G4cout << G4endl << "Percent Lost: " << (1. * fLostPhotons / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Bulk Absorbed: " << fBulkAbsorb << G4endl;
    G4cout << G4endl << "Percent Absorbed: " << (1. * fBulkAbsorb / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Absorbed OR Lost: " << fDetectedPhotons + fAbsorbedPhotons + fLostPhotons << G4endl;
    G4cout << G4endl << "Percent Detected OR Absorbed OR Lost: " << ((1. * fDetectedPhotons + fAbsorbedPhotons + fLostPhotons) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Boundary Absorbed OR Lost OR Bulk Absorbed: " << fDetectedPhotons + fAbsorbedPhotons + fLostPhotons + fBulkAbsorb << G4endl;
    G4cout << G4endl << "Percent Detected OR Boundary Absorbed OR Lost OR Bulk Absorbed: " << ((1. * fDetectedPhotons + fAbsorbedPhotons + fLostPhotons + fBulkAbsorb) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Killed: " << fKilled << " Vs Bulk Absorb: " << fBulkAbsorb << G4endl;
    
    // NOTE: ^ Removed to save time during batch processing, may want an logic statement that 
    // checks whether in visualisation or batch, as this nice for vis
}
