#ifndef MyDetectorGeometry_HH
#define MyDetectorGeometry_HH

// G4 lib
#include "G4Types.hh"

// Forward declarations
class DetectorMaterials;
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
        DetectorBuild BuildDetector(G4LogicalVolume* worldLog, G4double const tableTopY, G4double const fCrystalDiameter, bool const fCheckOverlaps);
        
    private:
        // ...
        DetectorMaterials& fDetectorMaterials;
        
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
};

#endif
