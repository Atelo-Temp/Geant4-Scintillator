#include "MyPrimaryGenerator.hh"

#include "G4ParticleGun.hh"
// #include "G4ParticleDefinition.hh" // provides e-, e+, etc definitions // removed for decay physics
// #include "G4ParticleTable.hh" // removed for decay physics
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
    
    
    
    // Define the isotope - 137Cs (cesium not working, due to 30y half life? doubt it but idk) (B-)
    // G4int Z = 55; // Atomic number (num protons)
    // G4int A = 137; // Molecular mass (integer, not exact)
    
    // Flourine (F18) works (109 minutes half life) (B+)
    G4int Z = 9; // Atomic number (num protons)
    G4int A = 18; // Molecular mass (integer, not exact)
    
    // Titanium (44Ti) doesnt work (60y t1/2) (E)
    // G4int Z = 22; // Atomic number (num protons)
    // G4int A = 44; // Molecular mass (integer, not exact)
    
    // Cobalt (60Co) doesnt work (5y t1/2) (B-)
    // G4int Z = 27; // Atomic number (num protons)
    // G4int A = 60; // Molecular mass (integer, not exact)
    
    // Scandium (44Sc) doesnt work (4h t1/2) (B+)
    // G4int Z = 21; // Atomic number (num protons)
    // G4int A = 44; // Molecular mass (integer, not exact)
    
    // NOTE: Seems to be just B+ emitters that work
    
    // Xenon (137Xe) works (137Cs precursor) (4 min t1/2) (B-)
    // G4int Z = 54; // Atomic number (num protons)
    // G4int A = 137; // Molecular mass (integer, not exact)
    
    // NOTE: Is it due to J value ? Angular momentum ? GetIon can take 4th param ...
    
    // NOTE: Is it the time elapsed in one event or something?
    // ^ need to test some more longer lived isotopes
    
    // NOTE: Is it related to physics list or such ?
    
    // Sodium (22Na) (t 1/2 2y) (B+)
    // G4int Z = 11; // Atomic number (num protons)
    // G4int A = 22; // Molecular mass (integer, not exact)
    // NOTE: No annihilation photons seen (but can see positrons hitting lead shield)
    
    // Aluminium (26Al) (t 1/2 700,000 y) (B+)
    // G4int Z = 11; // Atomic number (num protons)
    // G4int A = 26; // Molecular mass (integer, not exact)
    // NOTE: No annihilation photons seen (but can see positrons interacting, and some gammas)
    
    
    // Define the particle charge (positive/negative due to losing/gaining electrons)
    G4double charge = 0. * eplus; // Specify full electron shells (isotope, not ion)
    // NOTE: The G4 unit "eplus" is the elementary charge
    
    // Define the kinetic energy of the atom
    G4double energy = 0. * keV; // Zero initial kinetic energy (static source)
    
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
