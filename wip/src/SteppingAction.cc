// ...
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include <G4OpBoundaryProcess.hh>
#include <G4ProcessVector.hh>
#include <G4StepStatus.hh>
#include <G4VProcess.hh>
#include <G4ios.hh>
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"

#include "G4OpticalPhoton.hh"

#include "G4Step.hh"


// Simple count all particles matching optical photon definition implementation
// void SteppingAction::UserSteppingAction(const G4Step* step) {
//     // ...
//     auto track = step->GetTrack();
//     
//     // If particle is not optical photon, break
//     if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
//         return;
//     }
//     
//     // Count one
//     fEventAction->countPhoton();
// }


// Count optical photons that take a step inside scoring region (NOTE: May count multiple steps, and miss photons which pass through)
// void SteppingAction::UserSteppingAction(const G4Step* step) {
    // If scoring volume is nullptr
//     if (!fScoringVolume) {
//         // Get a pointer to the detector construction registered with the run manager
//         
//         // auto detConst = G4RunManager::GetRunManager()->GetUserDetectorConstruction();
//         // const auto detConst = G4RunManager::GetRunManager()->GetUserDetectorConstruction();
//         
//         // NOTE: I have no idea why its implemeted this way and not one of the above
//         const auto detConst = static_cast<const DetectorConstruction*>(
//             G4RunManager::GetRunManager()->GetUserDetectorConstruction()
//         );
//         
//         // Assign scoring volume pointer to class property
//         fScoringVolume = detConst->GetScoringVolume();
//     }
    
    // ...
    // auto track = step->GetTrack();
    
    // If particle is not optical photon, break
    // if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
    //     return;
    // }
    
    // Get volume of current step
    // G4LogicalVolume* currVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    
    // // Check if we are in the scoring volume
    // if (currVolume != fScoringVolume) {
    //     return;
    // }
    
    // Count one
    // fEventAction->countPhoton();
// }


// ...
void SteppingAction::UserSteppingAction(const G4Step* step) {
    // ...
    auto track = step->GetTrack();
    
    // If particle is not optical photon, break
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
        return;
    }
    
    
    // TEST
    // auto x = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    // if (x != "Transportation" && x != "OpAbsorption") G4cout << x << G4endl;
    // return;
    // ...
    
    
    // Find the boundary process only once (cache the pointer)
    // NOTE: Is there a cleaner way to do this?
    // NOTE: May wanna make this its own method, if this is cleanest way, and call if == nullptr
    if (fBoundary == nullptr) {
        // Get pointer to the process list (for optical photons)
        // auto pv = track->GetDefinition()->GetProcessManager()->GetProcessList();
        G4ProcessVector* pv = track->GetDefinition()->GetProcessManager()->GetProcessList();
        
        // Iterate over list of process objects
        for (int i = 0; i < pv->size(); ++i) {
            // Pointer to current non-null process
            // auto process = (*pv)[i];
            G4VProcess* process = (*pv)[i];
          
            // Prevent attempted access to non-existent method
            if (process == nullptr) continue;
            
            // TEST
            // G4cout << process->GetProcessName() << G4endl;
            // ...
            
            // If the boundary process is found
            // NOTE: i.e. "Transportation", "OpAbsorption", "OpRayleigh", "OpMieHG", "OpBoundary"
            if (process->GetProcessName() == "OpBoundary") {
                // Cache the pointer to the boundary process
                fBoundary = dynamic_cast<G4OpBoundaryProcess*>(process);
                
                // End iteration when found
                break;
            }
        }
    }

    // Count one optical photon (for total generated, not absorbed, detected, etc)
    fEventAction->CountPhoton();
    // TODO: If a photon reflects it will be counted twice !!!
    
    // Get the post step point object for the particle
    auto endPoint = step->GetPostStepPoint();
    
    // If post step point not at a defined geometric boundary, break
    if (endPoint->GetStepStatus() != fGeomBoundary) return;
    
    // If at a boundary ...
    auto boundaryStatus = fBoundary->GetStatus();
    
    // This assumes that the volume causing detection is the photocathode
    // as it is the only volume with non-zero efficiency
    if (boundaryStatus == Detection) {
        // Photon is absorbed but status was detection, manually track it in event tally
        fEventAction->CountDetectedPhoton();
        
        // TODO: Get (x, y, z) coordinates
    } else if (boundaryStatus == Absorption) {
        // Photon was absorbed without detection
        fEventAction->CountAbsorbedPhoton();
        
        // TODO: Get (x, y, z) coordinates
    }
    // TODO: Maybe switch case here ^
    
    // Check if photocathode boundary somehow ...
    
    // NOTE: Is there alternative approach to finding process in list ?
    // auto x = endPoint->GetProcessDefinedStep()->GetProcessName();
    // G4cout << x << G4endl;
}
