#ifndef MySourceGeometry_HH
#define MySourceGeometry_HH

// G4 lib
#include "G4Types.hh"

// Forward declarations
class G4VPhysicalVolume;
class G4LogicalVolume;
class SourceMaterials;

/*
 * ...
 */
class SourceGeometry {
    public:
        // ...
        SourceGeometry(SourceMaterials& fSourceMaterials);
        
        // ...
        ~SourceGeometry() = default;
        
        // ...
        G4VPhysicalVolume* BuildSource(G4LogicalVolume* worldLog, G4double const tableTopY, G4double const detectorFaceZ, G4double const detectorX, G4double const fSourceDetectorDistance, bool const fCheckOverlaps);
        // TODO: Maybe just return source origin, radius, and height
        
    private:
        // ...
        SourceMaterials& fSourceMaterials;
};

#endif
