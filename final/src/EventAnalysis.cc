// User classes
#include "EventAnalysis.hh"
#include "OutputConfig.hh"
// #include "AnalysisRegistry.hh"
#include "HitManager.hh"

// G4 lib
#include "G4AnalysisManager.hh"
// #include "G4ios.hh" // for G4cout

/*
 * Constructor
 */
EventAnalysis::EventAnalysis(HitManager* hitManager) : fHitManager(hitManager) {
    // Cache a pointer to the analysis manager instance
    fAnalysisManager = G4AnalysisManager::Instance();
    
    // TEST
    // ...
    OutputConfig& outputConfig = OutputConfig::GetInstance();
    outputConfig.AddListener(this);
    
    // ...
    AnalysisRegistry& analysisRegistry = AnalysisRegistry::GetInstance();
    analysisRegistry.AddListener(this);
    
    G4cout << "\n\n>>>>> EVENT ANALYSIS INSTANTIATED\n" << G4endl;
}

/*
 * Fetch ntuple indices and update local cache
 */
void EventAnalysis::UpdateRegistryCache() {
    // Get readonly reference to registry singleton
    AnalysisRegistry const& registry = AnalysisRegistry::GetInstance();
    
    // Get readonly reference to ntuple indices object
    NtupleIDs const& ntupleIDs = registry.ReadNtupleIDs();
    
    // Cache flags relevant to this class
    fEventNtupleIDs = &(ntupleIDs.fEventNtupleIDs);
    // NOTE: Cache a pointer to the indices, dont copy the data itself
}

/*
 * Fetch output flags and update local cache
 */
void EventAnalysis::UpdateStateFlags() {
    //  Get readonly reference to config singleton
    OutputConfig const& outputConfig = OutputConfig::GetInstance();
    
    // Get readonly reference to output flags object
    StateFlags const& outputFlags = outputConfig.ReadStateFlags();
    
    // Cache flags relevant to this class
    fEventFlags = &(outputFlags.fEventFlags);
}

/*
 * Write data for the current event to the ntuples
 * 
 * TODO: Handlers for the two clauses to encapsulate logic & separate concerns
 */
void EventAnalysis::WriteEventData() const {    
    // ...
    G4int const fTotalPhotons = fHitManager->GetTotalPhotons();
    
    // Only write to ntuple when optical photons are generated via energy deposition
    if (fTotalPhotons > 0) {
        // ...
        G4int const fDetectedPhotons = fHitManager->GetDetectedPhotons();
        
        // Only write to histo when non-zero optical photons detected at the photocathode
        if (fDetectedPhotons > 0) {
            // Fill the per-event total detected photons ntuple
            fAnalysisManager->FillNtupleIColumn(
                fEventNtupleIDs->fDetectionNtuple.fNtupleID, 
                fEventNtupleIDs->fDetectionNtuple.fColumnID, 
                fDetectedPhotons
            ); // ntuple ID, column ID, fill value
            // NOTE: Only 1 column, hence column ID = 0
            fAnalysisManager->AddNtupleRow(fEventNtupleIDs->fDetectionNtuple.fNtupleID); // Save the row for Ntuple ID = 2
            
            // TEST: DEBUGGING THE HIGH COUNTS OF NEAR-ZERO DETECTIONS SINCE ADDING Al2O3 RINDEX
            // if (fDetectedPhotons < 10) {
            //     G4cout << "LOW PHOTON COUNT: " << fDetectedPhotons << G4endl;
            // }
        }
    
        // Calculate fractional detection efficiency and write to respective column
        double const detectionEfficiency = (1. * fDetectedPhotons) / fTotalPhotons;
        
        fAnalysisManager->FillNtupleDColumn(
            fEventNtupleIDs->fDetectionFractionNtuple.fNtupleID,
            fEventNtupleIDs->fDetectionFractionNtuple.fColumnID,
            detectionEfficiency
        ); // ntuple ID = 5, column ID = 0
        // NOTE: Cast to double (via 1. *)
        
        // ...
        G4int const fBoundaryAbsorbedPhotons = fHitManager->GetBoundaryAbsorptions();
        
        // Calculate fractional bulk absorption losses and write to respective column
        double const surfaceAbsoptionLosses = (1. * fBoundaryAbsorbedPhotons) / fTotalPhotons;
        
        fAnalysisManager->FillNtupleDColumn(
            fEventNtupleIDs->fBoundaryAbsorbFractionNtuple.fNtupleID,
            fEventNtupleIDs->fBoundaryAbsorbFractionNtuple.fColumnID,
            surfaceAbsoptionLosses
        ); // ntuple ID = 5, column ID = 2
        
        // ...
        G4int const fBulkAbsorbedPhotons = fHitManager->GetBulkAbsorptions();
                
        // Calculate fractional bulk absorption losses and write to respective column
        double const bulkAbsoptionLosses = (1. * fBulkAbsorbedPhotons) / fTotalPhotons;
        
        fAnalysisManager->FillNtupleDColumn(
            fEventNtupleIDs->fBulkAbsorbFractionNtuple.fNtupleID,
            fEventNtupleIDs->fBulkAbsorbFractionNtuple.fColumnID,
            bulkAbsoptionLosses
        ); // ntuple ID = 5, column ID = 1
        
        // Row complete
        fAnalysisManager->AddNtupleRow(fEventNtupleIDs->fDetectionFractionNtuple.fNtupleID); // Save the row for Ntuple ID = 5
        // NOTE: These columns share an ntuple, so any of their stored ntuple ids could be used
        // TODO: ^^ rethink this imo, feels a bit loose, only valid since i know its valid ...
        // maybe an if statement checking if their ntuple ids align, if not, add row for that id
    }
    // NOTE: While this could likely be merged with prior if clause, the explicit disambiguation is worthwhile imo
}

