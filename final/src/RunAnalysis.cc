// User class interfaces
#include "RunAnalysis.hh"
#include "OutputConfig.hh"
#include "AnalysisRegistry.hh"

// G4 lib
#include "G4AnalysisManager.hh"
#include "G4GenericAnalysisManager.hh"

/*
 * Constructor (NOTE: Currently does nothing)
 * 
 * 
 * > Tree:
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
 * 
 * 
 * TODO: Consider passing RunAction pointer to this classes constructor,
 * or just a pointer to IsMaster variable in RunAction class
 */
RunAnalysis::RunAnalysis() {
   // InitialiseDataStructures(); // NOTE: Creating data structures before run even starts is wasteful
   
   // .......
   // G4cout << "\n\n>>> INSTANTIATING PROGRAM STATE\n\n" << G4endl;
   // auto& instance = OutputConfig::GetInstance();
   // auto instance = OutputConfig::GetInstance();
   // auto& instance = OutputConfig::GetInstance("AAA");
   // G4cout << "\n\n>>>>> ANALYSIS MANAGER:: " << instance.value() << "\n\n" << G4endl;
   // G4cout << "\n\n>>> INSTANTIATED PROGRAM STATE\n\n" << G4endl;
   
   G4cout << "\n\n>>>>> RUN ANALYSIS INSTANTIATED\n\n" << G4endl;
}

// TODO: Extract file handling logic from RunAction
// void RunAnalysis::CreateFile() {} // creates root outfile and opens it
// void RunAnalysis::WriteAndClose() {} // writes stored data to outfile and closes it

/*
 * Private method to initialise histogram and ntuples
 * 
 * NOTE: Called internally at construction
 * 
 * NOTE: Depending on which ntuples are enabled, the IDs are going to completely change,
 * so will break code in EventAnalysis and SteppingAnalysis which relies on IDs, hence
 * need to store IDs in OutputConfig, so EventAnalysis and SteppingAnalysis can query
 * 
 * NOTE: G4AnalysisManager is thread-local, hence need to create ntuples on master, and 
 * all worker threads
 * 
 * NOTE:
 * analysisManager->CreateNtuple // method returns created ntuple ID
 * 
 * analysisManager->CreateNtupleIColumn // method has (ntupleID, name) overload
 * 
 * analysisManager->FinishNtuple // pass this the ID returned by CreateNtuple()
 * 
 * OutputConfig->SetNtupleID // or something like this ...
 * 
 * 
 * TODO: AnalysisRegistry being updated 4 times
 * ^ There is no need to update the AnalysisRegistry on master, and on each worker thread, 
 * since ntuple instantiation order is consistent amongst all threads, once on master would
 * be fine... but then id have to wrap every:
 * ntupleIDs.xyz = { ... };
 * in an IsMaster clause, passing said IsMaster variable or method from RunAction
 * 
 * 
 * TODO: Need to either have an individual ntuple for each bit of data, or group ntuples, and ntuple flags
 * 
 * 
 * TODO: Consider method for each ntuple
 * 
 * 
 * TODO: Maybe implement selective ntuple creation using output config flags
 * ^ may potentially be worth keeping schema consistent though, always enabled, and just have empty ntuples/columns
 * 
 * 
 * TODO: Instead of two publishers, maybe just a "RunStarting" notification publisher,
 * removing pub/sub methods from registry and config
 */
void RunAnalysis::InitialiseDataStructures() {
    // Get a pointer to the singleton analysis manager via the static method
    G4GenericAnalysisManager* iAnalysisManager = G4AnalysisManager::Instance();
    
    // Get a read-only reference to the singleton output config manager
    OutputConfig const& iOutputConfig = OutputConfig::GetInstance();
    
    // Get read-only reference to the output data flags object
    // StateFlags const& outputFlags = iOutputConfig.ReadStateFlags(); // TODO: Unused for now
    
    // Get a mutable reference to the singleton ntuple registry
    AnalysisRegistry& iAnalysisRegistry = AnalysisRegistry::GetInstance();
    
    // Get mutable reference to ntuple id object
    NtupleIDs& ntupleIDs = iAnalysisRegistry.GetNtupleIDs();
    
    // ...
    G4cout << "\n\n>>>>>>>>>>>>>> CREATING DATA STRUCTURES NOW <<<<<<<<<<<<<<<<<<<\n\n" << G4endl;
    
    // ...
    EventDataStructures(iAnalysisManager, ntupleIDs.fEventNtupleIDs);
    
    // ...
    StepDataDetectionStructures(iAnalysisManager, ntupleIDs.fStepDetectionNtupleIDs);
    
    // ...
    StepDataBoundaryAbsorbStructures(iAnalysisManager, ntupleIDs.fStepBoundaryAbsorbNtupleIDs);
    
    // ...
    StepDataBulkAbsorbStructures(iAnalysisManager, ntupleIDs.fStepBulkAbsorbNtupleIDs);
    
    // Notify the EventAnalysis and SteppingAnalysis classes to update their caches
    // if (!IsMaster) { // TODO
    iAnalysisRegistry.NotifyListeners(); // TEST
    iOutputConfig.NotifyListeners();
    // TODO: Instead of two publishers, maybe just a "RunStarting" notification publisher,
    // removing pub/sub methods from registry and config
    // }
    // NOTE: Since EventAnalysis and SteppingAnalysis instances dont exist on master thread,
    // listeners will be empty, so avoid redundant call with master thread check
}

