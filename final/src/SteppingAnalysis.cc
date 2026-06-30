// User classes
#include "SteppingAnalysis.hh"
#include "ProgramState.hh"
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
 *                              └─ ProgramState*
 * 
 * NOTE: Owned by SteppingAction
 */
SteppingAnalysis::SteppingAnalysis(HitManager* hitManager) : fHitManager(hitManager) {
    // Cache pointer to analysis manager singleton
    fAnalysisManager = G4AnalysisManager::Instance();
    
    // ...
    // ProgramState& instance = ProgramState::GetInstance(); // TODO: Class property
    
    // ...
    AnalysisRegistry& registry = AnalysisRegistry::GetInstance();
    
    // ...
    registry.AddListener(this); // TEST
    
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
    
    // ...
    AnalysisRegistry const& registry = AnalysisRegistry::GetInstance();
    
    // Get readonly reference to ntuple indices object
    NtupleIDs const& ntupleIDs = registry.ReadNtupleIDs();
    
    // ...
    fStepDetectionNtupleIDs = &(ntupleIDs.fStepDetectionNtupleIDs);
    fStepBoundaryAbsorbNtupleIDs = &(ntupleIDs.fStepBoundaryAbsorbNtupleIDs);
    fStepBulkAbsorbNtupleIDs = &(ntupleIDs.fStepBulkAbsorbNtupleIDs);
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
    
    // Get distance travelled by photon before bulk absorption
    G4double const distance = track->GetTrackLength();
    fAnalysisManager->FillNtupleDColumn(
        fStepBulkAbsorbNtupleIDs->fBulkAbsorbDistanceNtuple.fNtupleID,
        fStepBulkAbsorbNtupleIDs->fBulkAbsorbDistanceNtuple.fColumnID,
        distance
    ); // id = 4, column = 0, value = distance travelled
    fAnalysisManager->AddNtupleRow(fStepBulkAbsorbNtupleIDs->fBulkAbsorbDistanceNtuple.fNtupleID); // finish row for Ntuple id = 4
    // G4cout << "Distance Travelled Before Bulk Absorption: " << distance << " mm" << G4endl;
    
    // Retrieve number of reflections
    G4int const photonIdx = track->GetTrackID();
    G4int const numReflections = fHitManager->GetReflections(photonIdx);
    fAnalysisManager->FillNtupleIColumn(
        fStepBulkAbsorbNtupleIDs->fBulkAbsorbReflectionsNtuple.fNtupleID, 
        fStepBulkAbsorbNtupleIDs->fBulkAbsorbReflectionsNtuple.fColumnID, 
        numReflections
    ); // ...
    fAnalysisManager->AddNtupleRow(fStepBulkAbsorbNtupleIDs->fBulkAbsorbReflectionsNtuple.fNtupleID); // ...
    
    return;
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
        fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fNtupleID,
        fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fXColumnID,
        x
    ); // NtupleID = 0, 0th column, x
    fAnalysisManager->FillNtupleDColumn(
        fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fNtupleID,
        fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fYColumnID,
        y
    ); // NtupleID = 0, 1st column, y
    fAnalysisManager->FillNtupleDColumn(
        fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fNtupleID,
        fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fZColumnID,
        z
    ); // NtupleID = 0, 2nd column, z
    // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
    
    // Mark this row as complete (for Ntuple with passed ID)
    fAnalysisManager->AddNtupleRow(fStepDetectionNtupleIDs->fDetectionCoordsNtuple.fNtupleID); // Ntuple ID = 0
    
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
    
    // Get distance travelled by photon before detection (from creation to absorption in PC)
    G4double const distance = track->GetTrackLength();
    fAnalysisManager->FillNtupleDColumn(
        fStepDetectionNtupleIDs->fDetectionDistanceNtuple.fNtupleID,
        fStepDetectionNtupleIDs->fDetectionDistanceNtuple.fColumnID,
        distance
    ); // NtupleID = 3, column = 0, val = distance
    // analysisManager->AddNtupleRow(3); // finish row for NtupleID = 3 // NOTE: OMIT THIS IF USING SAME NTUPLE FOR ALL TRACK DATA
    // G4cout << "Distance Travelled Before Detection: " << distance << " mm" << G4endl;
    
    // Get time of flight information (local time gives time since photon birth until now, i.e. birth to detection)
    G4double const time = track->GetLocalTime();
    fAnalysisManager->FillNtupleDColumn(
        fStepDetectionNtupleIDs->fDetectionTimeOfFlightNtuple.fNtupleID,
        fStepDetectionNtupleIDs->fDetectionTimeOfFlightNtuple.fColumnID,
        time
    ); // NtupleID = 3, column = 1, val = time
    fAnalysisManager->AddNtupleRow(fStepDetectionNtupleIDs->fDetectionTimeOfFlightNtuple.fNtupleID); // Finish row for NtupleID = 3 // NOTE: NOW THAT ALL TRACK DATA WRITTEN, ADD THE ROW
    // NOTE: This method loses relative timing between different photons if they 
    // were created at different points along the primary particles track
    
    // Retrieve number of reflections
    G4int const photonIdx = track->GetTrackID();
    G4int const numReflections = fHitManager->GetReflections(photonIdx);
    fAnalysisManager->FillNtupleIColumn(
        fStepDetectionNtupleIDs->fDetectionReflectionsNtuple.fNtupleID,
        fStepDetectionNtupleIDs->fDetectionReflectionsNtuple.fColumnID,
        numReflections
    ); // TODO: Maybe clump all "detection" branches together in one ntuple
    fAnalysisManager->AddNtupleRow(fStepDetectionNtupleIDs->fDetectionReflectionsNtuple.fNtupleID);
    
    // TEST
    // Calculate angle of incidence for detected photon
    // endPoint->GetTouchable()->GetVolume()->GetObjectRotation();
    // G4ThreeVector const trans = endPoint->GetTouchable()->GetVolume()->GetObjectTranslation();
    // trans.angle();
    // trans.
    // TEST
    
    // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
    // producing a new row (linked to the event ID) for each interaction,
    // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
    
    return;
    
    // NOTE: Likely just make this a method of analysis class (as absorption xyz very similar)
}