/*
 * Inspect event state, print to stdout
 * 
 * NOTE: Readonly method
 * 
 * NOTE: Disabled to save time during batch processing, may want a logic statement that
 * checks whether in visualisation or batch, as this nice for vis
 * 
 * TODO: Reflection logging ?
 */
void EventAnalysis::LogEventData() const {    
    // ...
    G4int const fTotalPhotons = fHitManager->GetTotalPhotons();
    
    // ...
    G4int const fDetectedPhotons = fHitManager->GetDetectedPhotons();
    
    // ...
    G4int const fBoundaryAbsorbedPhotons = fHitManager->GetBoundaryAbsorptions();
    
    // ...
    G4int const fBulkAbsorbedPhotons = fHitManager->GetBulkAbsorptions();
    
    // ...
    G4int const fLostPhotons = fHitManager->GetLostPhotons();
    
    // Individual loss mechanisms
    G4cout << G4endl << "Optical Photons Generated: " << fTotalPhotons << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected: " << fDetectedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected: " << ((1. * fDetectedPhotons) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Boundary Absorbed: " << fBoundaryAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Boundary Absorbed: " << ((1. * fBoundaryAbsorbedPhotons) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Bulk Absorbed: " << fBulkAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Bulk Absorbed: " << ((1. * fBulkAbsorbedPhotons) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Lost: " << fLostPhotons << G4endl;
    G4cout << G4endl << "Percent Lost: " << (1. * fLostPhotons / fTotalPhotons) * 100 << G4endl;
    
    // Group stats
    G4cout << G4endl << "Optical Photons Detected OR Boundary Absorbed: " << fDetectedPhotons + fBoundaryAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected OR Boundary Absorbed: " << ((1. * (fDetectedPhotons + fBoundaryAbsorbedPhotons)) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Bulk Absorbed: " << fDetectedPhotons + fBulkAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected OR Bulk Absorbed: " << ((1. * (fDetectedPhotons + fBulkAbsorbedPhotons)) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Boundary Absorbed OR Bulk Absorbed: " << fBoundaryAbsorbedPhotons + fBulkAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Boundary Absorbed OR Bulk Absorbed: " << ((1. * (fBoundaryAbsorbedPhotons + fBulkAbsorbedPhotons)) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Boundary Absorbed OR Bulk Absorbed: " << fDetectedPhotons + fBoundaryAbsorbedPhotons + fBulkAbsorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected OR Boundary Absorbed OR Bulk Absorbed: " << ((1. * (fDetectedPhotons + fBoundaryAbsorbedPhotons + fBulkAbsorbedPhotons)) / fTotalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected OR Boundary Absorbed OR Bulk Absorbed OR Lost: " << fDetectedPhotons + fBoundaryAbsorbedPhotons + fBulkAbsorbedPhotons + fLostPhotons << G4endl;
    G4cout << G4endl << "Percent Detected OR Boundary Absorbed OR Bulk Absorbed OR Lost: " << ((1. * (fDetectedPhotons + fBoundaryAbsorbedPhotons + fBulkAbsorbedPhotons + fLostPhotons)) / fTotalPhotons) * 100 << G4endl;
}
