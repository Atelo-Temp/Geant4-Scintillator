#ifndef MyDetectorMaterials_HH
#define MyDetectorMaterials_HH

// ...
#include "G4OpticalSurface.hh"
// NOTE: Include instead of forward declaration else G4LogicalBorderSurface wont 
// recognise the type without importing G4OpticalSurface in DetectorGeometry.cc,
// but the type isnt explicitly used anywhere in that file

// Forward declarations
class G4Material;

/*
 * ...
 */
enum class ReflectorMaterial { Al2O3 }; // TODO: MgO, Teflon

/*
 * ...
 */
enum class EnclosureMaterial { Aluminium, StainlessSteel }; // TODO: Beryllium

/*
 * ...
 */
class DetectorMaterials {
    public:
        // ...
        DetectorMaterials() = default;
        
        // ...
        ~DetectorMaterials() = default;
        
        // ...
        void DefineDetectorMats();
        
        // ...
        void DefineOpticalProperties();
        
        // Generic getters (TEST)
        G4Material* GetReflectorMaterial(ReflectorMaterial material) const;
        G4Material* GetEnclosureMaterial(EnclosureMaterial material) const;
        
        // Material Getters
        G4Material* Air() const;
        G4Material* Wood() const;
        
        G4Material* NaI() const;
        G4Material* Al2O3() const;
        G4Material* Al() const;
        G4Material* PDMS() const;
        G4Material* Borosilicate() const;
        G4Material* Li() const;
        
        // Surface Getters
        G4OpticalSurface* ReflectorSurface() const;
        G4OpticalSurface* AluminiumSurface() const;
        G4OpticalSurface* GreaseSurface() const; // technically not needed
        G4OpticalSurface* WindowSurface() const; // technically not needed
        G4OpticalSurface* PhotocathodeSurface() const;
        
        
    private:
        // Materials
        G4Material* fAir; // not really detector material
        G4Material* fWood; // not really detector material
        
        G4Material* fNaI;
        G4Material* fAl2O3;
        G4Material* fAl;
        G4Material* fPDMS;
        G4Material* fBorosilicate;
        G4Material* fLi;
        
        // Surfaces
        G4OpticalSurface* fReflectorSurface;
        G4OpticalSurface* fAluminiumSurface;
        G4OpticalSurface* fGreaseSurface; // technically not needed
        G4OpticalSurface* fWindowSurface; // technically not needed
        G4OpticalSurface* fPhotocathodeSurface;
};

#endif