/*
 * Create ntuples that will be updated on a once per-event basis
 * 
 * TODO: Maybe merge these into one ntuple
 * 
 * TODO: Consider another approach:
 * 
 * Create relevant ntuple id object on the stack, return it, and populate reference in InitialiseDataStructures
 * EventNtupleIDs ntupleIds;
 * 
 * NOTE: This would make it easier to only update registry once, as wouldnt have to enclose every ntupleIDs
 * assignment in an IsMaster logic check
 */
void RunAnalysis::EventDataStructures(G4GenericAnalysisManager* analysisManager, EventNtupleIDs& ntupleIDs) {
    
    ////////////////
    // PHOTON COUNTS
    ////////////////
    
    // Store per-event photon detection data in an Ntuple
    
    // if (outputFlags.fDetectionNtuple) {
    // ...
    // ntupleIDs.fDetectionNtuple.fNtupleID = analysisManager->CreateNtuple("EventData", "Detected Photons");
    G4int const ntupleIDDetection = analysisManager->CreateNtuple("EventData", "Detected Photons"); // name, title
    // NOTE: This creates the Ntuple with: ID = 0
    
    // Define columns inside the Ntuple (each entry will contribute a row)
    G4int const ntupleColIDDetection = analysisManager->CreateNtupleIColumn(
        ntupleColIDDetection, // ntuple id
        "NumPhotons" // column name
    );
    // NOTE: I = integer data type
    
    // Mark the definition of the tuple columns as completed
    // analysisManager->FinishNtuple();
    // analysisManager->FinishNtuple(0);
    analysisManager->FinishNtuple(ntupleIDDetection); // ntuple id
    // NOTE: Dont technically need to pass 0 here, automatically finishes working ntuple (ID = 0),
    // however this explicit handling is useful for later modifications to data handling
    
    // Update the registry
    ntupleIDs.fDetectionNtuple = { ntupleIDDetection, ntupleColIDDetection }; // ntuple id, column id
    // }
    // Ensure value is reset to -1 (TODO: Potential future change for multiple runs)
    // else {
        // ntupleIDs.fDetectionCoordsNtuple = -1;
    // }
    
    ///////////////////////
    // DETECTION EFFICIENCY
    ///////////////////////
    
    // Fraction of photons detected in an event compared to total generated in that event
    
    // if (outputFlags.fDetectionFractionNtuple) {
    // ...
    G4int const ntupleIDEventData = analysisManager->CreateNtuple("EventDataOptical", "Detection Efficiency");
    // NOTE: Second call to createNtuple automatically assigns ntuple ID = 1
    
    G4int const ntupleColIDDetectionFraction = analysisManager->CreateNtupleDColumn(ntupleIDEventData, "DetectionEfficiency"); // col id = 0
    // NOTE: D = double (float maybe fine, but double gives increased precision)
    
    // analysisManager->FinishNtuple();
    
    ntupleIDs.fDetectionFractionNtuple = { ntupleIDEventData, ntupleColIDDetectionFraction };
    // }
    
    /////////////////////////
    // BULK ABSORPTION LOSSES
    /////////////////////////
    
    // Fraction of photons lost via bulk absorption in an event compared to total generated
    
    // if (outputFlags.fBulkAbsorbFractionNtuple) {
    // ...
    // G4int const ntupleIDEventData = analysisManager->CreateNtuple("EventInfoBulkAbsorption", "Bulk Absorption Losses");
    G4int const ntupleColIDBulkAbsorbFraction = analysisManager->CreateNtupleDColumn(ntupleIDEventData, "BulkAbsorptionLosses"); // col id = 1
    // analysisManager->FinishNtuple();
    
    ntupleIDs.fBulkAbsorbFractionNtuple = { ntupleIDEventData, ntupleColIDBulkAbsorbFraction };
    // }
    
    ////////////////////////////
    // SURFACE ABSORPTION LOSSES
    ////////////////////////////
    
    // Fraction of photons lost via surface absorption in an event compared to total generated
    
    // if (outputFlags.fBoundaryAbsorbFractionNtuple) {
    // ...
    // G4int const ntupleIDEventData = analysisManager->CreateNtuple("EventInfoSurfaceAbsorption", "Surface Absorption Losses");
    G4int const ntupleColIDBoundaryAbsorbFraction = analysisManager->CreateNtupleDColumn(ntupleIDEventData, "SurfaceAbsorptionLosses"); // col id = 2
    analysisManager->FinishNtuple(ntupleIDEventData);
    
    ntupleIDs.fBoundaryAbsorbFractionNtuple = { ntupleIDEventData, ntupleColIDBoundaryAbsorbFraction };
    // }
}

