// User class interfaces
#include "RunAnalysis.hh"
#include "ProgramState.hh"
#include "AnalysisRegistry.hh"

// G4 lib
#include "G4AnalysisManager.hh"
#include "G4GenericAnalysisManager.hh"

/*
 * Constructor
 * 
 * NOTE: Calls private method
 * 
 * Tree:
 * 
 * 
 * RunAction constructor calls:
 *      ↓
 * RunAnalysis constructor
 * 
 * 
 * RunAction
 *         |
 *         ├─ RunTimer
 *         └─ RunAnalysis
 */
RunAnalysis::RunAnalysis() {
   InitialiseDataStructures();
   
   // .......
   // G4cout << "\n\n>>> INSTANTIATING PROGRAM STATE\n\n" << G4endl;
   // auto& instance = ProgramState::GetInstance();
   // auto instance = ProgramState::GetInstance();
   // auto& instance = ProgramState::GetInstance("AAA");
   // G4cout << "\n\n>>>>> ANALYSIS MANAGER:: " << instance.value() << "\n\n" << G4endl;
   // G4cout << "\n\n>>> INSTANTIATED PROGRAM STATE\n\n" << G4endl;
}

/*
 * Private method to initialise histogram and ntuples
 * 
 * NOTE: Called internally at construction
 * 
 * TODO: Maybe method for each ntuple
 * 
 * TODO: Depending on which ntuples are enabled, the IDs are going to completely change,
 * so will break code in EventAnalysis and SteppingAnalysis which relies on IDs
 * ^^
 * ^^ going to need to store IDs
 *    ^^ potentially add them to ProgramState, so EventAnalysis and SteppingAnalysis can query
 * 
 * 
 * TODO:
 * analysisManager->CreateNtuple: returns ID
 * 
 * analysisManager->CreateNtuple[...]Column: has (ntupleId, name) overload
 * 
 * analysisManager->FinishNtuple: pass this returned ID of CreateNtuple()
 * 
 * ProgramState->SetNtupleID (or something like this)
 * 
 * 
 * TODO: Need to either have an individual ntuple for each bit of data, or group ntuples, and ntuple flags
 * 
 * TODO: If grouping, will also need to store column IDs ...
 */
void RunAnalysis::InitialiseDataStructures() {
    // Get a pointer to the singleton analysis manager via the static method
    G4GenericAnalysisManager* iAnalysisManager = G4AnalysisManager::Instance();
    
    // ...
    ProgramState& iProgramState = ProgramState::GetInstance();
    
    // ...
    StateFlags const& outputFlags = iProgramState.ReadStateFlags();
    
    // ...
    AnalysisRegistry& iAnalysisRegistry = AnalysisRegistry::GetInstance();
    
    // ...
    NtupleIDs& ntupleIDs = iAnalysisRegistry.GetNtupleIDs();
    
    // TODO: Am i going to need to delay calling InitialiseDataStructures(), since RunAction is instantiated
    
    G4cout << "\n\n>>>>>>>>>>>>>> CREATING DATA STRUCTURES NOW <<<<<<<<<<<<<<<<<<<\n\n" << G4endl;
    
    // ...
    EventDataStructures(iAnalysisManager, ntupleIDs);
    
    // ...
    StepDataDetectionStructures(iAnalysisManager, ntupleIDs);
    
    // ...
    StepDataBoundaryAbsorbStructures(iAnalysisManager, ntupleIDs);
    
    // ...
    StepDataBulkAbsorbStructures(iAnalysisManager, ntupleIDs);
}

// TODO: Extract file handling logic from RunAction
// void RunAnalysis::CreateFile() {} // creates root outfile and opens it
// void RunAnalysis::WriteAndClose() {} // writes stored data to outfile and closes it

/*
 * ...
 * 
 * TODO: Merge these into one ntuple
 */
