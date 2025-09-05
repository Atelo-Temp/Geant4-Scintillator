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
#include "G4SDManager.hh"

#include "G4Sphere.hh" // source

#include "G4RotationMatrix.hh"

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

    // Shielding material (lead) (optional)
    G4Material* lead = nist->FindOrBuildMaterial("G4_Pb");
    
    // Scintillator can material (aluminium)
    G4Material* Al = nist->FindOrBuildMaterial("G4_Al");
    
    // Optical window material
    // G4Material* ? = nist->FindOrBuildMaterial("");
    
    // Scintillation light reflector material (Al2O3)
    G4Material* Al2O3 = nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");
    
    // Tabletop material
    // G4Material* wood = nist->FindOrBuildMaterial("");
    
    // Source casing material
    // G4Material* plastic? = nist->FindOrBuildMaterial("");
    
    
    ////////////////////
    // SOURCE MATERIALS:
    ////////////////////

    // Source material (define Fluorine-18 isotope)
    auto F18 = new G4Isotope(
        "18F", // name
        9, // num protons (Z)
        18, // Atomic mass (num nucleons) (A),
        18.000938 * g / mole // Molar mass (grams per molecule) (~18g per mol)
    ); // NOTE: 1 mol contains avogadros number of particles (6.022 x 10^23)
    
    // Define an element from the isotope
    auto elF18 = new G4Element("Fluorine-18", "18F", 1); // name, symbol, num isotopes
    
    // Assign the defined isotope to the element
    elF18->AddIsotope(F18, 100.0 * perCent); // isotope, no other isotopes so 100%
    
    // Because isotope and element have no direct interaction in G4
    // to assign to logical volume, need to create a G4 material
    auto matF18 = new G4Material("F18Source", 1.51 * g / cm3, 1); // name, density (g/cm^3), phase (solid, liquid, gas)
    
    // Assign the element to the G4 material
    matF18->AddElement(elF18, 100.0 * perCent); // element, amount of element in material (100%)

    
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
    
    
    /////////////////
    // ALUMINIUM CAN:
    /////////////////
    
    // Define the thickness of the can
    // G4double canThick = 1.*mm;
    // G4double canOuterRad = crystalOuterRad + canThick;
    // G4double overHang = 0.*mm; // this will be added to both sides in Z direction to fit window
    
    // G4double inchToCM = 2.54;
    
    G4double canThick = 0.0508 * cm; // NOTE: Is 0.02' => 0.508 mm, according to ortec spec
    G4double canInnerRad = (((3.225 * 2.54) / 2) * cm) - canThick; // 3.225' dia => 8.1915cm dia => 4.09575 cm outer rad => 4.04495 cm inner rad
    G4double canOuterRad = ((3.225 * 2.54) / 2) * cm; // NOTE: Can is 3.225' diameter according to ortec spec
    // Radially there is a gap between crystal and can
    
    // NOTE: The face of the crystal seems to be in direct contact with the can though
    // G4double overHang = 0.*mm; // this will be added to both sides in Z direction to fit window
    // NOTE: This will = canThick * 2 (or at least the face will = canThick, may have to translate along Z if needed)
    G4double canLength = crystalHeight + canThick;
    // NOTE: Could also just leave can length as crystal length and place window on top, rather than slot it in
    
    // Solid
    auto can = new G4Tubs(
        "Can",
        // crystalOuterRad, // Inner rad = outer rad of crystal
        // canOuterRad, // Outer rad = inner rad + can thickness
        // crystalHeight, // TODO: Leaving same as crystal for now, but will want a lip for window
        canInnerRad,
        canOuterRad,
        canLength, // height
        startAngle, // 0 deg
        endAngle // 360 deg span
    );
    
    // Logical
    auto canLog = new G4LogicalVolume(
        can,
        Al, // Aluminium material
        "Can"
    );
    
    // Physical
    G4VPhysicalVolume* canPhys = new G4PVPlacement(
        nullptr, // no rotation
        crystalTrans, // same position as crystal
        canLog, // logical volume to place
        "Can", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // One copy
        checkOverlaps
    );
    
    
    //////////////////
    // OPTICAL WINDOW:
    //////////////////

    // TODO ...
    // G4Tubs, same radius as crystal, height matching lip of can,
    // translated to end of crystal
    
    // NOTE: It seems to be aluminium for ortec 3' NaI, may be wrong though, cant see otherwise
    
    // Solid
    auto window = new G4Tubs(
        "Window",
        0.*m,
        canInnerRad,
        canThick,
        startAngle,
        endAngle
    );
    
    // Logical
    auto windowLog = new G4LogicalVolume(
        window,
        Al, // Aluminium material
        "Window"
    );
    
    // Physical
    G4VPhysicalVolume* windowPhys = new G4PVPlacement(
        nullptr, // no rotation
        G4ThreeVector(crystalX, crystalY, crystalZ - crystalHeight), // Translated along z by half crystal height
        windowLog, // logical volume to place
        "Window", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // One copy
        checkOverlaps
    );
    
    
    /////////////
    // REFLECTOR:
    /////////////
    
    // The scintillation photons are emitted in all directions, so a high efficiency reflector
    // is used to surround the crystal (Al_{2}O_{3} and teflon)
    
    // Inner rad can (4.04495 cm) - outer rad crystal (3.81 cm) => 0.23495 cm reflector thickness
    
    // ...
    auto reflector = new G4Tubs(
        "Reflector",
        crystalOuterRad, // inner rad
        canInnerRad, // outer rad
        crystalHeight, // height
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // ...
    auto reflectorLog = new G4LogicalVolume(
        reflector,
        Al2O3,
        "Reflector"
    );
    
    // ...
    G4VPhysicalVolume* reflectorPhys = new G4PVPlacement(
        nullptr, // no rotation
        crystalTrans, // same position as crystal
        reflectorLog, // logical volume
        "Reflector", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        checkOverlaps
    );
    
    ////////////////////////
    // PHOTOMULTIPLIER TUBE:
    ////////////////////////

    // TODO ...
    
      
    ////////////
    // TABLETOP:
    ////////////
    
    // TODO: ... material needed
    
    G4double tableSize = 0.5*m; // 1m probably better but dont wanna make world massive (also is rectangle not square)
    G4double tableHeight = 0.05*m; // 5cm ? TODO: spitballing, need to refine this
    
    auto table = new G4Box("Table", tableSize * 0.5, tableSize * 0.5, tableHeight * 0.5);
    
    auto tableLog = new G4LogicalVolume(table, Al, "Table"); // TODO: Need actual table material
    
    // Rotate about z-axis 90 degrees
    auto tableRot = new G4RotationMatrix();
    tableRot->rotateX(90. * deg);
    
    // Translate in -y direction by radius of can + half thickness of table
    G4double tableTransY = -1. * (canOuterRad + (tableHeight * 0.5));
    
    G4VPhysicalVolume* tablePhys = new G4PVPlacement(
        tableRot, // Rotated 90 degrees in Z direction
        // nullptr,
        G4ThreeVector(0., tableTransY, 0.), 
        tableLog,
        "Table",
        worldLog,
        false,
        0,
        checkOverlaps // NOTE: Will overlap currently
    );
    
    
    /////////////
    // SHIELDING:
    /////////////

    // Define the lead shielding dimensions (optional)
    G4double shieldHeight = 10.*cm; // x & y (square shield)
    G4double shieldThickness = 2.*mm; // z (direction of particles)

    // Create the shielding solid
    auto shielding = new G4Box(
        "Shielding",
        // 0.5 * shieldHeight,
        // 0.5 * shieldHeight,
        canOuterRad,
        canOuterRad,
        0.5 * shieldThickness
    );
    // NOTE: Multiplying by 0.5 in args will ensure values match those listed above

    // Define the lead shielding
    auto shieldingLog = new G4LogicalVolume(shielding, lead, "Shielding");
    
    // Define coordinates for lead shielding (offset from mother origin)
    G4double shieldingX = 0.*m;
    G4double shieldingY = 0.*m;
    G4double shieldingZ = 0.05*m; // 5cm

    // Place the lead shielding between the origin and the scintillator
    // G4VPhysicalVolume* shieldingPhys = new G4PVPlacement(
    //     nullptr,
    //     G4ThreeVector(shieldingX, shieldingY, shieldingZ),
    //     shieldingLog,
    //     "Shielding",
    //     worldLog,
    //     false,
    //     0,
    //     checkOverlaps
    // );
    
    
    //////////
    // SOURCE:
    //////////
    
    // Fluorine source
    G4double sourceRadius = 1. * mm;
    auto solidSource = new G4Sphere(
        "Source", // name
        0., // minmum radius (0 = not hollow),
        sourceRadius, // maximum radius
        0. * deg, // minimum phi angle
        360. * deg, // maximum phi angle (NOTE: Assuming this is like span angle ?)
        0. * deg, // minimum theta angle
        180. * deg // maximum theta angle (NOTE: What are these last two for ?)
    );
    
    // Define the radioactive source with the created material
    auto sourceLog = new G4LogicalVolume(solidSource, matF18, "Source");
    
    // Place the radioactive source (offset from origin by 1cm => so 11cm from detector)
    // G4VPhysicalVolume* sourcePhys = new G4PVPlacement(
    //     nullptr,
    //     G4ThreeVector(0 * m, 0 * m, -0.01 * m),
    //     sourceLog,
    //     "Source",
    //     worldLog,
    //     false,
    //     0,
    //     checkOverlaps
    // );
    
    
    /////////////////
    // SOURCE CASING:
    /////////////////
    
    // TODO: ...

    
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
    
    auto canVisAtt = new G4VisAttributes(G4Color(0.5, 0.5, 0.5, 0.5)); // gray
    canVisAtt->SetForceSolid(true);
    canLog->SetVisAttributes(canVisAtt);
    
    auto reflectorVisAtt = new G4VisAttributes(G4Color(0., 0., 1.0, 0.25)); // blue
    reflectorVisAtt->SetForceSolid(true);
    reflectorLog->SetVisAttributes(reflectorVisAtt);
    
    auto windowVisAtt = new G4VisAttributes(G4Color(0.5, 0.5, 0.5, 0.5)); // gray
    windowVisAtt->SetForceSolid(true);
    windowLog->SetVisAttributes(windowVisAtt);
    
    // ...
    auto tableVisAtt = new G4VisAttributes(G4Color()); // white
    tableVisAtt->SetForceSolid(true);
    tableLog->SetVisAttributes(tableVisAtt);
 
    auto shieldingVisAtt = new G4VisAttributes(G4Color(1.0, 0., 0., 0.5)); // red
    shieldingVisAtt->SetForceSolid(true);
    shieldingLog->SetVisAttributes(shieldingVisAtt);

    // Assign colour to the source geometry
    auto sourceVisAtt = new G4VisAttributes(G4Color(0.0, 1.0, 0.0, 0.5)); // green
    sourceVisAtt->SetForceSolid(true);
    sourceLog->SetVisAttributes(sourceVisAtt);

    
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
