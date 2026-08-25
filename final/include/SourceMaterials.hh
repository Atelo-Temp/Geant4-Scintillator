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
        ~SourceMaterials();
        
        // Create custom cesium-137 material
        G4Material* Create137Cs();
        
        // ...
        void DefineSourceMats();
        
        // Getters
        G4Material* PMMA() const;
        G4Material* PLA() const;
        
    private:
        G4Material* fPMMA;
        G4Material* fPLA;
};

#endif
