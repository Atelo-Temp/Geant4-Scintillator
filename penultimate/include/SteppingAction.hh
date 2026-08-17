#ifndef MySteppingAction_HH
#define MySteppingAction_HH

// G4 Lib
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
// #include "G4LogicalVolume.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpAbsorption.hh"
#include "G4OpticalPhoton.hh"

// Forward declarations
class EventAction;
class SteppingAnalysis;
class HitManager;
// TODO: Consider forward declaration

/*
 * Particle step handler
 * 
 * NOTE: Extends abstract base class "G4UserSteppingAction" (preserving access modifiers)
 * 
 * TODO: Should i delete fEventAction && fBoundary in custom destructor ??
 */
class SteppingAction : public G4UserSteppingAction {
    public:
        // Constructor
        SteppingAction(EventAction* eventAction);
        
        // Destructor
        ~SteppingAction() override;
        
        // Intra-event step handler (takes pointer to step object)
        void UserSteppingAction(G4Step const* step) override;
        
        // Find and cache optical photon absorption and boundary processes
        void CacheProcesses(G4Track const* track);
        
    private:
        // Pointer to event object
        EventAction const* fEventAction = nullptr; // NOTE: Readonly
        
        // Pointer to optical photon particle definition
        G4OpticalPhoton const* fOpticalPhotonDefinition = nullptr; // NOTE: Readonly
        
        // Pointer to scoring region
        // G4LogicalVolume* fScoringVolume = nullptr;
        
        // Pointer to boundary process
        G4OpBoundaryProcess const* fBoundary = nullptr; // NOTE: Readonly
        
        // Pointer to bulk absorption process
        G4OpAbsorption const* fAbsorb = nullptr; // NOTE: Readonly
        
        // Pointer to hit manager instance
        HitManager* fHitManager = nullptr;
        // TODO: either pass hit manager into both the event action and stepping action constructors in action init
        // or make a getter for pointer on event action class, and fetch it once at construction of stepping action
        
        // Pointer to ntuple output handler
        SteppingAnalysis const* fSteppingAnalysis = nullptr; // NOTE: Readonly
};

#endif
