/*
 * Mandatory user initialisation class (no default behaviour provided)
 * NOTE: G4RunManager will check for existance of mondatory classes when Initialize() and BeamOn() are invoked
 *
 * Derived from abstract base class: G4VUserPhysicsList
 *
 * Requires the user to define:
 * - The particles to be used in the simulation
 * - All the physics processes to be simulated
 * - The range cut-off parameter
 *
 * Simplest example of PhysicsList() required to build a simulation program:
*/

#include "MyPhysicsList.hh" // Header file containing class interface

#include "G4EmStandardPhysics.hh" // Standard physics list for electromagnetic interactions

// Define the class constructor
PhysicsList::PhysicsList() {
    // Register EM Physics
    RegisterPhysics(new G4EmStandardPhysics());
}

// Define the destructor (optional given default setting in header file)
// PhysicsList::~PhysicsList() {}
