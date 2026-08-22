#ifndef MyDetectorMessenger_HH
#define MyDetectorMessenger_HH

// G4 lib
#include "G4UImessenger.hh"
#include <G4UIcommand.hh>
#include <unordered_map>

// forward declarations
class DetectorConstruction;

/*
 * ...
 */
class DetectorMessenger : public G4UImessenger {
    public:
        // Constructor
        DetectorMessenger(DetectorConstruction* detCon);
        
        // Destructor
        ~DetectorMessenger() override;
        
        // 
        void SetNewValue(G4UIcommand* cmd, G4String value) override;
        
    private:
        DetectorConstruction* fDetectorConstruction;
        
        // std::unordered_map<std::string, G4UIcommand*> fCommands;
        std::unordered_map<G4UIcommand*, std::string> fCommands;
        // std::unordered_set<G4UIcommand*> fCommands;
    
};

#endif
