// User classes
#include "EventAction.hh" // class interface

// G4 Lib
#include "G4Event.hh" // event object
#include "G4ios.hh" // for G4cout
#include "G4AnalysisManager.hh" // histogramming


// Define the constructor
EventAction::EventAction(RunAction* runAction) { fRunAction = runAction; }
// EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {} // NOTE: I think this does same thing

void EventAction::BeginOfEventAction(const G4Event*) {
    // Reset counters between events
    fTotalPhotons = 0.;
    fDetectedPhotons = 0.;
    fAbsorbedPhotons = 0;
}

void EventAction::EndOfEventAction(const G4Event*) {
    // Print to stdout
    G4cout << G4endl << "Optical Photons Generated: " << fTotalPhotons << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected: " << fDetectedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected: " << (fDetectedPhotons / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Absorbed: " << fAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Absorbed: " << (fAbsorbedPhotons / fTotalPhotons) * 100 << G4endl;
    
    // NOTE: ^ Removed to save time during batch processing, may want an logic statement that 
    // checks whether in visualisation or batch, as this nice for vis
    
    /////////////////
    // HISTOGRAMMING:
    /////////////////
    
    // Get a pointer to the analysis manager instance
    auto analysisManager = G4AnalysisManager::Instance();
    // TODO: Seems awfully inefficient to get a pointer every event ?
    
    // Fill the histogram (add a count to the appropriate energy bin)
    analysisManager->FillH1(0, fDetectedPhotons); // (id, value) only one histo, so id = 0
    // NOTE: On full 662 keV energy deposited will add to counts for that bin, etc
    
    // NOTE: If using multiple histograms for any reason (i.e. two detectors)
    // make sure to mark appropriate ID
}

void EventAction::CountPhoton() { fTotalPhotons += 1; }

void EventAction::CountDetectedPhoton() { fDetectedPhotons += 1; }

void EventAction::CountAbsorbedPhoton() { fAbsorbedPhotons += 1; }


