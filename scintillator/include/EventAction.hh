#ifndef MyEventAction_HH
#define MyEventAction_HH

#include "G4UserEventAction.hh"

#include "G4Types.hh"

// ...
class RunAction;

// ...
class EventAction : public G4UserEventAction {
    public:
        // Constructor takes pointer to run object
        // EventAction(RunAction* runAction);
        EventAction() = default;
        
        // Destructor
        ~EventAction() override = default;
        
        // Start of event handler
        void BeginOfEventAction(const G4Event* event) override;
        
        // End of event handler
        void EndOfEventAction(const G4Event* event) override;
        
        // Optical photon tracker
        void CountPhoton() { totalPhotons += 1; };
        
        // Photons incident upon the photocathode
        // ...
        
        // Photons detected by the photocathode
        void CountDetectedPhoton() { detectedPhotons += 1; };
        
        // Photons absorbed without detection
        void CountAbsorbedPhoton() { absorbedPhotons += 1; };
        
    protected:
        // Pointer to current run
        RunAction* runAction = nullptr;
        
        // Optical photon tally
        G4double totalPhotons = 0.;
        
        // Detected photon tally
        G4double detectedPhotons = 0.;
        
        // Absorbed photon tally
        G4double absorbedPhotons = 0.;
};

#endif
