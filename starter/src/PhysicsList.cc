// Mandatory user initialisation class (no default behaviour provided)
// NOTE: G4RunManager will check for existance of mondatory classes when Initialize() and BeamOn() are invoked

// Derived from abstract base class: G4VUserPhysicsList

// Requires the user to define:
// - The particles to be used in the simulation
// - All the physics processes to be simulated
// - The range cut-off parameter

// Simplest example of PhysicsList() required to build a simulation program

#include "PhysicsList.hh"

// A class derived from G4VUserPhysicsList must be implemented with the following methods:
// - ConstructParticle() for construction of particles
// - ConstructProcess() to construct processes and register them to particles

// The following may also be overridden:
// - SetCuts() to set a range cut value for all particles

//////////////////////////
// Constructing Particles:
//////////////////////////

// Particles can be anything from ordinary particles, such as electrons, to quarks
// They are orgnanised into their six major categories (i.e. lepton, meson, ...)

// Each particle is represented by its own class (derived from G4ParticleDefinition)
// The base class has read-only properties such as name, mass, charge, spin, etc...
// NOTE: Particle classes are found in: geant4/source/particles/


// Construct Particle should call all static member functions for the required particles
void MyPhysicsList::ConstructParticle() {
    // Constructing a proton and a geantino
    G4Proton::ProtonDefinition();
    G4Geantino::GeantinoDefinition();
    // NOTE: Geantino is a virtual particles used in simulation which does not interact with materials
}


// Due to the large number of Geant4 particles, particles from an entire category may be included
// i.e. G4BosonConstructor, G4LeptonConstructor, etc...

// void PhysicsList::ConstructLeptons() {
    // Construct all leptons
//     G4LeptonConstructor pConstructor;
//     pConstructor.ConstructParticle();
// }


//////////////////////////
// Constructing Processes:
//////////////////////////

// ...

// ! Got no clue on this shit ...


////////////////
// Setting Cuts:
////////////////

// SetCuts specifies a distance (range cut-off) which is internally converted to an energy for materials
// Hence the particle with threshold energy stops after travelling the range cut distance

// Default implementation of SetCuts() provides default cut value
// The default value is 0.7 mm for most of the built-in physics list

// ! Kinda get this but deffo need to revisit this chapter ...



// ..... Was kinda hoping to do the literal bare minimum functional program (i.e. precursors to B1) ...
// But it seems the UserGuide isnt sequential, the examples are unrelated

