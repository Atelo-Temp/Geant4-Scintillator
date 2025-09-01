#ifndef MyActionInitialisation_HH
#define MyActionInitialisation_HH

// Base class to inherit from
#include "G4VUserActionInitialization.hh"

// Define the class, inheriting from the base class
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