void RunAnalysis::EventDataStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs) {
    // ...
    
    ////////////////
    // PHOTON COUNTS
    ////////////////
    
    // Store per-event photon detection data in an Ntuple
    
    // if (outputFlags.fDetectionNtuple) {
        // ...
        G4int ntupleIDDetection = analysisManager->CreateNtuple("EventData", "Detected Photons"); // Create Ntuple ID = 2
        G4int ntupleColIDDetection = analysisManager->CreateNtupleIColumn("NumPhotons"); // Column 0: integer count
        analysisManager->FinishNtuple(ntupleIDDetection); // Finishes ID = 2
        
        ntupleIDs.fDetectionNtuple = { ntupleIDDetection, ntupleColIDDetection };
    // }
        
    ///////////////////////
    // DETECTION EFFICIENCY
    ///////////////////////
    
    // Fraction of photons detected in an event compared to total generated in that event
    
    // if (outputFlags.fDetectionFractionNtuple) {
        // ...
        // analysisManager->CreateNtuple("EventInfoEfficiency", "Detection Efficiency");
        G4int ntupleIDEventData = analysisManager->CreateNtuple("EventDataOptical", "Detection Efficiency");
        G4int ntupleColIDDetectionFraction = analysisManager->CreateNtupleDColumn("DetectionEfficiency"); // Column = 0
        // analysisManager->FinishNtuple(); // ID = 5
        
        ntupleIDs.fDetectionFractionNtuple = { ntupleIDEventData, ntupleColIDDetectionFraction };
    // }
    
    /////////////////////////
    // BULK ABSORPTION LOSSES
    /////////////////////////
    
    // Fraction of photons lost via bulk absorption in an event compared to total generated
    
    // if (outputFlags.fBulkAbsorbFractionNtuple) {
        // ...
        // analysisManager->CreateNtuple("EventInfoBulkAbsorption", "Bulk Absorption Losses");
        G4int ntupleColIDBulkAbsorbFraction = analysisManager->CreateNtupleDColumn("BulkAbsorptionLosses"); // Column = 1
        // analysisManager->FinishNtuple(); // ID = 5
        
        ntupleIDs.fBulkAbsorbFractionNtuple = { ntupleIDEventData, ntupleColIDBulkAbsorbFraction };
    // }
    
    ////////////////////////////
    // SURFACE ABSORPTION LOSSES
    ////////////////////////////
    
    // Fraction of photons lost via surface absorption in an event compared to total generated
    
    // if (outputFlags.fBoundaryAbsorbFractionNtuple) {
        // ...
        // analysisManager->CreateNtuple("EventInfoSurfaceAbsorption", "Surface Absorption Losses");
        G4int ntupleColIDBoundaryAbsorbFraction = analysisManager->CreateNtupleDColumn("SurfaceAbsorptionLosses"); // Column = 2
        analysisManager->FinishNtuple(ntupleIDEventData); // ID = 5
        
        ntupleIDs.fBoundaryAbsorbFractionNtuple = { ntupleIDEventData, ntupleColIDBoundaryAbsorbFraction };
    // }
        
    return;
}

/*
 * ....
 */
