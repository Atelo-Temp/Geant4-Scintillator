#ifndef MyRunAction_HH
#define MyRunAction_HH

// G4 lib
#include "G4UserRunAction.hh" // base class
#include "G4Run.hh" // run object

// Forward declarations
class AnalysisManager; // may be better to import
class Timer;

/*
 * Handles the run object with begin/end of run actions
 * 
 * NOTE: Inherits from G4 abstract base class "G4UserRunAction"
 */
class RunAction : public G4UserRunAction {
    public:
        // Constructor
        RunAction();
        
        // Destructor
        ~RunAction() override;
        
        // Called at the start of a run (handing pointer to the run object)
        void BeginOfRunAction(G4Run const* run) override;
        
        // Called at the end of a run (handing pointer to the run object)
        void EndOfRunAction(G4Run const* run) override;

    private:
        // Store a pointer to analysis manager, which executes code on construction
        AnalysisManager* fAnalysis = nullptr;
        
        // Run timer pointer
        Timer* fTimer = nullptr;
};

#endif
