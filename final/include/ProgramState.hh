#ifndef MyProgramState_HH
#define MyProgramState_HH

// Forward declarations
class ProgramStateMessenger;
// NOTE: ProgramState class only holds a pointer to a ProgramStateMessenger instance on the data segment,
// since a pointer has a fixed memory size, there is no need to import here, import in ".cc" file
// NOTE: Since both ProgramState and ProgramStateMessenger hold references to oneanother, importing
// would also create circular dependencies

/*
 * Flags to control program output
 * 
 * NOTE: These will be set prior to the run, so dont need to worry about mutex for changing flags
 * 
 * During the run, they will only be accessed in readonly mode
 */
struct StateFlags {
    // Event flags
    bool fDetectionNtuple = true; // Per-event detections
    bool fDetectionFractionNtuple = true; // Per-event detections fraction
    bool fBoundaryAbsorbFractionNtuple = true; // Per-event boundary absorptions fraction
    bool fBulkAbsorbFractionNtuple = true; // Per-event bulk absorptions fraction
    
    // Step detection flags
    bool fDetectionCoordsNtuple = true;
    bool fDetectionDistanceNtuple = true;
    bool fDetectionTimeOfFlightNtuple = true;
    bool fDetectionReflectionsNtuple = true;
    
    // Step boundary absorption flags
    // bool fBoundaryAbsorbNtuple = true; // NOTE: Not writing per-event boundary absorption counts
    bool fBoundaryAbsorbCoordsNtuple = true;
    // bool fBoundaryAbsorbDistanceNtuple = true; // NOTE: Not yet implemented
    // bool fBoundaryAbsorbTimeOfFlightNtuple = true; // NOTE: Not yet implemented
    // bool fBoundaryAbsorbReflectionsNtuple = true;  // NOTE: Not yet implemented
    
    // Step bulk absorption flags
    // bool fBulkAbsorbNtuple = true; // NOTE: Not writing per-event bulk absorption counts
    // bool fBulkAbsorbCoordsNtuple = true; // NOTE: Not yet implemented
    bool fBulkAbsorbDistanceNtuple = true;
    // bool fBulkAbsorbTimeOfFlightNtuple = true; // NOTE: Not yet implemented
    bool fBulkAbsorbReflectionsNtuple = true;
};

/*
 * Singleton object responsible for:
 * - Storing program state flags
 * - Instantiating program state messenger (which exposes state flags to ui)
 * 
 * Singleton class defines the "GetInstance" method that serves as an alternative to the constructor,
 * and lets clients access the same instance of this class over and over again
 * 
 * NOTE: Meyers singleton (thread-safe) implementation eliminates need for mutex locking, while
 * remaining completely thread-safe
 * 
 * NOTE: Meyers singleton also means "delete" does not ever need to be explicitly called
 */
class ProgramState {
    public:
        // Delete copy constructor (Singletons should not be cloneable)
        // ProgramState(ProgramState& other) = delete;
        ProgramState(ProgramState const&) = delete;
        
        // Delete assignment operator (Singletons should not be assignable)
        // void operator=(ProgramState const&) = delete;
        ProgramState& operator=(ProgramState const&) = delete;
        
        // Static method controls access to singleton instance (get instance if exists, else instantiate)
        static ProgramState& GetInstance();
        
        // Business logic
        //
        // Messenger uses this before BeamOn to change settings
        StateFlags& GetStateFlags();
        // NOTE: Returning a reference to the object, which allows property mutation
        //
        // Worker threads use this during the run for thread-safe, fast reads
        const StateFlags& ReadStateFlags() const;
        // NOTE: Return type is readonly reference to flags
        // NOTE: Const modifier after method name tells compiler that calling this method will not alter 
        // the state of the ProgramState instance itself
        
    protected:
        // Business logic
        // 
        // Stack allocated config object
        StateFlags fStateFlags;
        //
        // Messenger which exposes config control to ui
        ProgramStateMessenger* fProgramStateMessenger = nullptr; // messenger instantiated and attached during construction
        // NOTE: Since ProgramStateMessenger needs to be instantiated, it makes sense to do so
        // in ProgramState constructor, since the two are linked, then have ProgramState control
        // ProgramStateMessenger's lifetime
    
    private:
        // Constructor, private to prevent direct construction via "new" operator, only self callable
        ProgramState();
        
        // Destructor, private to prevent direct destruction call via "delete" operator
        ~ProgramState();
        
        // Pointer to current instance
        // inline static ProgramState* fInstance = nullptr;
        // NOTE: Inline keyword allows for nullptr assignment inside of class definition
};

#endif