void RunAnalysis::StepDataDetectionStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs) {
    // ...
    
    //////////////////////////
    // PHOTON DETECTION COORDS
    //////////////////////////

    // ...
    
    // ...
    // if (outputFlags.fDetectionCoordsNtuple) {
        // ...
        // analysisManager->CreateNtuple("StepData", "Photon Coordinates"); // name, title
        // analysisManager->CreateNtuple("StepDataDetection", "Photon Coordinates"); // name, title
        // ntupleIDs.fDetectionCoordsNtuple = analysisManager->CreateNtuple("StepDataDetection", "Photon Coordinates"); // name, title
        G4int ntupleIDDetectionCoords = analysisManager->CreateNtuple("StepDataDetection", "Photon Coordinates"); // name, title
        // NOTE: This creates the Ntuple with: ID = 1
        
        // Define columns iniside the Ntuple (each entry will conbtribute a row)
        // analysisManager->CreateNtupleIColumn("iEvent"); // I = integer (event number)
        
        // Detection positions
        G4int ntupleXColIDDetectionCoords = analysisManager->CreateNtupleDColumn("fX"); // x position of the photon
        G4int ntupleYColIDDetectionCoords = analysisManager->CreateNtupleDColumn("fY"); // y position of the photon
        G4int ntupleZColIDDetectionCoords = analysisManager->CreateNtupleDColumn("fZ"); // y position of the photon
        // NOTE: D = double (float maybe fine, but double gives increased precision)
        
        // Mark the definition of the tuple columns as completed
        // analysisManager->FinishNtuple(0); // NOTE: === FinishNtuple(0);
        analysisManager->FinishNtuple(ntupleIDDetectionCoords); // NOTE: === FinishNtuple(0);
        // NOTE: Dont need to pass 0 here, automatically finishes ID = 0
        
        // ..
        ntupleIDs.fDetectionCoordsNtuple = { 
            ntupleIDDetectionCoords, 
            ntupleXColIDDetectionCoords, 
            ntupleYColIDDetectionCoords, 
            ntupleZColIDDetectionCoords
        };
    // }
    // Ensure value is reset to -1 (TODO: Potential future change for multiple runs)
    // else {
        // ntupleIDs.fDetectionCoordsNtuple = -1;
    // }
    
    // analysisManager->CreateNtupleDColumn("fGlobalTime"); // store the global time at the start of each event
    // analysisManager->CreateNtupleDColumn("fWlen"); // wavelength
    // NOTE: Could also define momentum, energy, etc ...
    
    ////////////////////////////////
    // DETECTED PHOTON TRACK LENGTHS
    ////////////////////////////////
    
    // Distance travelled by every photon detected (from birth to detection at PC)
    
    // if (outputFlags.fDetectionDistanceNtuple) {
        // ...
        G4int ntupleIDDetectionMetrics = analysisManager->CreateNtuple("TrackData", "Detected Photon Metrics"); // ID = 3
        G4int ntupleColIDDetectionDistance = analysisManager->CreateNtupleDColumn("DetectionDistance"); // Column = 0
        // analysisManager->FinishNtuple(); // ID = 3
        // NOTE: Could just add this to Ntuple(0), as its still step based data, just do another column
        // although, this and subsequent Ntuples are more macro track related than micro step related
        
        ntupleIDs.fDetectionDistanceNtuple = { ntupleIDDetectionMetrics, ntupleColIDDetectionDistance };
    // }
    
    ///////////////////////////////////
    // DETECTED PHOTON TEMPORAL PROFILE
    ///////////////////////////////////
    
    // Time of flight information for detected optical photons (at birth, t=0)
    
    // if (outputFlags.fDetectionTimeOfFlightNtuple) {
        // ...
        // analysisManager->CreateNtuple("TrackData", "Detected Photon Lifetime");
        G4int ntupleColIDDetectionTimeOfFlight = analysisManager->CreateNtupleDColumn("TimeOfFlight"); // Column = 1
        analysisManager->FinishNtuple(ntupleIDDetectionMetrics); // ID = 3
        // NOTE: Could just add this to Ntuple(0), as its still step based data, just do another column
        
        ntupleIDs.fDetectionTimeOfFlightNtuple = { ntupleIDDetectionMetrics, ntupleColIDDetectionTimeOfFlight };
    // }
    
    ///////////////////////////////////
    // NO. REFLECTIONS BEFORE DETECTION
    ///////////////////////////////////
    
    // Monitor mean reflections before detection between geometry/material property changes
    
    // if (outputFlags.fDetectionReflectionsNtuple) {
        // ...
        G4int ntupleIDDetectionReflections = analysisManager->CreateNtuple("ReflectionInfoDetection", "Reflections Before Detection");
        G4int ntupleColIDDetectionReflections = analysisManager->CreateNtupleIColumn("ReflectionsDetect"); // Column = 0
        analysisManager->FinishNtuple(ntupleIDDetectionReflections); // ID = 6
        
        ntupleIDs.fDetectionReflectionsNtuple = { ntupleIDDetectionReflections, ntupleColIDDetectionReflections };
    // }
    
    // TODO: Could group xyz detection, track length detection, time of flight detection, and reflections before detection
    // into one ntuple "DetectionInfo" or such
    
    ///////////////////////////////
    // DETECTION ANGLE OF INCIDENCE
    ///////////////////////////////
    
    // Monitor incidence angle of optical photons when detected at the photocathode
        
    // TODO
    
    return;
}

/*
 * ...
 */
