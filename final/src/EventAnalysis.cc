// User classes
#include "EventAnalysis.hh"
#include "ProgramState.hh"

// G4 lib
#include "G4AnalysisManager.hh"
// #include "G4ios.hh" // for G4cout

/*
 * Constructor
 */
EventAnalysis::EventAnalysis() {    
    // Cache a pointer to the analysis manager instance
    fAnalysisManager = G4AnalysisManager::Instance();
    
    // ...
    // auto& instance = ProgramState::GetInstance("BBB");
    // G4cout << "\n\n>>>>> EVENT ANALYSIS:: " << instance.value() << "\n\n" << G4endl;
    
    auto& instance = ProgramState::GetInstance();
    
    // Increase allocation for reflection vector map
    fReflectionMap.resize(40960);
}

/*
 * Reset counters between events
 */
void EventAnalysis::ResetCounters() {
    // Set integers to zero
    fTotalPhotons = 0;
    fDetectedPhotons = 0;
    fAbsorbedPhotons = 0;
    fBulkAbsorb = 0;
    fLostPhotons = 0; // NoRINDEX
    
    // Set all indices in reflection map to zero values
    // fReflectionMap.assign(fReflectionMap.size(), 0);
    std::fill(fReflectionMap.begin(), fReflectionMap.end(), 0);
}

/*
 * ...
 * 
 * TODO: Handlers for the two clauses to encapsulate logic & separate concerns
 */
void EventAnalysis::WriteEventData() {
    // Only write to histo when non-zero optical photons detected at the photocathode
    if (fDetectedPhotons > 0) {
        // Fill the per-event total detected photons ntuple
        fAnalysisManager->FillNtupleIColumn(2, 0, fDetectedPhotons); // ntuple ID, column ID, fill value
        // NOTE: Only 1 column, hence column ID = 0
        fAnalysisManager->AddNtupleRow(2); // Save the row for Ntuple ID = 2
        
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
        fAnalysisManager->FillNtupleDColumn(5, 0, detectionEfficiency); // ntuple ID = 5, column ID = 0
        // NOTE: Cast to double (via 1. *)
        
        // Calculate fractional bulk absorption losses and write to respective column
        double const bulkAbsoptionLosses = (1. * fBulkAbsorb) / fTotalPhotons;
        fAnalysisManager->FillNtupleDColumn(5, 1, bulkAbsoptionLosses); // ntuple ID = 5, column ID = 1
        
        // Calculate fractional bulk absorption losses and write to respective column
        double const surfaceAbsoptionLosses = (1. * fAbsorbedPhotons) / fTotalPhotons;
        fAnalysisManager->FillNtupleDColumn(5, 2, surfaceAbsoptionLosses); // ntuple ID = 5, column ID = 2
        
        // Row complete
        fAnalysisManager->AddNtupleRow(5); // Save the row for Ntuple ID = 5
    }
    // NOTE: While this could likely be merged with prior if clause, the explicit disambiguation is worthwhile imo
}

/*
 * Increment optical photons generated
 */
void EventAnalysis::CountPhoton() {
    fTotalPhotons += 1;
}

/*
 * Increment optical photons detected (at photocathode)
 */
void EventAnalysis::CountDetectedPhoton() {
    fDetectedPhotons += 1;
}

/*
 * Increment optical photons absorbed (at a boundary)
 */
void EventAnalysis::CountAbsorbedPhoton() {
    fAbsorbedPhotons += 1;
}

/*
 * Increment optical photons lost (due to no RINDEX etc)
 */
void EventAnalysis::CountLostPhoton() {
    fLostPhotons += 1;
}
// void EventAnalysis::CountLostPhoton(std::string medium) { fLostPhotons += 1; } // TODO: Add medium where each of these things occured (same for absorption, etc)

/*
 * Increment optical photons absorbed (in medium)
 */
void EventAnalysis::CountBulkAbsorption() {
    fBulkAbsorb += 1;
}

/*
 * Increment reflection counter for specified photon
 * 
 * NOTE: This is for any type of reflection:
 * - FresnelReflection
 * - TotalInternalReflection
 * - LambertianReflection
 * - LobeReflection
 * - SpikeReflection
 * - BackScattering
 * 
 * NOTE: Will increase size of vector if passed photonIdx exceeds capacity
 */
void EventAnalysis::CountReflection(G4int photonIdx) {
    if (photonIdx >= fReflectionMap.size()) fReflectionMap.resize(photonIdx + 2048, 0);
    fReflectionMap[photonIdx]++;
}

/*
 * Retrieve total number of reflections for specified photon
 */
G4int EventAnalysis::GetReflections(G4int photonIdx) {
    return fReflectionMap[photonIdx];
}

/*
 * Inspect event state, print to stdout
 */
void EventAnalysis::LogEventData() {
    // Individual loss mechanisms
    G4cout << G4endl << "Optical Photons Generated: " << fTotalPhotons << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected: " << fDetectedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected: " << ((1. * fDetectedPhotons) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Boundary Absorbed: " << fAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Boundary Absorbed: " << ((1. * fAbsorbedPhotons) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Bulk Absorbed: " << fBulkAbsorb << G4endl;
    G4cout << G4endl << "Percent Bulk Absorbed: " << ((1. * fBulkAbsorb) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Lost: " << fLostPhotons << G4endl;
    G4cout << G4endl << "Percent Lost: " << (1. * fLostPhotons / fTotalPhotons) * 100 << G4endl;
    
    // Group stats
    G4cout << G4endl << "Optical Photons Detected OR Boundary Absorbed: " << fDetectedPhotons + fAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected OR Boundary Absorbed: " << ((1. * (fDetectedPhotons + fAbsorbedPhotons)) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Bulk Absorbed: " << fDetectedPhotons + fBulkAbsorb << G4endl;
    G4cout << G4endl << "Percent Detected OR Bulk Absorbed: " << ((1. * (fDetectedPhotons + fBulkAbsorb)) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Boundary Absorbed OR Bulk Absorbed: " << fAbsorbedPhotons + fBulkAbsorb << G4endl;
    G4cout << G4endl << "Percent Boundary Absorbed OR Bulk Absorbed: " << ((1. * (fAbsorbedPhotons + fBulkAbsorb)) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Boundary Absorbed OR Bulk Absorbed: " << fDetectedPhotons + fAbsorbedPhotons + fBulkAbsorb << G4endl;
    G4cout << G4endl << "Percent Detected OR Boundary Absorbed OR Bulk Absorbed: " << ((1. * (fDetectedPhotons + fAbsorbedPhotons + fBulkAbsorb)) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Boundary Absorbed OR Bulk Absorbed OR Lost: " << fDetectedPhotons + fAbsorbedPhotons + fBulkAbsorb + fLostPhotons << G4endl;
    G4cout << G4endl << "Percent Detected OR Boundary Absorbed OR Bulk Absorbed OR Lost: " << ((1. * (fDetectedPhotons + fAbsorbedPhotons + fBulkAbsorb + fLostPhotons)) / fTotalPhotons) * 100 << G4endl;
    
    // NOTE: ^ Removed to save time during batch processing, may want an logic statement that 
    // checks whether in visualisation or batch, as this nice for vis
}
