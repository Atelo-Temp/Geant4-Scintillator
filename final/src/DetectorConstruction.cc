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

// User classes
// #include "DetectorConstruction.hh" // When leaving it as named here, causes errors (only in vscode), in scintillator/ no errors ...
#include "DetectorConstruction.hh"
// #include "MaterialDefinitions.hh"
#include "DetectorMaterials.hh"
#include "SourceMaterials.hh"
#include "DetectorMessenger.hh"

// G4 lib
// #include "G4NistManager.hh"
// #include "G4Element.hh"
#include "G4Material.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
// #include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

// #include "G4VisAttributes.hh"
// #include "G4Color.hh"

// #include "G4MaterialPropertiesTable.hh" // for optical photons
// #include "G4MaterialPropertyVector.hh" // can use instead of 2x std::vector

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
// #include "G4SurfaceProperty.hh"

#include "G4Types.hh"

// #include "G4UserLimits.hh"

#include "G4RunManager.hh"


// TEST
// #include "G4VVisManager.hh" // TEST
// #include "G4GeometryManager.hh"
// #include "G4PhysicalVolumeStore.hh"
// #include "G4LogicalVolumeStore.hh"
// #include "G4SolidStore.hh"
// #include "G4UImanager.hh"

// NOTE: Uses consistent units throughout (cm probably easiest to adhere to)

// TODO: There is overhang of the encapsulation at the back of the crystal 
// (of thickness = encapsulation thickness, as that was added to front for beta shield)

/*
 * Constructor
 */
DetectorConstruction::DetectorConstruction() {
    fDetectorMessenger = new DetectorMessenger(this);
    
    fDetectorMaterials = new DetectorMaterials();
    fSourceMaterials = new SourceMaterials();
}

/*
 * Destructor
 */
DetectorConstruction::~DetectorConstruction() {
    delete fDetectorMessenger;
    delete fDetectorMaterials;
    delete fSourceMaterials;
}

/*
 * The light leakage problem.
 * 
 * Light leakage at optical window outer radius.
 * 
 * This is probably a genuine design consideration, as the solution varies by schematic,
 * no designs seem to have reflector at the outer radius of the window,
 * many have either aluminium enclosure or hermetic seal there.
 * 
 * Also varies based on crystal assembly or PMT integrated assembly.
 * 
 * This may be a slight loss mechanism, having reflector there would improve resolution.
 * (Not sure how likely it is that polished aluminium is used in all these cases)
 * 
 * ... there is probably a logical reason why reflector wouldnt extend there,
 * (i.e. need for hermetic sealing due to hygroscopic crystal,
 * mounting the crystal onto the PMT,
 * differences in crystal assembly and integrated PMT assembly, etc)
 * 
 * I think the best solution is:
 * - Extend aluminium encapsulation to window Z, add hermetic seal between glass and encapsulation
 * (However, will need to define dielectric_metal surface between the glass and seal)
 * 
 * Else:
 * - Could cut encapsulation at grease Z, add some other part between window outer rad and can outer rad
 * 
 * NOTE: Going to do an aluminium seal, and give it uncoated aluminium reflectivity,
 * in practice, wouldnt want uncoated aluminium exposed to mechanical/environmental stress,
 * but its not exposed to the crystal itself here, and only has a few optical photons
 * interacting with it (from outer rad of window)
 */

/*
 * Define the geometry to be created when run manager intialises
 * 
 * NOTE: Construct() will be called via run manager
 */
G4VPhysicalVolume* DetectorConstruction::Construct() {
    // ...
    // G4GeometryManager::GetInstance()->OpenGeometry();
    // G4PhysicalVolumeStore::GetInstance()->Clean();
    // G4LogicalVolumeStore::GetInstance()->Clean();
    // G4SolidStore::GetInstance()->Clean();
    
    /////////////
    // MATERIALS:
    /////////////
    
    fDetectorMaterials->DefineDetectorMats();
    fSourceMaterials->DefineSourceMats();
    
    /////////
    // WORLD:
    /////////
    
    G4VPhysicalVolume* worldPhys = BuildWorld();
    G4LogicalVolume* worldLog = worldPhys->GetLogicalVolume();
    
    ////////////
    // Envelope:
    ////////////
    
    // TODO: ...
    
    ////////
    // TABLE
    ////////
    
    // ...
    G4VPhysicalVolume* tablePhys = BuildTable(worldLog);
    
    G4double tableTransY = tablePhys->GetTranslation().y();
    G4double tableHalfHeight = static_cast<G4Box*>(tablePhys->GetLogicalVolume()->GetSolid())->GetYHalfLength() / cm; // TODO: Maybe rethink this, bit messy
    // (NOTE: half length in mm, convert to cm with division by cm unit)
    G4double tableTopY = tableTransY + tableHalfHeight; // (0.5 * tableHeight)
    
    ////////////
    // DETECTOR:
    ////////////
    
    DetectorGeometry detectorVolumes = BuildDetector(worldLog, tableTopY);

    //////////
    // SOURCE:
    //////////
    
    BuildSource(worldLog, tableTopY, detectorVolumes.detectorFaceZ, detectorVolumes.detectorX);

    
    ////////////
    // SURFACES:
    ////////////
    
    // ...
    DefineBorderSurfaces(detectorVolumes);
    
    // ...
    // AssignColours();
    
    
    ///////////
    // SCORING:
    ///////////

    // Will need to access logical photocathode volume outside of this methods scope (for scoring)
    // NOTE: May want to double check that "DETECTED" flag matches photocathode geometry
    // fScoringVolume = photocathodeLog;
    // NOTE: Assigned to the class property defined in header file

    
    ///////
    // ...:
    ///////

    // Always return world
    return worldPhys;
}

/*
 * ....
 */
