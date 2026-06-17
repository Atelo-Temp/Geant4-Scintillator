// User class interfaces
#include "AnalysisManager.hh"

// G4 lib
#include "G4AnalysisManager.hh"

/*
 * Constructor
 * 
 * Calls private method
 */
AnalysisManager::AnalysisManager() {
   InitialiseDataStructures(); 
}

/*
 * Private method to initialise histogram and ntuples
 * 
 * NOTE: Called internally at construction
 */
void AnalysisManager::InitialiseDataStructures() {
    // Get a pointer to the singleton analysis manager via the static method
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Create histogram (one dimension to store deposited energy)
    analysisManager->CreateH1(
        "PhotonsSpectrum", // Name
        "Optical Photons", // Title
        1024, // Number of bins
        0., // Lower bounds (NOTE: EventAction now only writes to histo when photons > 0)
        5000. // Upper bounds (TODO: This needs to be dynamic ... Different sources will produce different amount of optical photons)
        // TODO: 1024. // Upper bounds (need to adjust EventAction to calculate channel instead of photons)
        // G4String& unitName = "none"
    );
    // NOTE: 100 bins from 0->1 MeV
    
    //////////////////////////
    // PHOTON DETECTION COORDS
    //////////////////////////
    
    // ...
    // analysisManager->CreateNtuple("StepData", "Photon Coordinates"); // name, title
    analysisManager->CreateNtuple("StepDataDetection", "Photon Coordinates"); // name, title
    // NOTE: This creates the Ntuple with: ID = 1
    
    // Define columns iniside the Ntuple (each entry will conbtribute a row)
    // analysisManager->CreateNtupleIColumn("iEvent"); // I = integer (event number)
    
    // Detection positions
    analysisManager->CreateNtupleDColumn("fX"); // x position of the photon
    analysisManager->CreateNtupleDColumn("fY"); // y position of the photon
    analysisManager->CreateNtupleDColumn("fZ"); // y position of the photon
    // NOTE: D = double (float maybe fine, but double gives increased precision)
    
    // Mark the definition of the tuple columns as completed
    analysisManager->FinishNtuple(); // NOTE: === FinishNtuple(0);
    // NOTE: Dont need to pass 0 here, automatically finishes ID = 0
    
    // analysisManager->CreateNtupleDColumn("fGlobalTime"); // store the global time at the start of each event
    // analysisManager->CreateNtupleDColumn("fWlen"); // wavelength
    // NOTE: Could also define momentum, energy, etc ...
    
    ////////////////////////////////////
    // PHOTON BOUNDARY ABSORPTION COORDS
    ////////////////////////////////////
    
    // NOTE: Different to bulk absorption
    
    // ...
    // analysisManager->CreateNtuple("StepData", "Photon Coordinates");
    analysisManager->CreateNtuple("StepDataAbsorption", "Photon Coordinates"); // Create Ntuple ID = 1
    // NOTE: Second call to createNtuple automatically assigns ID = 1
    
    // Absorption positions
    analysisManager->CreateNtupleDColumn("aX"); // x position of the photon
    analysisManager->CreateNtupleDColumn("aY"); // y position of the photon
    analysisManager->CreateNtupleDColumn("aZ"); // y position of the photon
    
    // Mark the definition of the tuple columns as completed
    analysisManager->FinishNtuple(); // automatically finishes ID = 1
    
    ////////////////
    // PHOTON COUNTS
    ////////////////
    
    // Store per-event photon detection data in an Ntuple
    analysisManager->CreateNtuple("EventData", "Detected Photons"); // Create Ntuple ID = 2
    analysisManager->CreateNtupleIColumn("NumPhotons"); // Column 0: integer count
    analysisManager->FinishNtuple(); // Finishes ID = 2
    
    ////////////////////////////////
    // DETECTED PHOTON TRACK LENGTHS
    ////////////////////////////////
    
    // Distance travelled by every photon detected (from birth to detection at PC)
    
    // TEST TEST TEST
    analysisManager->CreateNtuple("TrackData", "Detected Photon Track Length"); // ID = 3
    analysisManager->CreateNtupleDColumn("DetectionDistance"); // Column = 0
    // analysisManager->FinishNtuple(); // ID = 3
    // NOTE: Could just add this to Ntuple(0), as its still step based data, just do another column
    // although, this and subsequent Ntuples are more macro track related than micro step related
    
    ///////////////////////////////////
    // DETECTED PHOTON TEMPORAL PROFILE
    ///////////////////////////////////
    
    // Time of flight information for detected optical photons (at birth, t=0)
    
    // TEST TEST TEST
    // analysisManager->CreateNtuple("TrackData", "Detected Photon Lifetime");
    analysisManager->CreateNtupleDColumn("TimeOfFlight"); // Column = 1
    analysisManager->FinishNtuple(); // ID = 3
    // NOTE: Could just add this to Ntuple(0), as its still step based data, just do another column
    
    ////////////////////////////////
    // ABSORBED PHOTON TRACK LENGTHS
    ////////////////////////////////
    
    // Distance travelled by photons lost to bulk absorption in the crystal (or grease/pmt window)
    
    // TEST TEST TEST
    analysisManager->CreateNtuple("TrackDataAbsorb", "Bulk Absorbed Photon Track Length");
    analysisManager->CreateNtupleDColumn("AbsorptionDistance"); // Column = 0
    analysisManager->FinishNtuple(); // ID = 4
    // NOTE: Could just add this to Ntuple(0), as its still step based data, just do another column
    
    ///////////////////////
    // DETECTION EFFICIENCY
    ///////////////////////
    
    // Fraction of photons detected in an event compared to total generated in that event
    
    // TEST TEST TEST
    // analysisManager->CreateNtuple("EventInfo", "Detection Efficiency");
    analysisManager->CreateNtuple("EventDataOptical", "Detection Efficiency");
    analysisManager->CreateNtupleDColumn("DetectionEfficiency"); // Column = 0
    // analysisManager->FinishNtuple(); // ID = 5
    // TEST TEST TEST
    
    /////////////////////////
    // BULK ABSORPTION LOSSES
    /////////////////////////
    
    // Fraction of photons lost via bulk absorption in an event compared to total generated
    
    // TEST TEST TEST
    // analysisManager->CreateNtuple("EventInfo", "Bulk Absorption Losses");
    analysisManager->CreateNtupleDColumn("BulkAbsorptionLosses"); // Column = 1
    // analysisManager->FinishNtuple(); // ID = 5
    // TEST TEST TEST
    
    ////////////////////////////
    // SURFACE ABSORPTION LOSSES
    ////////////////////////////
    
    // Fraction of photons lost via surface absorption in an event compared to total generated
    
    // TEST TEST TEST
    // analysisManager->CreateNtuple("EventInfo", "Bulk Absorption Losses");
    analysisManager->CreateNtupleDColumn("SurfaceAbsorptionLosses"); // Column = 2
    analysisManager->FinishNtuple(); // ID = 5
    // TEST TEST TEST
    
    ///////////////////////////////////
    // NO. REFLECTIONS BEFORE DETECTION
    ///////////////////////////////////
    
    // Monitor mean reflections before detection between geometry/material property changes
    
    // ...
    
    /////////////////////////////////////////
    // NO. REFLECTIONS BEFORE BULK ABSORPTION
    /////////////////////////////////////////
    
    // Monitor mean reflections before bulk absorption between geometry/material property changes
    
    // TODO: Maybe reflections before surface absorption too, but imo less pressing
    
    // ...
    
    ///////////////////////////////
    // DETECTION ANGLE OF INCIDENCE
    ///////////////////////////////
    
    // Monitor incidence angle of optical photons when detected at the photocathode
}

// TODO: Extract file handling logic from RunAction
// void AnalysisManager::CreateFile() {} // creates root outfile and opens it
// void AnalysisManager::WriteAndClose() {} // writes stored data to outfile and closes it