/*
 * Handle optical photon being absorbed without detection at a boundary (i.e., reflector or photocathode)
 * 
 * NOTE: Takes readonly pointer (pointer to const)
 */
void SteppingAnalysis::HandleBoundaryAbsorb(G4StepPoint const* endPoint) const {
    // TODO: Can get absorption volume via same process as above,
    // counts absorptions in crystal vs reflector, vs photocathode
    
    // Get (x, y, z) coordinates at point where optical photon absorbed by reflector or photocathode
    G4ThreeVector const absorptionPosition = endPoint->GetPosition();
    double const x = absorptionPosition[0];
    double const y = absorptionPosition[1];
    double const z = absorptionPosition[2];
    
    // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
    fAnalysisManager->FillNtupleDColumn(
        fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fNtupleID,
        fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fXColumnID,
        x
    ); // NtupleID = 1, 3rd column, x
    fAnalysisManager->FillNtupleDColumn(
        fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fNtupleID,
        fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fYColumnID,
        y
    ); // NtupleID = 1, 4th column, y
    fAnalysisManager->FillNtupleDColumn(
        fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fNtupleID,
        fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fZColumnID,
        z
    ); // NtupleID = 1, 5th column, z
    // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
    
    // Mark this row as complete (for Ntuple with passed ID)
    fAnalysisManager->AddNtupleRow(fStepBoundaryAbsorbNtupleIDs->fBoundaryAbsorbCoordsNtuple.fNtupleID); // Ntuple ID = 1
    
    // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
    // producing a new row (linked to the event ID) for each interaction,
    // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
    
    return;
}
