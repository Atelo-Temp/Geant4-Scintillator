// User lib
#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

// G4 lib
// #include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

/*
 * Define commands at class construction
 */
DetectorMessenger::DetectorMessenger(DetectorConstruction* detCon) : fDetectorConstruction(detCon) {
    // Define new directories
    auto detectorDir = new G4UIdirectory("/experiment/");
    detectorDir->SetGuidance("Detector geometry and radioactive isotope control.");
    
    // Various commands for modifying detector geometry
    auto crystalSizeCmd = new G4UIcmdWithADoubleAndUnit("/experiment/detector/crystalSize", this);
    crystalSizeCmd->SetGuidance("Set the dimensions of the scintillator crystal.");
    crystalSizeCmd->SetParameterName("crystal-diameter", false);
    new G4UnitDefinition("inch", "in", "Length", 2.54 * cm);
    crystalSizeCmd->SetDefaultUnit("inch");
    
    // crystalSizeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    // crystalSizeCmd->SetToBeBroadcasted(false);
    
    fCommands[crystalSizeCmd] = "crystal_diameter";
    
    // ...
    auto sourceDetectorDistCmd = new G4UIcmdWithADoubleAndUnit("/experiment/source/sourceDetectorDist", this);
    sourceDetectorDistCmd->SetGuidance("Set the distance between the face of the detector and the source.");
    sourceDetectorDistCmd->SetParameterName("source-detector-distance", false);
    sourceDetectorDistCmd->SetDefaultUnit("cm");
    // sourceDetectorDistCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    // sourceDetectorDistCmd->SetToBeBroadcasted(false);
    
    fCommands[sourceDetectorDistCmd] = "source_detector_distance";
    
    // ...
    auto sourceCmd = new G4UIcmdWithAString("/experiment/source/isotope", this);
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
    
    G4cout << "SETTING VALUE: " << val << G4endl;
    
    // ...
    G4String const command = found->second;
    
    // ...
    if (command == "crystal_diameter") {
        // ..
        auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
        G4double const value = casted->GetNewDoubleValue(val);
        fDetectorConstruction->SetCrystalDiameter(value);
        
        G4cout << "CRYSTAL DIAMETER SET: " << value << G4endl;
    }
    else if (command == "source_detector_distance") {
        // ..
        // auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
        auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(found->first);
        G4double const value = casted->GetNewDoubleValue(val);
        fDetectorConstruction->SetSourceDetectorDistance(value);
        
        G4cout << "SOURCE DETECTOR DISTANCE SET" << G4endl;
    }
    else if (command == "isotope") {
        // ...
        fDetectorConstruction->SetSource(val);
        
        G4cout << "ISOTOPE SET" << G4endl;
    }
}