G4VPhysicalVolume* DetectorConstruction::BuildWorld() {
    // The largest volume is the "World" volume (top level container)
    // NOTE: The world volume must contain all other volumes in the detector geometry (with some margin)
    // NOTE: A box is the most simple (and efficient) shape to describe the world

    // Define the world box dimensions (x, y, z)
    G4double const world_hx = 100 * cm;
    G4double const world_hy = 100 * cm;
    G4double const world_hz = 100 * cm;

    // Create a box named "World" with the specified dimensions, using the G4Box class
    auto worldBox = new G4Box("World", 0.5 * world_hx, 0.5 * world_hy, 0.5 * world_hz);

    // NOTE: The constructor takes its args as half of the total box size, hence this world extends from:
    // -0.5 to +0.5 along the X axis
    // -0.5 to +0.5 along the Y axis
    // -0.5 to +0.5 along the Z axis    
    
    // To create a logical volume: a solid and material are required (as a minimum)
    
    // ...
    // G4Material* air = G4Material::GetMaterial(...); // TODO ........................................................................................................
    // TODO: This isnt really a detector material
    auto air = fDetectorMaterials->Air();
    
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
        fCheckOverlaps // Check for overlapping geometry
    );
    // NOTE: ^THIS COULD BE AUTO TYPED (type is inferred by PVPlacement)
    
    return worldPhys;
}

/*
 * ...
 */
