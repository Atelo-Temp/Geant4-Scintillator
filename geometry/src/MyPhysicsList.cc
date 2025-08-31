#include "MyPhysicsList.hh"

#include "G4EmStandardPhysics.hh" // standard physics list for electromagnetic interactions

// Define the class constructor
PhysicsList::PhysicsList() {
    // Register EM Physics
    RegisterPhysics(new G4EmStandardPhysics());
}

// Define the constructor (optional given default setting in header file)
// PhysicsList::~PhysicsList() {}
