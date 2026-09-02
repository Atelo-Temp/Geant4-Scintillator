#ifndef MySourceGeometry_HH
#define MySourceGeometry_HH

// G4 lib
#include "G4Types.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh" // NOTE: TEMP

// Forward declarations
class G4VPhysicalVolume;
class G4LogicalVolume;
class SourceMaterials;

/*
 * ...
 * 
 * TODO: Metastable silver might be an interesting one to compliment B-, B+, ε
 * since it shows non-zero parent isotope energy works, and it sometimes undergoes
 * isometric transtion (IT) decay mode
 */
enum class Isotopes {
    Cs137, // Cesium 137
    Co60, // Cobalt 60
    Na22, // Sodium 22
    Ba133, // Barium 133
    // mAg108 // Metastable Silver 108
    // Ge68 // Germanium 68
    // Am241 // Americium 241
    // Ti44 // Titanium 44
    // Eu152 // Europium 152
    // Bi207 // Bismuth 207
};

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
        // G4VPhysicalVolume* BuildSource(G4LogicalVolume* worldLog, G4double const tableTopY, G4double const detectorFaceZ, G4double const detectorX, G4double const fSourceDetectorDistance, bool const fCheckOverlaps);
        // G4VPhysicalVolume* BuildSource(G4LogicalVolume* worldLog, G4double const tableTopY, G4double const detectorFaceZ, G4double const detectorX, bool const fCheckOverlaps);
        void BuildSource(G4LogicalVolume* worldLog, G4double const tableTopY, G4double const detectorFaceZ, G4double const detectorX, bool const fCheckOverlaps);
        // TODO: Maybe just return source origin, radius, and height
        
        void SetSource(G4String const isotope);
        void SetSourceDetectorDistance(G4double const distance);
        
        // ...
        Isotopes GetSource() const { return fSource; };
        G4double GetSourceRadius() const { return fSourceRadius; };
        G4double GetSourceThickness() const { return fSourceThickness; };
        G4ThreeVector GetSourceOrigin() const { return fSourceCoords; };
        
    private:
        // ...
        SourceMaterials& fSourceMaterials;
        
        // ...
        G4double fSourceDetectorDistance = 3. * cm; // source placed 3cm from face of detector by default
        Isotopes fSource = Isotopes::Cs137;
        
        // G4LogicalVolume* fSourceVolume = nullptr; // NOTE: TEMP UNTIL PLACING PHYSICAL SOURCE VOL
        G4double fSourceRadius;
        G4double fSourceThickness;
        G4ThreeVector fSourceCoords; // NOTE: TEMP UNTIL PLACING PHYSICAL SOURCE VOL
        // G4VPhysicalVolume* fSourceVolume = nullptr;
};

#endif
