// User lib
#include "Timer.hh"

// C++ lib
#include <chrono>

// G4 lib
#include "G4ios.hh"

/*
 * Get a millisecond based timestamp for runtime tracking and random seeding
 */
long Timer::GetTimestamp() {
    // Capture the current system date/time (for generating a unique seed)
    std::chrono::time_point time = std::chrono::system_clock::now();
    
    // Convert the OS dependent "time point" into a unix millisecond time point
    std::chrono::time_point casted = std::chrono::time_point_cast<std::chrono::milliseconds>(time);
    
    // Convert the "time point" to an unix epoch "duration" in milliseconds
    std::chrono::duration epoch = casted.time_since_epoch();
    
    // Convert the "duration" type to a timestamp of type long (suitable for seeding)
    long const timestamp = epoch.count();
    
    return timestamp;
}

/*
 * Grabs current timestamp and stores it as a class property
 */
long Timer::StartTimer() {
    // Set start time as current timestamp
    fStartTime = GetTimestamp();
    
    // TODO: May do this here rather than in RunAction
    // G4cout << "Starting Run At: " << fStartTime << G4endl;
    
    return fStartTime;
}

/*
 * Get timestamp at end of run, subtract start from end, convert stamp to hrs/mins/seconds
 * 
 * TODO: maybe return vals
 */
void Timer::EndTimer() {
    long const endTime = GetTimestamp(); // get millisecond timestamp
    
    long const runDurationMS = endTime - fStartTime; // run duration in milliseconds
    
    long const runDurationS = runDurationMS / 1000; // milliseconds to seconds
    
    long const runDurationM = runDurationS / 60; // seconds to minutes
    
    long const runDurationH = runDurationM / 60; // minutes to hours
    
    long const remainderM = runDurationM % 60; // remainder minutes not divisible by 60
    
    long const remainderS = runDurationS % 60; // remainder seconds not divisible by 60
    
    // TEST ...
    // G4cout << "Finishing Run At: " << G4endl;
    G4cout << "Run Duration: " << runDurationH << "h " << remainderM << "m " << remainderS << "s" << G4endl;
    
    G4cout << "Run Duration (mins): " << runDurationM << "m " << G4endl;
    
    return;
}
