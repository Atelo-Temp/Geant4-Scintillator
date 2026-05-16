#ifndef MyPrimaryGenerator_HH
#define MyPrimaryGenerator_HH

#include "G4VUserPrimaryGeneratorAction.hh" // base class
// #include "G4ParticleGun.hh" // particle generator

#include "G4GeneralParticleSource.hh"

// class G4ParticleGun; // can just specify this without include as is done in examples
// not sure which is best practice, probably actually using .hh ?

// Define the class (inheriting from base class)
class PrimaryGenerator : public G4VUserPrimaryGeneratorAction {
    public:
        // Constructor
        PrimaryGenerator();

        // Destructor
        ~PrimaryGenerator() override;

        // Override method from base class
        void GeneratePrimaries(G4Event*) override;
        // Handles object from G4Event

    private:
        // Pointer to particle generator
        // G4ParticleGun* fParticleGun = nullptr;
        G4GeneralParticleSource* fParticleGun = nullptr;
};

#endif
