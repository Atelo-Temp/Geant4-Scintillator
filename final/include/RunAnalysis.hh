#ifndef MyRunAnalysis_HH
#define MyRunAnalysis_HH

// User lib
// #include "AnalysisRegistry.hh"

// Forward declarations
class G4GenericAnalysisManager;
struct EventDataNtupleIDs;
struct EventStatsNtupleIDs;
struct StepDetectionNtupleIDs;
struct StepBoundaryAbsorbNtupleIDs;
struct StepBulkAbsorbNtupleIDs;

/*
 * Class interface for output data structure initialiser
 * 
 * NOTE: No need to inherit from base class here:
 * 
 * G4AnalysisManager singleton can be accessed anywhere via static method, also the
 * G4AnalysisManager constructor cannot be changed
 */
class RunAnalysis {
    public:
        // Constructor
        RunAnalysis();
        
        // Destructor
        ~RunAnalysis() = default;
        
        // Should only be called on construction of this class (not accessible outside class)
        void InitialiseDataStructures();
        
    private:
        // ...
        void EventDataStructures(G4GenericAnalysisManager* analysisManager, EventDataNtupleIDs& ntupleIDs);
        
        // ...
        void EventStatsStructures(G4GenericAnalysisManager* analysisManager, EventStatsNtupleIDs& ntupleIDs);
        
        // ...
        void StepDataDetectionStructures(G4GenericAnalysisManager* analysisManager, StepDetectionNtupleIDs& ntupleIDs);
        
        // ...
        void StepDataBoundaryAbsorbStructures(G4GenericAnalysisManager* analysisManager, StepBoundaryAbsorbNtupleIDs& ntupleIDs);
        
        // ...
        void StepDataBulkAbsorbStructures(G4GenericAnalysisManager* analysisManager, StepBulkAbsorbNtupleIDs& ntupleIDs);
};

#endif
