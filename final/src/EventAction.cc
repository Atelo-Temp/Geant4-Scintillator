// User classes
#include "EventAction.hh" // class interface

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
    
    // Instantiate event analysis manager and cache pointer
    fEventAnalysis = new EventAnalysis();
}
// EventAction::EventAction(RunAction* runAction) : fRunAction(runAction) {}

/*
 * Destructor
 * 
 * NOTE: RunAction object deletion is not this class responsibility
 */
EventAction::~EventAction() {
    delete fEventAnalysis;
}

/*
 * Execute at the start of each event
 * 
 * NOTE: Event object param unused
 */
void EventAction::BeginOfEventAction(G4Event const* /*event*/) {
    // Reset counters between events
    fEventAnalysis->ResetCounters();
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
 * Getter for event analysis instance
 */
EventAnalysis* EventAction::GetEventAnalysisPtr() {
    return fEventAnalysis;
}
