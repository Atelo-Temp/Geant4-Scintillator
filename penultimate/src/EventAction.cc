// User classes
#include "EventAction.hh" // class interface
#include "EventAnalysis.hh"
#include "HitManager.hh"

// G4 Lib
#include "G4Event.hh" // event object

/*
 * Constructor
 * 
 * Takes pointer to the RunAction class and caches it in class property
 * 
 * NOTE: Member initialiser list:
 * ": fRunAction(runAction) {}"
 * 
 * Does same thing as:
 * "{ fRunAction = runAction; }"
 * 
 * TODO: RunAction is unused ...
 */
EventAction::EventAction(RunAction* runAction) {
    fRunAction = runAction;
    
    // ...
    fHitManager = new HitManager();
    
    // Instantiate event analysis manager and cache pointer
    fEventAnalysis = new EventAnalysis(fHitManager);
    
    G4cout << "\n\n>>>>> EVENT ACTION INSTANTIATED\n\n" << G4endl;
}
// EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}

/*
 * Destructor
 * 
 * NOTE: This class owns, and is responsible for, managing the lifetimes of HitManager
 * and EventAnalysis instances
 * 
 * NOTE: RunAction object deletion is not this class responsibility
 */
EventAction::~EventAction() {
    delete fEventAnalysis;
    delete fHitManager;
}

/*
 * Execute at the start of each event
 * 
 * NOTE: Event object param unused
 */
void EventAction::BeginOfEventAction(G4Event const* /*event*/) {
    // Reset counters between events
    fHitManager->ResetCounters();
}

/*
 * Execute at the end of each event
 * 
 * NOTE: Event object param unused
 * 
 * TODO: Make debug dynamic, detect if in interactive or batch mode
 */
void EventAction::EndOfEventAction(G4Event const* /*event*/) {
    // Log particle information
    // fEventAnalysis->LogEventData();
    // NOTE: Disable this if running in batch mode
    
    // ...
    fEventAnalysis->WriteEventData();
}

/*
 * Getter for hit manager instance
 */
HitManager* EventAction::GetHitManagerPtr() const {
    return fHitManager;
}
