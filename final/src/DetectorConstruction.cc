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
#include "DetectorGeometry.hh"
#include "SourceGeometry.hh"
#include "DetectorMessenger.hh"

// G4 lib
// #include "G4NistManager.hh"
// #include "G4Element.hh"
// #include "G4Material.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "G4Box.hh"
// #include "G4Tubs.hh"
// #include "G4Sphere.hh"
// #include "G4SubtractionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

// #include "G4VisAttributes.hh"
// #include "G4Color.hh"

// #include "G4MaterialPropertiesTable.hh" // for optical photons
// #include "G4MaterialPropertyVector.hh" // can use instead of 2x std::vector

// #include "G4OpticalSurface.hh"
// #include "G4LogicalBorderSurface.hh"
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
    // ...
    fDetectorMessenger = new DetectorMessenger(this);
    
    // ...
    fDetectorMaterials = new DetectorMaterials();
    fSourceMaterials = new SourceMaterials();
    
    // ...
    fDetectorGeometry = new DetectorGeometry(*fDetectorMaterials);
    fSourceGeometry = new SourceGeometry(*fSourceMaterials);
}

/*
 * Destructor
 */
DetectorConstruction::~DetectorConstruction() {
    delete fDetectorMessenger;
    delete fDetectorMaterials;
    delete fSourceMaterials;
    delete fDetectorGeometry;
    delete fSourceGeometry;
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
    fDetectorMaterials->DefineOpticalProperties();
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
    
    /////////
    // TABLE:
    /////////
    
    G4double const tableTopY = BuildTable(worldLog);
    
    ////////////
    // DETECTOR:
    ////////////
    
    DetectorBuild detectorBuild = fDetectorGeometry->BuildDetector(worldLog, tableTopY, fCrystalDiameter, fCheckOverlaps);
    // TODO: Rename struct and variable

    //////////
    // SOURCE:
    //////////
    
    G4VPhysicalVolume* sourcePhys = fSourceGeometry->BuildSource(worldLog, tableTopY, detectorBuild.detectorFaceZ, detectorBuild.detectorX, fSourceDetectorDistance, fCheckOverlaps);
    fSourceVolume = sourcePhys->GetLogicalVolume(); // NOTE: TEMP - REPLACE WITH PHYSICAL VOLUME
    fSourceCoords = sourcePhys->GetObjectTranslation(); // NOTE: TEMP - REPLACE WITH PHYSICAL VOLUME
    // TODO: Maybe even just return an object containing everything primary generator needs (radius, height, origin),
    // then cache that object as a class member, that way primary generator doesnt need to do a bunch of shite
    // to get the values it needs (i.e., static cast to cylinder etc)
    
    ///////////
    // SCORING:
    ///////////

    // Will need to access logical photocathode volume outside of this methods scope (for scoring)
    // NOTE: May want to double check that "DETECTED" flag matches photocathode geometry
    // fScoringVolume = photocathodeLog;
    // NOTE: Assigned to the class property defined in header file
    
    ///////
    // FIN:
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
G4double DetectorConstruction::BuildTable(G4LogicalVolume* worldLog) {
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
    G4Material* wood = fDetectorMaterials->Wood(); // ...........................................................................................
    
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
    
    // return tablePhys;
    
    G4double tableTopY = tableTransY + (0.5 * tableHeight);
    
    return tableTopY;
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
    
    G4RunManager::GetRunManager()->ReinitializeGeometry();
    // G4RunManager::GetRunManager()->GeometryHasBeenModified();
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
    
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

/*
 * ...
 */
void DetectorConstruction::SetSource(G4String isotope) {
    if (isotope == "137Cs") {
        fSource = Isotopes::Cs137;
    }
    else if (isotope == "60Co") {
        fSource = Isotopes::Co60;
    }
    else if (isotope == "22Na") {
        fSource = Isotopes::Na22;
    }
    else if (isotope == "133Ba") {
        fSource = Isotopes::Ba133;
    }
    else if (isotope == "241Am") {
        
    }
    
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}
