// User classes
#include "SourceMaterials.hh"

// G4 lib
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4SystemOfUnits.hh"
#include "G4Isotope.hh"
#include "G4NistManager.hh"

// TODO: Lazy load ? getters instantiate if not already instantiated

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
    sourceMat->AddElement(sourceElement, 50. * perCent); // element, amount of element in material (100%)
    // NOTE: In practice there would be non-zero amount of the daughter isotope too,
    // based on how old the source was (after 30y half of a "new" 137Cs source would be 137Ba)
    
    // Source is >30y old so at least 1/2 is 137Ba
    sourceMat->AddElement(daughterElement, 50. * perCent);
    
    // ...
    return sourceMat;
}

/*
 * ...
 */
G4Material* SourceMaterials::Create60Co() {
    // Source isotope (define Cobalt-60 isotope)
    auto sourceIsotope = new G4Isotope(
        "60Co", // name
        27, // num protons (Z)
        60, // Atomic mass (num nucleons) (A),
        59.9338222 * g/mole // Molar mass (grams per molecule)
    );
    // NOTE: 1 mol contains avogadros number of particles (6.022 x 10^23)
    
    // Daugher isotope (Nickel-60)
    auto daughterIsotope = new G4Isotope(
        "60Ni",
        28,
        60,
        59.93078513 * g/mole
    );
    
    // Define an element from the isotope
    auto sourceElement = new G4Element("Cobalt-60", "60Co", 1); // name, symbol, num isotopes
    
    // Assign the defined isotope to the element
    sourceElement->AddIsotope(sourceIsotope, 100.0 * perCent); // isotope, no other isotopes so 100%
    
    // Define daughter element, adding its isotope
    auto daughterElement = new G4Element("Nickel-60", "60Ni", 1);
    daughterElement->AddIsotope(daughterIsotope, 100.0 * perCent);
    
    // Because isotope and element have no direct interaction in G4, need to create a material to assign to logical volume
    auto sourceMat = new G4Material("60Co", 8.834 * g / cm3, 2); // name, density (g/cm^3), num components
    // NOTE: Density is estimate, in reality it wont usually be a pure 60Co source
    
    // Assign the element to the G4 material
    sourceMat->AddElement(sourceElement, 50. * perCent); // element, amount of element in material (100%)
    // NOTE: In practice there would be non-zero amount of the daughter isotope too,
    // based on how old the source was (after 30y half of a "new" 137Cs source would be 60Ni)
    
    // Source is >??y old so ?? is 60Ni
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
    
    // Cesium-137 (137Cs) source, 50% barium (137Ba) daughter product
    // auto sourceHandler = new MaterialDefinitions();
    // G4Material* sourceMat = sourceHandler->Create137Cs();
    
    // Ion-exchange bead
    fPolystyrene = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
    // TODO: Maybe explore cesium chloride powder, cesium titanate ceramic, or stable glass matrix
    
    // Polyethylene terephthalate (PET) aka Mylar
    fMylar = nist->FindOrBuildMaterial("G4_MYLAR");
    
    // ...
    fAluminium = nist->FindOrBuildMaterial("G4_Al");
    
    // Source Casing
    // G4Material* PVC = nist->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"); // density = 1.3 g/cm^3
    // NOTE: 2 part carbon (C), 3 part hydrogen (H), 1 part chlorine (Cl)
    
    // NOTE: Chlorine has relatively high Z, not as suitable as H, C, O, based PMMA
    
    // ...
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* O = nist->FindOrBuildElement("O");
    
    // PMMA (acrylic)
    fPMMA = new G4Material("PMMA", 1.18 * g/cm3, 3);
    fPMMA->AddElement(C, 5); // NOTE: MUST BE FRACTIONAL IF MATERIAL, PASS ELEMENT IF USING ATOMS
    fPMMA->AddElement(H, 8);
    fPMMA->AddElement(O, 2);
    // NOTE: Seperate opaque & transparent acrylic definitions are only relevant if optical photons 
    // are to interact with it, for high energy particles like gammas and x-rays, can use the same 
    // material for both
    
    // Source Holder (3d printed, likely PLA)
    fPLA = new G4Material("PLA", 1.24 * g/cm3, 3);
    fPLA->AddElement(C, 3);
    fPLA->AddElement(H, 4);
    fPLA->AddElement(O, 2);    
}

/*
 * ...
 */
G4Material* SourceMaterials::Polystyrene() const {
    return fPolystyrene;
};

/*
 * ...
 */
G4Material* SourceMaterials::Mylar() const {
    return fMylar;
};

/*
 * ...
 */
G4Material* SourceMaterials::Aluminium() const {
    return fAluminium;
};

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
