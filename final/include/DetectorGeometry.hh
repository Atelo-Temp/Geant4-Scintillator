#ifndef MyDetectorGeometry_HH
#define MyDetectorGeometry_HH

// User lib
#include "DetectorMaterials.hh"

// G4 lib
#include "G4Types.hh"
#include "G4SystemOfUnits.hh"

// Forward declarations
// class DetectorMaterials;
class G4VPhysicalVolume;
class G4LogicalVolume;

/*
 * ...
 */
struct DetectorBuild {    
    // ...
    G4double detectorFaceZ;
    G4double detectorX;
};

/*
 * ...
 */
// struct DetectorVolumes {
        // // ...
    // G4VPhysicalVolume* crystalPhys;
    // G4VPhysicalVolume* reflectorPhys;
    // G4VPhysicalVolume* enclosurePhys;
    // G4VPhysicalVolume* greasePhys;
    // G4VPhysicalVolume* windowPhys;
    // G4VPhysicalVolume* photocathodePhys;
    // G4VPhysicalVolume* sealPhys;
// };

/*
 * ...
 * 
 * TODO: Ordering of members and methods has gotten bit loose across the board in recent updates, enforce constant style
 */
class DetectorGeometry {
    public:
        // ...
        DetectorGeometry(DetectorMaterials& detectorMaterials);
        
        // ...
        ~DetectorGeometry() = default;
        
        // ...
        // DetectorBuild BuildDetector(G4LogicalVolume* worldLog, G4double const tableTopY, G4double const fCrystalDiameter, bool const fCheckOverlaps);
        DetectorBuild BuildDetector(G4LogicalVolume* worldLog, G4double const tableTopY, bool const fCheckOverlaps);
        
        // ...
        void SetCrystalDiameter(G4double const diameterInInches);
        
        // ...
        void SetAxialReflectorThickness(G4double const thickness);
        void SetRadialReflectorThickness(G4double const thickness);
        
        // ...
        void SetAxialEnclosureThickness(G4double const thickness);
        void SetRadialEnclosureThickness(G4double const thickness);
        void SetAxialEnclosureMaterial(G4String const material);
        void SetRadialEnclosureMaterial(G4String const material);
        
    private:
        // ...
        void DefineOpticalInterfaces(
            G4VPhysicalVolume* crystalPhys,
            G4VPhysicalVolume* reflectorRadialPhys,
            G4VPhysicalVolume* reflectorAxialPhys,
            // G4VPhysicalVolume* enclosureRadialPhys,
            // G4VPhysicalVolume* enclosureAxialPhys,
            G4VPhysicalVolume* greasePhys,
            G4VPhysicalVolume* windowPhys,
            G4VPhysicalVolume* sealPhys,
            G4VPhysicalVolume* photocathodePhys
        );
        
        // ...
        DetectorMaterials& fDetectorMaterials;
        
        // TODO: Maybe structs to group params for each component \/\/\/\/\/\/\/
        // then fEnclosure.RadialMaterial, etc
        // also means you practically never have to look at .cc file and wade through all the shite
        // just all nicely grouped here, idk
        
        // ...
        G4double fCrystalDiameter = 3. * (2.54 * cm); // 3 inches
        
        // ...
        G4double fReflectorAxialThickness = 0.23495 * cm;
        G4double fReflectorRadialThickness = 0.23495 * cm;
        ReflectorMaterial fReflectorMaterial = ReflectorMaterial::Al2O3;
        
        // ...
        G4double fEnclosureAxialThickness = 0.0508 * cm;
        G4double fEnclosureRadialThickness = 0.0508 * cm;
        EnclosureMaterial fEnclosureRadialMaterial = EnclosureMaterial::Aluminium;
        EnclosureMaterial fEnclosureAxialMaterial = EnclosureMaterial::Aluminium;
};

#endif
