// User classes
#include "SteppingAnalysis.hh"
#include "OutputConfig.hh"
// #include "AnalysisRegistry.hh"
#include "HitManager.hh"

// G4 lib
#include "G4AnalysisManager.hh"

/*
 * Constructor
 * 
 * SteppingAction
 *             | 
 *             ├─ EventAction*
 *             ├─ EventAnalysis*
 *             | 
 *             └─ SteppingAnalysis <<<
 *                              | 
 *                              ├─ AnalysisRegistry*
 *                              └─ OutputConfig*
 * 
 * NOTE: Owned by SteppingAction
 * 
 * TODO: Storing HitManager here as well as in SteppingAction is redundant, just pass as arg
 * ^ or dont pass as arg to methods below, just pass at construction and use class property
 * ^ probably former tbh
 */
SteppingAnalysis::SteppingAnalysis(HitManager* hitManager) : fHitManager(hitManager) {
    // Cache pointer to analysis manager singleton
    fAnalysisManager = G4AnalysisManager::Instance();
    
    // TEST
    // ...
    OutputConfig& outputConfig = OutputConfig::GetInstance();
    outputConfig.AddListener(this);
    
    // ...
    AnalysisRegistry& analysisRegistry = AnalysisRegistry::GetInstance();
    analysisRegistry.AddListener(this);
    
    G4cout << "\n\n>>>>> STEPPING ANALYSIS INSTANTIATED\n" << G4endl;
}

/*
 * Update ntuple indices cache
 * 
 * TODO: Maybe have this cache flags too
 * 
 * ^ Instead this just listens for BeginOfRunAction notification, then calls:
 * - UpdateRegistryCache (ntuple ids)
 * - UpdateStateFlags (enabled output flags)
 */
void SteppingAnalysis::UpdateRegistryCache() {
    // ...
    // G4cout << "\n\n>>>>> STEPPING ANALYSIS LISTENER\n\n" << G4endl;
    
    // Get readonly reference to registry singleton
    AnalysisRegistry const& registry = AnalysisRegistry::GetInstance();
    
    // Get readonly reference to ntuple indices object
    NtupleIDs const& ntupleIDs = registry.ReadNtupleIDs();
    
    // Cache indices relevant to this class
    fStepDetectionNtupleIDs = &(ntupleIDs.fStepDetectionNtupleIDs);
    fStepBoundaryAbsorbNtupleIDs = &(ntupleIDs.fStepBoundaryAbsorbNtupleIDs);
    fStepBulkAbsorbNtupleIDs = &(ntupleIDs.fStepBulkAbsorbNtupleIDs);
    // NOTE: Cache a pointer to the indices, dont copy the data itself
}

/*
 * Fetch output flags and update local cache
 */
void SteppingAnalysis::UpdateOutputFlagsCache() {
    // Get readonly reference to config singleton
    OutputConfig const& outputConfig = OutputConfig::GetInstance();
    
    // Get readonly reference to output flags object
    OutputFlags const& outputFlags = outputConfig.ReadOutputFlags();
    
    // Cache flags relevant to this class
    fStepDetectionFlags = &(outputFlags.fStepDetectionFlags);
    fStepBoundaryAbsorbFlags = &(outputFlags.fStepBoundaryAbsorbFlags);
    fStepBulkAbsorbFlags = &(outputFlags.fStepBulkAbsorbFlags);
    // NOTE: Cache a pointer to the indices, dont copy the data itself
}

/*
 * Track photons lost via bulk absorption in the crystal (and window/grease to a far lesser extent)
 * 
 * TODO: Eliminate hardcoded ntuple IDs
 * 
 * NOTE: Takes readonly pointers (pointer to const)
 */