G4VPhysicalVolume* DetectorConstruction::BuildTable(G4LogicalVolume* worldLog) {
    ////////////
    // TABLETOP:
    ////////////
    
    // TODO: MOVE THIS BEFORE DETECTOR, INTEAD OF TABLE MOVING IN Y DIRECTION WITH CHANGE IN CRYSTAL SIZE,
    // CRYSTAL Y = (0.5 * table height) + (0.5 * crystal diameter)
    // (it makes logical sense that the table would be present in the world volume first anyways, then
    // you place the detector on the table, then the source on the table)

    // MDF style wood proxy, ~1' -> 2' thick
    
    // Table geometry parameters
    G4double const tableWidth = 50. * cm; // x
    G4double const tableHeight = 5. * cm; // y -- 5cm ? TODO: spitballing, need to refine this
    G4double const tableLength = 100. * cm; // z
    
    // ...
    auto table = new G4Box("Table", tableWidth * 0.5, tableLength * 0.5, tableHeight * 0.5);
    
    // TODO: This isnt really a detector material
    auto wood = fDetectorMaterials->Wood(); // ...........................................................................................
    
    // Using wood as a proxy for MDF (which would actually be slightly different)
    auto tableLog = new G4LogicalVolume(table, wood, "Table");
    
    // Rotate about z-axis 90 degrees
    auto tableRot = new G4RotationMatrix();
    tableRot->rotateX(90. * deg);
    // NOTE: Could just change xyz lengths, but leaving this here as example of rotation matrix
    
    // Translate in -y direction by radius of can + half thickness of table
    // G4double const tableTransY = -1. * (enclosureOuterRad + (tableHeight * 0.5));
    // NOTE: So that bottom of enclosure rests on table
    
    // ...
    G4double const tableTransY = 0. * cm;
    
    // Place the table below the detector
    G4VPhysicalVolume* tablePhys = new G4PVPlacement(
        tableRot, // Rotated 90 degrees in Z direction
        // nullptr,
        G4ThreeVector(0., tableTransY, 0.), 
        tableLog,
        "Table",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    return tablePhys;
}

/*
 * ...
 */
DetectorGeometry DetectorConstruction::BuildDetector(G4LogicalVolume* worldLog, G4double tableTopY) {
    ////////////////////////
    // SCINTILLATOR CRYSTAL:
    ////////////////////////
    
    // Dimensions for cylindrical scintillator crystal (radii, height, span)
    G4double const crystalInnerRad = 0. * cm; // No centre hole
    
    // G4double const inchToCM = 2.54;
    
    // G4double const crystalSize = 3 * inchToCM; // 3 inch crystal = 7.62 cm diameter => (diameter / 2) = 3.81 cm outer radius
    // G4double const crystalSize = 2 * inchToCM; // 2 inch crystal = 5.08 cm diameter => (diameter / 2) = 2.54 cm outer radius
    // G4double const crystalSize = 1 * inchToCM; // 1 inch crystal = 2.54 cm diameter => (diameter / 2) = 1.27 cm outer radius
    
    // G4double const crystalOuterRad = (7.62 * 0.5) * cm; // 3 inch = 7.62 cm diameter => (diameter / 2) = 3.81 cm outer radius
    // G4double const crystalHeight = 7.62 * cm; // 3 inch height (this arg will be doubled, so will need to * 0.5)
    
    // G4double const crystalOuterRad = (crystalSize * 0.5) * cm; // 2 inch = 7.62 cm diameter => (diameter / 2) = 3.81 cm outer radius
    // G4double const crystalHeight = crystalSize * cm; // 3 inch height (this arg will be doubled, so will need to * 0.5)
    
    G4double const crystalOuterRad = (fCrystalDiameter * 0.5); // 2 inch = 7.62 cm diameter => (diameter / 2) = 3.81 cm outer radius
    G4double const crystalHeight = fCrystalDiameter; // 3 inch height (this arg will be doubled, so will need to * 0.5)
    
    G4double const startAngle = 0. * deg;
    G4double const endAngle = 360. * deg; // Full circumference cylinder

    // Create the scintillator crystal solid (the detector)
    auto scintillator = new G4Tubs(
        "Scintillator",
        crystalInnerRad,
        crystalOuterRad,
        crystalHeight * 0.5,
        startAngle,
        endAngle
    );
    // NOTE: Cylinder has radius of 3.81 cm and height of 7.62 cm (the "height" param is also a half length)
    
    // ...
    G4Material* NaI = fDetectorMaterials->NaI();
    
    // Define the scintillator crystal material as sodium iodide
    auto scintillatorLog = new G4LogicalVolume(scintillator, NaI, "Scintillator");
    
    // TEST: Create a region for the crystal (for 100 um cuts only in detector volume)
    auto crystalRegion = new G4Region("Scintillator"); // NOTE: Havent imported this ?
    scintillatorLog->SetRegion(crystalRegion);
    crystalRegion->AddRootLogicalVolume(scintillatorLog);
    // TODO: Is this redundant?
    
    
    /////////////
    // REFLECTOR:
    /////////////
    
    // The scintillation photons are emitted in all directions, so a high efficiency reflector
    // is used to surround the crystal (Al_{2}O_{3} and teflon), on all sides except the back,
    // to increase the amount of photons reaching the the photocathode
    
    // Inner rad can (4.04495 cm) - outer rad crystal (3.81 cm) => 0.23495 cm reflector thickness
    G4double const reflectorThickness = 0.23495 * cm;
    
    G4double const reflectorOuterRad = crystalOuterRad + reflectorThickness;
    G4double const reflectorHeight = crystalHeight + (reflectorThickness * 2);
    // NOTE: Same as crystal height, with reflector thickness added to both ends
    
    // Base volume which will be cut
    auto reflectorSolid = new G4Tubs(
        "ReflectorSolid",
        0. * cm, // inner rad (no hole, as cut will handle it in this case)
        reflectorOuterRad, // outer rad
        reflectorHeight * 0.5, // height 
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // The section to cut from the base volume
    auto reflectorCut = new G4Tubs(
        "ReflectorCut",
        0. * cm, // inner rad (no hole, solid cut)
        crystalOuterRad, // outer rad (cut a section with same rad as crystal)
        reflectorHeight * 0.5, // height (cut has same height base, but will be translated)
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    // NOTE: To be a "perfect cut", could do +0.5*thickness & translate by same amount too,
    // currently after translation part of this solid is cutting nothing, but tbh thats fine,
    // i think this approach is more readable
    
    // Create new solid with cut subtracted from base
    auto reflector = new G4SubtractionSolid(
        "Reflector", // name
        reflectorSolid, // the solid to subtract from
        reflectorCut, // the volume to subtract
        nullptr, // no rotation
        G4ThreeVector(0., 0., reflectorThickness) // cut translation (relative to base solid, not world)
    );
    
    // ...
    G4Material* Al2O3 = fDetectorMaterials->Al2O3();
    
    // Assign a material to the reflector solid
    auto reflectorLog = new G4LogicalVolume(
        reflector, // subtraction solid acts same as any other geometry here
        Al2O3, // reflector material
        "Reflector"
    );
    
    // NOTE: This current setup works, but leaves an overhang of reflector,
    // this isnt really an issue and maybe worth keeping,
    // but may also want reflector to stop in line with back of crystal,
    // just need to give "ReflectorSolid" (crystalheight + 0.5 * reflectorThick),
    // but it also kinda needs to 
    
    
    //////////////////
    // OPTICAL GREASE:
    //////////////////
    
    // Silicon gel with ~same thickness as reflector (2.3495 mm as per schematics)
    // NOTE: Some sources say only 0.1 mm thickness is suggested though ^ (and less kinda makes sense)
    // NOTE: Some sources say only 10-50 um thickness
    // NOTE: OST Photonics 2" NaI schematic states 2mm gel thickness
        
    // ... the optical grease will then be pressed against the PMT window
    // G4double const greaseThickness = reflectorThickness; // Same thickness as reflector (2.3495 mm)
    G4double const greaseThickness = 25 * um; // TEST
    
    // Optical grease (transmitting incident optical photons to the PMT window)
    auto grease = new G4Tubs(
        "OpticalGrease",
        crystalInnerRad, // 0cm
        crystalOuterRad, // same radius as crystal (slots into reflector)
        greaseThickness * 0.5, // (will be multiplied by 2 on placement)
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // PDMS (proxy for silicone gel)
    G4Material* PDMS = fDetectorMaterials->PDMS();
    
    // ...
    auto greaseLog = new G4LogicalVolume(grease, PDMS, "OpticalGrease");
    
    // TEST TEST TEST
    // Set maximum step size inside of grease volume
    // auto greaseStepLimit = new G4UserLimits(5. * um);
    // greaseLog->SetUserLimits(greaseStepLimit);
    // NOTE: Seems to have zero impact on spectrum compared to omitting this
    // TEST TEST TEST
    
    
    ////////////////////////
    // PHOTOMULTIPLIER TUBE:
    ////////////////////////

    // NOTE: Just modelling the PMT window (OPTICAL WINDOW)
    
    G4double const windowThick = 0.2 * cm; // 2mm according to hamamatsu handbook
    
    // Optical window (transmitting incident optical photons to the PMT window)
    auto window = new G4Tubs(
        "OpticalWindow",
        crystalInnerRad, // 0cm
        crystalOuterRad, // same radius as crystal (slots into reflector)
        windowThick * 0.5, // ... (will be multiplied by 2 on placement)
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // Borosilicate glass (high optical performance)
    G4Material* borosilicate = fDetectorMaterials->Borosilicate();
    
    // ...
    auto windowLog = new G4LogicalVolume(window, borosilicate, "OpticalWindow");
    
    
    ////////////////
    // PHOTOCATHODE:
    ////////////////
    
    // Coating inside of PMT optical window
    
    // ...
    // G4double const photocathodeThick = 0.1 * cm; // 1mm
    G4double const photocathodeThick = 20 * nm; // 20nm
    
    // Photocathode (absorbing or detecting incident optical photons)
    auto photocathode = new G4Tubs(
        "Photocathode",
        crystalInnerRad, // 0cm
        crystalOuterRad, // same radius as crystal & window (painted onto back of window)
        photocathodeThick * 0.5,
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // Lithium (alkali metal due to low binding energy of outer shell electron)
    G4Material* Li = fDetectorMaterials->Li(); // TODO: KCsSb by atoms (not that it really matters due to surface interaction)
    
    // ...
    auto photocathodeLog = new G4LogicalVolume(photocathode, Li, "Photocathode");
    
    
    ///////////////////////
    // ALUMINIUM ENCLOSURE:
    ///////////////////////
        
    // Enclosure is 3.225' outer diameter according to ortec spec, and 0.2' thickness (0.508mm)
    // G4double const inchToCM = 2.54;
    G4double const enclosureThick = 0.0508 * cm; // NOTE: 0.02' => 0.508 mm (added to both sides in Z direction)
    
    // G4double const enclosureOuterRad = ((3.225 * 2.54) / 2) * cm; // NOTE: 3.225' dia => 8.1915cm dia => 4.09575 cm outer rad
    // G4double const enclosureOuterRad = ((3.225 * inchToCM) / 2) * cm; // NOTE: 3.225' dia => 8.1915cm dia => 4.09575 cm outer rad
    
    G4double const enclosureOuterRad = reflectorOuterRad + enclosureThick; // NOTE: 3.225' dia => 8.1915cm dia => 4.09575 cm outer rad
    G4double const enclosureLength = reflectorHeight + (enclosureThick * 2); // NOTE: Same height as reflector, with thickness added to both ends
    // NOTE: 4.04495 cm inner rad
    
    // TODO: THIS IS HARDCODED CURRENTLY, SHOULD AUTOMATICALLY ADJUST WITH CHANGES TO CRYSTAL SIZE
    // 
    
    // Base volume which will be cut
    auto enclosureSolid = new G4Tubs(
        "EnclosureSolid",
        0. * cm, // inner rad (no hole, as cut will handle it in this case)
        enclosureOuterRad, // outer rad
        enclosureLength * 0.5, // height
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // The section to cut from the base volume
    auto enclosureCut = new G4Tubs(
        "EnclosureCut",
        0. * cm, // inner rad (no hole, solid cut)
        reflectorOuterRad, // outer rad (cut a section with same rad as reflector)
        enclosureLength * 0.5, // height (cut has same height as base)
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    // NOTE: To be a "perfect cut", could do +0.5*thickness & translate by same amount too,
    // currently after translation part of this solid is cutting nothing, but tbh thats fine,
    // i think this approach is more readable
    
    // Create new solid with cut subtracted from base
    auto enclosure = new G4SubtractionSolid(
        "Enclosure", // name
        enclosureSolid, // the solid to subtract from
        enclosureCut, // the volume to subtract
        nullptr, // no rotation
        G4ThreeVector(0., 0., enclosureThick) // cut translation (relative to base solid, not world)
    );
    
    // ...
    G4Material* Al = fDetectorMaterials->Al();
    
    // Assign a material to the enclosure solid
    auto enclosureLog = new G4LogicalVolume(
        enclosure, // subtraction solid acts same as any other geometry here
        Al, // enclosure material (aluminium)
        "Enclosure"
    );
    
    
    /////////////////
    // HERMETIC SEAL:
    /////////////////
    
    // NOTE: Im not 100% set on this design (with this component), but it will do for now
    // this could also be done with an addition solid,
    // but not sure i like the sound of that, as in reality it would be impossible to manufacture
    // (fiting crystal and reflector inside)
    
    // The seal could be something that is screwed in or welded to the enclosure
    
    // ...
    G4double const sealLength = windowThick;
    G4double const sealOuterRad = reflectorOuterRad;
    
    // Base volume which will be cut
    auto seal = new G4Tubs(
        "HermeticSeal",
        crystalOuterRad, // inner rad (no hole, as cut will handle it in this case)
        sealOuterRad, // outer rad
        sealLength * 0.5, // height
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // Assign a material to the seal solid
    auto sealLog = new G4LogicalVolume(
        seal, // subtraction solid acts same as any other geometry here
        Al, // seal material (aluminium)
        "HermeticSeal"
    );
    
    
    //////////////////////////////////////
    // DETECTOR PHYSICAL VOLUME PLACEMENT:
    //////////////////////////////////////
    
    // Define coordinates for scintillator crystal (offset from mother origin)
    G4double const detectorX = 0. * cm;
    G4double const detectorY = tableTopY + enclosureOuterRad;
    // G4double const detectorZ = 10. * cm; // 10cm (maybe 3cm as i have a lot of data for that distance)
    G4double const detectorZ = 0. * cm; // 10cm (maybe 3cm as i have a lot of data for that distance)
    
    // Define translation vector (relative to mother origin)
    auto const detectorOrigin = G4ThreeVector(detectorX, detectorY, detectorZ);
    
    // Place the enclosure
    G4VPhysicalVolume* enclosurePhys = new G4PVPlacement(
        nullptr, // no rotation
        detectorOrigin, // same position as crystal
        enclosureLog, // logical volume
        "Enclosure", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        fCheckOverlaps
    );
    
    // ...
    // ...
    
    // Place the reflector
    G4VPhysicalVolume* reflectorPhys = new G4PVPlacement(
        nullptr, // no rotation
        detectorOrigin, // same position as crystal
        reflectorLog, // logical volume
        "Reflector", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        fCheckOverlaps
    );
    
    // ...
    // ...

    // Place the sodium iodide scintillator crystal (inside of the world)
    G4VPhysicalVolume* crystalPhys = new G4PVPlacement(
        nullptr, // No rotation
        detectorOrigin, // Translation
        scintillatorLog, // The logical volume
        "Scintillator", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        fCheckOverlaps
    );
    // NOTE: This places the scintillator at the origin of the mother volume, shifted by 0.5 meter along Z
    
    // ...
    // ...
    
    // Translation along Z axis (relative to crystal origin)
    G4double const greaseZ = detectorZ + (crystalHeight * 0.5) + (greaseThickness * 0.5); // TEST
    // NOTE: Places it on crystal Z (centre of the crystal), 
    // translates it by half the crystal height (to account for it being centre of crystal),
    // due to 0.5 grease thickness being placed either side of this components origin,
    // need to shift it by a further 0.5 * grease thickness
    
    // Placed on the back side of the crystal
    G4VPhysicalVolume* greasePhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(detectorX, detectorY, greaseZ), // Translation
        greaseLog, // The logical volume
        "OpticalGrease", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        fCheckOverlaps
    );
    
    // ...
    // ...
    
    // Translation along Z axis (relative to optical grease origin)
    G4double const windowZ = greaseZ + (greaseThickness * 0.5) + (windowThick * 0.5); // TEST
    // NOTE: Places it on grease Z (centre of the grease),
    // translates it by half the grease height (to account for it being centre of grease),
    // due to 0.5 window thickness being placed either side of window origin,
    // need to shift it by 0.5 * its thickness
    
    // TEST
    // Translation along Z axis (relative to crystal origin)
    // G4double const windowZ = crystalZ + (crystalHeight * 0.5) + (windowThick * 0.5); // TEST
    // TEST
    
    // Placed against the optical grease
    G4VPhysicalVolume* windowPhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(detectorX, detectorY, windowZ), // Translation
        windowLog, // The logical volume
        "OpticalWindow", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        fCheckOverlaps
    );
    
    // ...
    // ...
    
    // Translation along Z axis (relative to optical window origin)
    G4double const photocathodeZ = windowZ + (windowThick * 0.5) + (photocathodeThick * 0.5);
    // NOTE: Places it on window Z (centre of the window),
    // translates it by half the window height (to account for it being centre of window),
    // due to 0.5 window thickness being placed either side of window origin,
    // need to shift it by 0.5 * its thickness
    
    // Placed on the inside of the PMT window
    G4VPhysicalVolume* photocathodePhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(detectorX, detectorY, photocathodeZ), // Translation
        photocathodeLog, // The logical volume
        "Photocathode", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        fCheckOverlaps
    );
    
    // ...
    // ...
    
    // Translation along Z axis (relative to optical window origin)
    // G4double const sealZ = windowZ;
    G4double const sealZ = detectorZ + (crystalHeight * 0.5) + reflectorThickness + (sealLength * 0.5); // TEST
    
    // Place the seal
    G4VPhysicalVolume* sealPhys = new G4PVPlacement(
        nullptr, // no rotation
        G4ThreeVector(detectorX, detectorY, sealZ), // same position as crystal
        sealLog, // logical volume
        "HermeticSeal", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        fCheckOverlaps
    );
    
    // ...
    G4double detectorFaceZ = detectorZ - ((crystalHeight * 0.5) + reflectorThickness + enclosureThick);
    
    // ...
    return { crystalPhys, reflectorPhys, enclosurePhys, greasePhys, windowPhys, photocathodePhys, sealPhys };
}

/*
 * ...
 */
void DetectorConstruction::BuildSource(G4LogicalVolume* worldLog, G4double tableTopY, G4double detectorFaceZ, G4double detectorX) {
    //////////
    // SOURCE:
    //////////
    
    // Source geometry specification
    // G4double const sourceRadius = 0.5 * cm; // 1cm diameter
    // G4double const sourceThickness = 0.1 * cm; // 1mm thickness
    
    // G4double const sourceRadius = (2.54 * 0.5) * cm; // 1" -> 2.54cm diameter -> 1.27cm radius (typical of disk sources)
    // NOTE: Lab sources must have smaller diameters as this wouldnt fit casing (they are made in house tbf)
    G4double const sourceRadius = (1.905 * 0.5) * cm; // 1" -> 2.54cm diameter -> 1.27cm radius (typical of disk sources)
    G4double const sourceThickness = 0.3175 * cm; // 0.125" -> 3.175mm thickness (typical of disk sources)
    
    // Source geometry definition (modelled as cylinder)
    auto solidSource = new G4Tubs(
        "Source", // name
        0., // inner radius (0 = not hollow)
        sourceRadius, // outer radius
        sourceThickness * 0.5, // thickness
        0. * deg, // start angle
        360 * deg // end angle (full span here)
    );
    
    // ...
    G4Material* sourceMat = fSourceMaterials->Create137Cs(); // TODO: This needs to be dependent on the macro command
    // TODO: Maybe an if clause to check if material already made inside of Create137Cs
    // this is both a creator and getter, unlike other material methods
    
    // Define the radioactive source with the created material
    auto sourceLog = new G4LogicalVolume(solidSource, sourceMat, "Source");
    
    /////////////////
    // SOURCE CASING:
    /////////////////
    
    // Encapsulates alpha and beta radiation
    
    // Box base solid
    // Subtraction solid of same geometry as source
    // likely slightly different in reality but itll do
    
    // Source encapsulation dimensions
    G4double const casingSizeX = 3. * cm; // 3cm wide
    G4double const casingSizeY = 3. * cm; // 3cm high
    // G4double const casingSizeZ = 1 * cm; // 1cm thick
    G4double const casingSizeZ = 0.7 * cm; // 7mm thick (the front and back windows then make up the extra 3mm, at 1.5mm thickness each)
    
    // Base geometry which will be cut
    auto casingBase = new G4Box("SourceCasingBase", casingSizeX * 0.5, casingSizeY * 0.5, casingSizeZ * 0.5);
    
    // Cut to be made in base geometry
    // auto casingCut = new G4Box("SourceCasingBase", casingSizeX * 0.25, casingSizeY * 0.25, casingSizeZ * 0.25);
    // auto casingCut = new G4Box("SourceCasingBase", casingSizeX * 0.25, casingSizeY * 0.25, casingSizeZ * 0.6);
    G4double const casingCutSizeX = casingSizeX - (2 * (0.3 * cm)); // thickness of remaining material on either side of cut ~3mm, so ~6mm total
    G4double const casingCutSizeY = casingSizeY - (2 * (0.3 * cm)); // likewise
    G4double const casingCutSizeZ = casingSizeZ * 1.1; // z cut slightly larger than z thickness to ensure no thin skin leftovers
    auto casingCut = new G4Box("SourceCasingCut", casingCutSizeX * 0.5, casingCutSizeY * 0.5, casingCutSizeZ * 0.5);
    
    // Create new solid with cut subtracted from base
    auto casing = new G4SubtractionSolid(
        "SourceCasing", // name
        casingBase, // the solid to subtract from
        casingCut, // the volume to subtract
        nullptr, // no rotation
        G4ThreeVector(0., 0., 0.) // cut translation (relative to base solid, not world)
    );
    
    // ...
    G4Material* PMMA = fSourceMaterials->PMMA();
    
    // Assign a material to the casing solid
    auto casingLog = new G4LogicalVolume(
        casing, // subtraction solid acts same as any other geometry here
        // PVC, // casing material (G4_POLYVINYL_CHLORIDE)
        PMMA, // casing material (acrylic - PMMA)
        "SourceCasing"
    );
    
    /////////////////////////
    // SOURCE CASING WINDOWS:
    /////////////////////////
    
    // Two of these will be placed, one on the front of the casing and one on the back
    G4double const casingWindowSizeZ = 0.15 * cm;
    
    // ...
    auto casingWindow = new G4Box("SourceCasingWindow", casingSizeX * 0.5, casingSizeY * 0.5, casingWindowSizeZ * 0.5);
    
    // ...
    auto casingWindowLog = new G4LogicalVolume(
        casingWindow,
        PMMA, // casing material (acrylic - PMMA)
        "SourceCasingWindow"
    );
    
    
    /////////////////
    // SOURCE HOLDER:
    /////////////////
    
    // Base geometry which will be cut
    G4double const holderSizeX = 4. * cm; // 4cm wide
    G4double const holderSizeY = 2. * cm; // 2cm high
    G4double const holderSizeZ = 1.95 * cm; // 1.95cm thick
    
    auto holderBase = new G4Box("SourceHolderBase", holderSizeX * 0.5, holderSizeY * 0.5, holderSizeZ * 0.5);
    
    // Make the 1st cut where source casing goes
    // (width of source casing @~3cm, half the height of the holder, thickness of source casing @~1cm)
    // auto holderCut1 = new G4Box("SourceHolderCut1", ((casingSizeX * 0.5) + 0.05 * cm), holderSizeY * 0.25, casingSizeZ * 0.5);
    auto holderCut1 = new G4Box("SourceHolderCut1", ((casingSizeX * 0.5) + 0.05 * cm), holderSizeY * 0.25, 0.5 * cm);
    
    // Translation for 1st cut relative to base:
    // Translate the box to cut from the origin of the base, up by half the bases height, so that the origin of the cut box
    // is aligned with the top of the base in y direction, then translate it down by half the cuts height so that the cut
    // is fully inside of the base
    // G4double const holderCutTransY = (0.5 * casingSizeY) - (0.5 * holderSizeY);
    G4double const holderCutTransY = (0.5 * holderSizeY) - (0.25 * holderSizeY);
    // G4double const holderCutTransY = (0.75 * holderSizeY);
    auto const holderCut1Trans = G4ThreeVector(0., holderCutTransY, 0.);
    
    // ...
    auto holderSubtracted = new G4SubtractionSolid(
        "SourceHolder",
        holderBase, // base solid to make cut in
        holderCut1, // cut to make in base
        nullptr,
        holderCut1Trans // holder cut translation
    );
    
    // Dimensions for 2nd cut where the faces in the z direction are fully removed
    G4double const cut2SizeX = 2.6 * cm; // make a cut 2.6cm wide
    G4double const cut2SizeY = (holderSizeY * 0.5) + (0.1 * cm); // cut half way into the holder in y direction (same as 1st cut) (plus extra 0.1cm to trim leftovers)
    G4double const cut2SizeZ = holderSizeZ * 1.1; // cut the full length of the holder in z direction (plus a little extra to trim paper thin leftovers from same sized cut)
    
    auto holderCut2 = new G4Box("SourceHolderCut2", cut2SizeX * 0.5, cut2SizeY * 0.5, cut2SizeZ * 0.5);
    
    // Translation for 2nd cut relative to base:
    // 
    
    // G4double const holderCut2TransY = 0.75 * holderSizeY;
    G4double const holderCut2TransY = (0.5 * holderSizeY) - ((0.5 * cut2SizeY) - (0.1 * cm));
    auto const holderCut2Trans = G4ThreeVector(0., holderCut2TransY, 0.);
    
    // Make the 2nd cut
    
    auto holder = new G4SubtractionSolid(
        "SourceHolder",
        holderSubtracted, // base solid to make cut in
        holderCut2, // cut to make in base
        nullptr,
        holderCut2Trans
    );
    
    // ...
    G4Material* PLA = fSourceMaterials->PLA();
    // the source holder looked 3D printed (and distinctly different from source casing and windows), likely PLA or ABS
    
    // Logical volume
    auto holderLog = new G4LogicalVolume(
        holder,
        PLA,
        "SourceHolder"
    );
    
    ////////////////////////////////////
    // SOURCE PHYSICAL VOLUME PLACEMENT:
    ////////////////////////////////////
    
    // NOTE: Face of the detector is 5.90425 cm from world origin (0, 0, 0)
    
    // 3cm source-detector (face) distance, as it was in lab work (and my recorded spectra)
    // G4double const sourceDetectorDist = 3. * cm;
    
    // Define translation along the z axis
    // G4double const sourceZ = detectorZ - ((crystalHeight * 0.5) + reflectorThickness + enclosureThick) - sourceDetectorDist;
    // G4double const sourceZ = detectorZ - ((crystalHeight * 0.5) + reflectorThickness + enclosureThick) - fSourceDetectorDistance;
    G4double const sourceZ = detectorFaceZ - fSourceDetectorDistance;
    // NOTE: place the source at the detector origin, shift it by half the crystal length,
    // then by reflector thickness, then by enclosure thickness (now resides at detector face)
    // -> then use source detector distance to specify distance from detector face
    
    ////
    // Source Holder
    ////
    
    G4double const holderY = tableTopY + (0.5 * holderSizeY);
    
    auto const holderTrans = G4ThreeVector(detectorX, holderY, sourceZ);
    
    G4VPhysicalVolume* holderPhys = new G4PVPlacement(
        nullptr,
        holderTrans,
        holderLog,
        "SourceHolder",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    ////
    // Source Casing
    ////
    
    // need to align the bottom of the casing with the centre of the holder
    // translating bottom of casing to aling with bottom of holder: 1/2 of casing height - 1/2 of holder height
    // then translating that up by half of the holder
    
    G4double const casingTransY = holderY + ((0.5 * casingSizeY) - (0.5 * holderSizeY)) + (0.5 * holderSizeY);
    auto const casingTrans = G4ThreeVector(detectorX, casingTransY, sourceZ);
    
    // Place the casing
    G4VPhysicalVolume* casingPhys = new G4PVPlacement(
        nullptr, // no rotation
        casingTrans, // same position as source
        casingLog, // logical volume
        "SourceCasing", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        fCheckOverlaps
    );
    
    ////
    // Radioactive Source Volume
    ////
    
    // ...
    auto const sourceTrans = G4ThreeVector(detectorX, casingTransY, sourceZ); // NOTE: SAME AS CASING TRANSLATION VECTOR ...
    // NOTE: Source is placed exactly in line with crystal in x plane,
    // y is determined by table and source enclosure, 
    // and is a specified distance from detector face in z
    
    // Place the radioactive source (TODO TODO TODO TODO)
    G4VPhysicalVolume* sourcePhys = new G4PVPlacement(
        nullptr,
        sourceTrans,
        sourceLog,
        "Source",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    
    ////
    // Casing Windows
    ////
    
    G4double const frontCasingWindowZ = sourceZ + (0.5 * casingSizeZ) + (0.5 * casingWindowSizeZ);
    auto const frontCasingWindowTrans = G4ThreeVector(detectorX, casingTransY, frontCasingWindowZ);
    
    G4VPhysicalVolume* frontCasingWindowPhys = new G4PVPlacement(
        nullptr,
        frontCasingWindowTrans,
        casingWindowLog,
        "FrontSourceCasingWindow",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    G4double const backCasingWindowZ = sourceZ - (0.5 * casingSizeZ) - (0.5 * casingWindowSizeZ);
    auto const backCasingWindowTrans = G4ThreeVector(detectorX, casingTransY, backCasingWindowZ);
    G4VPhysicalVolume* backCasingWindowPhys = new G4PVPlacement(
        nullptr,
        backCasingWindowTrans,
        casingWindowLog,
        "BackSourceCasingWindow",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    //////
    //
    //////
    
    // Assign the logical source volume to the class member
    // fSourceVolume = sourceLog;
    // TODO: This should be sourcePhys so that x,y,z can be extracted
    
    fSourceVolume = sourceLog; // NOTE: TEMP UNTIL PLACING PHYSICAL VOLUME
    fSourceCoords = sourceTrans; // NOTE: TEMP UNTIL PLACING PHYSICAL VOLUME
    // fSourceVolume = sourcePhys;
}

/*
 * ....
 */
void DetectorConstruction::DefineBorderSurfaces(DetectorGeometry detectorVolumes) {
    // ...
    // auto [crystalPhys, reflectorPhys, enclosurePhys, greasePhys, windowPhys, photocathodePhys, sealPhys] = detectorVolumes;
    
    // Define the border between the crystal and the reflector
    auto crystalReflectorBorder = new G4LogicalBorderSurface("CrystalToReflector", detectorVolumes.crystalPhys, detectorVolumes.reflectorPhys, fDetectorMaterials->ReflectorSurface());

    // NOTE: Crystal->Grease, and Grease->Window surfaces are not explicitly needed when rindex of each is passed
    // will default to: GLISUR, polished, dielectric_dielectric
    // ^ maybe do it just to explicitly pick UNIFIED
    auto crystalGreaseBorder = new G4LogicalBorderSurface("CrystalToGrease", detectorVolumes.crystalPhys, detectorVolumes.greasePhys, fDetectorMaterials->GreaseSurface());
    auto greaseWindowBorder = new G4LogicalBorderSurface("GreaseToWindow", detectorVolumes.greasePhys, detectorVolumes.windowPhys, fDetectorMaterials->WindowSurface());
    // NOTE: After testing, adding these two borders produces an identical spectrum to just 
    // leaving these two borders as default
    
    
    // TEST
    // auto crystalWindowBorder = new G4LogicalBorderSurface("CrystalToWindow", crystalPhys, windowPhys, fDetectorMaterials->greaseSurface());
    // auto crystalWindowBorder = new G4LogicalBorderSurface("CrystalToWindow", crystalPhys, windowPhys, fDetectorMaterials->windowSurface());
    // TEST
    
    
    // TEST \/\/\/\/
    // Define the border between the reflector and enclosure
    // auto reflectorEnclosureBorder = new G4LogicalBorderSurface("ReflectorToEnclosure", reflectorPhys, enclosurePhys, fDetectorMaterials->aluminiumSurface());
    //
    // Define the border between the reflector and hermetic seal
    // auto reflectorSealBorder = new G4LogicalBorderSurface("ReflectorToSeal", reflectorPhys, sealPhys, fDetectorMaterials->aluminiumSurface());
    // TEST ^^^^^^^
    
    // NOTE: ^^^ These are not needed, as groundbackpainted prevents refraction
    
    // TODO: Border back from: (enclosurePhys -> reflectorPhys) & (sealPhys - reflectorPhys)
    // auto enclosureReflectorBorder = new G4LogicalBorderSurface("EnclosureToReflector", enclosurePhys, reflectorPhys, fDetectorMaterials->reflectorSurface());
    // auto enclosureSealBorder = new G4LogicalBorderSurface("SealToReflector", sealPhys, reflectorPhys, fDetectorMaterials->reflectorSurface());
    // TODO: Border back from reflectorPhys->crystalPhys
    // auto reflectorCrystalBorder = new G4LogicalBorderSurface("ReflectorToCrystal", reflectorPhys, crystalPhys, fDetectorMaterials->reflectorSurface());
    
    // NOTE: ^^^ I DONT THINK THESE ARE ACTUALLY NEEDED (borders work both ways as is)
    
    // Define the border between the optical grease and the reflector
    auto greaseReflectorBorder = new G4LogicalBorderSurface("GreaseToReflector", detectorVolumes.greasePhys, detectorVolumes.reflectorPhys, fDetectorMaterials->ReflectorSurface()); // TEST (FIXES LOST PHOTONS)
    // TODO: Is the crystal->reflector surface sufficient here? Not really as the sigma alpha value is for crystal surface, and uses rindex air for gap
    
    // ...
    auto windowReflectorBorder = new G4LogicalBorderSurface("WindowToReflector", detectorVolumes.windowPhys, detectorVolumes.reflectorPhys, fDetectorMaterials->ReflectorSurface()); // TEST (for 25 um grease geom)
    
    // Define the border between the optical window and the hermetic seal
    auto windowSealBorder = new G4LogicalBorderSurface("WindowToSeal", detectorVolumes.windowPhys, detectorVolumes.sealPhys, fDetectorMaterials->AluminiumSurface()); // NOTE: UNCOMMENT ME
    
    // Define the border between the optical window and the photocathode
    auto windowPhotocathodeBorder = new G4LogicalBorderSurface("WindowToPhotocathode", detectorVolumes.windowPhys, detectorVolumes.photocathodePhys, fDetectorMaterials->PhotocathodeSurface());
}

/*
 * ...
 */
// void DetectorConstruction::AssignColours() {
//     /////////////
//     // COLOURING:
//     /////////////
// 
//     // Set visualiser colouring (R, G, B, opacity), and assign colours to the detector geometry
//     // NOTE: I think these could also be set via the visualiser init macro script
//    
//     // World
//     auto worldVisAtt = new G4VisAttributes(G4Color(0., 0., 1., 0.1)); // blue (transparent)
//     worldVisAtt->SetForceSolid(true); // ... (think this can be called w/ no arg for same effect)
//     worldLog->SetVisAttributes(worldVisAtt); // assign to the logical volume
// 
//     // Scintillator crystal
//     auto scintillatorVisAtt = new G4VisAttributes(G4Color(1., 1., 1., 0.5)); // white (part-transparent)
//     scintillatorVisAtt->SetForceSolid(true);
//     scintillatorLog->SetVisAttributes(scintillatorVisAtt);
//     
//     // Aluminium oxide reflector
//     auto reflectorVisAtt = new G4VisAttributes(G4Color(0., 0., 1.0, 0.5)); // blue
//     reflectorVisAtt->SetForceSolid(true);
//     reflectorLog->SetVisAttributes(reflectorVisAtt);
//     
//     // Aluminium enclosure
//     auto enclosureVisAtt = new G4VisAttributes(G4Color(0.8, 0.8, 0.8, 1.)); // mid-light gray (solid)
//     enclosureVisAtt->SetForceSolid(true);
//     enclosureLog->SetVisAttributes(enclosureVisAtt);
//     
//     // Optical grease
//     auto greaseVisAtt = new G4VisAttributes(G4Color(1.0, 1.0, 0., 0.75)); // yellow
//     greaseVisAtt->SetForceSolid(true);
//     greaseLog->SetVisAttributes(greaseVisAtt);
//     
//     // Optical window
//     auto windowVisAtt = new G4VisAttributes(G4Color(0.8, 0.8, 0.8, 0.25)); // mid-light gray (transparent)
//     windowVisAtt->SetForceSolid(true);
//     windowLog->SetVisAttributes(windowVisAtt);
//     
//     // Scoring photocathode
//     auto photocathodeVisAtt = new G4VisAttributes(G4Color(1., 0., 0., 0.5)); // red
//     photocathodeVisAtt->SetForceSolid(true);
//     photocathodeLog->SetVisAttributes(photocathodeVisAtt);
//     
//     // Hermetic seal
//     auto sealVisAtt = new G4VisAttributes(G4Color(0.9, 0.9, 0.9, 1.)); // light gray (opaque)
//     sealVisAtt->SetForceSolid(true);
//     sealLog->SetVisAttributes(sealVisAtt);
//     
//     // Source geometry
//     auto sourceVisAtt = new G4VisAttributes(G4Color(0.0, 1.0, 0.0, 0.5)); // green
//     sourceVisAtt->SetForceSolid(true);
//     sourceLog->SetVisAttributes(sourceVisAtt);
//     
//     // Source casing geometry
//     auto casingVisAtt = new G4VisAttributes(G4Color(0.8, 0.8, 0.8, 1.)); // mid-light gray (opaque)
//     casingVisAtt->SetForceSolid(true);
//     casingLog->SetVisAttributes(casingVisAtt);
//     
//     // Source casing windows geometry
//     auto casingWindowsVisAtt = new G4VisAttributes(G4Color(0.8, 0.8, 0.8, 0.25)); // mid-light gray (transparent)
//     casingWindowsVisAtt->SetForceSolid(true);
//     casingWindowLog->SetVisAttributes(casingWindowsVisAtt);
//     
//     // Source holder geometry
//     auto holderVisAtt = new G4VisAttributes(G4Color(0.1, 0.1, 0.1, 1.)); // charcoal (opaque)
//     holderVisAtt->SetForceSolid(true);
//     holderLog->SetVisAttributes(holderVisAtt);
//     
//     // Tabletop geometry
//     auto tableVisAtt = new G4VisAttributes(G4Color(0.95, 0.95, 0.95, 1.)); // light gray
//     tableVisAtt->SetForceSolid(true);
//     tableLog->SetVisAttributes(tableVisAtt);
// }

/*
 * ...
 */
// void DetectorConstruction::SetCrystalDiameter(G4double diameterInInches) {
void DetectorConstruction::SetCrystalDiameter(G4double diameter) {
    // fCrystalDiameter = diameterInInches * 2.54;
    
    // fCrystalDiameter = diameterInInches;
    fCrystalDiameter = diameter;
    
    // ...
    // ...
    
    // G4RunManager::GetRunManager()->GeometryHasBeenModified();
    // G4RunManager::GetRunManager()->ReinitializeGeometry();
    // G4RunManager::GetRunManager()->ReinitializeGeometry(true);
    
    // G4VVisManager* visManager = G4VVisManager::GetConcreteInstance();
    // if (visManager) {
    //     visManager->GeometryHasChanged();
    //     visManager->NotifyHandlers();
    //     G4cout << "UPDATING VIS" << G4endl;
    // }
    
    // ...
    // ...
    
    
    // // clean-up previous geometry
    // G4SolidStore::GetInstance()->Clean();
    // G4LogicalVolumeStore::GetInstance()->Clean();
    // G4PhysicalVolumeStore::GetInstance()->Clean();
    // 
    // //define new one
    // G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
    // G4RunManager::GetRunManager()->GeometryHasBeenModified();
    
    // ...
    // ...
    
    // G4RunManager::GetRunManager()->ReinitializeGeometry(true);
    // G4RunManager::GetRunManager()->GeometryHasBeenModified();
//     
//     G4UImanager* UImanager = G4UImanager::GetUIpointer();
//     if (UImanager) {
//         UImanager->ApplyCommand("/vis/scene/clear");
//         UImanager->ApplyCommand("/vis/add/volume");
//         UImanager->ApplyCommand("/vis/viewer/rebuild");
//         UImanager->ApplyCommand("/vis/viewer/update");
//     }
}

/*
 * ...
 */
void DetectorConstruction::SetSourceDetectorDistance(G4double distance) {
    fSourceDetectorDistance = distance;
    
    // G4RunManager::GetRunManager()->ReinitializeGeometry();
}

/*
 * ...
 */
void DetectorConstruction::SetSource(G4String isotope) {
    if (isotope == "137Cs") {
        
    } else if (isotope == "60Co") {
        
    } else if (isotope == "22Na") {
        
    } else if (isotope == "241Am") {
        
    }
    
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}
