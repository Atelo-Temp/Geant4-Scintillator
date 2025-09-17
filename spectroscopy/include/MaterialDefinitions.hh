#ifndef MyMaterialDefinitions_HH
#define MyMaterialDefinitions_HH

#include "G4Material.hh"

// ...
class MaterialDefinitions {
    public:
        // Constructor
        MaterialDefinitions() = default;
        
        // Constructor
        ~MaterialDefinitions() = default;
        
        // ...
        G4Material* Create137Cs();
};

#endif
