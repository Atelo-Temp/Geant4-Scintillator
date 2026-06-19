#ifndef MySteppingAction_HH
#define MySteppingAction_HH

// User classes
#include "EventAction.hh"

// G4 Lib
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
// #include "G4LogicalVolume.hh"
#include "G4OpBoundaryProcess.hh"

// Forward declarations
// class EventAction;
// TODO: Consider forward declaration

/*
 * Particle step handler
 * 
 * TODO: Should i delete fEventAction && fBoundary in custom destructor ??
 */
class SteppingAction : public G4UserSteppingAction {
    public:
        // Constructor (takes pointer to event object)
        SteppingAction(EventAction* eventAction);
        
        // Destructor
        ~SteppingAction() override = default;
        
        // Intra-event step handler (takes pointer to step object)
        void UserSteppingAction(G4Step const* step) override;
        
        // Find optical photon boundary process
        void FindBoundary(G4Track* track);
        
        // ...
        void HandleBulkAbsorb(G4Track* track);
        
        // ...
        void HandleDetection(G4StepPoint* endPoint, G4Track* track);
        
        // ...
        void HandleBoundaryAbsorb(G4StepPoint* endPoint);
        
    private:
        // Pointer to event object
        EventAction* fEventAction = nullptr;
        
        // Pointer to scoring region
        // G4LogicalVolume* fScoringVolume = nullptr;
        
        // Pointer to boundary process
        G4OpBoundaryProcess* fBoundary = nullptr;
};

#endif
