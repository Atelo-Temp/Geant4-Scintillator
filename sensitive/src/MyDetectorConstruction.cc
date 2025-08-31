/*
 * Mandatory user initialisation class (no default behaviour provided)
 * NOTE: G4RunManager will check for existance of mondatory classes when Initialize() and BeamOn() are invoked
 *
 * Derived from abstract base class: G4VUserDetectorConstruction
 *
 * Requires the user to define the entire detector setup, including:
 * - Its geometry
 * - The materials used in its construction
 * - A definition of its sensitive regions
 * - The readout schemes of the sensitive regions
 *
 * Simplest example of DetectorConstruction() required to build a simulation program:
*/

/*
 * Detector geometry in Geant4 is made of a number of volumes
 * Each volume is created by describing its shape and physical characteristics, then placed in a container volume
 * NOTE: The container is the "mother" volume and the nested volume the "daughter"

 * SOLID - A geometrical object with a shape and specific values for each dimension

 * LOGICAL VOLUME - Includes geometrical properties of the solid, and adds physical characteristics, i.e:
 * - Material
 * - Whether it contains sensitive detector elements
 * - The magnetic field
 * - etc ...

 * PHYSICAL VOLUME - Places a copy of the logical volume inside a larger container volume
 * NOTE: The coordinate system of the mother volume is used to specify where the daughter volume is placed
*/

// #include "DetectorConstruction.hh" // When leaving it as named here, causes errors (only in vscode), in scintillator/ no errors ...
#include "MyDetectorConstruction.hh"

#include "MySensitiveDetector.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
// #include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "G4Tubs.hh"

#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4SDManager.hh"


// Define the constructor & destructor (optional given default setting in header file)
// DetectorConstruction::DetectorConstruction() {}
// DetectorConstruction::~DetectorConstruction() {}

