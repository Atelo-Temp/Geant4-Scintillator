#ifndef MyEventAnalysis_HH
#define MyEventAnalysis_HH

// User lib
#include "AnalysisRegistry.hh" // RegistryListener
#include "OutputConfig.hh"

// G4 lib
#include "G4GenericAnalysisManager.hh"

// Forward declarations
// struct EventNtupleIDs;
class HitManager;

// TODO: If needed
// struct LostPhotonMap {
//     std::string location;
//     int frequency;
// };

/*
 * Handles writing output data to ntuples
 */
class EventAnalysis : public RegistryListener { // TEST
    public:
        // Constructor
        EventAnalysis(HitManager* hitManager);
        
        // Destructor
        ~EventAnalysis() override = default;
        
        // Write event stats to ntuples (detected photons, and fraction detected, boundary absorbed, and bulk absorbed)
        void WriteEventData() const;
        
        // Print particle information (not for use with batch mode, but handy for single runs via visualiser)
        void LogEventData() const; // NOTE: Readonly
        
        // TEST
        void UpdateRegistryCache() override;
        
    private:
        // Cached pointer to analysis manager singleton
        G4GenericAnalysisManager* fAnalysisManager = nullptr;
        
        // ...
        HitManager const* fHitManager = nullptr;
        
        // TEST ...
        EventNtupleIDs const* fEventNtupleIDs = nullptr;
        
        // ...
        EventFlags const* fEventFlags = nullptr;
};

#endif
