// ...
#include "EventAction.hh"
#include "G4Event.hh"

#include <G4ios.hh>


void EventAction::BeginOfEventAction(const G4Event*) {
    // Reset counters between events
    totalPhotons = 0.;
    detectedPhotons = 0.;
    absorbedPhotons = 0;
}

void EventAction::EndOfEventAction(const G4Event*) {
    // Print to stdout
    G4cout << G4endl << "Optical Photons Generated: " << totalPhotons << G4endl;
    
    G4cout << G4endl << "Optical Photons Detected: " << detectedPhotons << G4endl;
    G4cout << G4endl << "Percent Detected: " << (detectedPhotons / totalPhotons) * 100 << G4endl;
    
    G4cout << G4endl << "Optical Photons Absorbed: " << absorbedPhotons << G4endl;
    G4cout << G4endl << "Percent Absorbed: " << (absorbedPhotons / totalPhotons) * 100 << G4endl;
    
    // TODO: Could get photon (x, y, z) on detection AND absorption
    // will get 3D heatmap of where detection on photocathode occuring,
    // but also will see where in the detector photons being lost to absorption
}