void SteppingAnalysis::HandleBulkAbsorb(G4Track const* track, HitManager const* fHitManager) const {
    // ..
    // std::string volume = endPoint->GetTouchable()->GetVolume()->GetName();
    // G4cout << "BULK ABSORPTION IN MEDIUM: " << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Scintillator"
    
    // ...
    if (fStepBulkAbsorbFlags->fBulkAbsorbDistanceNtuple) {
        // Get distance travelled by photon before bulk absorption
        G4double const distance = track->GetTrackLength();
        
        fAnalysisManager->FillNtupleDColumn(
            fStepBulkAbsorbNtupleIDs->fNtupleID, // ntuple id
            fStepBulkAbsorbNtupleIDs->fBulkAbsorbDistanceNtuple.fColumnID, // column id
            distance // value
        );
        // G4cout << "Distance Travelled Before Bulk Absorption: " << distance << " mm" << G4endl;
    }
    
    // ...
    if (fStepBulkAbsorbFlags->fBulkAbsorbReflectionsNtuple) {
        // Retrieve number of reflections
        G4int const photonIdx = track->GetTrackID();
        
        G4int const numReflections = fHitManager->GetReflections(photonIdx);
        
        fAnalysisManager->FillNtupleIColumn(
            fStepBulkAbsorbNtupleIDs->fNtupleID, 
            fStepBulkAbsorbNtupleIDs->fBulkAbsorbReflectionsNtuple.fColumnID, 
            numReflections
        ); // ...
    }
    
    // If any of the step bulk absorb output ntuples are active
    // TODO: Feels like theres a better way to handle this add row logic
    if (
        fStepBulkAbsorbFlags->fBulkAbsorbDistanceNtuple || 
        fStepBulkAbsorbFlags->fBulkAbsorbReflectionsNtuple
    ) {
        // Mark this row as complete (for Ntuple with passed ID)
        fAnalysisManager->AddNtupleRow(fStepBulkAbsorbNtupleIDs->fNtupleID); // finish row for Ntuple id = X
    }
}

/*
 * Handle optical photon being detected at a boundary (i.e., photocathode)
 * 
 * NOTE: Takes readonly pointers (pointer to const)
 * 
 * TODO:
 * if (fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fNtupleID != fStepDetectionNtupleIDs->fDetectionDistanceNtuple.fNtupleID) {
 *  AddNtupleRow(fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fNtupleID)
 * }
 * if (fStepDetectionNtupleIDs->fDetectionDistanceNtuple.fNtupleID != fStepDetectionNtupleIDs->fDetectionTimeOfFlightNtuple.fNtupleID) {
 *  AddNtupleRow(fStepDetectionNtupleIDs->fDetectionDistanceNtuple.fNtupleID)
 * }
 * ... etc
 */
