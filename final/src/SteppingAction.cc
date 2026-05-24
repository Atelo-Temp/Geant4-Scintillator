// User classes
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include <G4TrackStatus.hh>

// G4 Lib
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessVector.hh"
#include "G4StepStatus.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "G4AnalysisManager.hh"

// Random seeding
#include "G4RandomTools.hh"

// Step handler, will excute on each step
void SteppingAction::UserSteppingAction(const G4Step* step) {    
    // Get the track object for the current step
    G4Track* track = step->GetTrack();
    
    // TODO: If particle is gamma (for escape x-rays specifically)
    // Check pre and post step touchable logic volume
    // If pre step volume is crystal, and post step is not crystal (reflector, enclosure, air, etc)
    // Get kinetic energy of particle
    // Flag it as an escape x-ray
    // Add to running total escaped energy tracker
    
    // If particle is not an optical photon, break
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
        return;
    }
    
    // Find the boundary process only once (cache the pointer)
    if (fBoundary == nullptr) {
        FindBoundary(track); // this->FindBoundary(track);
    }
 
    // Count one optical photon (for total generated, not absorbed, detected, etc)
    // NOTE: Without if clause, when a photon reflects it will be counted twice,
    // could subtract interactions from this value, but this probably a cleaner way
    if (track->GetCurrentStepNumber() == 1) fEventAction->CountPhoton();
    
    // Get the post step point object for the particle
    G4StepPoint* endPoint = step->GetPostStepPoint();
    
    
    
