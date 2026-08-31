// User classes
#include "PrimaryGenerator.hh"

// G4 lib
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh" // cm, m, etc definition
#include "G4IonTable.hh" // Get acces to predefinied ions (decay physics)
#include "G4GeneralParticleSource.hh"

// TODO:
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4Tubs.hh"

// NOTE: Cannot hand this class directly to the run manager as with the others,
// must hand this to "ActionInitialization" which will use it

/*
 * Define the class constructor for the particle generator
 * 
 * NOTE: Everything related to particle definition is removed when using decay physics
 */
PrimaryGenerator::PrimaryGenerator() {
    // Instantiate the particle gun with args
    fParticleGun = new G4GeneralParticleSource();
}

/*
 * Define the class destructor (must delete primary generator object initialised in constructor)
 */
PrimaryGenerator::~PrimaryGenerator() {
    // Delete the particle gun to free up memory
    delete fParticleGun;
}

/*
 * Define the method which takes over events (called at the beginning of each event)
 * 
 * In this method, the primary generator (particle gun) must be invoked.
 * 
 * NOTE: More than one generator can be invoked per event. Similarly, the same generator
 * can be invoked more than once. This allows more complicated primary events to be produced.
 * 
 * NOTE: Particle definition is moved from constructor to this method with decay (doesnt work if done there)
 */
void PrimaryGenerator::GeneratePrimaries(G4Event* event) {
    // Add a clause to check if particle has already been set, else set it at the start of the event before calling generate
    // NOTE: Default particle is geantino if nothing has been set, this changes default to 137Cs
    // if (fParticleGun->GetParticleDefinition()->GetParticleName() == "geantino") {
    //     Cesium137Source();
    // }
    
    // Place the source once
    if (!fPlaced) {
        // ...
        auto const detectorConstruction = static_cast<const DetectorConstruction*>(
            G4RunManager::GetRunManager()->GetUserDetectorConstruction()
        );
        // NOTE: Readonly pointer
        
        // ...
        SelectSource(detectorConstruction);
        
        // ...
        PlaceSource(detectorConstruction);
        
        // Dont execute this code again during the run
        fPlaced = true;
    }
    
    // Create vertex (shoot particle), handing over the event
    fParticleGun->GeneratePrimaryVertex(event);
}

/*
 * ...
 * 
 * TODO: Generate isotope could be called once at the end of this method, instead of in each
 */
void PrimaryGenerator::SelectSource(DetectorConstruction const* detectorConstruction) {
    // ...
    Isotopes isotope = detectorConstruction->GetSource();
    
    // ...
    switch (isotope) {
        case Isotopes::Cs137:
            Cesium137Source();
            break;
        case Isotopes::Co60:
            Cobalt60Source();
            break;
        case Isotopes::Ba133:
            Barium133Source();
            break;
        case Isotopes::Na22:
            Sodium22Source();
            break;
        default:
            G4cerr << "Error: Unrecognised isotope." << G4endl;
            break;
    }
}

/*
 * Define the Cesium 137 isotope
 * 
 * NOTE: Likely better to return isotope & charge here, then have a separate method for
 * assigning isotope and charge to the particle gun
 * 
 * TODO: Probably just make a small database of isotopes {element, Z, A}, passing struct to GenerateIsotope,
 * rather than methods with Z and A embedded
 */
void PrimaryGenerator::Cesium137Source() {
    // Cesium (137Cs) (Half life ~30y) (B-)
    G4int const Z = 55; // Atomic number (num protons)
    G4int const A = 137; // Molecular mass (integer, not exact)
    
    // ...
    GenerateIsotope(Z, A);
}

/*
 * ...
 */
void PrimaryGenerator::Cobalt60Source() {
    // Cobalt (60Co) (Half life ~5y) (B-)
    G4int const Z = 27; // Atomic number (num protons)
    G4int const A = 60; // Molecular mass (integer, not exact)
    
    // ...
    GenerateIsotope(Z, A);
}

/*
 * ...
 */
void PrimaryGenerator::Barium133Source() {
    // Barium (133Ba) (Half life ~10.5y) (ε)
    G4int const Z = 56; // Atomic number (num protons)
    G4int const A = 133; // Molecular mass (integer, not exact)
    
    // ...
    GenerateIsotope(Z, A);
}

/*
 * ...
 */
void PrimaryGenerator::Sodium22Source() {
    // Sodium (22Na) (Half life ~2.6y) (B+ primarily)
    G4int const Z = 11; // Atomic number (num protons)
    G4int const A = 22; // Molecular mass (integer, not exact)
    
    // ...
    GenerateIsotope(Z, A);
}

/*
 * ...
 * 
 * TODO: Maybe make charge and energy args with deafult values
 */
