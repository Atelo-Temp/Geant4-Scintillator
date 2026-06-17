// User classes
#include "EventAction.hh" // class interface

// G4 Lib
#include "G4Event.hh" // event object
#include "G4ios.hh" // for G4cout
#include "G4AnalysisManager.hh" // histogramming

/*
 * Constructor
 * 
 * Takes pointer to the RunAction class and caches it in class property
 * 
 * NOTE: Member initialiser list:
 * ": fRunAction(runAction) {}"
 * 
 * Does same thing as:
 * "{ fRunAction = runAction; }"
 */
EventAction::EventAction(RunAction* runAction) { 
    fRunAction = runAction;
}
// EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}

/*
 * Execute at the start of each event
 * 
 * NOTE: Event object param unused
 */
void EventAction::BeginOfEventAction(G4Event const* /*event*/) {
    // Reset counters between events
    fTotalPhotons = 0;
    fDetectedPhotons = 0;
    fAbsorbedPhotons = 0;
    fBulkAbsorb = 0;
    fLostPhotons = 0; // NoRINDEX
}

/*
 * Execute at the end of each event
 * 
 * NOTE: Event object param unused
 */
void EventAction::EndOfEventAction(G4Event const* /*event*/) {
    // Log particle information
    // Debug();
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
        
        // Fill the per-event total detected photons ntuple
        analysisManager->FillNtupleIColumn(2, 0, fDetectedPhotons); // ntuple ID, column ID, fill value
        // NOTE: Only 1 column, hence column ID = 0
        analysisManager->AddNtupleRow(2); // Save the row for Ntuple ID = 2
        
        // TEST: DEBUGGING THE HIGH COUNTS OF NEAR-ZERO DETECTIONS SINCE ADDING Al2O3 RINDEX
        // if (fDetectedPhotons < 10) {
        //     G4cout << "LOW PHOTON COUNT: " << fDetectedPhotons << G4endl;
        // }
    }
    // NOTE: If using multiple histograms for any reason (i.e. two detectors) make sure to mark appropriate ID
    
    // Only write to ntuple when optical photons are generated via energy deposition
    if (fTotalPhotons > 0) {
        // Calculate fractional detection efficiency and write to respective column
        double const detectionEfficiency = (1. * fDetectedPhotons) / fTotalPhotons;
        analysisManager->FillNtupleDColumn(5, 0, detectionEfficiency); // ntuple ID = 5, column ID = 0
        // NOTE: Converting to double (via 1. *)
        
        // Calculate fractional bulk absorption losses and write to respective column
        double const bulkAbsoptionLosses = (1. * fBulkAbsorb) / fTotalPhotons;
        analysisManager->FillNtupleDColumn(5, 1, bulkAbsoptionLosses); // ntuple ID = 5, column ID = 1
        
        // Calculate fractional bulk absorption losses and write to respective column
        double const surfaceAbsoptionLosses = (1. * fAbsorbedPhotons) / fTotalPhotons;
        analysisManager->FillNtupleDColumn(5, 2, surfaceAbsoptionLosses); // ntuple ID = 5, column ID = 2
        
        // Row complete
        analysisManager->AddNtupleRow(5); // Save the row for Ntuple ID = 5
    }
    // NOTE: While this could likely be merged with prior if clause, the explicit disambiguation is worthwhile imo
}

/*
 * Increment optical photons generated
 */
void EventAction::CountPhoton() { fTotalPhotons += 1; }

/*
 * Increment optical photons detected (at photocathode)
 */
void EventAction::CountDetectedPhoton() { fDetectedPhotons += 1; }

/*
 * Increment optical photons absorbed (at a boundary)
 */
void EventAction::CountAbsorbedPhoton() { fAbsorbedPhotons += 1; }

/*
 * Increment optical photons lost (due to no RINDEX etc)
 */
void EventAction::CountLostPhoton() { fLostPhotons += 1; }
// void EventAction::CountLostPhoton(std::string medium) { fLostPhotons += 1; } // TODO: Add medium where each of these things occured (same for absorption, etc)

/*
 * Increment optical photons absorbed (in medium)
 */
void EventAction::CountBulkAbsorption() { fBulkAbsorb += 1; }

/*
 * Inspect event state
 */
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
    
    // NOTE: ^ Removed to save time during batch processing, may want an logic statement that 
    // checks whether in visualisation or batch, as this nice for vis
}
