// User classes
#include "OutputConfig.hh"
#include "OutputConfigMessenger.hh"

/*
 * Private constructor
 * 
 * Instantiates the messenger on the heap and caches the pointer
 */
OutputConfig::OutputConfig() {
    fOutputConfigMessenger = new OutputConfigMessenger(*this);
    
    G4cout << "\n\n>>>>> PROGRAM STATE INSTANTIATED\n\n" << G4endl;
}

/*
 * Private destructor
 * 
 * NOTE: Since fInstance is allocated in the "data segment", not the stack or heap,
 * C++ will automatically destroy it when the program exits by calling this destructor
 * 
 * Automatic destruction of OutputConfig at the end of the program triggers destruction
 * of OutputConfigMessenger
 */
OutputConfig::~OutputConfig() {
    delete fOutputConfigMessenger;
}

/*
 * Get the singleton instance
 * 
 * NOTE: Lazy initialisation, the OutputConfig constructor runs only when execution 
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
OutputConfig& OutputConfig::GetInstance() {
    // If the instance hasnt been instantiated, do so
    static OutputConfig fInstance;
    // NOTE: This line is only executed ONCE by the very first thread that calls it
    // NOTE: C++ guarantees this initialisation is completely thread-safe
    
    // Every subsequent call skips the creation and just returns the reference
    return fInstance;
}

/*
 * Messenger uses this before BeamOn to change settings
 * 
 * NOTE: Returning a reference to the object, which allows property mutation
 */
OutputFlags& OutputConfig::GetOutputFlags() {
    return fOutputFlags;
}

/*
 * Worker threads use this during the run for thread-safe, fast reads
 * 
 * NOTE: Return type is readonly reference to flags to avoid any accidental mutations
 * 
 * NOTE: Const modifier after method name tells compiler that calling this method will 
 * not alter the state of the OutputConfig instance itself
 */
const OutputFlags& OutputConfig::ReadOutputFlags() const {
    return fOutputFlags;
}

/*
 * NOTE: For debugging whether one instance is shared amongst all analysis classes/threads
 */
// OutputConfig::OutputConfig(std::string const value) : value_(value) {
//     G4cout << "\n>>> CONSTRUCTING MESSENGER\n\n" << G4endl;
//     fOutputConfigMessenger = new OutputConfigMessenger(*this);
//     G4cout << "\n>>> CONSTRUCTED MESSENGER\n\n" << G4endl;
// }

/*
 * NOTE: For debugging whether one instance is shared amongst all analysis classes/threads
 */
// OutputConfig& OutputConfig::GetInstance(std::string const& value) {
//     G4cout << "\n>>> GETTING INSTANCE\n\n" << G4endl;
//  
//     static OutputConfig fInstance(value);
//     return fInstance;
//     
//     G4cout << "\n>>> GOT INSTANCE\n\n" << G4endl;
// }

/*
 * ...
 */
void OutputConfig::AddListener(OutputConfigListener* listener) {
    // ...
    fListeners.push_back(listener);
    
    return;
}

/*
 * ...
 */
void OutputConfig::NotifyListeners() const {
    // ...
    for (int i = 0; i < fListeners.size(); i++) {
        fListeners[i]->UpdateOutputFlagsCache();
    }
    
    return;
}
