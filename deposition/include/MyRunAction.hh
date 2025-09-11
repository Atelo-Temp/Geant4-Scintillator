#ifndef MyRunAction_HH
#define MyRunAction_HH

// ...
#include "G4UserRunAction.hh"  // base class
#include "G4Run.hh" // run object

// class G4Run; // can be used in place of header

// ...
class RunAction : public G4UserRunAction {
    public:
        // Constructor
        RunAction();

        // Destructor
        ~RunAction() override = default;

        // Called at the start of a run (handing pointer to the run object)
        void BeginOfRunAction(const G4Run*) override;

        // Called at the end of a run (handing pointer to the run object)
        void EndOfRunAction(const G4Run*) override;
};

#endif