//     // >>>>> TEST: #1
//     if ((track->GetTrackStatus() == fStopAndKill) && (endPoint->GetStepStatus() != fGeomBoundary)) {
//         fEventAction->CountBulkAbsorption();
//         
//         // G4String volume = endPoint->GetTouchable()->GetVolume()->GetName();
//         // std::string volume = endPoint->GetTouchable()->GetVolume()->GetName();
//         // fEventAction->CountBulkAbsorption(volume); // 
//         
//         // G4cout << "BULK ABSORPTION IN MEDIUM: " << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Scintillator"
//         
//         // if (endPoint->GetTouchable()->GetVolume()->GetName() != "Scintillator") {
//             // G4cout << "BULK ABSORPTION IN MEDIUM: " << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // ....
//         // }
//         
//         if (endPoint->GetTouchable()->GetVolume()->GetName() == "Scintillator") {
//             // Get distance travelled by photon before bulk absorption
//             G4double distance = track->GetTrackLength();
//             
//             // G4cout << "Distance Travelled Before Bulk Absorption: " << distance << " mm" << G4endl;
//             G4cout << "Distance Travelled Before Bulk Absorption: " << (distance / 10) << " cm" << G4endl;
//         }
//     }
//     // TEST
//     
//     // >>>>> TEST: #2
//     auto const process = endPoint->GetProcessDefinedStep();
//     if (process && (process->GetProcessName() == "OpAbsorption")) {
//         fEventAction->CountKill();
//     }
//     // TEST
    
    
    
    // If post step point not at a defined geometric boundary, break
    if (endPoint->GetStepStatus() != fGeomBoundary) return;
    
    // If at a boundary ...
    G4OpBoundaryProcessStatus boundaryStatus = fBoundary->GetStatus();
    
    // TODO: Timing window
    // if (!initialPEtime) initialPEtime == ...
    // currPEtime = intialPEtime - globalTime
    
    // TODO: Angular QE
    // Compute incidence angle of PE
    // Weight detection probability
    
    // NOTE: Type:
    // G4OpBoundaryProcessStatus::
    // to see all available boundary statuses
    
    // This assumes that the volume causing detection is the photocathode
    // as it is the only volume with non-zero efficiency
    if (boundaryStatus == Detection) {
        // Photon is absorbed but status was detection, manually track it in event tally
        fEventAction->CountDetectedPhoton();
        
        // TODO: May be worth double checking the boundary is the photocathode
        // although since its only one with efficiency vector, it will be
        
        // G4cout << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Scoring"
        // auto x = endPoint->GetTouchable()->GetVolume()->GetName();
        // if (x != "Photocathode") G4cout << x << G4endl;
        
        // if (endPoint->GetTouchable()->GetVolume() == fScoringVolume)
        // NOTE: Get fScoringVolume via public method on DetectorConstruction
        
        // Get (x, y, z) coordinates at point where optical photon detected by photocathode
        G4ThreeVector detectionPosition = endPoint->GetPosition();
        double x = detectionPosition[0];
        double y = detectionPosition[1];
        double z = detectionPosition[2];
        
        // TEST: Get the global time (TODO: For timing window)
        // auto detectTime = endPoint->GetGlobalTime();
        
        // Get pointer to analysis manager singleton
        auto analysisManager = G4AnalysisManager::Instance();
        
        // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
        analysisManager->FillNtupleDColumn(0, 0, x); // id, 0th column, x
        analysisManager->FillNtupleDColumn(0, 1, y); // id, 1st column, y
        analysisManager->FillNtupleDColumn(0, 2, z); // id, 2nd column, z
        // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
        
        // Mark this row as complete (for Ntuple with passed ID)
        analysisManager->AddNtupleRow(0); // Ntuple ID = 0
        
        // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
        // producing a new row (linked to the event ID) for each interaction,
        // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
        
        // NOTE: Likely just make this a method of analysis class (as absorption xyz very similar)
    } 
    // ...
    else if (boundaryStatus == Absorption) {
        // Photon was absorbed without detection
        fEventAction->CountAbsorbedPhoton();
        
        // TODO: Can get absorption volume via same process as above,
        // counts absorptions in crystal vs reflector, vs photocathode
        
        // Get (x, y, z) coordinates at point where optical photon absorbed by reflector or photocathode
        G4ThreeVector absorptionPosition = endPoint->GetPosition();
        double x = absorptionPosition[0];
        double y = absorptionPosition[1];
        double z = absorptionPosition[2];
        
        // Get pointer to analysis manager singleton
        auto analysisManager = G4AnalysisManager::Instance();
        
        // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
        analysisManager->FillNtupleDColumn(0, 3, x); // id, 3rd column, x
        analysisManager->FillNtupleDColumn(0, 4, y); // id, 4th column, y
        analysisManager->FillNtupleDColumn(0, 5, z); // id, 5th column, z
        // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
        
        // Mark this row as complete (for Ntuple with passed ID)
        analysisManager->AddNtupleRow(0); // Ntuple ID = 0
        
        // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
        // producing a new row (linked to the event ID) for each interaction,
        // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
    } 
    // TODO: check no loss via lack of rindex
    else if (boundaryStatus == G4OpBoundaryProcessStatus::NoRINDEX) {
        G4cout << G4endl << "OPTICAL PHOTON LOST TO LACK OF RINDEX" << G4endl;
        G4cout << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Reflector"
        
        fEventAction->CountLostPhoton();
        
        // TODO:
        // std::string volume = endPoint->GetTouchable()->GetVolume()->GetName();
        // fEventAction->CountLostPhoton(volume);
    }
    // NOTE: Is the photon actually being killed though
    
    // TODO:
    // else if (boundaryStatus == LambertianReflection) {}, etc ??
    
    // TODO: Maybe switch case here ^
    // Can also do xyz of reflection position, etc
}

// Find the boundary process and assign it to the class property "fBoundary"
// NOTE: Is there a cleaner way to do this?
void SteppingAction::FindBoundary(G4Track* track) {
    // Get pointer to the process list (for optical photons)
    G4ProcessVector* pv = track->GetDefinition()->GetProcessManager()->GetProcessList();
    
    // Iterate over list of process objects
    for (int i = 0; i < pv->size(); ++i) {
        // Pointer to current non-null process
        G4VProcess* process = (*pv)[i]; 
        // NOTE: Since "pv" is a pointer to a collection, rather than a direct object,
        // it needs to be dereferenced first (*pv), to reveal the actual object in memory,
        // so that processes can be accessed via index
        
        // Prevent attempted access to non-existent method
        if (process == nullptr) continue;
        
        // If the boundary process is found
        // NOTE: i.e. "Transportation", "OpAbsorption", "OpRayleigh", "OpMieHG", "OpBoundary"
        if (process->GetProcessName() == "OpBoundary") {
            // Cache the pointer to the boundary process
            fBoundary = dynamic_cast<G4OpBoundaryProcess*>(process);
            
            // End iteration when found
            break;
        }
    }
    // TODO: Error handling? Return 1/0 if fBoundary not set/set
}
