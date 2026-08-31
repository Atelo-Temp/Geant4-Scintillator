#ifndef MyPrimaryGenerator_HH
#define MyPrimaryGenerator_HH

// G4 lib
#include "G4VUserPrimaryGeneratorAction.hh" // base class
#include "G4Types.hh"

// Forward declarations
class G4GeneralParticleSource; // particle generator
class DetectorConstruction;

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

    private:
        // ...
        void SelectSource(DetectorConstruction const* detectorConstruction);
        
        // Assign cesium 137 as gps ion
        void Cesium137Source();
        void Cobalt60Source();
        void Barium133Source();
        void Sodium22Source();
        
        // ...
        void GenerateIsotope(G4int const Z, G4int const A);
        
        // Assign coordinates to gps ion, specify distribution, etc
        void PlaceSource(DetectorConstruction const* detectorConstruction);
        
        // Pointer to particle generator
        G4GeneralParticleSource* fParticleGun = nullptr;
        
        // Logic check to see whether gps has been assinged coordinates etc
        bool fPlaced = false;
};

#endif
