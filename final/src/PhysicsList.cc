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
*/

// User classes
#include "PhysicsList.hh" // Header file containing class interface

// G4 lib
// #include "G4EmStandardPhysics.hh" // Standard physics list for electromagnetic interactions
#include "G4EmStandardPhysics_option4.hh" // TODO: See below
#include "G4OpticalPhysics.hh" // scintillation
#include "G4RadioactiveDecayPhysics.hh" // physics list
#include "G4DecayPhysics.hh" // handling different decay models
// TEST ... PIXE, cuts
#include "G4EmParameters.hh"
#include "G4RegionStore.hh"
#include "G4SystemOfUnits.hh"

/*
 * Constructor
 * 
 * Registers chosen physics
 */
PhysicsList::PhysicsList() {
    // Register EM Physics
    // RegisterPhysics(new G4EmStandardPhysics());
    // TODO: Consider G4EmStandardPhysics_option4 better but more computationally expensive EM modelling
    RegisterPhysics(new G4EmStandardPhysics_option4());
    
    // Enable PIXE atomic de-excitation
    G4EmParameters::Instance()->SetPixe(true); // NOTE: Must be called after passing physics list
    // G4EmParameters::Instance()->SetDeexcitationIgnoreCut(true); // NOTE: Already true
    // TODO: Set cuts to 100 um

    // Register scintillation physics
    RegisterPhysics(new G4OpticalPhysics());    
        
    // Register radioactive decay physics
    RegisterPhysics(new G4RadioactiveDecayPhysics());
    
    // Register decay physics
    RegisterPhysics(new G4DecayPhysics());
}

/*
 * ...
 */
void PhysicsList::SetCuts() {
    // Default production thresholds for world volume
    SetCutsWithDefault();
    
    // Production thresholds for detector regions
    G4String regionName = "Scintillator";
    G4Region* region = G4RegionStore::GetInstance()->GetRegion(regionName);
    auto cuts = new G4ProductionCuts();
    cuts->SetProductionCut(100 * um);
    region->SetProductionCuts(cuts);
}
