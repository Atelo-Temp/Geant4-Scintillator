// To calculate energy deposited in detector the sensitive detector is the easiest method
// NOTE: A scoring region can also be used

#include "MySensitiveDetector.hh"

#include "G4AnalysisManager.hh" // For histogramming
#include "G4RunManager.hh" // For event number (not mandatory)

#include "G4SystemOfUnits.hh"

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
    
    /////////////////
    // HISTOGRAMMING:
    /////////////////
    
    // Get a pointer to the analysis manager instance
    auto analysisManager = G4AnalysisManager::Instance();
    // TODO: Seems awfully inefficient to get a pointer every event ?
    
    // Fill the histogram (add a count to the appropriate energy bin)
    analysisManager->FillH1(0, fTotalEdep); // (id, value) only one histo, so id = 0
    // NOTE: On full 662 keV energy deposited will add to counts for that bin, etc
    
    // NOTE: If using multiple histograms for any reason (i.e. two detectors)
    // make sure to mark appropriate ID
}

// Hit processing implementation (intra-event steps)
G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory* touchHist) {
    /////////////////////
    // NTUPLE GENERATION:
    /////////////////////
    
    // Get the run manager instance, the current event object, and its ID (optional for histogramming)
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID(); // idk about this shite, looks bit dirty chaining like that
    
    // Get a pointer to the analysis manager instance
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Get the pre-step point from the step object
    G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
    // NOTE: There are pre & post step points, relating to the initial and final particle state for a given step
    // i.e. particle state before an interaction, and after ...
    // has methods such as; GetCharge(), GetMass(), GetMomentum(), etc
        
    // Get global time at time of interaction (step) (NOTE: Or is it start of step time?)
    G4double fGlobalTime = preStepPoint->GetGlobalTime();
    
    // Get (x, y, z), coordinates at point of interaction (NOTE: or is it start coordinates for that step?)
    G4ThreeVector posPhoton = preStepPoint->GetPosition();
    
    // Get the momentum vector
    G4ThreeVector momPhoton = preStepPoint->GetMomentum();
    
    // Convert to scalar magnitude (energy of the photon)
    G4double magPhoton = momPhoton.mag();
    
    // Calculate the wavelength (using energy -> wl conversion factor)
    G4double fWlen = ((1.239841939 * eV) / magPhoton) * 1E+03; // x10^3
    
    // Store this data in the nTuples (create a few rows)
    analysisManager->FillNtupleIColumn(0, 0, eventID); // 0th column
    analysisManager->FillNtupleDColumn(0, 1, posPhoton[0]); // 1st column, x
    analysisManager->FillNtupleDColumn(0, 2, posPhoton[1]); // 2nd column, y
    analysisManager->FillNtupleDColumn(0, 3, posPhoton[2]); // 3rd column, z
    analysisManager->FillNtupleDColumn(0, 4, fGlobalTime); // 4th column
    analysisManager->FillNtupleDColumn(0, 5, fWlen); // 5th column
    // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
    
    // Mark this row as complete
    analysisManager->AddNtupleRow(0);
    
    // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
    // producing a new row (linked to the event ID) for each interaction,
    // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
    
    //////////////////
    // ENERGY DEPOSIT:
    //////////////////
    
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
