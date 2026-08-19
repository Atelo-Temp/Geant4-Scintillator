// User classes
#include "SteppingAction.hh"
// #include "AnalysisManager.hh"
// #include "DetectorConstruction.hh"
#include "SteppingAnalysis.hh"
#include "EventAction.hh"
#include "HitManager.hh"

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
// #include "G4TrackStatus.hh"
// #include "G4RandomTools.hh" // Random seeding
#include "G4OpAbsorption.hh"

/*
 * Constructor
 * 
 * TODO: May want to use initialiser list here
 * 
 * NOTE: Destruction of EventAction, EventAnalysis, etc is not this classes responsibility, hence default destructor
 * 
 * TODO: EventAction unused beyond extraction of analysis class, consider removing
 */
SteppingAction::SteppingAction(EventAction* eventAction) {
    // ...
    fEventAction = eventAction;
    
    // Cache pointer to event analysis manager
    fHitManager = fEventAction->GetHitManagerPtr();
    
    // Cache optical photon definition
    fOpticalPhotonDefinition = G4OpticalPhoton::OpticalPhotonDefinition();
    
    // Instantiate analysis instance with the hit manager
    fSteppingAnalysis = new SteppingAnalysis(fHitManager);
    
    G4cout << "\n\n>>>>> STEPPING ACTION INSTANTIATED\n\n" << G4endl;
}
// SteppingAction::SteppingAction(EventAction* eventAction) : fEventAction(eventAction) {}

/*
 * Destructor
 * 
 * NOTE: SteppingAction is responsible for SteppingAnalysis lifetime
 */
SteppingAction::~SteppingAction() {
    delete fSteppingAnalysis;
}

/*
 * Step handler, will execute on each step a particle takes
 * 
 * TODO: Can some improvements be made to efficiency?
 */
