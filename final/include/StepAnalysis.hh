#ifndef MyStepAnalysis_HH
#define MyStepAnalysis_HH

// User classes
#include "EventAnalysis.hh"

// G4 lib
#include "G4Step.hh"
#include "G4GenericAnalysisManager.hh"

/*
 * ...
 */
class StepAnalysis {
    public:
        // Constructor
        StepAnalysis();
        
        // Destructor
        ~StepAnalysis() = default;
        
        // ...
        void HandleBulkAbsorb(G4Track* track, EventAnalysis* fEventAnalysis);
        
        // ...
        void HandleDetection(G4StepPoint* endPoint, G4Track* track, EventAnalysis* fEventAnalysis);
        
        // ...
        void HandleBoundaryAbsorb(G4StepPoint* endPoint);
        
    private:  
        // Cached pointer to analysis manager singleton
        G4GenericAnalysisManager* fAnalysisManager = nullptr;
};

#endif
