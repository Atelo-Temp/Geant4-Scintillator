#ifndef MyOutputConfig_HH
#define MyOutputConfig_HH

// C lib
// #include <string> // NOTE: For debugging
#include <vector>

// Forward declarations
class OutputConfigMessenger;
// NOTE: OutputConfig class only holds a pointer to a OutputConfigMessenger instance on the data segment,
// since a pointer has a fixed memory size, there is no need to import here, import in ".cc" file
// NOTE: Since both OutputConfig and OutputConfigMessenger hold references to oneanother, importing
// would also create circular dependencies

/*
 * Per-event output data
 */
struct EventFlags {
    bool fDetectionNtuple = true; // Per-event detections
    // bool fBoundaryAbsorbNtuple = true; // NOTE: Not writing per-event boundary absorption counts
    // bool fBulkAbsorbNtuple = true; // NOTE: Not writing per-event bulk absorption counts
    bool fDetectionFractionNtuple = true; // Per-event detections fraction
    bool fBoundaryAbsorbFractionNtuple = true; // Per-event boundary absorptions fraction
    bool fBulkAbsorbFractionNtuple = true; // Per-event bulk absorptions fraction
};

/*
 * Step detection flags
 */
struct StepDetectionFlags {
    bool fDetectionCoordsNtuple = true;
    bool fDetectionDistanceNtuple = true;
    bool fDetectionTimeOfFlightNtuple = true;
    bool fDetectionReflectionsNtuple = true;
};

/*
 * Step boundary absorption flags
 */
struct StepBoundaryAbsorbFlags {
    bool fBoundaryAbsorbCoordsNtuple = true;
    // bool fBoundaryAbsorbDistanceNtuple = true; // NOTE: Not yet implemented
    // bool fBoundaryAbsorbTimeOfFlightNtuple = true; // NOTE: Not yet implemented
    // bool fBoundaryAbsorbReflectionsNtuple = true;  // NOTE: Not yet implemented
};

/*
 * Step bulk absorption flags
 */
struct StepBulkAbsorbFlags {
    // bool fBulkAbsorbCoordsNtuple = true; // NOTE: Not yet implemented
    bool fBulkAbsorbDistanceNtuple = true;
    // bool fBulkAbsorbTimeOfFlightNtuple = true; // NOTE: Not yet implemented
    bool fBulkAbsorbReflectionsNtuple = true;
};

/*
 * Flags to control program output
 * 
 * NOTE: These will be set prior to the run, so dont need to worry about mutex for changing flags
 * 
 * During the run, they will only be accessed in readonly mode
 */
struct OutputFlags {
    // ...
    EventFlags fEventFlags;
    
    // ...
    StepDetectionFlags fStepDetectionFlags;
    
    // ...
    StepBoundaryAbsorbFlags fStepBoundaryAbsorbFlags;
    
    // ...
    StepBulkAbsorbFlags fStepBulkAbsorbFlags;
};

/*
 * Abstract base class interface for OutputConfig notification listeners
 * 
 * NOTE: Cannot be instantiated, only extended
 * 
 * NOTE: C++ generates a default constructor automatically, defined virtual constructor not needed
 * 
 * TODO: Consider making this a separate singleton, or mediator
 */
class OutputConfigListener {
    public:
        // Virtual destructor
        virtual ~OutputConfigListener() = default;
        // NOTE: When you delete a derived class object via a OutputConfigListener*, the program needs to know
        // it must call the derived class destructor first, then the base class destructor. Without a virtual
        // destructor, only the base destructor runs, leaking resources managed by the derived constructor.
        
        // Pure virtual method
        virtual void UpdateOutputFlagsCache() = 0;
        // NOTE: An empty definition here "{}" would mean derived classes are not forced to override the 
        // base method, by setting the method to "= 0", the class cannot be instantiated directly, and 
        // derived classes must override this method in order to be instantiated
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
class OutputConfig {
    public:
        // Delete copy constructor (Singletons should not be cloneable)
        // OutputConfig(OutputConfig& other) = delete;
        OutputConfig(OutputConfig const&) = delete;
        
        // Delete assignment operator (Singletons should not be assignable)
        // void operator=(OutputConfig const&) = delete;
        OutputConfig& operator=(OutputConfig const&) = delete;
        
        // Static method controls access to singleton instance (get instance if exists, else instantiate)
        static OutputConfig& GetInstance();
        // static OutputConfig* GetInstance();
        
        // NOTE: For debugging
        // static OutputConfig& GetInstance(std::string const& value);
        
        // Business logic
        //
        // Messenger uses this before BeamOn to change settings
        OutputFlags& GetOutputFlags();
        // NOTE: Returning a reference to the object, which allows property mutation
        //
        // Worker threads use this during the run for thread-safe, fast reads
        const OutputFlags& ReadOutputFlags() const;
        // NOTE: Return type is readonly reference to flags
        // NOTE: Const modifier after method name tells compiler that calling this method will not alter 
        // the state of the OutputConfig instance itself
        
        // NOTE: For debugging
        // std::string value() const {
        //     return value_;
        // }
        
        // ...
        // void AddListener(std::function<void()> callback);
        void AddListener(OutputConfigListener* listener);
        
        // ...
        void NotifyListeners();
        
    protected:
        // Business logic
        // 
        // Stack allocated config object
        OutputFlags fOutputFlags;
        //
        // Messenger which exposes config control to ui
        OutputConfigMessenger* fOutputConfigMessenger = nullptr; // messenger instantiated and attached during construction
        // NOTE: Since OutputConfigMessenger needs to be instantiated, it makes sense to do so
        // in OutputConfig constructor, since the two are linked, then have OutputConfig control
        // OutputConfigMessenger's lifetime
        
        // NOTE: For debugging
        // std::string value_;
    
    private:
        // Constructor, private to prevent direct construction via "new" operator, only self callable
        OutputConfig();
        
        // NOTE: For debugging
        // OutputConfig(std::string const value);
        
        // Destructor, private to prevent direct destruction call via "delete" operator
        ~OutputConfig();
        
        // Pointer to current instance
        // inline static OutputConfig* fInstance = nullptr;
        // NOTE: Inline keyword allows for nullptr assignment inside of class definition
        
        // ...
        std::vector<OutputConfigListener*> fListeners = {};
};

#endif
