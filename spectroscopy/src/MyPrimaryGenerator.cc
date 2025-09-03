#include "MyPrimaryGenerator.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh" // cm, m, etc definition

#include "G4IonTable.hh" // Get acces to predefinied ions (decay physics)

// Define the class constructor for the particle generator
// NOTE: Everything related to particle definition is removed when using decay physics
PrimaryGenerator::PrimaryGenerator() {
    // Specify the number of particles per event
    G4int numParticles = 1;

    // Instantiate the particle gun with args
    fParticleGun = new G4ParticleGun(numParticles);

    // Define the position of the particle
    G4double x = 0. * m;
    G4double y = 0. * m;
    // G4double z = 0. * m;
    G4double z = -0.01 * m; // small 1cm offset for visibility

    // Create a position vector with the defined components
    G4ThreeVector pos(x, y, z);

    // Define the particle direction (p for momentum)
    G4double px = 0.;
    G4double py = 0.;
    G4double pz = 0.;
    // NOTE: For decay modelling, zero momentum to simulate static source

    G4ThreeVector mom(px, py, pz);

    // Pass definitions to the particle gun
    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleMomentumDirection(mom);
}

// Define the class destructor
PrimaryGenerator::~PrimaryGenerator() {
    // Delete the particle gun to free up memory
    delete fParticleGun;
}

// Define the method which takes over events
void PrimaryGenerator::GeneratePrimaries(G4Event* anEvent) {
    // NOTE: Particle definition is moved from constructor to this method with decay    
    
    // Define the isotope
    
    // Cesium (137Cs) (Half life ~30y) (B-)
    // G4int Z = 55; // Atomic number (num protons)
    // G4int A = 137; // Molecular mass (integer, not exact)
    
    // Xenon (137Xe) (t1/2 ~4 min) (B-)
    // G4int Z = 54;
    // G4int A = 137;
    // NOTE: 137Cs precursor, can see e- and 455 keV gamma, then another e- and 662 keV gamma
    
    // Barium (137Ba) (stable)
    // G4int Z = 56;
    // G4int A = 137;
    // NOTE: 137Cs daughter, no emission as expected
    
    // Flourine (18F) (t1/2 ~109m) (B+)
    G4int Z = 9;
    G4int A = 18;
    // NOTE: Works (positron annihilates)
    
    // Titanium (44Ti) (t1/2 ~60y) (e- cap)
    // G4int Z = 22;
    // G4int A = 44;
    
    // Scandium (44Sc) (t1/2 ~4h) (B+)
    // G4int Z = 21;
    // G4int A = 44;
    // NOTE: 44Ti daughter
    
    // Cobalt (60Co) (t1/2 ~5y) (B-)
    // G4int Z = 27;
    // G4int A = 60;

    // Sodium (22Na) (t1/2 ~2y) (B+)
    // G4int Z = 11;
    // G4int A = 22;
    
    // Aluminium (26Al) (t1/2 ~700,000y) (B+)
    // G4int Z = 13;
    // G4int A = 26;
    
    // Sodium (26Na) (t1/2 ~1s) (B-)
    // G4int Z = 11;
    // G4int A = 26;
    // NOTE: Can see 1808 keV gamma, and B- electron
    
    // Neon (24Ne) (t1/2 ~3.38m) (B-)
    // G4int Z = 10;
    // G4int A = 24;
    // NOTE: Can see 472 keV gamma, B- e-, but also the daughter 24Na 1368 keV & 2754 keV gammas
    
    // Germanium (68Ge) (t1/2 ~270d) (e- cap)
    // G4int Z = 32;
    // G4int A = 68;
    
    // Barium (133Ba) (t1/2 ~10.5y) (e- cap)
    // G4int Z = 56;
    // G4int A = 133;
    
    // Bismuth (207Bi) (t1/2 ~31y) (e- cap)
    // G4int Z = 83;
    // G4int A = 207;
    
    // Americium (241Am) (t1/2 ~432y) (alpha)
    // G4int Z = 95;
    // G4int A = 241;
    
    // Europium (152Eu) (t1/2 ~13.5y) (B- && e- cap)
    // G4int Z = 63;
    // G4int A = 152;
    
    // Metastable silver (108mAg) (t1/2 ~438y) (IT && e- cap)
    // G4int Z = 32;
    // G4int A = 108;
    // TODO: Probably need to set e level, havent tested but wouldnt work as intended as is
    
    // NOTE: For isotopes with t 1/2 > 1 year, the time threshold for radioactive decay of ions
    // must be adjusted, currently set in main() prior to initialisation,
    // can also be set in physics list or via macro
    
    
    // Define the particle charge (positive/negative due to losing/gaining electrons)
    G4double charge = 0. * eplus; // Specify full electron shells (isotope, not ion)
    // NOTE: The G4 unit "eplus" is the elementary charge
    
    // Define the kinetic energy of the atom
    G4double energy = 0. * eV; // Zero initial kinetic energy (static source)
    
    // Get a pointer to the ion table instance
    auto ionTable = G4IonTable::GetIonTable();
    
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
    fParticleGun->SetParticleEnergy(energy); // TODO: Seems unnecessary given arg to "GetIon()" ?
    // NOTE: Setting energy here is actually mandatory to ensure proper behaviour
    // positron will not reach rest inside of source volume otherwise
    // and will always go in Z direction
    
    // Create vertex (shoot particle), handing over the event
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

// NOTE: Cannot hand this class directly to the run manager as with the others,
// must hand this to "ActionInitialization" which will use it
