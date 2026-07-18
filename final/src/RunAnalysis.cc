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
    // ...
    G4int const ntupleIdEventData = analysisManager->CreateNtuple("EventData", "Per-event Statistics"); // name, title
    // NOTE: This creates the Ntuple with: ID = 0
    
    // Update the registry
    ntupleIDs.fNtupleID = ntupleIdEventData;
    
    ////////////////
    // PHOTON COUNTS
    ////////////////
    
    // Store per-event photon detection data in an Ntuple
    
    // if (outputFlags.fDetectionNtuple) {
    // ...
    
    // Define columns inside the Ntuple (each entry will contribute a row)
    ntupleIDs.fDetectionNtuple.fColumnID = analysisManager->CreateNtupleIColumn(
        ntupleIdEventData, // ntuple id
        "NumPhotons" // column name
    );
    // NOTE: I = integer data type
    
    // Ensure value is reset to -1 (TODO: Potential future change for multiple runs)
    // else {
        // ntupleIDs.fDetectionNtuple.fColumnID = -1;
    // }
    
    ///////////////////////
    // DETECTION EFFICIENCY
    ///////////////////////
    
    // Fraction of photons detected in an event compared to total generated in that event
    
    // if (outputFlags.fDetectionFractionNtuple) {
    // ...
    ntupleIDs.fDetectionFractionNtuple.fColumnID = analysisManager->CreateNtupleDColumn(ntupleIdEventData, "DetectionEfficiency"); // col id = 0
    // NOTE: D = double (float maybe fine, but double gives increased precision)
    // }
    
    /////////////////////////
    // BULK ABSORPTION LOSSES
    /////////////////////////
    
    // Fraction of photons lost via bulk absorption in an event compared to total generated (Bulk Absorption Losses)
    
    // if (outputFlags.fBulkAbsorbFractionNtuple) {
    // ...
    ntupleIDs.fBulkAbsorbFractionNtuple.fColumnID = analysisManager->CreateNtupleDColumn(ntupleIdEventData, "BulkAbsorptionLosses"); // col id = 1
    // }
    
    ////////////////////////////
    // SURFACE ABSORPTION LOSSES
    ////////////////////////////
    
    // Fraction of photons lost via surface absorption in an event compared to total generated (SurfaceAbsorptionLosses")
    
    // if (outputFlags.fBoundaryAbsorbFractionNtuple) {
    // ...
    ntupleIDs.fBoundaryAbsorbFractionNtuple.fColumnID = analysisManager->CreateNtupleDColumn(ntupleIdEventData, "SurfaceAbsorptionLosses"); // col id = 2
    // }
    
    // Mark the definition of the tuple columns as completed
    analysisManager->FinishNtuple(ntupleIdEventData); // ntuple id
    // NOTE: Dont technically need to pass 0 here, automatically finishes working ntuple (ID = 0),
    // however this explicit handling is useful for later modifications to data handling
}

/*
 * Create ntuples that will be updated on a step-wise basis, for detection events
 * 
 * TODO: Consider merging these into one ntuple
 */
void RunAnalysis::StepDataDetectionStructures(G4GenericAnalysisManager* analysisManager, StepDetectionNtupleIDs& ntupleIDs) {
    // ...
    G4int const ntupleIdDetectionData = analysisManager->CreateNtuple("StepDataDetection", "Detected Photon Metrics"); // name, title
    // NOTE: Second call to createNtuple automatically assigns ntuple ID = 1
    
    // ...
    ntupleIDs.fNtupleID = ntupleIdDetectionData;
    
    //////////////////////////
    // PHOTON DETECTION COORDS
    //////////////////////////

    // Coordinates of photons marked as detected at the photocathode
    
    // ...
    // if (outputFlags.fDetectionCoordsNtuple) {
    // ...
    
    // Detection positions
    G4int const xColumnIdDetectionCoords = analysisManager->CreateNtupleDColumn(ntupleIdDetectionData, "fX"); // x position of the photon
    G4int const yColumnIdDetectionCoords = analysisManager->CreateNtupleDColumn(ntupleIdDetectionData, "fY"); // y position of the photon
    G4int const zColumnIdDetectionCoords = analysisManager->CreateNtupleDColumn(ntupleIdDetectionData, "fZ"); // y position of the photon
    
    // ...
    ntupleIDs.fDetectionCoordsNtuple = {
        xColumnIdDetectionCoords,
        yColumnIdDetectionCoords,
        zColumnIdDetectionCoords
    };
    // }
    
    ////////////////////////////////
    // DETECTED PHOTON TRACK LENGTHS
    ////////////////////////////////
    
    // Distance travelled by every photon detected (from birth to detection at PC)
    
    // if (outputFlags.fDetectionDistanceNtuple) {
    // ...
    ntupleIDs.fDetectionDistanceNtuple.fColumnID = analysisManager->CreateNtupleDColumn(ntupleIdDetectionData, "DetectionDistance");
    // }
    
    ///////////////////////////////////
    // DETECTED PHOTON TEMPORAL PROFILE
    ///////////////////////////////////
    
    // Time of flight information for detected optical photons (at birth, t=0) (Detected Photon Lifetime)
    
    // if (outputFlags.fDetectionTimeOfFlightNtuple) {
    // ...
    ntupleIDs.fDetectionTimeOfFlightNtuple.fColumnID = analysisManager->CreateNtupleDColumn(ntupleIdDetectionData, "DetectionTimeOfFlight");
    // }
    
    ///////////////////////////////////
    // NO. REFLECTIONS BEFORE DETECTION
    ///////////////////////////////////
    
    // Monitor mean reflections before detection between geometry/material property changes
    
    // if (outputFlags.fDetectionReflectionsNtuple) {
    // ...
    ntupleIDs.fDetectionReflectionsNtuple.fColumnID = analysisManager->CreateNtupleIColumn(ntupleIdDetectionData, "DetectionReflections");
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
    
    // TODO: Bulk absorb / boundary absorb wavelengths ??
    
    // ...
    
    // analysisManager->CreateNtupleDColumn("fGlobalTime"); // store the global time at the start of each event
    // NOTE: Could also define momentum, energy, etc ...
    
    // ...
    analysisManager->FinishNtuple(ntupleIdDetectionData);
}

