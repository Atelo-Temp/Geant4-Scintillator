#ifndef MyEventAction_HH
#define MyEventAction_HH

// G4 Lib
#include "G4UserEventAction.hh"
#include "G4Types.hh"
#include <vector>

// Forward declaration
class RunAction;
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

// TODO: If needed
// struct LostPhotonMap {
//     std::string location;
//     int frequency;
// };

/*
 * Handles the event object with begin/end of event actions
 */
class EventAction : public G4UserEventAction {
    public:
        // Constructor takes pointer to run object
        EventAction(RunAction* runAction);
        
        // Destructor
        ~EventAction() override = default;
        
        // Start of event handler
        void BeginOfEventAction(G4Event const* event) override;
        
        // End of event handler
        void EndOfEventAction(G4Event const* event) override;
        
        // Increment number of photons spawned in this event
        void CountPhoton();
        
        // Increment number of photons incident upon the photocathode
        // ...
        
        // Increment number of photons detected by the photocathode
        void CountDetectedPhoton();
        
        // Increment number of photons absorbed at a boundary without detection
        void CountAbsorbedPhoton();
        
        // Increment number of photons lost due to bulk absorption
        void CountBulkAbsorption();
        
        // Increment number of photons lost due to NoRINDEX
        void CountLostPhoton();
        
        // Increment number of reflections a photon has undergone
        void CountReflection(G4int photonIdx);
        
        // Get nnumber of reflections a photon has undergone
        G4int GetReflections(G4int photonIdx);
        
        // Print particle information (not for use with batch mode, but handy for single runs via visualiser)
        void Debug();
        
    private:
        // Pointer to current run
        RunAction* fRunAction = nullptr;
        
        // Optical photon tally
        G4int fTotalPhotons = 0;
        
        // Detected photon tally
        G4int fDetectedPhotons = 0;
        
        // Boundary absorbed photon tally
        G4int fAbsorbedPhotons = 0;
        
        // Bulk absorbed photon tally
        G4int fBulkAbsorb = 0;
        
        // Lost photon tally
        G4int fLostPhotons = 0;
        
        // Reflections a photon has undergone
        std::vector<G4int> fReflectionMap = {};
};

#endif
