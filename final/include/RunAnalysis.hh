#ifndef MyRunAnalysis_HH
#define MyRunAnalysis_HH

// Forward declarations
class G4GenericAnalysisManager;
class NtupleIDs;

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
        void EventDataStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs);
        
        // ...
        void StepDataDetectionStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs);
        
        // ...
        void StepDataBoundaryAbsorbStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs);
        
        // ...
        void StepDataBulkAbsorbStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs);
};

#endif
