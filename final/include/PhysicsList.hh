// Protect header file to ensure not implemented several times
#ifndef MyPhysicsList_HH
#define MyPhysicsList_HH

// G4 lib
#include "G4VModularPhysicsList.hh" // Base class to inherit from

/*
 * Define physics list class (inheriting from abstract class, using colon notation)
 */
class PhysicsList : public G4VModularPhysicsList {
    public:
        // Constructor (must be defined in ".cc" file)
        PhysicsList();

        // Destructor
        ~PhysicsList() override = default; // Must be marked with override
        // NOTE: Marking as default means no implementation in ".cc" file needed
        
        // TEST ...
        void SetCuts() override;
};

#endif
