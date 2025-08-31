#include "MyPrimaryGenerator.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh" // provides e-, e+, etc definitions
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh" // cm, m, etc definition

// Define the class constructor for the particle generator
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
    G4double px = 0.;
    G4double py = 0.;
    G4double pz = 1.;

    G4ThreeVector mom(px, py, pz);

    // Get a pointer to the particle table
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Define the particle type as a gamma-ray photon
    // G4ParticleDefinition* particle = particleTable->FindParticle("e+");
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    // G4String particleName;
    // G4ParticleDefinition* particle = particleTable->FindParticle(particleName = "gamma"); // NOTE: SEEMS TO STILL SAY e+ IN VIS ?


    // Pass definitions to the particle gun
    fParticleGun->SetParticlePosition(pos);

    fParticleGun->SetParticleMomentumDirection(mom);
    // fParticleGun->SetParticleEnergy(1. * GeV); // positron (actually kinda crazy to see how much gamma produced by 1 GeV positron hitting lead shielding & NaI crystal)
    fParticleGun->SetParticleEnergy(0.662 * MeV); // Set a magnitude for the momentum vector (662 keV - 137Cs)

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