/*
 * Create ntuples that will be updated on a step-wise basis, for detection events
 * 
 * TODO: Consider merging these into one ntuple
 */
void RunAnalysis::StepDataDetectionStructures(G4GenericAnalysisManager* analysisManager, StepDetectionNtupleIDs& ntupleIDs) {
    //////////////////////////
    // PHOTON DETECTION COORDS
    //////////////////////////

    // Coordinates of photons marked as detected at the photocathode
    
    // ...
    // if (outputFlags.fDetectionCoordsNtuple) {
    // ...
    G4int const ntupleIDDetectionCoords = analysisManager->CreateNtuple("StepDataDetection", "Photon Coordinates"); // name, title
    
    // Detection positions
    G4int const ntupleXColIDDetectionCoords = analysisManager->CreateNtupleDColumn(ntupleIDDetectionCoords, "fX"); // x position of the photon
    G4int const ntupleYColIDDetectionCoords = analysisManager->CreateNtupleDColumn(ntupleIDDetectionCoords, "fY"); // y position of the photon
    G4int const ntupleZColIDDetectionCoords = analysisManager->CreateNtupleDColumn(ntupleIDDetectionCoords, "fZ"); // y position of the photon
    
    analysisManager->FinishNtuple(ntupleIDDetectionCoords);
    
    ntupleIDs.fDetectionCoordsNtuple = {
        ntupleIDDetectionCoords,
        ntupleXColIDDetectionCoords,
        ntupleYColIDDetectionCoords,
        ntupleZColIDDetectionCoords
    };
    // }
    
    ////////////////////////////////
    // DETECTED PHOTON TRACK LENGTHS
    ////////////////////////////////
    
    // Distance travelled by every photon detected (from birth to detection at PC)
    
    // if (outputFlags.fDetectionDistanceNtuple) {
    // ...
    G4int const ntupleIDDetectionMetrics = analysisManager->CreateNtuple("TrackData", "Detected Photon Metrics");
    G4int const ntupleColIDDetectionDistance = analysisManager->CreateNtupleDColumn(ntupleIDDetectionMetrics, "DetectionDistance");
    // analysisManager->FinishNtuple(ntupleIDDetectionMetrics);
    
    ntupleIDs.fDetectionDistanceNtuple = { ntupleIDDetectionMetrics, ntupleColIDDetectionDistance };
    // }
    
    ///////////////////////////////////
    // DETECTED PHOTON TEMPORAL PROFILE
    ///////////////////////////////////
    
    // Time of flight information for detected optical photons (at birth, t=0)
    
    // if (outputFlags.fDetectionTimeOfFlightNtuple) {
    // ...
    // G4int const ntupleIDDetectionMetrics = analysisManager->CreateNtuple("TrackData", "Detected Photon Lifetime");
    G4int const ntupleColIDDetectionTimeOfFlight = analysisManager->CreateNtupleDColumn(ntupleIDDetectionMetrics, "TimeOfFlight");
    analysisManager->FinishNtuple(ntupleIDDetectionMetrics);
    
    ntupleIDs.fDetectionTimeOfFlightNtuple = { ntupleIDDetectionMetrics, ntupleColIDDetectionTimeOfFlight };
    // }
    
    ///////////////////////////////////
    // NO. REFLECTIONS BEFORE DETECTION
    ///////////////////////////////////
    
    // Monitor mean reflections before detection between geometry/material property changes
    
    // if (outputFlags.fDetectionReflectionsNtuple) {
    // ...
    G4int const ntupleIDDetectionReflections = analysisManager->CreateNtuple("ReflectionInfoDetection", "Reflections Before Detection");
    G4int const ntupleColIDDetectionReflections = analysisManager->CreateNtupleIColumn(ntupleIDDetectionReflections, "ReflectionsDetect");
    analysisManager->FinishNtuple(ntupleIDDetectionReflections);
    
    ntupleIDs.fDetectionReflectionsNtuple = { ntupleIDDetectionReflections, ntupleColIDDetectionReflections };
    // }
    
    // TODO: Could group xyz detection, track length detection, time of flight detection, and reflections before detection
    // into one ntuple "DetectionInfo" or such
    
    ///////////////////////////////
    // DETECTION ANGLE OF INCIDENCE
    ///////////////////////////////
    
    // Monitor incidence angle of optical photons when detected at the photocathode
        
    // TODO: ...
    
    /////////////////////////////
    // DETECTED PHOTON WAVELENGTH
    /////////////////////////////
    
    // Monitor where the bulk of detections are coming from in the electromagnetic spectrum
    
    // TODO: ...
    // analysisManager->CreateNtupleDColumn("fWlen"); // wavelength
    
    // ...
    
    // analysisManager->CreateNtupleDColumn("fGlobalTime"); // store the global time at the start of each event
    // NOTE: Could also define momentum, energy, etc ...
}

