#include "PrimaryGenerator.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh" // cm, m, etc definition

#include "G4IonTable.hh" // Get acces to predefinied ions (decay physics)

// TEST ...
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
// TEST ....

// Define the class constructor for the particle generator
// NOTE: Everything related to particle definition is removed when using decay physics
PrimaryGenerator::PrimaryGenerator() {
    // Specify the number of particles per event
    G4int numParticles = 1;

    // Instantiate the particle gun with args
    fParticleGun = new G4ParticleGun(numParticles);
    
    // TEST ...     (NOTE: Could also pass this into constructor in main() ...) Not sure if this will even work like it does in stepping action, initialise hasnt been called yet
    // const auto detConst = static_cast<const DetectorConstruction*>(
    //     G4RunManager::GetRunManager()->GetUserDetectorConstruction()
    // );
    // G4double z = detConst->GetSourceDetectorDist();
    // TEST ...

    // Define the position of the particle
    G4double x = 0. * cm;
    G4double y = 0. * cm;
    G4double z = -1. * cm; // small 1cm offset for visibility
    // TODO: This does not reflect position of source geometry and encapsulation
    // Seems janky to manually define it as (5.90425 * cm) - sourceDetectorDist though
    // G4double z = (5.90425 * cm) - 3. * cm; // NOTE: See DetectorConstruction.cc notes

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
     * 
     * NOTE: Particle definition is moved from constructor to this method with decay
     */

    // Add a clause to check if particle has already been set, else set it at the start of the event before calling generate
    // NOTE: Default particle is geantino if nothing has been set, this changes default to 137Cs
    if (fParticleGun->GetParticleDefinition()->GetParticleName() == "geantino") {
        // Define the isotope
        
        // Cesium (137Cs) (Half life ~30y) (B-)
        G4int Z = 55; // Atomic number (num protons)
        G4int A = 137; // Molecular mass (integer, not exact)
        
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
    }
    
    // Create vertex (shoot particle), handing over the event
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

// NOTE: Cannot hand this class directly to the run manager as with the others,
// must hand this to "ActionInitialization" which will use it
