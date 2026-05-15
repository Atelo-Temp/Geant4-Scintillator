#ifndef MyDetectorConstruction_HH
#define MyDetectorConstruction_HH

#include "G4VUserDetectorConstruction.hh"

// #include "G4LogicalVolume.hh"
// #include "G4VPhysicalVolume.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

// namespace GEOMETRY {

// Inherit from the supplied abstract class (the colon)
class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        // DetectorConstruction();
        // virtual ~DetectorConstruction(); 
        // destructor (~) has to be virtual function to override the method in abstract class

        // This is the way its done in examples ...
        DetectorConstruction() = default;
        ~DetectorConstruction() override = default; 

        // virtual G4VPhysicalVolume *Construction();
        // G4VPhysicalVolume *Construction();
        G4VPhysicalVolume* Construct() override;
        // ^ likewise ...
        
        // Public method to get stored pointer to scoring region
        // G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; };

    // protected:
        // Local storage to access scoring volume outside the scope of "Construct()"
        // G4LogicalVolume* fScoringVolume = nullptr; // logical volume

        // Construct sensitive detector (SD) or field (electronic, magnetic, etc..)
        // void ConstructSDandField() override; // Will ignore field for now
};

// }

#endif
