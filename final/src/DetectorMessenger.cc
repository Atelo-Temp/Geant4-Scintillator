// User lib
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

// G4 lib
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"

/*
 * ...
 */
DetectorMessenger::DetectorMessenger(DetectorConstruction* detCon) : fDetectorConstruction(detCon) {
    // Define commands
    
    // Various commands for modifying detector geometry
    auto crystalSizeCmd = new G4UIcmdWithADouble("/detector/crystalSize", this);
    crystalSizeCmd->SetGuidance("Set the dimensions of the scintillator crystal.");
    crystalSizeCmd->SetParameterName("crystal_diameter", false);
    // crystalSizeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    // crystalSizeCmd->SetToBeBroadcasted(false);
    
    fCommands[crystalSizeCmd] = "crystal_diameter";
    
    auto sourceDetectorDistCmd = new G4UIcmdWithADouble("/source/sourceDetectorDist", this);
    sourceDetectorDistCmd->SetGuidance("Set the distance between the face of the detector and the source.");
    sourceDetectorDistCmd->SetParameterName("source_detector_distance", false);
    // sourceDetectorDistCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    // sourceDetectorDistCmd->SetToBeBroadcasted(false);
    
    fCommands[sourceDetectorDistCmd] = "source_detector_distance";
    
    auto sourceCmd = new G4UIcmdWithAString("/source/isotope", this);
    sourceCmd->SetGuidance("Select the radioactive isotope to use as the source.");
    sourceCmd->SetParameterName("isotope", false);
    // TODO: List supported sources
    
    fCommands[sourceCmd] = "isotope";
};

/*
 * ...
 */
DetectorMessenger::~DetectorMessenger() {
    // Delete commands    
    for (auto entry = fCommands.begin(); entry != fCommands.end(); entry++) {
        delete entry->first;
    }
}

/*
 * ...
 */
void DetectorMessenger::SetNewValue(G4UIcommand* cmd, G4String val) {
    // ...
    auto const found = fCommands.find(cmd);
    
    if (found == fCommands.end()) {
        G4cout << "Command not found." << G4endl;
        return;
    }
    
    // ...
    G4String const command = found->second;
    
    // ...
    if (command == "crystal_diameter") {
        // ..
        auto casted = static_cast<G4UIcmdWithADouble*>(cmd);
        G4double const value = casted->GetNewDoubleValue(val);
        fDetectorConstruction->SetCrystalDiameter(value);
    }
    else if (command == "source_detector_distance") {
        // ..
        auto casted = static_cast<G4UIcmdWithADouble*>(cmd);
        G4double const value = casted->GetNewDoubleValue(val);
        fDetectorConstruction->SetCrystalDiameter(value);
    }
    else if (command == "isotope") {
        // ...
        fDetectorConstruction->SetSource(val);
    }
}
