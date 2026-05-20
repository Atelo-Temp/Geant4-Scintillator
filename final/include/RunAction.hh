#ifndef MyRunAction_HH
#define MyRunAction_HH

#include "G4UserRunAction.hh" // base class
#include "G4Run.hh" // run object

class AnalysisManager; // may be better to import
class Timer;

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

    private:
        // Store a pointer to analysis manager, which executes code on construction
        AnalysisManager* fAnalysis = nullptr;
        
        // ...
        Timer* fTimer = nullptr;
};

#endif
