#ifndef MyUserTrackInformation_HH
#define MyUserTrackInformation_HH

// G4 lib
#include "G4VUserTrackInformation.hh"

/*
 * Stores user information associated with a G4Track class object
 * 
 * NOTE: Extends G4 abstract base class "G4VUserTrackInformation"
 * 
 * NOTE: "public" inheritance preserves original modifiers:
 * - public stays public
 * - protected stays protected
 * - private stays private
 * 
 * Omitting "public" defaults to private inheritance, where all methods/members become private
 */
class UserTrackInformation : public G4VUserTrackInformation {
    public:
        // Constructor
        UserTrackInformation() = default;
        
        // Destructor
        ~UserTrackInformation() override = default;
        
        // Increment reflection counter by one
        void CountReflection();
        
        // Retrieve the reflection counter
        G4int GetReflections();
    
    private:
        // Track total number of reflections (all types)
        G4int fReflections = 0;
};

#endif
