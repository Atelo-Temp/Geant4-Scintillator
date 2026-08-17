#ifndef MyEventAction_HH
#define MyEventAction_HH

// G4 Lib
#include "G4UserEventAction.hh"

// Forward declarations
class RunAction;
class EventAnalysis;
class HitManager;
// NOTE: Sometimes preferred over #include, as it reduces compilation times and 
// prevents cyclic dependencies (when #include'ing, the entire file is copied 
// into this file, if that file #include's other files, those get copied too)

// NOTE: Forward declarations are suitable when the the only interaction with
// said class are via: 
// - pointers (since pointers always have a fixed memory size, as they only 
// store memory addresses, i.e. 8 bytes on 64 bit machine) 
// - references
// - when said class is used as a function return type or argument

// NOTE: The interface below meets this criteria

// NOTE: Explicitly include the file when:
// - Inheriting from XYZ
// - Using XYZ as a value/member variable (i.e., XYZ my_object;), as otherwise 
// the compiler will throw an error since it does not know the size of the
// dependency
// - Calling methods or accessing members of XYZ (i.e., when you need to know 
// what is inside of the class)

/*
 * Handles the event object with begin/end of event actions
 * 
 * NOTE: Extends abstract base class "G4UserEventAction" (preserving access modifiers)
 */
class EventAction : public G4UserEventAction {
    public:
        // Constructor
        EventAction(RunAction* runAction);
        
        // Destructor
        ~EventAction() override;
        
        // Start of event handler
        void BeginOfEventAction(G4Event const* event) override;
        
        // End of event handler
        void EndOfEventAction(G4Event const* event) override;
        
        // Getter for cached event analysis instance pointer
        // EventAnalysis* GetEventAnalysisPtr() const;
        HitManager* GetHitManagerPtr() const;
        
    private:
        // Pointer to current run object
        RunAction* fRunAction = nullptr;
        
        // Pointer to event analysis manager instance
        EventAnalysis* fEventAnalysis = nullptr;
        
        // ...
        HitManager* fHitManager = nullptr;
};

#endif
