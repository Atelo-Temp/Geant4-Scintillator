#ifndef MySourceMaterials_HH
#define MySourceMaterials_HH

// G4 lib
// #include "G4Material.hh"

// Forward declarations
class G4Material;

/*
 * Handles creating custom materials
 */
class SourceMaterials {
    public:
        // Constructor
        SourceMaterials() = default;
        
        // Constructor
        ~SourceMaterials() = default;
        
        // Create custom cesium-137 material
        G4Material* Create137Cs();
        G4Material* Create60Co();
        
        // ...
        void DefineSourceMats();
        
        // Getters
        G4Material* Polystyrene() const;
        G4Material* Mylar() const;
        G4Material* Aluminium() const;
        G4Material* PMMA() const;
        G4Material* PLA() const;
        
    private:
        G4Material* fPolystyrene;
        G4Material* fMylar;
        G4Material* fAluminium;
        G4Material* fPMMA;
        G4Material* fPLA;
};

#endif