void SteppingAnalysis::HandleDetection(G4StepPoint const* endPoint, G4Track const* track, HitManager const* fHitManager) const {
    // TODO: May be worth double checking the boundary is the photocathode
    // ...although since its only one with efficiency vector, it will be
    
    // G4cout << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Scoring"
    // auto detectionVolume = endPoint->GetTouchable()->GetVolume()->GetName();
    // if (detectionVolume != "Photocathode") G4cout << detectionVolume << G4endl;
    
    // if (endPoint->GetTouchable()->GetVolume() == fScoringVolume)
    // NOTE: Get fScoringVolume via public method on DetectorConstruction
    
    // ...
    if (fStepDetectionFlags->fDetectionCoordsNtuple) {
        // Get (x, y, z) coordinates at point where optical photon detected by photocathode
        G4ThreeVector const detectionPosition = endPoint->GetPosition();
        double const x = detectionPosition[0];
        double const y = detectionPosition[1];
        double const z = detectionPosition[2];
        
        // TODO: Maybe cache/reuse a little bit, might actually reduce chances of human error too tbh
        // \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
        // auto& ntuple = fStepDetectionNtupleIDs->fDetectionCoordsNtuple;
        // auto& ntupleID = ntuple.fNtupleID;
        // ntuple.fXColumnID;
        // TODO ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        
        // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
        fAnalysisManager->FillNtupleDColumn(
            fStepDetectionNtupleIDs->fNtupleID,
            fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fXColumnID,
            x
        ); // NtupleID = 0, 0th column, x
        fAnalysisManager->FillNtupleDColumn(
            fStepDetectionNtupleIDs->fNtupleID,
            fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fYColumnID,
            y
        ); // NtupleID = 0, 1st column, y
        fAnalysisManager->FillNtupleDColumn(
            fStepDetectionNtupleIDs->fNtupleID,
            fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fZColumnID,
            z
        ); // NtupleID = 0, 2nd column, z
        // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
    }
    
    // TEST TEST TEST
    // Get the global time (TODO: For timing window)
    // auto detectTime = endPoint->GetGlobalTime();
    // NOTE: May actually need to store the time at the first detection of a photon for the current event 
    // (which would produce first avalanche in current event, beginning the charge integration process)
    // then compare that to easch subsequent detection
    // i.e.
    // if (!fIntegrationStart) fIntegrationStart = endPoint->GetGlobalTime();
    // else time = endPoint->GetGlobalTime() - fIntegrationStart;
    // NOTE: Would want to store fIntegrationStart in fEventAction btw, and provide a setter function,
    // like fEventAction->CountDetectedPhoton(), as it would need to be nullified at the start of each event,
    // again much like photon counts
    // TEST TEST TEST
    
    // ...
    if (fStepDetectionFlags->fDetectionDistanceNtuple) {
        // Get distance travelled by photon before detection (from creation to absorption in PC)
        G4double const distance = track->GetTrackLength();
        
        fAnalysisManager->FillNtupleDColumn(
            fStepDetectionNtupleIDs->fNtupleID,
            fStepDetectionNtupleIDs->fDetectionDistanceNtuple.fColumnID,
            distance
        );
        // G4cout << "Distance Travelled Before Detection: " << distance << " mm" << G4endl;
    }
    
    // ...
    if (fStepDetectionFlags->fDetectionTimeOfFlightNtuple) {
        // Get time of flight information (local time gives time since photon birth until now, i.e. birth to detection)
        G4double const time = track->GetLocalTime();
        
        fAnalysisManager->FillNtupleDColumn(
            fStepDetectionNtupleIDs->fNtupleID,
            fStepDetectionNtupleIDs->fDetectionTimeOfFlightNtuple.fColumnID,
            time
        );
        
        // NOTE: This method loses relative timing between different photons if they 
        // were created at different points along the primary particles track
    }
    
    // ...
    if (fStepDetectionFlags->fDetectionReflectionsNtuple) {
        // Retrieve number of reflections
        G4int const photonIdx = track->GetTrackID();
        
        G4int const numReflections = fHitManager->GetReflections(photonIdx);
        
        fAnalysisManager->FillNtupleIColumn(
            fStepDetectionNtupleIDs->fNtupleID,
            fStepDetectionNtupleIDs->fDetectionReflectionsNtuple.fColumnID,
            numReflections
        );
    }
    
    // TEST
    // Calculate angle of incidence for detected photon
    // endPoint->GetTouchable()->GetVolume()->GetObjectRotation();
    // G4ThreeVector const trans = endPoint->GetTouchable()->GetVolume()->GetObjectTranslation();
    // trans.angle();
    // trans.
    // TEST
    
    // If any of the step detection output ntuples are active
    // TODO: Feels like theres a better way to handle this add row logic
    if (
        fStepDetectionFlags->fDetectionCoordsNtuple ||
        fStepDetectionFlags->fDetectionDistanceNtuple ||
        fStepDetectionFlags->fDetectionTimeOfFlightNtuple ||
        fStepDetectionFlags->fDetectionReflectionsNtuple
    ) {
        // Mark this row as complete (for Ntuple with passed ID)
        fAnalysisManager->AddNtupleRow(fStepDetectionNtupleIDs->fNtupleID);
    }
}

/*
 * Handle optical photon being absorbed without detection at a boundary (i.e., reflector or photocathode)
 * 
 * NOTE: Takes readonly pointer (pointer to const)
 */
void SteppingAnalysis::HandleBoundaryAbsorb(G4StepPoint const* endPoint) const {
    // TODO: Can get absorption volume via same process as above,
    // counts absorptions in crystal vs reflector, vs photocathode
    
    if (fStepBoundaryAbsorbFlags->fBoundaryAbsorbCoordsNtuple) {
        // Get (x, y, z) coordinates at point where optical photon absorbed by reflector or photocathode
        G4ThreeVector const absorptionPosition = endPoint->GetPosition();
        double const x = absorptionPosition[0];
        double const y = absorptionPosition[1];
        double const z = absorptionPosition[2];
        
        // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
        fAnalysisManager->FillNtupleDColumn(
            fStepBoundaryAbsorbNtupleIDs->fNtupleID,
            fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fXColumnID,
            x
        );
        fAnalysisManager->FillNtupleDColumn(
            fStepBoundaryAbsorbNtupleIDs->fNtupleID,
            fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fYColumnID,
            y
        );
        fAnalysisManager->FillNtupleDColumn(
            fStepBoundaryAbsorbNtupleIDs->fNtupleID,
            fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fZColumnID,
            z
        );
        // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
        
        // Mark this row as complete (for Ntuple with passed ID)
        fAnalysisManager->AddNtupleRow(fStepBoundaryAbsorbNtupleIDs->fNtupleID);
        // TODO: This is fine for now, but if another boundary absorb stat is added, extract out
    }
    
    // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
    // producing a new row (linked to the event ID) for each interaction,
    // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
}
