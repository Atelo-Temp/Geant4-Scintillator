#ifndef MyActionInitialisation_HH
#define MyActionInitialisation_HH

// G4 lib
#include "G4VUserActionInitialization.hh"

/*
 * Mandatory user initialisation class (no default behaviour provided)
 * 
 * NOTE: Inherits from the abstract base class "G4VUserActionInitialization"
 */
class ActionInitialization : public G4VUserActionInitialization {
    public:
        // Constructor
        ActionInitialization() = default;

        // Destructor
        ~ActionInitialization() override = default;

        // For multithreading mode (master thread)
        void BuildForMaster() const override;

        // Single threaded
        void Build() const override;
};

#endif
