#ifndef MyEventAction_HH
#define MyEventAction_HH

#include "G4UserEventAction.hh"

#include "G4Types.hh"

// Forward declaration
class RunAction;

// ...
class EventAction : public G4UserEventAction {
    public:
        // Constructor takes pointer to run object
        EventAction(RunAction* runAction);
        
        // Destructor
        ~EventAction() override = default;
        
        // Start of event handler
        void BeginOfEventAction(G4Event const *event) override;
        
        // End of event handler
        void EndOfEventAction(const G4Event* event) override;
        
        // Optical photon tracker
        void CountPhoton();
        
        // Photons incident upon the photocathode
        // ...
        
        // Photons detected by the photocathode
        void CountDetectedPhoton();
        
        // Photons absorbed without detection
        void CountAbsorbedPhoton();
        
    protected:
        // Pointer to current run
        RunAction* fRunAction = nullptr;
        
        // Optical photon tally
        G4int fTotalPhotons = 0;
        
        // Detected photon tally
        G4int fDetectedPhotons = 0;
        
        // Absorbed photon tally
        G4int fAbsorbedPhotons = 0;
};

#endif
