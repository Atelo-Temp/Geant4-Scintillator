// Mandatory user initialisation class (no default behaviour provided)
// NOTE: G4RunManager will check for existance of mondatory classes when Initialize() and BeamOn() are invoked

// Derived from abstract base class: G4VUserDetectorConstruction

// Requires the user to define the entire detector setup, including:
// - Its geometry
// - The materials used in its construction
// - A definition of its sensitive regions
// - The readout schemes of the sensitive regions

// Simplest example of DetectorConstruction() required to build a simulation program


// Detector geometry in Geant4 is made of a number of volumes
// Each volume is created by describing its shape and physical characteristics, then placed in a container volume
// NOTE: The container is the "mother" volume and the nested volume the "daughter"


// SOLID - A geometrical object with a shape and specific values for each dimension

// LOGICAL VOLUME - Includes geometrical properties of the solid, and adds physical characteristics, i.e:
// - Material
// - Whether it contains sensitive detector elements
// - The magnetic field
// - etc ...

// PHYSICAL VOLUME - Places a copy of the logical volume inside a larger container volume
// NOTE: The coordinate system of the mother volume is used to specify where the daughter volume is placed


#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"


//////////
// SOLIDS:
//////////

// The largest volume is the "World" volume (top level container)
// NOTE: The world volume must contain all other volumes in the detector geometry (with some margin)
// NOTE: A box is the most simple (and efficient) shape to describe the world

// Define the world box dimensions (x, y, z)
G4double world_hx = 3.0*m;
G4double world_hy = 1.0*m;
G4double world_hz = 1.0*m;

// Create a box named "World" with the specified dimensions, using the G4Box class
G4Box* worldBox = new G4Box("World", world_hx, world_hy, world_hz);

// NOTE: The constructor takes its args as half of the total box size
// Hence, this world extends from:
// -3.0 to +3.0 along the X axis
// -1.0 to +1.0 along the Y axis
// -1.0 to +1.0 along the Z axis


// In the world we can place a cylinder to act as the detector

// Define the cylinder dimensions (radii, height, span)
G4double innerRadius = 0.*cm; // assuming this prevents it being hollow ? a non-zero and less than outer radius would have centre hole ?
G4double outerRadius = 60.*cm; // what is the "*" suffix
G4double hz = 25.*cm;
G4double startAngle = 0.*deg;
G4double spanningAngle = 360.*deg; // Full circumference cylinder (i.e. 0 - 180 give half cylider)

// Create a cylinder named "Tracker", using the G4Tubs class
G4Tubs* trackerTube = new G4Tubs("Tracker", innerRadius, outerRadius, hz, startAngle, spanningAngle);
// NOTE: Cylinder has radius of 60 cm and length 50 cm (the "hz" param is also a half length)


///////////////////
// LOGICAL VOLUMES:
///////////////////

// To create a logical volume, a solid, and material are required (as a minimum)

// Using the world box solid, a logical volume can be created by filling it with argon gas
G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, Ar, "World");

// Likewise, the cylinder solid can be filled with aluminium
G4LogicalVolume* trackerLog = new G4LogicalVolume(trackerTube, Al, "Tracker");

// NOTE: Here, the solid is passed as an arg, its material is specified, then the logical volume is named
// Logical volume names can be the same as solid names i guess ?


////////////////////
// PHYSICAL VOLUMES:
////////////////////

// To place a volume, the logical volume must be placed inside of an existing volume
// NOTE: The world has no container, instead it is created with a null mother pointer

// Place the world (must have a null mother pointer, unrotated, and placed at global origin)
G4PhysicalVolume* worldPhys = new G4PVPlacement(
    nullptr, // No rotation
    G4ThreeVector() // No args defaults to (0, 0, 0)
    worldLog, // The logical volume to place
    "World", // The name (physical, separate from logical names)
    nullptr, // No mother volume
    false, // No boolean operations
    0 // Its copy number
);

// Define cylinder translation (offset from mother origin)
G4double pos_x = -1.0*meter;
G4double pos_y = 0.0*meter;
G4double pos_z = 0.0*meter;

// Place the cylinder (inside of the world)
G4PhysicalVolume* trackerPhys = new G4PVPlacement(
    0, // No rotation (with respect to mother)
    G4ThreeVector(pos_x, pos_y, pos_z) // Translation position
    trackerLog, // The logical volume
    "Tracker", // The name (physical)
    worldLog, // Its mother volume (logical)
    false, // No boolean operations
    0 // Its copy number
);
// NOTE: This places the tracker at the origin of the mother volume, shifted by 1 meter along X