// namespace GEOMETRY {
G4VPhysicalVolume* DetectorConstruction::Construct() {

    // Flag for checking geometry overlap
    G4bool checkOverlaps = true;

    /////////////
    // MATERIALS:
    /////////////

    // Get a pointer to the material manager instance
    G4NistManager* nist = G4NistManager::Instance();

    // World material
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

    // Scintillator material
    G4Material* NaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");

    // Shielding material (optional)
    G4Material* lead = nist->FindOrBuildMaterial("G4_Pb");


    //////////
    // SOLIDS:
    //////////

    // The largest volume is the "World" volume (top level container)
    // NOTE: The world volume must contain all other volumes in the detector geometry (with some margin)
    // NOTE: A box is the most simple (and efficient) shape to describe the world

    // Define the world box dimensions (x, y, z)
    G4double world_hx = 1.0*m;
    G4double world_hy = 1.0*m;
    G4double world_hz = 1.0*m;

    // Create a box named "World" with the specified dimensions, using the G4Box class
    auto worldBox = new G4Box("World", world_hx, world_hy, world_hz);

    // NOTE: The constructor takes its args as half of the total box size, hence this world extends from:
    // -3.0 to +3.0 along the X axis
    // -1.0 to +1.0 along the Y axis
    // -1.0 to +1.0 along the Z axis

    // Dimensions for cylindrical scintillator crystal (radii, height, span)
    G4double innerRad = 0.*m; // No centre hole
    G4double outerRad = 7.62*cm * 0.5; // 3 inch = 7.62 cm diameter => diameter / 2 = radius
    G4double cylinderHeight = 7.62*cm * 0.5; // 3 inch height (this arg will be doubled)
    G4double startAngle = 0.*deg;
    G4double endAngle = 360.*deg; // Full circumference cylinder

    // Create the scintillator crystal solid (the detector)
    auto scintillator = new G4Tubs(
        "Scintillator",
        innerRad,
        outerRad,
        cylinderHeight,
        startAngle,
        endAngle
    );
    // NOTE: Cylinder has radius of 3.81 cm and height of 7.62 cm (the "height" param is also a half length)

    // Define the lead shielding dimensions (optional)
    G4double shieldHeight = 10.*cm; // x & y (square shield)
    G4double shieldThickness = 2.*mm; // z (direction of particles)

    // Create the shielding solid
    auto shielding = new G4Box(
        "Shielding",
        0.5 * shieldHeight,
        0.5 * shieldHeight,
        0.5 * shieldThickness
    );
    // NOTE: Multiplying by 0.5 in args will ensure values match those listed above


    ///////////////////
    // LOGICAL VOLUMES:
    ///////////////////

    // To create a logical volume, a solid, and material are required (as a minimum)

    // Using the world box solid, a logical volume can be created by filling it with argon gas
    // G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, Air, "World");
    auto worldLog = new G4LogicalVolume(worldBox, air, "World");

    // NOTE: Here, the solid is passed as an arg, its material is specified, then the logical volume is named
    // Logical volume names can be the same as solid names i guess ?

    // Define the scintillator crystal material as sodium iodide
    auto scintillatorLog = new G4LogicalVolume(scintillator, NaI, "Scintillator");

    // Define the lead shielding
    auto shieldingLog = new G4LogicalVolume(shielding, lead, "Shielding");


    ////////////////////
    // PHYSICAL VOLUMES:
    ////////////////////

    // To place a volume, the logical volume must be placed inside of an existing volume
    // NOTE: The world has no container, instead it is created with a null mother pointer

    // Place the world (must have a null mother pointer, unrotated, and placed at global origin)
    G4VPhysicalVolume* worldPhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(), // No args defaults to (0, 0, 0)
        worldLog, // The logical volume to place
        "World", // The name (physical, separate from logical names)
        nullptr, // No mother volume
        false, // No boolean operations
        0, // Its copy number
        checkOverlaps // Check for overlapping geometry
    );
    // NOTE: ^THIS COULD BE AUTO TYPED (type is inferred by PVPlacement)

    // Define coordinates for scintillator crystal (offset from mother origin)
    G4double crystalX = 0.*m;
    G4double crystalY = 0.*m;
    G4double crystalZ = 0.1*m; // 10cm (maybe 3cm as i have a lot of data for that distance)

    // Place the sodium iodide scintillator crystal (inside of the world)
    G4VPhysicalVolume* scintillatorPhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(crystalX, crystalY, crystalZ), // Translation
        scintillatorLog, // The logical volume
        "Scintillator", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        checkOverlaps
    );
    // NOTE: This places the scintillator at the origin of the mother volume, shifted by 0.5 meter along Z

    // Define coordinates for scintillator crystal (offset from mother origin)
    G4double shieldingX = 0.*m;
    G4double shieldingY = 0.*m;
    G4double shieldingZ = 0.05*m; // 5cm

    // Place the lead shielding between the origin and the scintillator
    G4VPhysicalVolume* shieldingPhys = new G4PVPlacement(
        nullptr,
        G4ThreeVector(shieldingX, shieldingY, shieldingZ),
        shieldingLog,
        "Shielding",
        worldLog,
        false,
        0,
        checkOverlaps
    );


    /////////////
    // COLOURING:
    /////////////

    // Set visualiser colouring (R, G, B, opacity)
    // NOTE: I think these could also be set via the visualiser init macro script

    // ...
    auto shieldingVisAtt = new G4VisAttributes(G4Color(1.0, 0., 0., 0.5)); // red
    shieldingVisAtt->SetForceSolid(true); // ...
    shieldingLog->SetVisAttributes(shieldingVisAtt); // assign to the logical volume

    auto scintillatorVisAtt = new G4VisAttributes(G4Color(1.0, 1.0, 0., 0.5)); // yellow
    scintillatorVisAtt->SetForceSolid(true);
    scintillatorLog->SetVisAttributes(scintillatorVisAtt);

    auto worldVisAtt = new G4VisAttributes(G4Color(0., 0., 1., 0.1)); // blue (opaque)
    worldVisAtt->SetForceSolid(true);
    worldLog->SetVisAttributes(worldVisAtt);


    ///////////
    // SCORING:
    ///////////

    // Will need to access logical scintillator volume outside of this methods scope (for scoring)
    fScoringVolume = scintillatorLog;
    // NOTE: Assigned to the class property defined in header file


    ///////
    // ...:
    ///////

    // Always return world
    return worldPhys;
}


// Construct the sensitive detector, assign it to the logical scoring volume, and register it with the SD manager
void DetectorConstruction::ConstructSDandField() {
    // Instantiate the sensitive detector class we defined (Supplying a name for the detector)
    auto sd = new SensitiveDetector("SD");

    // Access the detector construction class property storing the scoring volume and set SD
    fScoringVolume->SetSensitiveDetector(sd);

    // Get pointer for sensitive detector manager, and register the detector
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);

    // NOTE: Registering "sd" will ensure the energy deposit methods in "Sensitive Detector" class:
    // - Initialise()
    // - endOfEvent()
    // ... are called when needed
}

// }
