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
    
    // TODO: subdirs for: /detector/reflector/, /detector/enclosure/
    
    // Various commands for modifying detector geometry
    auto crystalSizeCmd = new G4UIcmdWithADoubleAndUnit("/experiment/detector/crystalSize", this);
    crystalSizeCmd->SetGuidance("Set the dimensions of the scintillator crystal.");
    crystalSizeCmd->SetParameterName("crystal_diameter", false);
    new G4UnitDefinition("inch", "in", "Length", 2.54 * cm);
    crystalSizeCmd->SetDefaultUnit("inch");
    // crystalSizeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    // crystalSizeCmd->SetToBeBroadcasted(false);
    
    fCommands[crystalSizeCmd] = CommandName::crystal_diameter;
    
    // REFLECTOR THICKNESS
    
    // ..
    auto reflectorThicknessAxialCmd = new G4UIcmdWithADoubleAndUnit("/experiment/detector/reflectorAxialThickness", this);
    reflectorThicknessAxialCmd->SetGuidance("Set the thickness of the reflector axially (at the radiation entrance window).");
    reflectorThicknessAxialCmd->SetParameterName("reflector_thickness_axial", false);
    reflectorThicknessAxialCmd->SetDefaultUnit("cm");
    
    fCommands[reflectorThicknessAxialCmd] = CommandName::reflector_thickness_axial;
    
    // ...
    auto reflectorThicknessRadialCmd = new G4UIcmdWithADoubleAndUnit("/experiment/detector/reflectorRadialThickness", this);
    reflectorThicknessRadialCmd->SetGuidance("Set the thickness of the reflector radially.");
    reflectorThicknessRadialCmd->SetParameterName("reflector_thickness_radial", false);
    reflectorThicknessRadialCmd->SetDefaultUnit("cm");
    
    fCommands[reflectorThicknessRadialCmd] = CommandName::reflector_thickness_radial;
    
    // REFLECTOR MATERIAL
    
    // TODO: Reflector density, reflector material
    // ...
    
    // REFLECTOR DENSITY
    
    // ...
    
    // ENCLOSURE THICKNESS
    
    // ..
    auto enclosureThicknessAxialCmd = new G4UIcmdWithADoubleAndUnit("/experiment/detector/enclosureAxialThickness", this);
    enclosureThicknessAxialCmd->SetGuidance("Set the thickness of the enclosure axially (at the radiation entrance window).");
    enclosureThicknessAxialCmd->SetParameterName("enclosure_thickness_axial", false);
    enclosureThicknessAxialCmd->SetDefaultUnit("cm");
    
    fCommands[enclosureThicknessAxialCmd] = CommandName::enclosure_thickness_axial;
    
    // ...
    auto enclosureThicknessRadialCmd = new G4UIcmdWithADoubleAndUnit("/experiment/detector/enclosureRadialThickness", this);
    enclosureThicknessRadialCmd->SetGuidance("Set the thickness of the enclosure radially.");
    enclosureThicknessRadialCmd->SetParameterName("enclosure_thickness_radial", false);
    enclosureThicknessRadialCmd->SetDefaultUnit("cm");
    
    fCommands[enclosureThicknessRadialCmd] = CommandName::enclosure_thickness_radial;
    
    // ENCLOSURE MATERIAL
    
    // ...
    auto enclosureMaterialAxialCmd = new G4UIcmdWithAString("/experiment/detector/enclosureAxialMaterial", this);
    enclosureMaterialAxialCmd->SetGuidance("Set the material of the enclosure axially (at the radiation entrance window).");
    enclosureMaterialAxialCmd->SetParameterName("enclosure_material_axial", false);
    enclosureMaterialAxialCmd->SetCandidates("Aluminium StainlessSteel"); // TODO: List supported sources
    
    fCommands[enclosureMaterialAxialCmd] = CommandName::enclosure_material_axial;
    
    // ...
    auto enclosureMaterialRadialCmd = new G4UIcmdWithAString("/experiment/detector/enclosureRadialMaterial", this);
    enclosureMaterialRadialCmd->SetGuidance("Set the material of the enclosure radially.");
    enclosureMaterialRadialCmd->SetParameterName("enclosure_material_radial", false);
    enclosureMaterialRadialCmd->SetCandidates("Aluminium StainlessSteel"); // TODO: List supported sources
    
    fCommands[enclosureMaterialRadialCmd] = CommandName::enclosure_material_radial;
    
    
    // SOURCE DETECTOR DISTANCE
    
    // ...
    auto sourceDetectorDistCmd = new G4UIcmdWithADoubleAndUnit("/experiment/source/sourceDetectorDist", this);
    sourceDetectorDistCmd->SetGuidance("Set the distance between the face of the detector and the source.");
    sourceDetectorDistCmd->SetParameterName("source_detector_distance", false);
    sourceDetectorDistCmd->SetDefaultUnit("cm");
    // sourceDetectorDistCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    // sourceDetectorDistCmd->SetToBeBroadcasted(false);
    
    fCommands[sourceDetectorDistCmd] = CommandName::source_detector_distance;
    
    // SOURCE ISOTOPE
    
    // ...
    auto sourceCmd = new G4UIcmdWithAString("/experiment/source/isotope", this);
    sourceCmd->SetGuidance("Select the radioactive isotope to use as the source.");
    sourceCmd->SetParameterName("isotope", false);
    sourceCmd->SetCandidates("137Cs 60Co 22Na 133Ba 152Eu 207Bi"); // TODO: List supported sources
    
    fCommands[sourceCmd] = CommandName::isotope;
    
    // SOURCE WINDOW MATERIAL
    
    // ...
    // ...
    auto sourceWindowMaterialCmd = new G4UIcmdWithAString("/experiment/source/sourceWindowMaterial", this);
    sourceWindowMaterialCmd->SetGuidance("Set the material of the source retainer window.");
    sourceWindowMaterialCmd->SetParameterName("source_window_material", false);
    sourceWindowMaterialCmd->SetCandidates("Mylar StainlessSteel"); // TODO: List supported sources
    
    fCommands[sourceWindowMaterialCmd] = CommandName::source_window_material;
    
    // SOURCE WINDOW THICKNESS
    
    // ...
    auto sourceWindowThicknessCmd = new G4UIcmdWithADoubleAndUnit("/experiment/source/sourceWindowThickness", this);
    sourceWindowThicknessCmd->SetGuidance("Set the thickness of the source retainer window.");
    sourceWindowThicknessCmd->SetParameterName("source_window_thickness", false);
    sourceWindowThicknessCmd->SetDefaultUnit("cm");
    sourceWindowThicknessCmd->SetRange("source_window_thickness >= 0. && source_window_thickness <= 0.15");
    
    fCommands[sourceWindowThicknessCmd] = CommandName::source_window_thickness;
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
 * 
 * TODO: Handle double commands in separate switch statement to remove repetition of casting
 */
