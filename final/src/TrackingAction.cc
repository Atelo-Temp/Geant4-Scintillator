// User classes
#include "TrackingAction.hh"
#include "UserTrackInformation.hh"

// G4 lib
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4TrackingManager.hh"

/*
 * Execute at the start of the track
 */
void TrackingAction::PreUserTrackingAction(G4Track const* track) {
    // Only for optical photons   
    if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
        // Assign custom track info object
        // track->SetUserInformation(new UserTrackInformation());
        // ...
        fpTrackingManager->SetUserTrackInformation(new UserTrackInformation());
        // NOTE: Achieves the same thing as track->SetUserInformation()
    }
}
