#ifndef MyAnalysisManager_HH
#define MyAnalysisManager_HH

/*
 * Class interface for output data structure initialiser
 * 
 * NOTE: No need to inherit from base class here:
 * 
 * G4AnalysisManager can be accessed anywhere via static method,
 * also G4AnalysisManager constructor cannot be changed
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
};

#endif