void SteppingAction::UserSteppingAction(const G4Step* step) {    
    // Get the track object for the current step
    G4Track const* track = step->GetTrack();
    
    // TODO: If particle is gamma (for escape x-rays specifically)
    // Check pre and post step touchable logic volume
    // If pre step volume is crystal, and post step is not crystal (reflector, enclosure, air, etc)
    // Get kinetic energy of particle
    // Flag it as an escape x-ray
    // Add to running total escaped energy tracker
    
    // TODO: If particle is gamma (for optical photon temporal profile)
    // set fStartTime to local time
    
    // If particle is not an optical photon, break
    if (track->GetDefinition() != fOpticalPhotonDefinition) {
        return;
    }
    
    // Find the boundary process only once (cache the pointer)
    if (fBoundary == nullptr || fAbsorb == nullptr) {
        CacheProcesses(track); // this->FindBoundary(track);
    }
 
    // Count one optical photon (for total generated, not absorbed, detected, etc)
    // NOTE: Without if clause, when a photon reflects it will be counted twice,
    // could subtract interactions from this value, but this probably a cleaner way
    if (track->GetCurrentStepNumber() == 1) {
        fHitManager->CountPhoton();
    }
    
    // Get the post step point object for the particle
    G4StepPoint const* endPoint = step->GetPostStepPoint();
    // NOTE: We only need readonly methods, so const*
    
    // ...
    G4VProcess const* process = endPoint->GetProcessDefinedStep();
    
    // ...
    if (!process) return; // NOTE: Not sure about this return
    
    // If photon was absorbed in medium (not at a boundary)
    if (process == fAbsorb) {
        // NOTE: Double check for boundary not necessary, it would be "OpBoundary" otherwise
        
        // Increment bulk absorption counter via hit manager
        fHitManager->CountBulkAbsorption();
        
        // Forward to the bulk absorption event handler for ntuple writing
        fSteppingAnalysis->HandleBulkAbsorb(track, fHitManager);
        
        // Save a step status request and comparison by returning early
        return;
    }
    
    // NOTE: OpRayleigh, OpAbsorption, OpBoundary, OpScintillation (relevant process names)
    
    // If post step point not at a defined geometric boundary, break
    if (endPoint->GetStepStatus() != fGeomBoundary) return;
    // if (process->GetProcessName() != "OpBoundary") return;
    // if (process != fBoundary) return;
    
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
    
    // Photon is absorbed but status was detection
    // NOTE: This assumes that the volume causing detection is the photocathode
    // as it is the only volume with non-zero efficiency
    if (boundaryStatus == Detection) {
        // Manually track it in event tally
        fHitManager->CountDetectedPhoton();
        
        // Forward to the detection event handler for ntuple writing
        fSteppingAnalysis->HandleDetection(endPoint, track, fHitManager);
    } 
    // If an optical photon is absorbed without detection at a boundary (i.e., reflector or photocathode)
    else if (boundaryStatus == Absorption) {
        // Increment boundary absorption counter via hit manager
        fHitManager->CountBoundaryAbsorption();
        
        // Forward to the boundary absorption event handler for ntuple writing
        fSteppingAnalysis->HandleBoundaryAbsorb(endPoint);
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
        G4int const photonIdx = track->GetTrackID();
        fHitManager->CountReflection(photonIdx);
    }
    // Check no loss via lack of rindex
    else if (boundaryStatus == G4OpBoundaryProcessStatus::NoRINDEX) {
        // G4cout << G4endl << "OPTICAL PHOTON LOST TO LACK OF RINDEX" << G4endl;
        // G4cout << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // "Reflector"
        
        // if (endPoint->GetTouchable()->GetVolume()->GetName() != "Reflector") {
        //     G4cout << G4endl << "OPTICAL PHOTON LOST TO LACK OF RINDEX" << G4endl;
        //     G4cout << endPoint->GetTouchable()->GetVolume()->GetName() << G4endl; // Volume that photon was lost in, i.e.: "Reflector"
        //     G4cout << "Event ID: " << fEventAction->fEventID << G4endl; // Id of event in which this occured
        // }
        
        // Photon was killed due to lack of rindex
        fHitManager->CountLostPhoton();
        
        // TODO:
        // std::string volume = endPoint->GetTouchable()->GetVolume()->GetName();
        // fEventAction->CountLostPhoton(volume);
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
 * Find the relevant processes and assign them to the class property "fBoundary" & "fAbsorb" 
 * 
 * NOTE: i.e. "Transportation", "OpAbsorption", "OpRayleigh", "OpMieHG", "OpBoundary"
 * 
 * NOTE: Is there a cleaner way to do this?
 */
void SteppingAction::CacheProcesses(G4Track const* track) { // TODO: RENAME - CacheProcesses()
    // Get pointer to the process list (for optical photons)
    G4ProcessVector* pv = track->GetDefinition()->GetProcessManager()->GetProcessList();
    
    // Iterate over list of process objects
    for (int i = 0; i < pv->size(); i++) {
        // Pointer to current non-null process
        G4VProcess* process = (*pv)[i]; 
        // NOTE: Since "pv" is a pointer to a collection, rather than a direct object,
        // it needs to be dereferenced first (*pv), to reveal the actual object in memory,
        // so that processes can be accessed via index
        
        // Prevent attempted access to non-existent method
        if (process == nullptr) {
            continue;
        }
        
        // If the boundary process is found
        if (process->GetProcessName() == "OpBoundary") {
            // Cache the pointer to the boundary process
            fBoundary = dynamic_cast<G4OpBoundaryProcess*>(process);
        }
        
        // If the bulk absorption process is found
        if (process->GetProcessName() == "OpAbsorption") {
            // Cache the pointer to the bulk absorption process
            fAbsorb = dynamic_cast<G4OpAbsorption*>(process);
        }
        
        // End iteration early if both processes are found
        if (fBoundary && fAbsorb) {
            break;
        }
    }
    // TODO: Error handling? Return 1/0 if fBoundary not set/set
}
