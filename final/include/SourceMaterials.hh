#ifndef MySourceMaterials_HH
#define MySourceMaterials_HH

// G4 lib
// #include "G4Material.hh"

// Forward declarations
class G4Material;

/*
 * ...
 */
enum class SourceWindowMaterial {
    Mylar,
    StainlessSteel,
    Aluminium,
    TitaniumAlloy
};

/*
 * Handles creating custom materials
 */
class SourceMaterials {
    public:
        // Constructor
        SourceMaterials() = default;
        
        // Constructor
        ~SourceMaterials() = default;
        
        // ...
        void DefineSourceMats();
        
        // ...
        G4Material* GetSourceWindowMaterial(SourceWindowMaterial const material) const;
        
        // Getters
        G4Material* Polystyrene() const;
        G4Material* Mylar() const;
        G4Material* Aluminium() const;
        G4Material* PMMA() const;
        G4Material* PLA() const;
        
    private:
        // Create custom cesium-137 material
        G4Material* Create137Cs();
        G4Material* Create60Co();
        
        G4Material* fPolystyrene = nullptr;
        G4Material* fMylar = nullptr;
        G4Material* fAluminium = nullptr;
        G4Material* fPMMA = nullptr;
        G4Material* fPLA = nullptr;
};

#endif
