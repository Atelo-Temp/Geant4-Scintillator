#ifndef MyAnalysisRegistry_HH
#define MyAnalysisRegistry_HH

// G4 lib
#include "G4Types.hh"

/*
 * ...
 */
// struct NtupleIDs {
//     // Event flags
//     G4int fDetectionNtuple = -1; // Per-event detections
//     G4int fDetectionFractionNtuple = -1; // Per-event detections fraction
//     G4int fBoundaryAbsorbFractionNtuple = -1; // Per-event boundary absorptions fraction
//     G4int fBulkAbsorbFractionNtuple = -1; // Per-event bulk absorptions fraction
//     
//     // Step detection flags
//     G4int fDetectionCoordsNtuple = -1;
//     G4int fDetectionDistanceNtuple = -1;
//     G4int fDetectionTimeOfFlightNtuple = -1;
//     G4int fDetectionReflectionsNtuple = -1;
//     
//     // Step boundary absorption flags
//     // G4int fBoundaryAbsorbNtuple = -1; // NOTE: Not writing per-event boundary absorption counts
//     G4int fBoundaryAbsorbCoordsNtuple = -1;
//     // G4int fBoundaryAbsorbDistanceNtuple = -1; // NOTE: Not yet implemented
//     // G4int fBoundaryAbsorbTimeOfFlightNtuple = -1; // NOTE: Not yet implemented
//     // G4int fBoundaryAbsorbReflectionsNtuple = -1;  // NOTE: Not yet implemented
//     
//     // Step bulk absorption flags
//     // G4int fBulkAbsorbNtuple = -1; // NOTE: Not writing per-event bulk absorption counts
//     // G4int fBulkAbsorbCoordsNtuple = -1; // NOTE: Not yet implemented
//     G4int fBulkAbsorbDistanceNtuple = -1;
//     // G4int fBulkAbsorbTimeOfFlightNtuple = -1; // NOTE: Not yet implemented
//     G4int fBulkAbsorbReflectionsNtuple = -1;
// };

/*
 * ...
 */
struct NtupleIndices {
    G4int fNtupleID;
    G4int fColumnID;
};

/*
 * ...
 */
struct NtupleIndicesCoords {
    G4int fNtupleID;
    G4int fXColumnID;
    G4int fYColumnID;
    G4int fZColumnID;
};

/*
 * ...
 */
struct NtupleIDs {
    // Event flags
    NtupleIndices fDetectionNtuple = {-1, -1}; // Per-event detections
    NtupleIndices fDetectionFractionNtuple = {-1, -1}; // Per-event detections fraction
    NtupleIndices fBoundaryAbsorbFractionNtuple = {-1, -1}; // Per-event boundary absorptions fraction
    NtupleIndices fBulkAbsorbFractionNtuple = {-1, -1}; // Per-event bulk absorptions fraction
    
    // Step detection flags
    NtupleIndicesCoords fDetectionCoordsNtuple = {-1, -1, -1, -1};
    NtupleIndices fDetectionDistanceNtuple = {-1, -1};
    NtupleIndices fDetectionTimeOfFlightNtuple = {-1, -1};
    NtupleIndices fDetectionReflectionsNtuple = {-1, -1};
    
    // Step boundary absorption flags
    // NtupleIndices fBoundaryAbsorbNtuple = {-1, -1}; // NOTE: Not writing per-event boundary absorption counts
    NtupleIndicesCoords fBoundaryAbsorbCoordsNtuple = {-1, -1, -1, -1};
    // NtupleIndices fBoundaryAbsorbDistanceNtuple = {-1, -1}; // NOTE: Not yet implemented
    // NtupleIndices fBoundaryAbsorbTimeOfFlightNtuple = {-1, -1}; // NOTE: Not yet implemented
    // NtupleIndices fBoundaryAbsorbReflectionsNtuple = {-1, -1};  // NOTE: Not yet implemented
    
    // Step bulk absorption flags
    // NtupleIndices fBulkAbsorbNtuple = {-1, -1}; // NOTE: Not writing per-event bulk absorption counts
    // NtupleIndicesCoords fBulkAbsorbCoordsNtuple = {-1, -1, -1, -1}; // NOTE: Not yet implemented
    NtupleIndices fBulkAbsorbDistanceNtuple = {-1, -1};
    // NtupleIndices fBulkAbsorbTimeOfFlightNtuple = {-1, -1}; // NOTE: Not yet implemented
    NtupleIndices fBulkAbsorbReflectionsNtuple = {-1, -1};
};

/*
 * Singleton object responsible for:
 * - Storing ntuple ids
 * 
 * Singleton class defines the "GetInstance" method that serves as an alternative to the constructor,
 * and lets clients access the same instance of this class over and over again
 * 
 * NOTE: Meyers singleton (thread-safe) implementation eliminates need for mutex locking, while
 * remaining completely thread-safe
 * 
 * NOTE: Meyers singleton also means "delete" does not ever need to be explicitly called
 */
class AnalysisRegistry {
    public:
        // Delete copy constructor (Singletons should not be cloneable)
        // AnalysisRegistry(AnalysisRegistry& other) = delete;
        AnalysisRegistry(AnalysisRegistry const&) = delete;
        
        // Delete assignment operator (Singletons should not be assignable)
        // void operator=(AnalysisRegistry const&) = delete;
        AnalysisRegistry& operator=(AnalysisRegistry const&) = delete;
        
        // Static method controls access to singleton instance (get instance if exists, else instantiate)
        static AnalysisRegistry& GetInstance();
        // static AnalysisRegistry* GetInstance();
        
        // NOTE: For debugging
        // static AnalysisRegistry& GetInstance(std::string const& value);
        
        // Business logic
        //
        // Messenger uses this before BeamOn to change settings
        NtupleIDs& GetNtupleIDs();
        // NOTE: Returning a reference to the object, which allows property mutation
        //
        // Worker threads use this during the run for thread-safe, fast reads
        const NtupleIDs& ReadNtupleIDs() const;
        // NOTE: Return type is readonly reference to flags
        // NOTE: Const modifier after method name tells compiler that calling this method will not alter 
        // the state of the AnalysisRegistry instance itself
        
        // NOTE: For debugging
        // std::string value() const {
        //     return value_;
        // }
        
    protected:
        // Business logic
        // 
        // Stack allocated object containing ids for each created ntuple
        NtupleIDs fNtupleIDs;
        
        // NOTE: For debugging
        // std::string value_;
    
    private:
        // Constructor, private to prevent direct construction via "new" operator, only self callable
        AnalysisRegistry() = default;
        
        // NOTE: For debugging
        // AnalysisRegistry(std::string const value);
        
        // Destructor, private to prevent direct destruction call via "delete" operator
        ~AnalysisRegistry() = default;
};

#endif
