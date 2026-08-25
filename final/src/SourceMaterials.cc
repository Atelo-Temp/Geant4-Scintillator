// User classes
#include "SourceMaterials.hh"

// G4 lib
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4SystemOfUnits.hh"
#include "G4Isotope.hh"
#include "G4NistManager.hh"

/*
 * Destructor
 */
SourceMaterials::~SourceMaterials() {
    delete fPMMA;
    delete fPLA;
}

/*
 * Create and return a cesium 137 material
 * 
 * NOTE: With 50% barium 137 content due to 137Cs decay
 * 
 * TODO: This could likely be extracted out to private member "CreateSource",
 * taking all params from dedicated CreateXYZ public methods
 */
G4Material* SourceMaterials::Create137Cs() {
    // Source isotope (define Cesium-137 isotope)
    auto sourceIsotope = new G4Isotope(
        "137Cs", // name
        55, // num protons (Z)
        137, // Atomic mass (num nucleons) (A),
        136.907089 * g/mole // Molar mass (grams per molecule) (~18g per mol)
    );
    // NOTE: 1 mol contains avogadros number of particles (6.022 x 10^23)
    
    // Daugher isotope (Barium-137)
    auto daughterIsotope = new G4Isotope(
        "137Ba",
        56,
        137,
        137.33 * g/mole
    );
    
    // Define an element from the isotope
    auto sourceElement = new G4Element("Cesium-137", "137Cs", 1); // name, symbol, num isotopes
    
    // Assign the defined isotope to the element
    sourceElement->AddIsotope(sourceIsotope, 100.0 * perCent); // isotope, no other isotopes so 100%
    
    // Define daughter element, adding its isotope
    auto daughterElement = new G4Element("Barium-137", "137Ba", 1);
    daughterElement->AddIsotope(daughterIsotope, 100.0 * perCent);
    
    // Because isotope and element have no direct interaction in G4, need to create a material to assign to logical volume
    auto sourceMat = new G4Material("137Cs", 1.886 * g / cm3, 2); // name, density (g/cm^3), num components
    // NOTE: Density is estimate, in reality it wont usually be a pure 137Cs source,
    // usually embedded in a matrix, encapsulated (stainless steel), or a mixed compound (cesium chloride),
    // i.e. cesium oxide ceramic matrix (more like 1.47 g/cm3)
    
    // Assign the element to the G4 material
    // sourceMat->AddElement(sourceElement, 100.0 * perCent); // element, amount of element in material (100%)
    sourceMat->AddElement(sourceElement, 50. * perCent); // element, amount of element in material (100%)
    // TODO: In practice there would be non-zero amount of the daughter isotope too,
    // based on how old the source was (after 30y half of a "new" 137Cs source would be 137Ba)
    
    // isotope, source is >30y old so 1/2 is 137Ba
    sourceMat->AddElement(daughterElement, 50. * perCent);
    
    // ...
    return sourceMat;
}

/*
 * ....
 */
void SourceMaterials::DefineSourceMats() {
    
    // Get a pointer to the material manager instance
    G4NistManager* nist = G4NistManager::Instance();
    
    ////////////////////
    // SOURCE MATERIALS:
    ////////////////////
    
    // TODO: Expose a messenger for source selection that sets both the gps ion, and the source materials here

//     // Source material (define Cesium-18 isotope)
//     auto sourceIsotope = new G4Isotope(
//         "137Cs", // name
//         55, // num protons (Z)
//         137, // Atomic mass (num nucleons) (A),
//         136.907089 * g / mole // Molar mass (grams per molecule) (~18g per mol)
//     );
//     // NOTE: 1 mol contains avogadros number of particles (6.022 x 10^23)
//     
//     // Define an element from the isotope
//     auto sourceElement = new G4Element("Cesium-137", "137Cs", 1); // name, symbol, num isotopes
//     
//     // Assign the defined isotope to the element
//     sourceElement->AddIsotope(sourceIsotope, 100.0 * perCent); // isotope, no other isotopes so 100%
//     
//     // Because isotope and element have no direct interaction in G4, need to create a material to assign to logical volume
//     auto sourceMat = new G4Material("137Cs", 1.886 * g / cm3, 1); // name, density (g/cm^3), phase (solid, liquid, gas)
//     // NOTE: Density is estimate, in reality it wont usually be a pure 137Cs source,
//     // usually embedded in a matrix, encapsulated (stainless steel), or a mixed compound (cesium chloride),
//     // i.e. cesium oxide ceramic matrix (more like 1.47 g/cm3)
//     
//     // Assign the element to the G4 material
//     sourceMat->AddElement(sourceElement, 100.0 * perCent); // element, amount of element in material (100%)
//     // TODO: In practice there would be non-zero amount of the daughter isotope too,
//     // based on how old the source was (after 30y half of a "new" 137Cs source would be 137Ba)
    
    // Cesium-137 (137Cs) source, 50% barium (137Ba) daughter product
    // auto sourceHandler = new MaterialDefinitions();
    // G4Material* sourceMat = sourceHandler->Create137Cs();
    
    // Source Casing
    // G4Material* PVC = nist->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"); // density = 1.3 g/cm^3
    // NOTE: 2 part carbon (C), 3 part hydrogen (H), 1 part chlorine (Cl)
    
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* O = nist->FindOrBuildElement("O");
    
    // TODO: Chlorine has relatively high Z, not as suitable as H, C, O, based PMMA
    // PMMA
    fPMMA = new G4Material("PMMA", 1.18 * g/cm3, 3);
    fPMMA->AddElement(C, 5); // NOTE: MUST BE FRACTIONAL IF MATERIAL, PASS ELEMENT IF USING ATOMS
    fPMMA->AddElement(H, 8);
    fPMMA->AddElement(O, 2);
    
    // Source Holder (3d printed, likely PLA)
    fPLA = new G4Material("PLA", 1.24 * g/cm3, 3);
    fPLA->AddElement(C, 3);
    fPLA->AddElement(H, 4);
    fPLA->AddElement(O, 2);
    
    // TODO: Should "MaterialDefinitions -- i.e. sourceHandler" be deleted...    
}

/*
 * ...
 */
G4Material* SourceMaterials::PMMA() const {
    return fPMMA;
};

/*
 * ...
 */
G4Material* SourceMaterials::PLA() const {
    return fPLA;
};
