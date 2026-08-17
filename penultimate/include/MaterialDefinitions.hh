#ifndef MyMaterialDefinitions_HH
#define MyMaterialDefinitions_HH

// G4 lib
#include "G4Material.hh"

/*
 * Handles creating custom materials
 */
class MaterialDefinitions {
    public:
        // Constructor
        MaterialDefinitions() = default;
        
        // Constructor
        ~MaterialDefinitions() = default;
        
        // Create custom cesium-137 material
        G4Material* Create137Cs();
};

#endif
