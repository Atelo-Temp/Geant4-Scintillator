#ifndef MyRunAction_HH
#define MyRunAction_HH

#include "G4UserRunAction.hh" // base class
#include "G4Run.hh" // run object

class AnalysisManager; // may be better to import

// Class inherits from G4 abstract base
class RunAction : public G4UserRunAction {
    public:
        // Constructor
        RunAction();
        
        // Destructor
        ~RunAction() override;
        
        // Called at the start of a run (handing pointer to the run object)
        void BeginOfRunAction(const G4Run*) override;
        
        // Called at the end of a run (handing pointer to the run object)
        void EndOfRunAction(const G4Run*) override;
        
        // To be called in the constructor, for runtime tracking and random seeding
        long GenerateTimestamp();
        
        // To be called at the end of the run, for runtime tracking
        void HandleRuntime();
    private:
        // Store a pointer to analysis manager, which executes code on construction
        AnalysisManager* fAnalysis = nullptr;
        
        // Stores millisecond timestamp corresponding to start of the run
        long fStartTime = -1;
};

#endif
