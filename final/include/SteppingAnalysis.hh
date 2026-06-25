#ifndef MyStepAnalysis_HH
#define MyStepAnalysis_HH

// User classes
#include "EventAnalysis.hh"
#include "AnalysisRegistry.hh"

// G4 lib
#include "G4Step.hh"
#include "G4GenericAnalysisManager.hh"

/*
 * Handles writing output data to ntuples
 */
// class SteppingAnalysis {
class SteppingAnalysis : public RegistryListener { // TEST
    public:
        // Constructor
        SteppingAnalysis();
        
        // Destructor
        ~SteppingAnalysis() = default;
        
        // ...
        void HandleBulkAbsorb(G4Track const* track, EventAnalysis const* fEventAnalysis);
        
        // ...
        void HandleDetection(G4StepPoint const* endPoint, G4Track const* track, EventAnalysis const* fEventAnalysis);
        
        // ...
        void HandleBoundaryAbsorb(G4StepPoint const* endPoint);
        
        // TEST
        void UpdateRegistryCache() override;
        
    private:  
        // Cached pointer to analysis manager singleton
        G4GenericAnalysisManager* fAnalysisManager = nullptr;
};

#endif
