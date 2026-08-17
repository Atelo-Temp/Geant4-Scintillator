#ifndef MyDetectorConstruction_HH
#define MyDetectorConstruction_HH

// G4 lib
#include "G4VUserDetectorConstruction.hh"
// #include "G4LogicalVolume.hh"
// #include "G4VPhysicalVolume.hh"

// Forward declarations
class G4VPhysicalVolume;
// class G4LogicalVolume;

/*
 * ...
 * 
 * Inherits from the abstract base class "G4VUserDetectorConstruction" (denoted by the colon)
 * 
 * NOTE: Mandatory user initialisation class (no default behaviour provided)
 */
class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        // Default constructor
        DetectorConstruction() = default;
        
        // Default destructor
        ~DetectorConstruction() override = default;

        // Main entry point, called by run manager to construct detector volume
        G4VPhysicalVolume* Construct() override;
        
        // virtual G4VPhysicalVolume *Construction();
        // G4VPhysicalVolume *Construction();
        
        // Public method to get stored pointer to scoring region
        // G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; };

    // private:
        // Local storage to access scoring volume outside the scope of "Construct()"
        // G4LogicalVolume* fScoringVolume = nullptr; // logical volume

        // Construct sensitive detector (SD) or field (electronic, magnetic, etc..)
        // void ConstructSDandField() override; // Will ignore field for now
};

#endif
