// User lib
#include "AnalysisRegistry.hh"

/*
 * Get the singleton instance
 * 
 * NOTE: Lazy initialisation, the ProgramState constructor runs only when execution 
 * hits GetInstance() for the first time
 * 
 * Before calling GetInstance() the local static variable "fInstance" does not 
 * exist, the compiler reserves space for the object in the "data segment" at
 * compile time, but this space is just uninitialised memory
 * 
 * The first time GetInstance() is called, if multiple threads try to initialise
 * the variable at the same time, the runtime forces them to wait until the first
 * thread finishes creating it, then each just returns the instance
 * 
 * On repeated calls to GetInstance(), the active instance will just be returned
 * 
 * NOTE: C++11 language specification guarantees that static local variables are
 * initialised in a thread-safe way
 * 
 * NOTE: Allocates the instance in the data segment (not the stack or heap)
 */
AnalysisRegistry& AnalysisRegistry::GetInstance() {
    // If the instance hasnt been instantiated, do so
    static AnalysisRegistry fInstance;
    // NOTE: This line is only executed ONCE by the very first thread that calls it
    // NOTE: C++ guarantees this initialisation is completely thread-safe
    
    // Every subsequent call skips the creation and just returns the reference
    return fInstance;
}

/*
 * RunAnalysis uses this before BeamOn is called to set ids for created ntuples
 * 
 * NOTE: Returning a reference to the object, which allows property mutation
 */
NtupleIDs& AnalysisRegistry::GetNtupleIDs() {
    return fNtupleIDs;
}

/*
 * Worker threads use this during the run for thread-safe, fast reads
 * 
 * NOTE: Return type is readonly reference to flags to avoid any accidental mutations
 * 
 * NOTE: Const modifier after method name tells compiler that calling this method will 
 * not alter the state of the AnalysisRegistry instance itself
 */
const NtupleIDs& AnalysisRegistry::ReadNtupleIDs() const {
    return fNtupleIDs;
}

/*
 * ...
 */
void AnalysisRegistry::AddListener(RegistryListener* callback) {
    // ...
    fListeners.push_back(callback);
    
    return;
}

/*
 * ...
 */
void AnalysisRegistry::NotifyListeners() const {
    // ...
    for (int i = 0; i < fListeners.size(); i++) {
        fListeners[i]->UpdateRegistryCache();
    }
    
    return;
}
