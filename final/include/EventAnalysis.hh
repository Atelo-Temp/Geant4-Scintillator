#ifndef MyEventAnalysis_HH
#define MyEventAnalysis_HH

// User lib
#include "AnalysisRegistry.hh" // RegistryListener
#include "ProgramState.hh"

// C lib
#include <vector>

// G4 lib
#include "G4Types.hh"
#include "G4GenericAnalysisManager.hh"

// Forward declarations
// struct EventNtupleIDs;

// TODO: If needed
// struct LostPhotonMap {
//     std::string location;
//     int frequency;
// };

/*
 * ...
 */
class EventAnalysis : public RegistryListener { // TEST
    public:
        // Constructor
        EventAnalysis();
        
        // Destructor
        ~EventAnalysis() override = default;
        
        // Reset tallys to zero and all reflection map indices values to zero
        void ResetCounters();
        
        // Write event stats to ntuples (detected photons, and fraction detected, boundary absorbed, and bulk absorbed)
        void WriteEventData() const;
        
        // Increment number of photons spawned in this event
        void CountPhoton();
        
        // Increment number of photons incident upon the photocathode
        // ...
        
        // Increment number of photons detected by the photocathode
        void CountDetectedPhoton();
        
        // Increment number of photons absorbed at a boundary without detection
        void CountAbsorbedPhoton();
        
        // Increment number of photons lost due to bulk absorption
        void CountBulkAbsorption();
        
        // Increment number of photons lost due to NoRINDEX
        void CountLostPhoton();
        
        // Increment number of reflections a photon has undergone
        void CountReflection(G4int photonIdx);
        
        // Get number of reflections a photon has undergone
        G4int GetReflections(G4int photonIdx) const; // NOTE: Readonly
        
        // Print particle information (not for use with batch mode, but handy for single runs via visualiser)
        void LogEventData() const; // NOTE: Readonly
        
        // TEST
        void UpdateRegistryCache() override;
        
    private:
        // Cached pointer to analysis manager singleton
        G4GenericAnalysisManager* fAnalysisManager = nullptr;
        
        // TEST ...
        EventNtupleIDs const* fEventNtupleIDs = nullptr;
        
        // ...
        EventFlags const* fEventFlags = nullptr;
        
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
