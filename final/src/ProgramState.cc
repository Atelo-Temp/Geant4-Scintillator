// User classes
#include "ProgramState.hh"

/*
 * The first time GetInstance() is called, the storage location is locked before
 * checking if instance is nullptr, then an instance is initialised if so, and 
 * finally the instantiated instance is returned
 * 
 * On repeated calls to GetInstance(), the active instance will just be returned
 */
ProgramState* ProgramState::GetInstance() {
    // Thread safety via mutex lock
    std::lock_guard<std::mutex> lock(fMutex);
    
    // If the instance hasnt been instantiated, do so
    if (!fInstance) fInstance = new ProgramState();
    
    return fInstance;
}
