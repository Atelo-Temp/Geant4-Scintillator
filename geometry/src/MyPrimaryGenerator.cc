#include "MyPrimaryGenerator.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh" // provides e-, e+, etc definitions
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh" // cm, m, etc definition

// Define the class constructor
PrimaryGenerator::PrimaryGenerator() {
    // Specify the number of particles per event
    G4int numParticles = 1;

    // Instantiate the particle gun with args
    fParticleGun = new G4ParticleGun(numParticles);

    // Define the position of the particle
    G4double x = 0. * m;
    G4double y = 0. * m;
    G4double z = 0. * m;

    // Create a position vector with the defined components
    G4ThreeVector pos(x, y, z);

    // Define the particle direction (p for momentum)
    G4double px = 0. * m;
    G4double py = 0. * m;
    G4double pz = 0. * m;

    G4ThreeVector mom(px, py, pz);

    // Get a pointer to the particle table
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Define the particle type as a positron
    G4ParticleDefinition* particle = particleTable->FindParticle("e+");

    // Pass definitions to the particle gun
    fParticleGun->SetParticlePosition(pos);

    fParticleGun->SetParticleMomentumDirection(mom);
    fParticleGun->SetParticleEnergy(1. * GeV); // Set a magnitude for the momentum vector

    fParticleGun->SetParticleDefinition(particle);
}

// Define the class destructor
PrimaryGenerator::~PrimaryGenerator() {
    // Delete the particle gun to free up memory
    delete fParticleGun;
}

// Define the method which takes over events
void PrimaryGenerator::GeneratePrimaries(G4Event* anEvent) {
    // Create vertex (shoot particle), handing over the event
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

// NOTE: Cannot hand this class directly to the run manager as with the others,
// must hand this to "ActionInitialization" which will use it