/*
 * Create ntuples that will be updated on a step-wise basis, for boundary absorption events
 * 
 * TODO: Consider merging these into one ntuple
 */
void RunAnalysis::StepDataBoundaryAbsorbStructures(G4GenericAnalysisManager* analysisManager, StepBoundaryAbsorbNtupleIDs& ntupleIDs) {
    // ...
    G4int const ntupleIdBoundaryAbsorb = analysisManager->CreateNtuple("StepDataBoundaryAbsorption", "Boundary Absorbed Photon Metrics"); // name, title
    
    // ...
    ntupleIDs.fNtupleID = ntupleIdBoundaryAbsorb;
    
    ////////////////////////////////////
    // PHOTON BOUNDARY ABSORPTION COORDS
    ////////////////////////////////////
    
    // ...
    // NOTE: Different to bulk absorption
    
    // if (outputFlags.fBoundaryAbsorbCoordsNtuple) {
    // ...
    
    // Absorption positions
    G4int const xColumnIdBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn(ntupleIdBoundaryAbsorb, "aX"); // x position of the photon
    G4int const yColumnIdBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn(ntupleIdBoundaryAbsorb, "aY"); // y position of the photon
    G4int const zColumnIdBoundaryAbsorbCoords = analysisManager->CreateNtupleDColumn(ntupleIdBoundaryAbsorb, "aZ"); // y position of the photon
    
    // ...
    ntupleIDs.fBoundaryAbsorbCoordsNtuple = {
        xColumnIdBoundaryAbsorbCoords,
        yColumnIdBoundaryAbsorbCoords,
        zColumnIdBoundaryAbsorbCoords
    };
    // }
        
    /////////////////////////////////////////////
    // NO. REFLECTIONS BEFORE BOUNDARY ABSORPTION
    /////////////////////////////////////////////
    
    // TODO: Maybe reflections before surface absorption too, but imo less pressing
    
    // ...
    
    // ...
    // Mark the definition of the tuple columns as completed
    analysisManager->FinishNtuple(ntupleIdBoundaryAbsorb);
}

/*
 * Create ntuples that will be updated on a step-wise basis, for bulk absorption events
 * 
 * TODO: Consider merging these into one ntuple
 */
void RunAnalysis::StepDataBulkAbsorbStructures(G4GenericAnalysisManager* analysisManager, StepBulkAbsorbNtupleIDs& ntupleIDs) {
    // ...
    G4int const ntupleIdBulkAbsorb = analysisManager->CreateNtuple("StepDataBulkAbsorption", "Bulk Absorbed Photon Metrics"); // name, title
    
    // ...
    ntupleIDs.fNtupleID = ntupleIdBulkAbsorb;
    
    ////////////////////////////////
    // PHOTON BULK ABSORPTION COORDS
    ////////////////////////////////
    
    // TODO: X, Y, Z coords of bulk absorption? See where its accumulating ?
    
    // ...
    
    /////////////////////////////////////
    // BULK ABSORBED PHOTON TRACK LENGTHS
    /////////////////////////////////////
    
    // Distance travelled by photons lost to bulk absorption in the crystal (or grease/pmt window) (Bulk Absorbed Photon Track Length)
    
    // if (outputFlags.fBulkAbsorbDistanceNtuple) {
    // ...
    ntupleIDs.fBulkAbsorbDistanceNtuple.fColumnID = analysisManager->CreateNtupleDColumn(ntupleIdBulkAbsorb, "AbsorptionDistance");
    // }
    
    ////////////////////////////////////////
    // BULK ABSORBED PHOTON TEMPORAL PROFILE
    ////////////////////////////////////////
    
    // TODO: Time of flight for bulk absorbed photons ?
        
    /////////////////////////////////////////
    // NO. REFLECTIONS BEFORE BULK ABSORPTION
    /////////////////////////////////////////
    
    // Monitor mean reflections before bulk absorption between geometry/material property changes (Reflections Before Bulk Absorption)
    
    // if (outputFlags.fBulkAbsorbReflectionsNtuple) {
    // ...
    ntupleIDs.fBulkAbsorbReflectionsNtuple.fColumnID = analysisManager->CreateNtupleIColumn(ntupleIdBulkAbsorb, "ReflectionsBulkAbsorb");
    // }
    
    // ...
    analysisManager->FinishNtuple(ntupleIdBulkAbsorb);
}