void PrimaryGenerator::GenerateIsotope(G4int const Z, G4int const A) {
    // NOTE: For isotopes with t 1/2 > 1 year, the time threshold for radioactive decay of ions
    // must be adjusted, currently set in main() prior to initialisation,
    // can also be set in physics list or via macro
    
    // Define the particle charge (positive/negative due to losing/gaining electrons)
    G4double const charge = 0. * eplus; // Specify full electron shells (isotope, not ion)
    // NOTE: The G4 unit "eplus" is the elementary charge
    
    // Define the kinetic energy of the atom
    G4double const energy = 0. * eV; // Zero initial kinetic energy (static source)
    
    // Get a pointer to the ion table instance
    G4IonTable* ionTable = G4IonTable::GetIonTable();
    
    // Find the isotope with the given atomic number and molecular mass
    G4ParticleDefinition* isotope = ionTable->GetIon(Z, A, energy);
    // G4ParticleDefinition* isotope = ionTable->GetIon(Z, A);
    // NOTE: The "energy" param defaults to zero, so is optional,
    // but being explicit makes it easy to change later if needed
    
    // NOTE: Geant4 considers isotopes as ions, even with full electron shell,
    // kinda odd, but also an isotope is an ion with no pos/neg charge
    
    // Assign the isotope to the particle gun, specifying charge and energy
    fParticleGun->SetParticleDefinition(isotope);
    fParticleGun->SetParticleCharge(charge); // TODO: Does this default to 0 ?
    // fParticleGun->SetParticleEnergy(energy); // TODO: Seems unnecessary given arg to "GetIon()" ?
    // NOTE: Setting energy here is actually mandatory to ensure proper behaviour
    // positron will not reach rest inside of source volume otherwise
    // and will always go in Z direction
    // NOTE: GPS removes SetParticleCharge method (only applicable to gun)
}

/*
 * Assign coordinates to gps ion, specify distribution and dimensions
 * 
 * TODO: Not sure primary generator should require this intimate knowledge of det const
 * maybe just cache source shape, outer rad, half z, and origin in a struct
 */
void PrimaryGenerator::PlaceSource(DetectorConstruction const* detectorConstruction) {
    // ...
    // fParticleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0., -1 * cm, 2.90425 * cm));
    
    G4SingleParticleSource const* particle = fParticleGun->GetCurrentSource();
    G4SPSPosDistribution* position = particle->GetPosDist();
    
    // TEST ...
    // auto const detectorConstruction = static_cast<const DetectorConstruction*>(
    //     G4RunManager::GetRunManager()->GetUserDetectorConstruction()
    // );
    G4LogicalVolume const* sourceLog = detectorConstruction->GetSourceVolume(); // TODO: Should be physical volume so i can get coords, and logical, and solid
    // G4VPhysicalVolume* sourcePhys = detectorConstruction->GetSourceVolume(); // TODO
    
    G4VSolid const* sourceSolid = sourceLog->GetSolid();
    G4String const shapeType = sourceSolid->GetEntityType();
    
    G4double outerRadius;
    G4double halfThickness;
    
    if (shapeType == "G4Tubs") {
        // ...
        auto cylinder = static_cast<G4Tubs const*>(sourceSolid);
        outerRadius = cylinder->GetOuterRadius();
        halfThickness = cylinder->GetZHalfLength();
    }
    
    // G4double const crystalFaceZ = 5.90425 * cm; // TODO: calculate this as: crystal origin - 1/2 crystal height - reflector thick - enclosure thick
    // ^^^ use acquired detector construction instance above
    
    // G4double const sourceDetectorDist = 3 * cm; // TODO: expose messenger to alter this via macros, get the best of both worlds,
    // able to alter it on the fly instead of hardcoding/recompiling, and automatically derived based on detector geometry
    // ^^ will also allow for a change in macro source-detector dist to move the source holder, casing, and physical source itself

    // Get position vector with the defined components
    G4ThreeVector const origin = detectorConstruction->GetSourceOrigin();

    // Assign position of the particle    
    position->SetCentreCoords(origin);
    // NOTE: Primary generator should have no knowledge of source-detector distance, etc,
    // thats purely detector construction and detector messenger job, just derive here
    
    // Specify volumetric source
    position->SetPosDisType("Volume");
    
    // confine start positions to particular volume
    position->ConfineSourceToVolume("SourceActive");
    // TODO: could extract name from sourcePhys too tbh, one less point of failure
    
    // ...
    position->SetPosDisShape("Cylinder");
    
    // For cylinder, user gives redius and z-half length
    position->SetRadius(outerRadius); // TODO: derive these from detector construction,
    position->SetHalfZ(halfThickness); // or from class member itself like GetSourceOrigin
    
    // TODO: Remove all these from test.mac and 137Cs.mac (just leave isotope selection to the macro)
    // ^ but actually abstract away isotope selection a bit, macro should just specify one of the four
    // available sources, and it changes what is instantiated as ion here in the code
}
