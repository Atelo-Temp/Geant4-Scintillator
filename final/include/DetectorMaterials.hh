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
        G4Material* fAir = nullptr; // not really detector material
        G4Material* fWood = nullptr; // not really detector material
        
        G4Material* fNaI = nullptr;
        G4Material* fAl2O3 = nullptr;
        G4Material* fAl = nullptr;
        G4Material* fPDMS = nullptr;
        G4Material* fBorosilicate = nullptr;
        G4Material* fLi = nullptr;
        
        // Surfaces
        G4OpticalSurface* fReflectorSurface = nullptr;
        G4OpticalSurface* fAluminiumSurface = nullptr;
        G4OpticalSurface* fGreaseSurface = nullptr; // technically not needed
        G4OpticalSurface* fWindowSurface = nullptr; // technically not needed
        G4OpticalSurface* fPhotocathodeSurface = nullptr;
};

#endif
