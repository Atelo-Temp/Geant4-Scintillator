// User classes
#include "SteppingAnalysis.hh"
#include "ProgramState.hh"
#include "AnalysisRegistry.hh"

// G4 lib
#include "G4AnalysisManager.hh"

/*
 * Constructor
 * 
 * SteppingAction
 *             | 
 *             ├─ EventAction*
 *             ├─ EventAnalysis*
 *             └─ SteppingAnalysis <<<
 *                              | 
 *                              ├─ AnalysisRegistry*
 *                              └─ ProgramState*
 * 
 * NOTE: Owned by SteppingAction
 */
SteppingAnalysis::SteppingAnalysis() {
    // Cache pointer to analysis manager singleton
    fAnalysisManager = G4AnalysisManager::Instance();
    
    // auto& instance = ProgramState::GetInstance("CCC");
    // G4cout << "\n\n>>>>> STEP ANALYSIS:: " << instance.value() << "\n\n" << G4endl;
    
    ProgramState& instance = ProgramState::GetInstance(); // TODO: Class property
    
    AnalysisRegistry& registry = AnalysisRegistry::GetInstance();
}

/*
 * Track photons lost via bulk absorption in the crystal (and window/grease to a far lesser extent)
 * 
 * TODO: Eliminate hardcoded ntuple IDs
 * 
 * NOTE: Takes readonly pointers
 */
void SteppingAnalysis::HandleBulkAbsorb(G4Track const* track, EventAnalysis const* fEventAnalysis) {
    // ..
    // std::string volume = endPoint->GetTouchable()->GetVolume()->GetName();
    // G4cout << "BULK ABSORPTION IN MEDIUM: " << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Scintillator"
    
    // Get distance travelled by photon before bulk absorption
    G4double const distance = track->GetTrackLength();
    fAnalysisManager->FillNtupleDColumn(4, 0, distance); // id = 4, column = 0, value = distance travelled
    fAnalysisManager->AddNtupleRow(4); // finish row for Ntuple id = 4
    // G4cout << "Distance Travelled Before Bulk Absorption: " << distance << " mm" << G4endl;
    
    // Retrieve number of reflections
    // G4int const numReflections = userTrackInfo->GetReflections();
    G4int const photonIdx = track->GetTrackID();
    G4int const numReflections = fEventAnalysis->GetReflections(photonIdx);
    fAnalysisManager->FillNtupleIColumn(7, 0, numReflections); // ...
    fAnalysisManager->AddNtupleRow(7); // ...
    
    return;
}

/*
 * Handle optical photon being detected at a boundary (i.e., photocathode)
 * 
 * NOTE: Takes readonly pointers
 */
void SteppingAnalysis::HandleDetection(G4StepPoint const* endPoint, G4Track const* track, EventAnalysis const* fEventAnalysis) {
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
    
    // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
    fAnalysisManager->FillNtupleDColumn(0, 0, x); // NtupleID = 0, 0th column, x
    fAnalysisManager->FillNtupleDColumn(0, 1, y); // NtupleID = 0, 1st column, y
    fAnalysisManager->FillNtupleDColumn(0, 2, z); // NtupleID = 0, 2nd column, z
    // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
    
    // Mark this row as complete (for Ntuple with passed ID)
    fAnalysisManager->AddNtupleRow(0); // Ntuple ID = 0
    
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
    fAnalysisManager->FillNtupleDColumn(3, 0, distance); // NtupleID = 3, column = 0, val = distance
    // analysisManager->AddNtupleRow(3); // finish row for NtupleID = 3 // NOTE: OMIT THIS IF USING SAME NTUPLE FOR ALL TRACK DATA
    // G4cout << "Distance Travelled Before Detection: " << distance << " mm" << G4endl;
    
    // Get time of flight information (local time gives time since photon birth until now, i.e. birth to detection)
    G4double const time = track->GetLocalTime();
    fAnalysisManager->FillNtupleDColumn(3, 1, time); // NtupleID = 3, column = 1, val = time
    fAnalysisManager->AddNtupleRow(3); // Finish row for NtupleID = 3 // NOTE: NOW THAT ALL TRACK DATA WRITTEN, ADD THE ROW
    // NOTE: This method loses relative timing between different photons if they 
    // were created at different points along the primary particles track
    
    // Retrieve number of reflections
    G4int const photonIdx = track->GetTrackID();
    G4int const numReflections = fEventAnalysis->GetReflections(photonIdx);
    fAnalysisManager->FillNtupleIColumn(6, 0, numReflections); // TODO: Maybe clump all "detection" branches together in one ntuple
    fAnalysisManager->AddNtupleRow(6);
    
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
 * NOTE: Takes readonly pointer
 */
void SteppingAnalysis::HandleBoundaryAbsorb(G4StepPoint const* endPoint) {
    // TODO: Can get absorption volume via same process as above,
    // counts absorptions in crystal vs reflector, vs photocathode
    
    // Get (x, y, z) coordinates at point where optical photon absorbed by reflector or photocathode
    G4ThreeVector const absorptionPosition = endPoint->GetPosition();
    double const x = absorptionPosition[0];
    double const y = absorptionPosition[1];
    double const z = absorptionPosition[2];
    
    // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
    fAnalysisManager->FillNtupleDColumn(1, 0, x); // NtupleID = 1, 3rd column, x
    fAnalysisManager->FillNtupleDColumn(1, 1, y); // NtupleID = 1, 4th column, y
    fAnalysisManager->FillNtupleDColumn(1, 2, z); // NtupleID = 1, 5th column, z
    // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
    
    // Mark this row as complete (for Ntuple with passed ID)
    fAnalysisManager->AddNtupleRow(1); // Ntuple ID = 1
    
    // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
    // producing a new row (linked to the event ID) for each interaction,
    // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
    
    return;
}
