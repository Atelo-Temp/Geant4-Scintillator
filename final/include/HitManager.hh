#ifndef MyHitManager_HH
#define MyHitManager_HH

// C lib
#include <vector>

// G4 lib
#include "G4Types.hh"

/*
 * ...
 */
class HitManager {
    public:
        // Constructor
        HitManager();
        
        // Destructor
        ~HitManager() = default;
        
        // Reset tallys to zero and all reflection map indices values to zero
        void ResetCounters();
        
        // Increment number of photons spawned in this event
        void CountPhoton();
        
        // ...
        G4int GetTotalPhotons() const;
        
        // Increment number of photons incident upon the photocathode
        // ...
        
        // Increment number of photons detected by the photocathode
        void CountDetectedPhoton();
        
        // ...
        G4int GetDetectedPhotons() const;
        
        // Increment number of photons absorbed at a boundary without detection
        // void CountAbsorbedPhoton();
        void CountBoundaryAbsorption();
        
        // ...
        // G4int GetAbsorbedPhotons() const;
        G4int GetBoundaryAbsorptions() const;
        
        // Increment number of photons lost due to bulk absorption
        void CountBulkAbsorption();
        
        // ...
        G4int GetBulkAbsorptions() const;
        
        // Increment number of photons lost due to NoRINDEX
        void CountLostPhoton();
        
        // ...
        G4int GetLostPhotons() const;
        
        // Increment number of reflections a photon has undergone
        void CountReflection(G4int photonIdx);
        
        // Get number of reflections a photon has undergone
        G4int GetReflections(G4int photonIdx) const; // NOTE: Readonly
        
    private:
        // Optical photon tally
        G4int fTotalPhotons = 0;
        
        // Detected photon tally
        G4int fDetectedPhotons = 0;
        
        // Boundary absorbed photon tally
        G4int fAbsorbedPhotons = 0;
        
        // Bulk absorbed photon tally
        G4int fBulkAbsorb = 0;
        
        // Lost photon tally
        G4int fLostPhotons = 0;
        
        // Reflections a photon has undergone
        std::vector<G4int> fReflectionMap = {};
};

#endif