void DetectorMessenger::SetNewValue(G4UIcommand* cmd, G4String val) {
    // ...
    auto const found = fCommands.find(cmd);
    
    // ...
    if (found == fCommands.end()) {
        G4cerr << "Error: Command not found." << G4endl;
        return;
    }
    
    // ...
    G4cout << "SETTING VALUE: " << val << G4endl;
    
    // ...
    CommandName const commandName = found->second;
    
    // ...
    switch (commandName) {
        case CommandName::crystal_diameter: {
            auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
            G4double const value = casted->GetNewDoubleValue(val);
            fDetectorConstruction->SetCrystalDiameter(value);
            G4cout << "CRYSTAL DIAMETER SET: " << value << G4endl;
            break;
        }
        case CommandName::reflector_thickness_axial: {
            auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
            G4double const value = casted->GetNewDoubleValue(val);
            fDetectorConstruction->SetAxialReflectorThickness(value);
            G4cout << "AXIAL REFLECTOR THICKNESS SET: " << value << G4endl;
            break;
        }
        case CommandName::reflector_thickness_radial: {
            auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
            G4double const value = casted->GetNewDoubleValue(val);
            fDetectorConstruction->SetRadialReflectorThickness(value);
            G4cout << "RADIAL REFLECTOR THICKNESS SET: " << value << G4endl;
            break;
        }
        // case CommandName::reflector_material {
        //     break;
        // }
        case CommandName::enclosure_thickness_axial: {
            auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
            G4double const value = casted->GetNewDoubleValue(val);
            fDetectorConstruction->SetAxialEnclosureThickness(value);
            G4cout << "AXIAL ENCLOSURE THICKNESS SET: " << value << G4endl;
            break;
        }
        case CommandName::enclosure_thickness_radial: {
            auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
            G4double const value = casted->GetNewDoubleValue(val);
            fDetectorConstruction->SetRadialEnclosureThickness(value);
            G4cout << "RADIAL ENCLOSURE THICKNESS SET: " << value << G4endl;
            break;
        }
        case CommandName::enclosure_material_axial: {
            fDetectorConstruction->SetAxialEnclosureMaterial(val);
            G4cout << "AXIAL ENCLOSURE MATERIAL SET: " << val << G4endl;
            break;
        }
        case CommandName::enclosure_material_radial: {
            fDetectorConstruction->SetRadialEnclosureMaterial(val);
            G4cout << "RADIAL ENCLOSURE MATERIAL SET: " << val << G4endl;
            break;
        }
        case CommandName::source_detector_distance: {
            // auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
            auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(found->first);
            G4double const value = casted->GetNewDoubleValue(val);
            fDetectorConstruction->SetSourceDetectorDistance(value);
            G4cout << "SOURCE DETECTOR DISTANCE SET: " << value << G4endl;
            break;
        }
        case CommandName::isotope: {
            fDetectorConstruction->SetSource(val);
            G4cout << "ISOTOPE SET: " << val << G4endl;
            break;
        }
        case CommandName::source_window_material: {
            fDetectorConstruction->SetSourceWindowMaterial(val);
            G4cout << "SOURCE WINDOW MATERIAL SET: " << val << G4endl;
            break;
        }
        case CommandName::source_window_thickness: {
            auto casted = static_cast<G4UIcmdWithADoubleAndUnit*>(cmd);
            G4double const value = casted->GetNewDoubleValue(val);
            fDetectorConstruction->SetSourceWindowThickness(value);
            G4cout << "SOURCE WINDOW THICKNESS SET: " << value << G4endl;
            break;
        }
        default:
            G4cerr << "Error: Command name invalid." << G4endl;
            break;
    }
}
