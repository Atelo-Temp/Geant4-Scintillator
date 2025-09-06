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
#include "DetectorConstruction.hh"

#include <G4ThreeVector.hh>

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "G4Tubs.hh"

#include "G4VisAttributes.hh"
#include "G4Color.hh"

#include "G4MaterialPropertiesTable.hh" // for optical photons
// #include "G4MaterialPropertyVector.hh" // can use instead of 2x std::vector


// namespace GEOMETRY {

// Define the geometry to be created when run manager intialises
G4VPhysicalVolume* DetectorConstruction::Construct() {

    // Flag for checking geometry overlap
    G4bool checkOverlaps = true;

    
    //////////////////////
    // GEOMETRY MATERIALS:
    //////////////////////

    // Get a pointer to the material manager instance
    G4NistManager* nist = G4NistManager::Instance();

    // World material
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");

    // Scintillator material
    G4Material* NaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    
    /////////////////
    // SCINTILLATION:
    /////////////////
    
    // ...
    // std::vector<G4double> energy = {2.034*eV, 3.*eV, 4.136*eV};
    // std::vector<G4double> rindex = {1.3435, 1.351, 1.3608};
    // std::vector<G4double> absorption = {344.8*cm, 850.*cm, 1450.*cm};
    // auto x = new G4MaterialPropertyVector(); // can use instead of std::vector
    // x->InsertValues(); 
    
    // From online source
    // std::vector<G4double> energy = {1.239841939*eV/0.9, 1.239841939*eV/0.2}; // 900 nm, 200 nm
    // std::vector<G4double> rindex = {1.3435, 1.351, 1.3608};
    /*std::vector<G4double> rindex = {1.78, 1.78};*/ // TODO: Should be a function of wavelength though, not const
    // have seeen 1.85 @emission max
    
    // std::vector<G4double> energy = {2*eV, 2.8*eV};    
    // std::vector<G4double> rindex = {1.78, 1.836}; // refractiveindex.info
    
    // std::vector<G4double> energy = {1.9587*eV, 2.8437*eV};
    // std::vector<G4double> rindex = {1.778, 1.8391}; // refractiveindex.info
    // NOTE: This may be inferred as linear (but its not)
    
    // NOTE: Added a central curve value to show non-linear trend
    std::vector<G4double> energy = {1.9587*eV, 2.3991*eV, 2.8437*eV};
    std::vector<G4double> rindex = {1.7779, 1.8043, 1.8391}; // refractiveindex.info
    
    // ...
    auto MPT = new G4MaterialPropertiesTable();
    
    // Property independent of energy (const property)
    //
    // The number of scintillation photons generated per unit energy deposited in the medium
    MPT->AddConstProperty("SCINTILLATIONYIELD", 38000. / MeV); // 38 photons per keV deposited
    // MPT->AddConstProperty("SCINTILLATIONYIELD", 10. / MeV); // 38 photons per keV deposited
    
    // Properties that depend on energy
    // NOTE: Vector lengths must be the same, 1st vector is energy, 2nd is property value at that energy
    //
    // Refractive index ...
    MPT->AddProperty("RINDEX", energy, rindex);
    // NOTE: ...
    //
    // Absorption length is the average distance travelled by a photon before being absorbed by the medium 
    // (i.e. it is the mean free path returned by the GetMeanFreePath method)
    // MPT->AddProperty("ABSLENGTH", energy, absorption);
    
    // Rise time (defaults to zero)
    // MPT->AddConstProperty("SCINTILLATIONRISETIME1", 0. * ns);
    // NOTE: If a non-zero rise time is wanted, set the optical parameter "setFiniteRiseTime" to true
    
    // ~250ns decay time constant (some say 230-250) (at room temperature, increasing at lower temps)
    MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 250. * ns);
    // NOTE: Number of photons emitted follows an exponential function
    // creation time of photons is chosen from a distribution with these characterisics
    
    // Factor to vary width of yield distribution
    MPT->AddConstProperty("RESOLUTIONSCALE", 1.); // 1. to start, tune later
    // NOTE: val > 1. broadens intrinsic Poisson stats (captures non-proportionality-ish behaviour)
    
    
    // The energy spectrum of the emitted photons is specified using the energy-dependent material property "SCINTILLATIONCOMPONENT1"
    // NOTE: This is essential to generate the correct number of photons (25156 for 662 keV, instead of 5-10)
    std::vector<G4double> emi = {1., 1., 1.};
    MPT->AddProperty("SCINTILLATIONCOMPONENT1", energy, emi); // "Fast component"
    // NOTE: Tells Geant4 how many photons for each wavelength (or energy)
    
    
    // MPT->AddConstProperty("SCINTILLATIONYIELD1", 1.); // 100% in the single component (NOTE: idk what this is)
    
    
    // Absorption length ...
    std::vector<G4double> absorption = {30.*cm, 30.*cm, 30.*cm};
    MPT->AddProperty("ABSLENGTH", energy, absorption);
    // NOTE: This has effect on air too (WITHOUT SPECIFYING THIS, SIM WILL HANG INDEFINITELY, WHEN AIR RINDEX SPECIFIED)
    
    
    // NOTE: Need at least: refractive index, emission spectrum, yield, decay time, absorption length
    
    
    // TODO: Rayleigh Scattering ?
    // MPT->AddConstProperty("RAYLEIGH", ...)
    
    NaI->SetMaterialPropertiesTable(MPT);
    
    
    // NOTE: This literally kills the visualiser ... (WAS DUE TO NOT SPECIFYING ABSLENGTH)
    auto MPT2 = new G4MaterialPropertiesTable();
    // // MPT2->AddProperty("RINDEX", "Air");
    std::vector<G4double> rindexAir = {1., 1., 1.};
    MPT2->AddProperty("RINDEX", energy, rindexAir);
    air->SetMaterialPropertiesTable(MPT2);
    
    
    // TODO: Add reflector ? Al203 (rindex available on refractiveindex.info)
    // ... and aluminium can/beta blocker ? (maybe not if reflector covers crystal face)
    // ... and borosilicate PMT glass window
    // ^ or make another project to avoid overcrowding this one with new features ?

    
    /////////
    // WORLD:
    /////////
    
    // The largest volume is the "World" volume (top level container)
    // NOTE: The world volume must contain all other volumes in the detector geometry (with some margin)
    // NOTE: A box is the most simple (and efficient) shape to describe the world

    // Define the world box dimensions (x, y, z)
    G4double world_hx = 1.0*m;
    G4double world_hy = 1.0*m;
    G4double world_hz = 1.0*m;

    // Create a box named "World" with the specified dimensions, using the G4Box class
    auto worldBox = new G4Box("World", 0.5 * world_hx, 0.5 * world_hy, 0.5 * world_hz);

    // NOTE: The constructor takes its args as half of the total box size, hence this world extends from:
    // -0.5 to +0.5 along the X axis
    // -0.5 to +0.5 along the Y axis
    // -0.5 to +0.5 along the Z axis    
    
    // To create a logical volume, a solid, and material are required (as a minimum)

    // Using the world box solid, a logical volume can be created by filling it with air
    auto worldLog = new G4LogicalVolume(worldBox, air, "World");

    // NOTE: Here, the solid is passed as an arg, its material is specified, then the logical volume is named
    // Logical volume names can be the same as solid names i guess ?
    
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
    
    
    ////////////
    // Envelope:
    ////////////
    
    // TODO: ...
    
    
    ////////////////////////
    // SCINTILLATOR CRYSTAL:
    ////////////////////////
    
    // Dimensions for cylindrical scintillator crystal (radii, height, span)
    G4double crystalInnerRad = 0.*m; // No centre hole
    G4double crystalOuterRad = 7.62*cm * 0.5; // 3 inch = 7.62 cm diameter => (diameter / 2) = 3.81 cm outer radius
    G4double crystalHeight = 7.62*cm * 0.5; // 3 inch height (this arg will be doubled)
    G4double startAngle = 0.*deg;
    G4double endAngle = 360.*deg; // Full circumference cylinder

    // Create the scintillator crystal solid (the detector)
    auto scintillator = new G4Tubs(
        "Scintillator",
        crystalInnerRad,
        crystalOuterRad,
        crystalHeight,
        startAngle,
        endAngle
    );
    // NOTE: Cylinder has radius of 3.81 cm and height of 7.62 cm (the "height" param is also a half length)
    
    
    // Define the scintillator crystal material as sodium iodide
    auto scintillatorLog = new G4LogicalVolume(scintillator, NaI, "Scintillator");
    
    // Define coordinates for scintillator crystal (offset from mother origin)
    G4double crystalX = 0.*m;
    G4double crystalY = 0.*m;
    G4double crystalZ = 0.1*m; // 10cm (maybe 3cm as i have a lot of data for that distance)
    
    // Define translation vector (relative to mother origin)
    auto crystalTrans = G4ThreeVector(crystalX, crystalY, crystalZ);

    // Place the sodium iodide scintillator crystal (inside of the world)
    G4VPhysicalVolume* scintillatorPhys = new G4PVPlacement(
        nullptr, // No rotation
        crystalTrans, // Translation
        scintillatorLog, // The logical volume
        "Scintillator", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        checkOverlaps
    );
    // NOTE: This places the scintillator at the origin of the mother volume, shifted by 0.5 meter along Z

    
    /////////////
    // COLOURING:
    /////////////

    // Set visualiser colouring (R, G, B, opacity), and assign colours to the detector geometry
    // NOTE: I think these could also be set via the visualiser init macro script
   
    // World
    auto worldVisAtt = new G4VisAttributes(G4Color(0., 0., 1., 0.1)); // blue (opaque)
    worldVisAtt->SetForceSolid(true); // ... (think this can be called w/ no arg for same effect)
    worldLog->SetVisAttributes(worldVisAtt); // assign to the logical volume

    // Scintillator geometry
    auto scintillatorVisAtt = new G4VisAttributes(G4Color(1.0, 1.0, 0., 0.75)); // yellow
    scintillatorVisAtt->SetForceSolid(true);
    scintillatorLog->SetVisAttributes(scintillatorVisAtt);

    
    ///////////
    // SCORING:
    ///////////

    // Will need to access logical scintillator volume outside of this methods scope (for scoring)
    // fScoringVolume = scintillatorLog;
    // NOTE: Assigned to the class property defined in header file


    ///////
    // ...:
    ///////

    // Always return world
    return worldPhys;
}


// Construct the sensitive detector, assign it to the logical scoring volume, and register it with the SD manager
// void DetectorConstruction::ConstructSDandField() {
//     // Instantiate the sensitive detector class we defined (Supplying a name for the detector)
//     auto sd = new SensitiveDetector("SD");
// 
//     // Access the detector construction class property storing the scoring volume and set SD
//     fScoringVolume->SetSensitiveDetector(sd);
// 
//     // Get pointer for sensitive detector manager, and register the detector
//     G4SDManager::GetSDMpointer()->AddNewDetector(sd);
// 
//     // NOTE: Registering "sd" will ensure the energy deposit methods in "Sensitive Detector" class:
//     // - Initialise()
//     // - endOfEvent()
//     // ... are called when needed
// }

// }
