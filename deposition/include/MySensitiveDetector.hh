#ifndef MySensitiveDetector_HH
#define MySensitiveDetector_HH

// The base class to inherit from
#include "G4VSensitiveDetector.hh"

// Class interface, inheriting from G4 base class
class SensitiveDetector : public G4VSensitiveDetector {
    public:
        // Constructor
        SensitiveDetector(G4String);

        // Destructor
        ~SensitiveDetector() override = default;

    private:
        // Total energy depostied per event in sensitive detector
        G4double fTotalEdep;

        // Always called by Geant4 when a new event starts
        void Initialize(G4HCofThisEvent*) override; // Takes object as param
        // NOTE: HC = Hit collection

        // ^ Not mandatory for this detector (will be keeping it somewhat simple)
        // Only important for analysis and reconstruction within G4, or adding electronic noise
        // But must be included regardless for functionality

        // Always called on completion of an event
        void EndOfEvent(G4HCofThisEvent*) override;

        // Main function which handles whatever happens while a particle is inside the detector
        // For every step inside of the detector, information can be extracted
        G4bool ProcessHits(G4Step*, G4TouchableHistory*) override; // Boolean return value
        // NOTE: Need access to the step object
        // Touchable history gives access to the volumes (not needed for now)
};

#endif
