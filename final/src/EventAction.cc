// User classes
#include "EventAction.hh" // class interface

// G4 Lib
#include "G4Event.hh" // event object
#include "G4ios.hh" // for G4cout
#include "G4AnalysisManager.hh" // histogramming

// Electronics smearing (simulating the PMT)
#include <Randomize.hh>


// Define the constructor
EventAction::EventAction(RunAction* runAction) { fRunAction = runAction; }
// EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {} 
// NOTE: ^^ I think: "fRunAction(runAction)", does same thing as: "fRunAction = runAction"

void EventAction::BeginOfEventAction(const G4Event*) {
    // Reset counters between events
    fTotalPhotons = 0;
    fDetectedPhotons = 0;
    fAbsorbedPhotons = 0;
}

void EventAction::EndOfEventAction(const G4Event*) {
    // Print to stdout
//     G4cout << G4endl << "Optical Photons Generated: " << fTotalPhotons << G4endl;
//     
//     G4cout << G4endl << "Optical Photons Detected: " << fDetectedPhotons << G4endl;
//     G4cout << G4endl << "Percent Detected: " << (fDetectedPhotons / fTotalPhotons) * 100 << G4endl;
//     
//     G4cout << G4endl << "Optical Photons Absorbed: " << fAbsorbedPhotons << G4endl;
//     G4cout << G4endl << "Percent Absorbed: " << (fAbsorbedPhotons / fTotalPhotons) * 100 << G4endl;
    
    // NOTE: ^ Removed to save time during batch processing, may want an logic statement that 
    // checks whether in visualisation or batch, as this nice for vis
    
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


