#ifndef MyStepAnalysis_HH
#define MyStepAnalysis_HH

// User classes
// #include "EventAnalysis.hh"
#include "AnalysisRegistry.hh"
#include "OutputConfig.hh" // .....

// G4 lib
#include "G4Step.hh"

// Forward declarations
// struct StepDetectionNtupleIDs;
// struct StepBoundaryAbsorbNtupleIDs;
// struct StepBulkAbsorbNtupleIDs;
class G4GenericAnalysisManager;
class HitManager;

/*
 * Handles writing output data to ntuples
 */
// class SteppingAnalysis {
class SteppingAnalysis : public RegistryListener { // TEST
    public:
        // Constructor
        SteppingAnalysis(HitManager* hitManager);
        
        // Destructor
        ~SteppingAnalysis() override = default;
        
        // ...
        void HandleBulkAbsorb(G4Track const* track, HitManager const* fHitManager) const;
        
        // ...
        void HandleDetection(G4StepPoint const* endPoint, G4Track const* track, HitManager const* fHitManager) const;
        
        // ...
        void HandleBoundaryAbsorb(G4StepPoint const* endPoint) const;
        
        // TEST
        void UpdateRegistryCache() override;
        
    private:  
        // Cached pointer to analysis manager singleton
        G4GenericAnalysisManager* fAnalysisManager = nullptr;
        
        // ...
        HitManager* fHitManager = nullptr;
        
        // ...
        StepDetectionNtupleIDs const* fStepDetectionNtupleIDs = nullptr;
        StepBoundaryAbsorbNtupleIDs const* fStepBoundaryAbsorbNtupleIDs = nullptr;
        StepBulkAbsorbNtupleIDs const* fStepBulkAbsorbNtupleIDs = nullptr;
        
        // ....
        StepDetectionFlags const* fStepDetectionFlags = nullptr;
        StepBoundaryAbsorbFlags const* fStepBoundaryAbsorbFlags = nullptr;
        StepBulkAbsorbFlags const* fStepBulkAbsorbFlags = nullptr;
};

#endif
