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
class DetectorMaterials;
class SourceMaterials;
class DetectorGeometry;
class SourceGeometry;

// /*
//  * ...
//  */
// struct DetectorGeometry {
//     // ...
//     G4VPhysicalVolume* crystalPhys;
//     G4VPhysicalVolume* reflectorPhys;
//     G4VPhysicalVolume* enclosurePhys;
//     G4VPhysicalVolume* greasePhys;
//     G4VPhysicalVolume* windowPhys;
//     G4VPhysicalVolume* photocathodePhys;
//     G4VPhysicalVolume* sealPhys;
//     
//     // ...
//     G4double detectorFaceZ;
//     G4double detectorX;
// };

/*
 * ...
 * 
 * Inherits from the abstract base class "G4VUserDetectorConstruction" (denoted by the colon)
 * 
 * NOTE: Mandatory user initialisation class (no default behaviour provided)
 * 
 * 
 * TODO: Ordering of members and methods has gotten bit loose across the board in recent updates, enforce constant style
 */
class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        // Default constructor
        DetectorConstruction();
        
        // Default destructor
        ~DetectorConstruction() override;

        // Main entry point, called by run manager to construct detector volume
        G4VPhysicalVolume* Construct() override;
        
        // Public method to get stored pointer to scoring region
        // G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; };
        
        
        // TEST
        G4LogicalVolume* GetSourceVolume() const { return fSourceVolume; }; // NOTE: TEMP UNTIL PLACING PHYSICAL SOURCE VOL
        G4ThreeVector GetSourceOrigin() const { return fSourceCoords; }; // NOTE: TEMP UNTIL PLACING PHYSICAL SOURCE VOL
        // G4VPhysicalVolume* GetSourceVolume() const { return fSourceVolume; };
        
        
        // TEST - Methods to be exposed by and called by detector messenger
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
        
        // ...
        DetectorMessenger* fDetectorMessenger = nullptr;
        
        // Flag for checking geometry overlap
        G4bool fCheckOverlaps = true;
        
        // TEST
        DetectorMaterials* fDetectorMaterials = nullptr;
        SourceMaterials* fSourceMaterials = nullptr;
        
        // ...
        G4VPhysicalVolume* BuildWorld();
        G4double BuildTable(G4LogicalVolume* worldLog);
        
        // ...
        DetectorGeometry* fDetectorGeometry = nullptr;
        SourceGeometry* fSourceGeometry = nullptr;
};

#endif
