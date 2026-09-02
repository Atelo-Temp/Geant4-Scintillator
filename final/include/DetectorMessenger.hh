#ifndef MyDetectorMessenger_HH
#define MyDetectorMessenger_HH

// G4 lib
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"

// C lib
#include <unordered_map>

// forward declarations
class DetectorConstruction;
class G4UIcmdWithADoubleAndUnit;

/*
 * ...
 */
enum class CommandName {
    crystal_diameter,
    reflector_thickness_axial,
    reflector_thickness_radial,
    // reflector_material, // TODO
    enclosure_thickness_axial,
    enclosure_thickness_radial,
    enclosure_material_axial,
    enclosure_material_radial,
    source_detector_distance,
    isotope,
    source_window_material,
    source_window_thickness
};

/*
 * ...
 */
class DetectorMessenger : public G4UImessenger {
    public:
        // Constructor
        DetectorMessenger(DetectorConstruction* detCon);
        
        // Destructor
        ~DetectorMessenger() override;
        
        // ...
        void SetNewValue(G4UIcommand* cmd, G4String value) override;
        
    private:
        // ...
        DetectorConstruction* fDetectorConstruction;
        
        // std::unordered_map<std::string, G4UIcommand*> fCommands;
        // std::unordered_map<G4UIcommand*, std::string> fCommands;
        // std::unordered_set<G4UIcommand*> fCommands;
        std::unordered_map<G4UIcommand*, CommandName> fCommands;
};

#endif