void RunAnalysis::StepDataBoundaryAbsorbStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs) {
    // ...
    
    ////////////////////////////////////
    // PHOTON BOUNDARY ABSORPTION COORDS
    ////////////////////////////////////
    
    // ...
    // NOTE: Different to bulk absorption
    
    // if (outputFlags.fBoundaryAbsorbCoordsNtuple) {
        // ...
        // analysisManager->CreateNtuple("StepData", "Photon Coordinates");
        G4int ntupleIDBoundaryAbsorbCoords = analysisManager->CreateNtuple("StepDataAbsorption", "Photon Coordinates"); // Create Ntuple ID = 1
        // NOTE: Second call to createNtuple automatically assigns ID = 1
        
        // Absorption positions
        G4int ntupleXColIDBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn("aX"); // x position of the photon
        G4int ntupleYColIDBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn("aY"); // y position of the photon
        G4int ntupleZColIDBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn("aZ"); // y position of the photon
        
        // Mark the definition of the tuple columns as completed
        // analysisManager->FinishNtuple(1); // automatically finishes ID = 1
        analysisManager->FinishNtuple(ntupleIDBoundaryAbsorbCoords); // automatically finishes ID = 1
        
        // ...
        ntupleIDs.fBoundaryAbsorbCoordsNtuple = {
            ntupleIDBoundaryAbsorbCoords,
            ntupleXColIDBoundaryAbsorbCoords,
            ntupleYColIDBoundaryAbsorbCoords,
            ntupleZColIDBoundaryAbsorbCoords
        };
    // }
        
    /////////////////////////////////////////////
    // NO. REFLECTIONS BEFORE BOUNDARY ABSORPTION
    /////////////////////////////////////////////
    
    // TODO: Maybe reflections before surface absorption too, but imo less pressing
        
    return;
}

/*
 * ...
 */
void RunAnalysis::StepDataBulkAbsorbStructures(G4GenericAnalysisManager* analysisManager, NtupleIDs& ntupleIDs) {
    // ...
    
    ////////////////////////////////
    // PHOTON BULK ABSORPTION COORDS
    ////////////////////////////////
    
    // TODO: X, Y, Z coords of bulk absorption? See where its accumulating ?
    
    // ...
    
    /////////////////////////////////////
    // BULK ABSORBED PHOTON TRACK LENGTHS
    /////////////////////////////////////
    
    // Distance travelled by photons lost to bulk absorption in the crystal (or grease/pmt window)
    
    // if (outputFlags.fBulkAbsorbDistanceNtuple) {
        // ...
        G4int ntupleIDBulkAbsorbDistance = analysisManager->CreateNtuple("TrackDataAbsorb", "Bulk Absorbed Photon Track Length");
        G4int ntupleColIDBulkAbsorbDistance = analysisManager->CreateNtupleDColumn("AbsorptionDistance"); // Column = 0
        analysisManager->FinishNtuple(ntupleIDBulkAbsorbDistance); // ID = 4
        // NOTE: Could just add this to Ntuple(0), as its still step based data, just do another column
        
        ntupleIDs.fBulkAbsorbDistanceNtuple = { ntupleIDBulkAbsorbDistance, ntupleColIDBulkAbsorbDistance };
    // }
    
    ////////////////////////////////////////
    // BULK ABSORBED PHOTON TEMPORAL PROFILE
    ////////////////////////////////////////
    
    // TODO: Time of flight for bulk absorbed photons ?
        
    /////////////////////////////////////////
    // NO. REFLECTIONS BEFORE BULK ABSORPTION
    /////////////////////////////////////////
    
    // Monitor mean reflections before bulk absorption between geometry/material property changes
    
    // if (outputFlags.fBulkAbsorbReflectionsNtuple) {
        // ...
        G4int ntupleIDBulkAbsorbReflections = analysisManager->CreateNtuple("ReflectionInfoBulkAbsorption", "Reflections Before Bulk Absorption");
        G4int ntupleColIDBulkAbsorbReflections = analysisManager->CreateNtupleIColumn("ReflectionsBulkAbsorb"); // Column = 0
        analysisManager->FinishNtuple(ntupleIDBulkAbsorbReflections); // ID = 7
        
        ntupleIDs.fBulkAbsorbReflectionsNtuple = { ntupleIDBulkAbsorbReflections, ntupleColIDBulkAbsorbReflections };
    // }
    
    // TODO: Could group xyz detection, track length detection, time of flight detection, and reflections before detection
    // into one ntuple "DetectionInfo" or such
        
    return;
}