/*
 * Create ntuples that will be updated on a step-wise basis, for boundary absorption events
 * 
 * TODO: Consider merging these into one ntuple
 */
void RunAnalysis::StepDataBoundaryAbsorbStructures(G4GenericAnalysisManager* analysisManager, StepBoundaryAbsorbNtupleIDs& ntupleIDs) {
    ////////////////////////////////////
    // PHOTON BOUNDARY ABSORPTION COORDS
    ////////////////////////////////////
    
    // ...
    // NOTE: Different to bulk absorption
    
    // if (outputFlags.fBoundaryAbsorbCoordsNtuple) {
    // ...
    G4int const ntupleIDBoundaryAbsorbCoords = analysisManager->CreateNtuple("StepDataAbsorption", "Photon Coordinates");
    
    // Absorption positions
    G4int const ntupleXColIDBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn(ntupleIDBoundaryAbsorbCoords, "aX"); // x position of the photon
    G4int const ntupleYColIDBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn(ntupleIDBoundaryAbsorbCoords, "aY"); // y position of the photon
    G4int const ntupleZColIDBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn(ntupleIDBoundaryAbsorbCoords, "aZ"); // y position of the photon
    
    // Mark the definition of the tuple columns as completed
    analysisManager->FinishNtuple(ntupleIDBoundaryAbsorbCoords);
    
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
}

/*
 * Create ntuples that will be updated on a step-wise basis, for bulk absorption events
 * 
 * TODO: Consider merging these into one ntuple
 */
void RunAnalysis::StepDataBulkAbsorbStructures(G4GenericAnalysisManager* analysisManager, StepBulkAbsorbNtupleIDs& ntupleIDs) {
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
    G4int const ntupleIDBulkAbsorbDistance = analysisManager->CreateNtuple("TrackDataAbsorb", "Bulk Absorbed Photon Track Length");
    G4int const ntupleColIDBulkAbsorbDistance = analysisManager->CreateNtupleDColumn(ntupleIDBulkAbsorbDistance, "AbsorptionDistance");
    analysisManager->FinishNtuple(ntupleIDBulkAbsorbDistance);
    
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
    G4int const ntupleIDBulkAbsorbReflections = analysisManager->CreateNtuple("ReflectionInfoBulkAbsorption", "Reflections Before Bulk Absorption");
    G4int const ntupleColIDBulkAbsorbReflections = analysisManager->CreateNtupleIColumn(ntupleIDBulkAbsorbReflections, "ReflectionsBulkAbsorb");
    analysisManager->FinishNtuple(ntupleIDBulkAbsorbReflections);
    
    ntupleIDs.fBulkAbsorbReflectionsNtuple = { ntupleIDBulkAbsorbReflections, ntupleColIDBulkAbsorbReflections };
    // }
    
    // TODO: Could group xyz detection, track length detection, time of flight detection, and reflections before detection
    // into one ntuple "DetectionInfo" or such
}
