// User classes
#include "ProgramState.hh"
#include "ProgramStateMessenger.hh"

/*
 * Private constructor
 * 
 * Instantiates the messenger on the heap and caches the pointer
 */
ProgramState::ProgramState() {
    fProgramStateMessenger = new ProgramStateMessenger(); // TODO: If the messenger isnt instantiated until GetInstance() is called, i dont think messenger is going to work ....
                                                        // ^^^^ But if run action calls GetInstance() in its constructor, as it was always going to, i think its fine ??
}

/*
 * Private destructor
 * 
 * NOTE: Since fInstance is allocated in the "data segment", not the stack or heap,
 * C++ will automatically destroy it when the program exits by calling this destructor
 * 
 * Automatic destruction of ProgramState at the end of the program triggers destruction
 * of ProgramStateMessenger
 */
ProgramState::~ProgramState() {
    delete fProgramStateMessenger;
}

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
ProgramState& ProgramState::GetInstance() {
    // If the instance hasnt been instantiated, do so
    static ProgramState fInstance;
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
StateFlags& ProgramState::GetStateFlags() {
    return fStateFlags;
}

/*
 * Worker threads use this during the run for thread-safe, fast reads
 * 
 * NOTE: Return type is readonly reference to flags to avoid any accidental mutations
 * 
 * NOTE: Const modifier after method name tells compiler that calling this method will 
 * not alter the state of the ProgramState instance itself
 */
const StateFlags& ProgramState::ReadStateFlags() const {
    return fStateFlags;
}
