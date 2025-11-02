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

#include "PhysicsList.hh" // Header file containing class interface

// #include "G4EmStandardPhysics.hh" // Standard physics list for electromagnetic interactions
#include "G4EmStandardPhysics_option4.hh" // TODO: See below

#include "G4OpticalPhysics.hh"

#include "G4RadioactiveDecayPhysics.hh" // physics list
#include "G4DecayPhysics.hh" // handling different decay models

#include "G4EmParameters.hh"

// Define the class constructor
PhysicsList::PhysicsList() {
    // Register EM Physics
    // RegisterPhysics(new G4EmStandardPhysics());
    // TODO: Consider G4EmStandardPhysics_option4 better but more computationally expensive EM modelling
    RegisterPhysics(new G4EmStandardPhysics_option4());
    
    // Enable PIXE atomic de-excitation
    G4EmParameters::Instance()->SetPixe(true); // NOTE: Must be called after passing physics list

    // Register scintillation physics
    RegisterPhysics(new G4OpticalPhysics());    
        
    // Register radioactive decay physics
    RegisterPhysics(new G4RadioactiveDecayPhysics());
    
    // Register decay physics
    RegisterPhysics(new G4DecayPhysics());
}

// Define the destructor (optional given default setting in header file)
// PhysicsList::~PhysicsList() {}
