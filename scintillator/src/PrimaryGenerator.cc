#include "PrimaryGenerator.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh" // cm, m, etc definition
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"


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
    G4double z = -0.01 * m; // small 1cm offset for visibility

    // Create a position vector with the defined components
    G4ThreeVector pos(x, y, z);

    // Define the particle direction (p for momentum)
    G4double px = 0.;
    G4double py = 0.;
    G4double pz = 1.;
    // NOTE: Shoot gamma in Z direction

    G4ThreeVector mom(px, py, pz);
    
    // Get gamma-ray photon particle definition
    G4ParticleDefinition* gamma = G4ParticleTable::GetParticleTable()->FindParticle("gamma");

    // Pass definitions to the particle gun
    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleMomentumDirection(mom);
    fParticleGun->SetParticleEnergy(662. * keV); // Set a magnitude for the momentum vector (kinetic energy)
    fParticleGun->SetParticleDefinition(gamma);
}

// Define the class destructor (must delete primary generator object initialised in constructor)
PrimaryGenerator::~PrimaryGenerator() {
    // Delete the particle gun to free up memory
    delete fParticleGun;
}

// Define the method which takes over events (called at the beginning of each event)
void PrimaryGenerator::GeneratePrimaries(G4Event* anEvent) {
    /*
     * In this method, the primary generator (particle gun) must be invoked.
     * 
     * NOTE: More than one generator can be invoked per event. Similarly, the same generator
     * can be invoked more than once. This allows more complicated primary events to be produced.
     */
    
    // Create vertex (shoot particle), handing over the event
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

// NOTE: Cannot hand this class directly to the run manager as with the others,
// must hand this to "ActionInitialization" which will use it
