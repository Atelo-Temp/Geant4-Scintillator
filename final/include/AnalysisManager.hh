#ifndef MyAnalysisManager_HH
#define MyAnalysisManager_HH

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
class AnalysisManager {
    public:
        // Constructor
        AnalysisManager();
        
        // Destructor
        ~AnalysisManager() = default;
        
    private:
        // Should only be called on construction of this class (not accessible outside class)
        void InitialiseDataStructures();
        
        void EventDataStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs);
        
        void StepDataDetectionStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs);
        
        void StepDataBoundaryAbsorbStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs);
        
        void StepDataBulkAbsorbStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs);
};

#endif
