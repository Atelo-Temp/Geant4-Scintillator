#ifndef MyPrimaryGenerator_HH
#define MyPrimaryGenerator_HH

// G4 lib
#include "G4VUserPrimaryGeneratorAction.hh" // base class
#include "G4GeneralParticleSource.hh"  // particle generator

// Forward declarations
// class G4GeneralParticleSource; // can just specify this without include as is done in examples
// not sure which is best practice, probably actually using .hh ?

/*
 * Handles particle generation
 * 
 * NOTE: Inherits from base class "G4VUserPrimaryGeneratorAction"
 * 
 * In this class the initial state of the primary event must be described
 */
class PrimaryGenerator : public G4VUserPrimaryGeneratorAction {
    public:
        // Constructor
        PrimaryGenerator();

        // Destructor
        ~PrimaryGenerator() override;

        // Override method from base class
        void GeneratePrimaries(G4Event* event) override;
        // Handles object from G4Event
        
        // Assign cesium 137 as gps ion
        void Cesium137Source();

    private:
        // Pointer to particle generator
        G4GeneralParticleSource* fParticleGun = nullptr;
};

#endif
