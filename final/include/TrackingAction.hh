#ifndef MyTrackingAction_HH
#define MyTrackingAction_HH

#include "G4UserTrackingAction.hh"

/*
 * This class represents actions taken by the user at the start/end point of processing a track
 * 
 * NOTE: Extends "G4UserTrackingAction"
 */
class TrackingAction : public G4UserTrackingAction {
    public:
        // Constructor
        TrackingAction() = default;
        
        // Destructor
        ~TrackingAction() override = default;
        
        // Execute at the start of the track
        void PreUserTrackingAction(G4Track const* track) override;
        
        // Execute at the end of the track
        // void PostUserTrackingAction(G4Track const* track) override;
        // NOTE: Unused
};

#endif
