#ifndef MyDetectorConstruction_HH
#define MyDetectorConstruction_HH

// G4 lib
#include "G4VUserDetectorConstruction.hh"
// #include "G4LogicalVolume.hh"
// #include "G4VPhysicalVolume.hh"

#include "G4ThreeVector.hh" // NOTE: TEMP
#include "G4SystemOfUnits.hh"

// Forward declarations
class G4VPhysicalVolume;
// class G4LogicalVolume;
// class G4ThreeVector;
class DetectorMessenger;

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
        // DetectorConstruction() = default;
        DetectorConstruction();
        
        // Default destructor
        // ~DetectorConstruction() override = default;
        ~DetectorConstruction() override;

        // Main entry point, called by run manager to construct detector volume
        G4VPhysicalVolume* Construct() override;
        
        // virtual G4VPhysicalVolume *Construction();
        // G4VPhysicalVolume *Construction();
        
        // Public method to get stored pointer to scoring region
        // G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; };
        
        
        // TEST
        G4LogicalVolume* GetSourceVolume() const { return fSourceVolume; }; // NOTE: TEMP UNTIL PLACING PHYSICAL SOURCE VOL
        G4ThreeVector GetSourceOrigin() const { return fSourceCoords; }; // NOTE: TEMP UNTIL PLACING PHYSICAL SOURCE VOL
        // G4VPhysicalVolume* GetSourceVolume() const { return fSourceVolume; };
        
        
        // TEST
        void SetCrystalDiameter(G4double const diameterInInches);
        void SetSourceDetectorDistance(G4double const distance);
        void SetSource(G4String const isotope);

    private:
        // Local storage to access scoring volume outside the scope of "Construct()"
        // G4LogicalVolume* fScoringVolume = nullptr; // logical volume

        // Construct sensitive detector (SD) or field (electronic, magnetic, etc..)
        // void ConstructSDandField() override; // Will ignore field for now
        
        
        // TEST
        G4LogicalVolume* fSourceVolume = nullptr; // NOTE: TEMP UNTIL PLACING PHYSICAL SOURCE VOL
        G4ThreeVector fSourceCoords; // NOTE: TEMP UNTIL PLACING PHYSICAL SOURCE VOL
        
        // G4VPhysicalVolume* fSourceVolume = nullptr;
        
        // TEST
        
        G4double fCrystalDiameter = 3. * (2.54 * cm); // 3 inches default (unit doesnt exist, will be converted in .cc file though)
        G4double fSourceDetectorDistance = 3. * cm;
        G4String fSource = "137Cs";
        
        
        // 
        DetectorMessenger* fDetectorMessenger;
};

#endif
