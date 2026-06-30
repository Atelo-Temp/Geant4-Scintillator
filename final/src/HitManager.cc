// User lib
#include "HitManager.hh"

/*
 * Constructor
 */
HitManager::HitManager() {
    // Increase allocation for reflection vector map
    fReflectionMap.resize(40960);
}

/*
 * Reset counters between events
 */
void HitManager::ResetCounters() {
    // Set integers to zero
    fTotalPhotons = 0;
    fDetectedPhotons = 0;
    fAbsorbedPhotons = 0;
    fBulkAbsorb = 0;
    fLostPhotons = 0; // NoRINDEX
    
    // Set all indices in reflection map to zero values
    // fReflectionMap.assign(fReflectionMap.size(), 0);
    std::fill(fReflectionMap.begin(), fReflectionMap.end(), 0);
}

/*
 * Increment optical photons generated
 */
void HitManager::CountPhoton() {
    fTotalPhotons += 1;
}

/*
 * ...
 */
G4int HitManager::GetPhotons() const {
    return fTotalPhotons;
}

/*
 * Increment optical photons detected (at photocathode)
 */
void HitManager::CountDetectedPhoton() {
    fDetectedPhotons += 1;
}

/*
 * ...
 */
G4int HitManager::GetDetectedPhotons() const {
    return fDetectedPhotons;
}

/*
 * Increment optical photons absorbed (at a boundary)
 */
void HitManager::CountBoundaryAbsorption() {
    fAbsorbedPhotons += 1;
}

/*
 * ...
 */
G4int HitManager::GetBoundaryAbsorptions() const {
    return fAbsorbedPhotons;
}

/*
 * Increment optical photons lost (due to no RINDEX etc)
 */
void HitManager::CountLostPhoton() {
    fLostPhotons += 1;
}

/*
 * ...
 */
G4int HitManager::GetLostPhotons() const {
    return fLostPhotons;
}
// void HitManager::CountLostPhoton(std::string medium) { fLostPhotons += 1; } // TODO: Add medium where each of these things occured (same for absorption, etc)

/*
 * Increment optical photons absorbed (in medium)
 */
void HitManager::CountBulkAbsorption() {
    fBulkAbsorb += 1;
}

/*
 * ...
 */
G4int HitManager::GetBulkAbsorptions() const {
    return fBulkAbsorb;
}

/*
 * Increment reflection counter for specified photon
 * 
 * NOTE: This is for any type of reflection:
 * - FresnelReflection
 * - TotalInternalReflection
 * - LambertianReflection
 * - LobeReflection
 * - SpikeReflection
 * - BackScattering
 * 
 * NOTE: Will increase size of vector if passed photonIdx exceeds capacity
 */
void HitManager::CountReflection(G4int photonIdx) {
    if (photonIdx >= fReflectionMap.size()) fReflectionMap.resize(photonIdx + 2048, 0);
    fReflectionMap[photonIdx]++;
}

/*
 * Retrieve total number of reflections for specified photon
 * 
 * NOTE: Doesnt alter class data (readonly), so const method
 */
G4int HitManager::GetReflections(G4int photonIdx) const {
    return fReflectionMap[photonIdx];
}
