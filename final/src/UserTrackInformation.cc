// User classes
#include "UserTrackInformation.hh"

/*
 * Increment reflection counter for particle
 * 
 * NOTE: This is for any type of reflection:
 * - FresnelReflection
 * - TotalInternalReflection
 * - LambertianReflection
 * - LobeReflection
 * - SpikeReflection
 * - BackScattering
 */
void UserTrackInformation::CountReflection() {
    fReflections++;
}

/*
 * Retrieve total number of reflections for particle
 */
G4int UserTrackInformation::GetReflections() {
    return fReflections;
}
