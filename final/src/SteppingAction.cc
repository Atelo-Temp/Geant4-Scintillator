// User classes
#include "SteppingAction.hh"
// #include "AnalysisManager.hh"
// #include "DetectorConstruction.hh"
// #include "UserTrackInformation.hh" // NOTE: Removed for time being

// G4 Lib
#include "G4OpBoundaryProcess.hh"
#include "G4ProcessVector.hh"
#include "G4StepStatus.hh"
#include "G4VProcess.hh"
// #include "G4ios.hh"
// #include "G4LogicalVolume.hh"
// #include "G4RunManager.hh"
#include "G4ProcessManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "G4AnalysisManager.hh"
// #include "G4VUserTrackInformation.hh" // NOTE: Removed for time being
// #include "G4TrackStatus.hh"
// #include "G4RandomTools.hh" // Random seeding

/*
 * Constructor
 * 
 * TODO: May want to use initialiser list here
 */
SteppingAction::SteppingAction(EventAction* eventAction) {
    fEventAction = eventAction; 
}
// SteppingAction::SteppingAction(EventAction* eventAction) : fEventAction(eventAction) {}

/*
 * Step handler, will execute on each step a particle takes
 */
void SteppingAction::UserSteppingAction(const G4Step* step) {    
    // Get the track object for the current step
    G4Track* track = step->GetTrack();
    
    // TODO: If particle is gamma (for escape x-rays specifically)
    // Check pre and post step touchable logic volume
    // If pre step volume is crystal, and post step is not crystal (reflector, enclosure, air, etc)
    // Get kinetic energy of particle
    // Flag it as an escape x-ray
    // Add to running total escaped energy tracker
    
    // TODO: If particle is gamma (for optical photon temporal profile)
    // set fStartTime to local time
    
    // If particle is not an optical photon, break
    if (track->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) {
        return;
    }
    
    // Find the boundary process only once (cache the pointer)
    if (fBoundary == nullptr) {
        FindBoundary(track); // this->FindBoundary(track);
    }
 
    // Count one optical photon (for total generated, not absorbed, detected, etc)
    // NOTE: Without if clause, when a photon reflects it will be counted twice,
    // could subtract interactions from this value, but this probably a cleaner way
    if (track->GetCurrentStepNumber() == 1) {
        fEventAction->CountPhoton();
    }
    
    // Get the post step point object for the particle
    G4StepPoint* endPoint = step->GetPostStepPoint();
    
    // ...
    G4VProcess const* process = endPoint->GetProcessDefinedStep();
    
    // ...
    if (!process) return; // NOTE: Not sure about this return
    
    
    // TEST ...
    // Get track info object assigned to optical photon at start of its track
    // G4VUserTrackInformation* trackInfo = track->GetUserInformation();
    // auto userTrackInfo = static_cast<UserTrackInformation*>(trackInfo);
    // NOTE: Calling this here as bulk absorb needs it, detection needs it, and reflection needs it
    // TEST ....
    
    
    // >>>>> TEST TEST TEST: Track photons lost via bulk absorption in the crystal (and window/grease to a far lesser extent)
    // If photon was absorbed in medium (not at a boundary)
    // if ((track->GetTrackStatus() == fStopAndKill) && (endPoint->GetStepStatus() != fGeomBoundary)) {
    // if (process && (process->GetProcessName() == "OpAbsorption") && (endPoint->GetStepStatus() != fGeomBoundary)) {
    if ((process->GetProcessName() == "OpAbsorption") && (endPoint->GetStepStatus() != fGeomBoundary)) { // NOTE: not sure the double check for boundary is needed, it would be "OpBoundary" otherwise
        // ..
        // std::string volume = endPoint->GetTouchable()->GetVolume()->GetName();
        // G4cout << "BULK ABSORPTION IN MEDIUM: " << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Scintillator"
        
        // Increment bulk absorption counter
        fEventAction->CountBulkAbsorption();
    
        // TEST TEST TEST
        // Get distance travelled by photon before bulk absorption
        G4double const distance = track->GetTrackLength();
        G4GenericAnalysisManager* analysisManager = G4AnalysisManager::Instance(); // TODO: Maybe call this only once like track info above, most paths below use it
        analysisManager->FillNtupleDColumn(4, 0, distance); // id = 4, column = 0, value = distance travelled
        analysisManager->AddNtupleRow(4); // finish row for Ntuple id = 4
        // G4cout << "Distance Travelled Before Bulk Absorption: " << distance << " mm" << G4endl;
        // TEST TEST TEST
        
        // Retrieve number of reflections
        // G4int const numReflections = userTrackInfo->GetReflections();
        G4int const photonIdx = track->GetTrackID();
        G4int const numReflections = fEventAction->GetReflections(photonIdx);
        analysisManager->FillNtupleIColumn(7, 0, numReflections); // ...
        analysisManager->AddNtupleRow(7); // ...
        
        // No need to proceed, exit early
        // return;
    }
    // TEST TEST TEST 
    
    
    // NOTE: Instead of returning inside one of those code blocks,
    // can just let this next clause do so ....
    
    // NOTE: OpRayleigh, OpAbsorption, OpBoundary, OpScintillation (relevant process names)
    
    // If post step point not at a defined geometric boundary, break
    if (endPoint->GetStepStatus() != fGeomBoundary) return;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////// HANDLE BOUNDARY INTERACTION METHOD \/\/\/\/\/ ???
    
    // if (process->GetProcessName() != "OpBoundary") return; // TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST
    
    // If at a boundary ...
    G4OpBoundaryProcessStatus const boundaryStatus = fBoundary->GetStatus();
    
    // TODO: Timing window
    // if (!initialPEtime) initialPEtime == ...
    // currPEtime = intialPEtime - globalTime
    
    // TODO: Angular QE
    // Compute incidence angle of PE
    // Weight detection probability
    
    // NOTE: Type:
    // G4OpBoundaryProcessStatus::
    // to see all available boundary statuses
    
    // This assumes that the volume causing detection is the photocathode
    // as it is the only volume with non-zero efficiency
    if (boundaryStatus == Detection) {
        // Photon is absorbed but status was detection, manually track it in event tally
        fEventAction->CountDetectedPhoton();
        
        // TODO: May be worth double checking the boundary is the photocathode
        // ...although since its only one with efficiency vector, it will be
        
        // G4cout << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Scoring"
        // auto detectionVolume = endPoint->GetTouchable()->GetVolume()->GetName();
        // if (detectionVolume != "Photocathode") G4cout << detectionVolume << G4endl;
        
        // if (endPoint->GetTouchable()->GetVolume() == fScoringVolume)
        // NOTE: Get fScoringVolume via public method on DetectorConstruction
        
        // Get pointer to analysis manager singleton
        G4GenericAnalysisManager* analysisManager = G4AnalysisManager::Instance();
        
        // Get (x, y, z) coordinates at point where optical photon detected by photocathode
        G4ThreeVector const detectionPosition = endPoint->GetPosition();
        double const x = detectionPosition[0];
        double const y = detectionPosition[1];
        double const z = detectionPosition[2];
        
        // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
        analysisManager->FillNtupleDColumn(0, 0, x); // NtupleID = 0, 0th column, x
        analysisManager->FillNtupleDColumn(0, 1, y); // NtupleID = 0, 1st column, y
        analysisManager->FillNtupleDColumn(0, 2, z); // NtupleID = 0, 2nd column, z
        // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
        
        // Mark this row as complete (for Ntuple with passed ID)
        analysisManager->AddNtupleRow(0); // Ntuple ID = 0
        
        // TEST TEST TEST
        // Get the global time (TODO: For timing window)
        // auto detectTime = endPoint->GetGlobalTime();
        // NOTE: May actually need to store the time at the first detection of a photon for the current event 
        // (which would produce first avalanche in current event, beginning the charge integration process)
        // then compare that to easch subsequent detection
        // i.e.
        // if (!fIntegrationStart) fIntegrationStart = endPoint->GetGlobalTime();
        // else time = endPoint->GetGlobalTime() - fIntegrationStart;
        // NOTE: Would want to store fIntegrationStart in fEventAction btw, and provide a setter function,
        // like fEventAction->CountDetectedPhoton(), as it would need to be nullified at the start of each event,
        // again much like photon counts
        // TEST TEST TEST
        
        // TEST TEST TEST
        // Get distance travelled by photon before detection (from creation to absorption in PC)
        G4double const distance = track->GetTrackLength();
        analysisManager->FillNtupleDColumn(3, 0, distance); // NtupleID = 3, column = 0, val = distance
        // analysisManager->AddNtupleRow(3); // finish row for NtupleID = 3 // NOTE: OMIT THIS IF USING SAME NTUPLE FOR ALL TRACK DATA
        // G4cout << "Distance Travelled Before Detection: " << distance << " mm" << G4endl;
        // TEST TEST TEST
        
        // TEST TEST TEST
        // Get time of flight information (local time gives time since photon birth until now, i.e. birth to detection)
        G4double const time = track->GetLocalTime(); 
        analysisManager->FillNtupleDColumn(3, 1, time); // NtupleID = 3, column = 1, val = time
        analysisManager->AddNtupleRow(3); // Finish row for NtupleID = 3 // NOTE: NOW THAT ALL TRACK DATA WRITTEN, ADD THE ROW
        // NOTE: This method loses relative timing between different photons if they 
        // were created at different points along the primary particles track
        // TEST TEST TEST
        
        // TEST
        // Get custom track info object
        // G4VUserTrackInformation* trackInfo = track->GetUserInformation();
        // auto userTrackInfo = static_cast<UserTrackInformation*>(trackInfo);
        // Retrieve number of reflections
        // G4int const numReflections = userTrackInfo->GetReflections();
        G4int const photonIdx = track->GetTrackID();
        G4int const numReflections = fEventAction->GetReflections(photonIdx);
        analysisManager->FillNtupleIColumn(6, 0, numReflections); // TODO: Maybe clump all "detection" branches together in one ntuple
        analysisManager->AddNtupleRow(6);
        // TEST
        
        // TEST
        // Calculate angle of incidence for detected photon
        // endPoint->GetTouchable()->GetVolume()->GetObjectRotation();
        // G4ThreeVector const trans = endPoint->GetTouchable()->GetVolume()->GetObjectTranslation();
        // trans.angle();
        // trans.
        // TEST
        
        // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
        // producing a new row (linked to the event ID) for each interaction,
        // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
        
        // NOTE: Likely just make this a method of analysis class (as absorption xyz very similar)
    } 
    // If an optical photon is absorbed without detection at a boundary (i.e., reflector or photocathode)
    else if (boundaryStatus == Absorption) {
        // Photon was absorbed without detection
        fEventAction->CountAbsorbedPhoton();
        
        // TODO: Can get absorption volume via same process as above,
        // counts absorptions in crystal vs reflector, vs photocathode
        
        // Get pointer to analysis manager singleton
        G4GenericAnalysisManager* analysisManager = G4AnalysisManager::Instance();
        
        // Get (x, y, z) coordinates at point where optical photon absorbed by reflector or photocathode
        G4ThreeVector const absorptionPosition = endPoint->GetPosition();
        double const x = absorptionPosition[0];
        double const y = absorptionPosition[1];
        double const z = absorptionPosition[2];
        
        // Store this data in the nTuples (create a few rows) (IColumn = int, DColumn = double)
        analysisManager->FillNtupleDColumn(1, 0, x); // NtupleID = 1, 3rd column, x
        analysisManager->FillNtupleDColumn(1, 1, y); // NtupleID = 1, 4th column, y
        analysisManager->FillNtupleDColumn(1, 2, z); // NtupleID = 1, 5th column, z
        // NOTE: Takes tuple ID (0 as we only made one), column number in this row, and the entry
        
        // Mark this row as complete (for Ntuple with passed ID)
        analysisManager->AddNtupleRow(1); // Ntuple ID = 1
        
        // For every photon that enters the detector and interacts, each interaction will call "ProcessHits()",
        // producing a new row (linked to the event ID) for each interaction,
        // i.e. multiple compton scatters inside the detector, for a given photon, will produce rows pertaining to each energy deposit
    } 
    // Check no loss via lack of rindex
    else if (boundaryStatus == G4OpBoundaryProcessStatus::NoRINDEX) {
        // G4cout << G4endl << "OPTICAL PHOTON LOST TO LACK OF RINDEX" << G4endl;
        // G4cout << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Reflector"
        
        // if (endPoint->GetTouchable()->GetVolume()->GetName() != "Reflector") {
        //     G4cout << G4endl << "OPTICAL PHOTON LOST TO LACK OF RINDEX" << G4endl;
        //     G4cout << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Reflector"
        // }
        
        // Photon was killed due to lack of rindex
        fEventAction->CountLostPhoton();
        
        // TODO:
        // std::string volume = endPoint->GetTouchable()->GetVolume()->GetName();
        // fEventAction->CountLostPhoton(volume);
    }
    // Handle all types of reflection
    else if (
        boundaryStatus == G4OpBoundaryProcessStatus::FresnelReflection ||
        boundaryStatus == G4OpBoundaryProcessStatus::TotalInternalReflection ||
        boundaryStatus == G4OpBoundaryProcessStatus::LambertianReflection ||
        boundaryStatus == G4OpBoundaryProcessStatus::LobeReflection ||
        boundaryStatus == G4OpBoundaryProcessStatus::SpikeReflection ||
        boundaryStatus == G4OpBoundaryProcessStatus::BackScattering
    ) {
        // Get custom track info object
        // G4VUserTrackInformation* trackInfo = track->GetUserInformation();
        // auto userTrackInfo = static_cast<UserTrackInformation*>(trackInfo);
        
        // Increment reflection counter
        // userTrackInfo->CountReflection();
        
        // Increment reflection counter
        G4int photonIdx = track->GetTrackID();
        fEventAction->CountReflection(photonIdx);
    }
    // ...
    else if (boundaryStatus == G4OpBoundaryProcessStatus::StepTooSmall) {
        // TODO: ...
    }
    
    // TODO: With lambertian reflection set to 1 at reflector surface,
    // could check if boundary volume is "Reflector", and if so, plot the angle
    // of reflection (to generate a plot of the lambertian distribution)
    // else if (boundaryStatus == LambertianReflection) {}, etc ??
    // Can also do xyz of reflection position, etc
    
    // TODO: Maybe switch case here ^
}

/*
 * Find the boundary process and assign it to the class property "fBoundary"
 * 
 * NOTE: Is there a cleaner way to do this?
 */
void SteppingAction::FindBoundary(G4Track* track) {
    // Get pointer to the process list (for optical photons)
    G4ProcessVector* pv = track->GetDefinition()->GetProcessManager()->GetProcessList();
    
    // Iterate over list of process objects
    for (int i = 0; i < pv->size(); ++i) {
        // Pointer to current non-null process
        G4VProcess* process = (*pv)[i]; 
        // NOTE: Since "pv" is a pointer to a collection, rather than a direct object,
        // it needs to be dereferenced first (*pv), to reveal the actual object in memory,
        // so that processes can be accessed via index
        
        // Prevent attempted access to non-existent method
        if (process == nullptr) continue;
        
        // If the boundary process is found
        // NOTE: i.e. "Transportation", "OpAbsorption", "OpRayleigh", "OpMieHG", "OpBoundary"
        if (process->GetProcessName() == "OpBoundary") {
            // Cache the pointer to the boundary process
            fBoundary = dynamic_cast<G4OpBoundaryProcess*>(process);
            
            // End iteration when found
            break;
        }
    }
    // TODO: Error handling? Return 1/0 if fBoundary not set/set
}
