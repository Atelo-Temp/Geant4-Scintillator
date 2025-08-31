#ifndef MyDetectorConstruction_HH
#define MyDetectorConstruction_HH

#include "G4VUserDetectorConstruction.hh"

// #include "G4NistManager.hh"
// #include "G4Box.hh"
// #include "G4Tubs.hh"
// #include "G4LogicalVolume.hh"
// #include "G4VPhysicalVolume.hh"
// #include "G4PVPlacement.hh"
// #include "G4SystemOfUnits.hh"

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

    // protected:
};

// }

#endif
