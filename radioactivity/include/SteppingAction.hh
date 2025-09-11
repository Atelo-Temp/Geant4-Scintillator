#ifndef MySteppingAction_HH
#define MySteppingAction_HH

#include "EventAction.hh"

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4LogicalVolume.hh"
#include "G4OpBoundaryProcess.hh"

// Defined in "EventAction.hh"
// class EventAction;


// Step handler
class SteppingAction : public G4UserSteppingAction {
    public:
        // Constructor (takes pointer to event object)
        SteppingAction(EventAction* eventAction) { fEventAction = eventAction; };
        
        // Destructor
        ~SteppingAction() override = default;
        
        // Intra-event step handler (takes pointer to step object)
        void UserSteppingAction(const G4Step*) override;
        
    private:
        // Pointer to event object
        EventAction* fEventAction = nullptr;
        
        // Pointer to scoring region
        G4LogicalVolume* fScoringVolume = nullptr;
        
        // Pointer to boundary process
        G4OpBoundaryProcess* fBoundary = nullptr;
};

#endif
