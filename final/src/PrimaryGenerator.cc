// User classes
#include "PrimaryGenerator.hh"

// G4 lib
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh" // cm, m, etc definition
#include "G4IonTable.hh" // Get acces to predefinied ions (decay physics)
#include "G4GeneralParticleSource.hh"

// TODO:
// #include "DetectorConstruction.hh"
// #include "G4RunManager.hh"

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
    if (fParticleGun->GetParticleDefinition()->GetParticleName() == "geantino") {
        Cesium137Source();
    }
    
    // Create vertex (shoot particle), handing over the event
    fParticleGun->GeneratePrimaryVertex(event);
}

/*
 * Define the Cesium 137 isotope
 * 
 * NOTE: Likely better to return isotope & charge here, then have a separate method for
 * assigning isotope and charge to the particle gun
 */
void PrimaryGenerator::Cesium137Source() {
    // Cesium (137Cs) (Half life ~30y) (B-)
    G4int const Z = 55; // Atomic number (num protons)
    G4int const A = 137; // Molecular mass (integer, not exact)
    
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
    
    
    ///////////////////////////////////////////////////
    // TODO: Extract to new method (but only call once)
    ///////////////////////////////////////////////////
    
    // ...
    // fParticleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0., -1 * cm, 2.90425 * cm));
    
    G4SingleParticleSource* particle = fParticleGun->GetCurrentSource();
    G4SPSPosDistribution* position = particle->GetPosDist();
    
    // TEST ...
    // auto const detConst = static_cast<const DetectorConstruction*>(
    //     G4RunManager::GetRunManager()->GetUserDetectorConstruction()
    // );
    // G4double const z = detConst->GetCrystalFaceZ();
    // TEST ...
    
    G4double const crystalFaceZ = 5.90425 * cm; // TODO: calculate this as: crystal origin - 1/2 crystal height - reflector thick - enclosure thick
    // ^^^ use acquired detector construction instance above
    
    G4double const sourceDetectorDist = 3 * cm; // TODO: expose messenger to alter this via macros, get the best of both worlds,
    // able to alter it on the fly instead of hardcoding/recompiling, and automatically derived based on detector geometry
    // ^^ will also allow for a change in macro source-detector dist to move the source holder, casing, and physical source itself
    
    // Define the position of the particle
    G4double const x = 0. * cm;
    G4double const y = -1 * cm;
    // G4double z = 2.90425 * cm; // small 1cm offset for visibility
    G4double const z = crystalFaceZ - sourceDetectorDist; // small 1cm offset for visibility

    // Create a position vector with the defined components
    G4ThreeVector const sourceOrigin(x, y, z);
    
    position->SetCentreCoords(sourceOrigin);
    
    // position->SetPosDisType("Volume"); // volumetric source // TODO
    // position->ConfineSourceToVolume("Source"); // confine start positions to particular volume // TODO
    
    // ...
    // position->SetPosDisShape("Cylinder"); // TODO
    
    // For cylinder, user gives redius and z-half length
    // position->SetRadius(0. * cm); // TODO: derive these from detector construction
    // position->SetHalfZ(0. * cm);
    
    // TODO: Remove all these from test.mac and 137Cs.mac (just leave isotope selection to the macro)
}
