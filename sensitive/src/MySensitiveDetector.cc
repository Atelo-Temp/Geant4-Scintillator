// To calculate energy deposited in detector the sensitive detector is the easiest method
// NOTE: A scoring region can also be used

#include "MySensitiveDetector.hh"

// Constructor implementation (must explicitly initialize base class which doesnt have default constructor)
SensitiveDetector::SensitiveDetector(G4String name) : G4VSensitiveDetector(name) {
    // At construction, set energy deposited to zero
    fTotalEdep = 0.;
}

// Initialise method implementation
void SensitiveDetector::Initialize(G4HCofThisEvent*) {
    // At initialisation, reset energy deposited to zero (NOTE: Is this necessary w/ constructor? For between runs or something?)
    fTotalEdep = 0.; // NOTE: When a new event starts?
}

// Event completion implementation
void SensitiveDetector::EndOfEvent(G4HCofThisEvent*) {
    // At the end of each event write out total energy in terminal
    G4cout << "Deposited Energy: " << fTotalEdep << " MeV" << G4endl;
    // NOTE: G4cout is custom? stdout
    // NOTE: Standard unit in G4 is MeV
    // NOTE: G4endl = End line.
}

// Hit processing implementation
G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory* touchHist) {
    // Access the step and get the energy deposited
    G4double fEdep = aStep->GetTotalEnergyDeposit();

    // NOTE: Debug
    // G4cout << fEdep << G4endl;

    // If non-zero energy was deposited in the current step, add to total
    if (fEdep > 0) {
        fTotalEdep += fEdep;
    }
    // NOTE: In every step that the photon loses energy, the energy deposited in the scintillator
    // is added to the total energy deposited, which will be the final value taken out
    // at the end of each event

    // i.e. a compton scatter deposits 0.5 * 662 keV in the detector, through 3 seperate scatter events, before leaving the detector
    // eDep will get the energy deposited in a single scatter (each of the 3 will have a fraction of 0.5 * 662 keV)
    // totalEdep will get the cumulative energy deposited by the scatters (0.5 * 662 keV)
    // Much like how a scintillator detector in the lab works (compton region)

    // ..
    return true;
}
