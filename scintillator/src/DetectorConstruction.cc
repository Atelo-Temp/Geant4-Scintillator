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

#include "G4NistManager.hh"
#include "G4ThreeVector.hh"

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

#include "G4SubtractionSolid.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include <G4SurfaceProperty.hh>


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
    
    // Scintillation light reflector material (Al2O3)
    G4Material* Al2O3 = nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");
    
    // Scoring material
    // G4Material* SiO2 = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"); // silicon gel
    G4Material* Li = nist->FindOrBuildMaterial("G4_Li"); // lithium photocathode
    
    
    /////////////////
    // SCINTILLATION:
    /////////////////
    
    // Instantiate a new material properties table, to be assigned to the scintillator material
    auto MPT = new G4MaterialPropertiesTable();
    // NOTE: Need at least: refractive index, emission spectrum, absorption length, yield, decay time
    
    // Wavelength range listed for NaI (refractiveindex.info)
    std::vector<G4double> energy = {1.9587*eV, 2.3991*eV, 2.8437*eV}; // Wavelength (~436nm - 633nm)
    // std::vector<G4double> energy = {1.239841939*eV/0.633, 1.239841939*eV/0.436}; // 436 nm - 633 nm (smallest must go first)
    // NOTE: Visible light ranges from ~400 nm (violet) to ~700 nm (red)
    
    // Refractive index (n) - The ratio of speed of light in air/vaccum (c) to SOL in medium (v) (NOTE: n = (c / v))
    std::vector<G4double> rindex = {1.7779, 1.8043, 1.8391}; // A function of wavelength (~436nm - 633nm)
    // NOTE: Added a central curve value to show non-linear trend

    // Properties that depend on energy
    // NOTE: Vector lengths must be the same, 1st vector is energy, 2nd is property value at that energy
    //
    // Refractive index as a function of wavelength
    MPT->AddProperty("RINDEX", energy, rindex);
    
    // The energy spectrum of the emitted scintillation photons
    // NOTE: This is essential to generate the correct number of photons (25156 for 662 keV, instead of 5-10)
    std::vector<G4double> emi = {1., 1., 1.}; // same amount of photons for each wavelength
    MPT->AddProperty("SCINTILLATIONCOMPONENT1", energy, emi); // "Fast component"
    // NOTE: Tells Geant4 how many photons for each wavelength (or energy)
    // The scintillation photons will have a spectrum, depending on wavelength,
    // may have more photons in red spectrum than blue spectrum
    
    // Absorption length is the average distance travelled by a photon before being absorbed by the medium 
    // (i.e. it is the mean free path returned by the GetMeanFreePath method)
    // std::vector<G4double> absorption = {30.*cm, 30.*cm, 30.*cm};
        std::vector<G4double> absorption = {100.*cm, 100.*cm, 100.*cm};
    MPT->AddProperty("ABSLENGTH", energy, absorption); // NOTE: Trivial in that the process merely kills the particle
    // NOTE: This has effect on air too (WITHOUT SPECIFYING THIS, SIM WILL HANG INDEFINITELY, WHEN AIR RINDEX SPECIFIED)
    
    // Properties independent of energy (const property)
    //
    // The number of scintillation photons generated per unit energy deposited in the medium
    MPT->AddConstProperty("SCINTILLATIONYIELD", 38000. / MeV); // 38 photons per keV deposited
    // MPT->AddConstProperty("SCINTILLATIONYIELD", 10. / MeV); // 38 photons per keV deposited
        
    // Rise time (defaults to zero)
    // MPT->AddConstProperty("SCINTILLATIONRISETIME1", 0. * ns);
    // NOTE: If a non-zero rise time is wanted, set the optical parameter "setFiniteRiseTime" to true
    
    // Exponential decay time constant (some say 230-250) (at room temperature, increasing at lower temps)
    MPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 250. * ns);
    // NOTE: Number of photons emitted follows an exponential function,
    // creation time of photons is chosen from a distribution with these characterisics
    
    // Factor to vary width of yield distribution
    MPT->AddConstProperty("RESOLUTIONSCALE", 1.); // 1. to start, tune later
    // NOTE: val > 1. broadens intrinsic Poisson stats (captures non-proportionality-ish behaviour)
    
    // ...
    // MPT->AddConstProperty("SCINTILLATIONYIELD1", 1.); // 100% in the single component (NOTE: idk what this is)

    // TODO: Rayleigh Scattering ?
    // MPT->AddConstProperty("RAYLEIGH", ...)
    
    // Assign the defined material properties to the sodium iodide material
    NaI->SetMaterialPropertiesTable(MPT);
    
    
    // Assign a refractive index to air 
    // NOTE: Scintillation photons will not attempt to leave the crystal otherwise, hiding need for reflector material
    auto MPT2 = new G4MaterialPropertiesTable();
    std::vector<G4double> rindexAir = {1., 1., 1.}; // MPT2->AddProperty("RINDEX", "Air") NOTE: Default available
    MPT2->AddProperty("RINDEX", energy, rindexAir);
    air->SetMaterialPropertiesTable(MPT2);
    // NOTE: If absorption length is not specified above, giving air a rindex will cause sim to hang indefinitely
    
    
    
    
    // Reflector MPT
    auto MPT3 = new G4MaterialPropertiesTable();
    
    // Reflector refractive index
    std::vector<G4double> energyAl203 = {0.24797*eV, 0.36487*eV, 1.02210*eV, 3.70430*eV, 6.19920*eV};
    std::vector<G4double> rindexAl203 = {1.6240, 1.6990, 1.7519, 1.8015, 1.9127};
    MPT3->AddProperty("RINDEX", energyAl203, rindexAl203); 
    // NOTE: Not sure if i can use new energy vector here
    
    // ....
    auto reflectorSurface = new G4OpticalSurface("ReflectorSurface");
    // reflectorSurface->SetType(dielectric_dielectric);
    // reflectorSurface->SetModel(unified);
    // reflectorSurface->SetFinish(polished);
    
    // Reflector reflectivity 
    std::vector<G4double> energyAl203Reflector = {1.239841939*eV * 1.10, 1.239841939*eV / 0.350}; // 1100 nm - 350 nm (smallest must go first)
    std::vector<G4double> reflectivity = {1., 1.}; // all photons will be reflected (reflectance of 0.3% in practice)
    
    // ...
    MPT3->AddProperty("REFLECTIVITY", energyAl203Reflector, reflectivity);
    
    // ...
    reflectorSurface->SetMaterialPropertiesTable(MPT3);
    
    // NOTE: There could be things wrong here, but visually it appears to work
    
    
    
    
    // Scoring MPT (silicon dioxide)
    // auto MPT4 = new G4MaterialPropertiesTable();
    // // ...
    // auto scoringSurface = new G4OpticalSurface("ScoringSurface");
    // std::vector<G4double> energyScoring = {1.239841939*eV / 0.700, 1.239841939*eV / 0.400}; // 400 nm - 700 nm (visible range)
    // std::vector<G4double> rindexScoring = {1.5406, 1.5574};
    // std::vector<G4double> reflectivityScoring = {0.08, 0.08}; // TODO: This is leting less through than i thought
    // // std::vector<G4double> reflectivityScoring = {1., 1.}; // NOTE: This lets all (or most) through, kinda backwards, need to go over docs again
    // MPT4->AddProperty("RINDEX", energyScoring, rindexScoring);
    // MPT4->AddProperty("REFLECTIVITY", energyScoring, reflectivityScoring);
    // SiO2->SetMaterialPropertiesTable(MPT4);
    // scoringSurface->SetMaterialPropertiesTable(MPT4);
    
    
    
    // Scoring MPT (Lithium Photocathode)
    auto MPT4 = new G4MaterialPropertiesTable();
//     auto scoringSurface = new G4OpticalSurface("Photocathode");
//     
//     scoringSurface->SetType(dielectric_metal); // Reflection or absorption (no refraction) -> absorption = detection
//     scoringSurface->SetModel(unified);
//     scoringSurface->SetFinish(polished); // ground or polish (only options for dielectric_metal)
    
    auto photocathodeSurface = new G4OpticalSurface("Photocathode", glisur, polished, dielectric_metal); // NOTE: more convinent really
    
    std::vector<G4double> energyScoring = {1.239841939*eV / 0.700, 1.239841939*eV / 0.551, 1.239841939*eV / 0.400}; // 400 nm - 700 nm (visible range)
    // std::vector<G4double> rindexScoring = {0.16159, 0.14359, 0.26258}; // NOTE: May not be needed as dielectric_metal only reflect/absorb
    // std::vector<G4double> reflectivityScoring = {0.9, 0.9, 0.9}; // Li apparently 90% reflectivity between 400-700 nm
    std::vector<G4double> reflectivityScoring = {0.05, 0.05, 0.05}; // NOTE: But that massively decreases efficiency ...
    // NOTE: There must be a methodology used to decrease reflectivity in this application
    std::vector<G4double> efficiency = {0.25, 0.25, 0.25}; // 25% QE starter (flat efficiency)
    
    // MPT4->AddProperty("RINDEX", energyScoring, rindexScoring); // NOTE: MAY BE ABLE TO REMOVE THIS .. ?? (Just reflectivity & efficiency)
    // NOTE: RINDEX would be assigned to Li itself
    MPT4->AddProperty("REFLECTIVITY", energyScoring, reflectivityScoring); // 1 minus the absorption coeffcient
    MPT4->AddProperty("EFFICIENCY", energyScoring, efficiency); // Chance of an absorbed photon to be detected
    // NOTE: Must specificy efficiency
    
    // Li->SetMaterialPropertiesTable(MPT4); // NOTE: Do i need to set it to both the material (like scintillator) and surface (for logical border) ?
    photocathodeSurface->SetMaterialPropertiesTable(MPT4); // NOTE: I think in this case its just the surface (it does not work if solely applied to the Li)
    // NOTE: Also, its only applied to the surface in "examples/extended/optical/LXe"
    
    // NOTE: Only seeing ~1% of the total optical photons being "DETECTED" on full energy deposition
    // NOTE: Actually ranges from ~1% to 7% seemingly
    
    
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
    G4double crystalHeight = 7.62*cm; // 3 inch height (this arg will be doubled, so will need to * 0.5)
    G4double startAngle = 0.*deg;
    G4double endAngle = 360.*deg; // Full circumference cylinder

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
    
    
    // Define the scintillator crystal material as sodium iodide
    auto scintillatorLog = new G4LogicalVolume(scintillator, NaI, "Scintillator");
    
    // Define coordinates for scintillator crystal (offset from mother origin)
    G4double crystalX = 0.*m;
    G4double crystalY = 0.*m;
    G4double crystalZ = 0.1*m; // 10cm (maybe 3cm as i have a lot of data for that distance)
    
    // Define translation vector (relative to mother origin)
    auto crystalTrans = G4ThreeVector(crystalX, crystalY, crystalZ);

    // Place the sodium iodide scintillator crystal (inside of the world)
    G4VPhysicalVolume* crystalPhys = new G4PVPlacement(
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
    // REFLECTOR:
    /////////////
    
    // The scintillation photons are emitted in all directions, so a high efficiency reflector
    // is used to surround the crystal (Al_{2}O_{3} and teflon)
    
    // Inner rad can (4.04495 cm) - outer rad crystal (3.81 cm) => 0.23495 cm reflector thickness
//     G4double reflectorThickness = 0.23495 * cm;
//     G4double reflectorOuterRad = crystalOuterRad + reflectorThickness;
//     
//     // ...
//     auto reflector = new G4Tubs(
//         "Reflector",
//         crystalOuterRad, // inner rad
//         reflectorOuterRad, // outer rad
//         crystalHeight * 0.5, // height
//         startAngle, // 0 deg
//         endAngle // 360 deg (full span)
//     );
//     
//     // ...
//     auto reflectorLog = new G4LogicalVolume(
//         reflector,
//         Al2O3,
//         "Reflector"
//     );
//     
//     // ...
//     G4VPhysicalVolume* reflectorPhys = new G4PVPlacement(
//         nullptr, // no rotation
//         crystalTrans, // same position as crystal
//         reflectorLog, // logical volume
//         "Reflector", // name
//         worldLog, // mother volume (logical)
//         false, // no boolean ops
//         0, // one copy
//         checkOverlaps
//     );
//     
//     
//     // Could also use subtraction solid, but i think it ends up similar amount of code ...
//     
//     // ...
//     auto reflectorFace = new G4Tubs(
//         "ReflectorFace",
//         0. * cm, // inner rad
//         reflectorOuterRad, // outer rad
//         reflectorThickness * 0.5, // height
//         startAngle, // 0 deg
//         endAngle // 360 deg (full span)
//     );
//     
//     // ...
//     auto reflectorFaceLog = new G4LogicalVolume(
//         reflectorFace,
//         Al2O3,
//         "ReflectorFace"
//     );
//     
//     // ...
//     G4VPhysicalVolume* reflectorFacePhys = new G4PVPlacement(
//         nullptr, // no rotation
//         G4ThreeVector(crystalX, crystalY, crystalZ - ((crystalHeight * 0.5) + (reflectorThickness * 0.5))), // same position as crystal
//         reflectorFaceLog, // logical volume
//         "ReflectorFace", // name
//         worldLog, // mother volume (logical)
//         false, // no boolean ops
//         0, // one copy
//         checkOverlaps
//     );
    
    
    // Inner rad can (4.04495 cm) - outer rad crystal (3.81 cm) => 0.23495 cm reflector thickness
    G4double reflectorThickness = 0.23495 * cm;
    G4double reflectorOuterRad = crystalOuterRad + reflectorThickness;
    
    // Base volume which will be cut
    auto reflectorSolid = new G4Tubs(
        "ReflectorSolid",
        0. * cm, // inner rad (no hole, as cut will handle it in this case)
        reflectorOuterRad, // outer rad
        (crystalHeight * 0.5) + reflectorThickness, // height 
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // The section to cut from the base volume
    auto reflectorCut = new G4Tubs(
        "ReflectorCut",
        0. * cm, // inner rad (no hole, solid cut)
        crystalOuterRad, // outer rad (cut a section with same rad as crystal)
        (crystalHeight * 0.5) + reflectorThickness, // height (cut has same height base)
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
    
    // Assign a material to the reflector solid
    auto reflectorLog = new G4LogicalVolume(
        reflector, // subtraction solid acts same as any other geometry here
        Al2O3, // reflector material
        "Reflector"
    );
    
    // Place the reflector
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
    
    // NOTE: This current setup works, but leaves an overhang of reflector,
    // this isnt really an issue and maybe worth keeping,
    // but may also want reflector to stop in line with back of crystal,
    // just need to give "ReflectorSolid" (crystalheight + 0.5 * reflectorThick)
    
    
    ///////////
    // SCORING:
    ///////////
    
    // Silicon gel
//     // ...
//     auto scoring = new G4Tubs(
//         "Scoring",
//         crystalInnerRad,
//         crystalOuterRad,
//         reflectorThickness * 0.5,
//         startAngle,
//         endAngle
//     );
//     
//     
//     // ...
//     auto scoringLog = new G4LogicalVolume(scoring, SiO2, "Scoring");
//     
//     // ...
//     G4VPhysicalVolume* scoringPhys = new G4PVPlacement(
//         nullptr, // No rotation
//         G4ThreeVector(crystalX, crystalY, crystalZ + (0.5 * crystalHeight) + (0.5 * reflectorThickness)), // Translation
//         scoringLog, // The logical volume
//         "Scoring", // Name
//         worldLog, // Mother volume (logical)
//         false, // No boolean ops
//         0, // Copy number
//         checkOverlaps
//     );
    
    // ...
    auto scoring = new G4Tubs(
        "Scoring",
        crystalInnerRad,
        crystalOuterRad,
        reflectorThickness * 0.5, // NOTE: Probably want much thinner photocathode
        startAngle,
        endAngle
    );
    
    
    // ...
    auto scoringLog = new G4LogicalVolume(scoring, Li, "Scoring");
    
    // ...
    G4VPhysicalVolume* scoringPhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(crystalX, crystalY, crystalZ + (0.5 * crystalHeight) + (0.5 * reflectorThickness)), // Translation
        scoringLog, // The logical volume
        "Scoring", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        checkOverlaps
    );
    
    
    ////////////
    // SURFACES:
    ////////////
    
    // ...
    auto crystalReflectorBorder = new G4LogicalBorderSurface("CrystalToReflector", crystalPhys, reflectorPhys, reflectorSurface);
    
    auto crystalScoringBorder = new G4LogicalBorderSurface("CrystalToPhotocathode", crystalPhys, scoringPhys, photocathodeSurface);
    
    
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
    
    // Aluminium oxide reflector
    auto reflectorVisAtt = new G4VisAttributes(G4Color(0., 0., 1.0, 0.5)); // blue
    reflectorVisAtt->SetForceSolid(true);
    reflectorLog->SetVisAttributes(reflectorVisAtt);
    
    // reflectorFaceLog->SetVisAttributes(reflectorVisAtt);
    
    // Scoring region
    auto scoringVisAtt = new G4VisAttributes(G4Color(1., 0., 0., 0.5)); // red
    scoringVisAtt->SetForceSolid(true);
    scoringLog->SetVisAttributes(scoringVisAtt);
    
    
    ///////////
    // SCORING:
    ///////////

    // Will need to access logical scintillator volume outside of this methods scope (for scoring)
    // fScoringVolume = scintillatorLog;
    // NOTE: Assigned to the class property defined in header file
    
    // fScoringVolume = scoringLog;


    ///////
    // ...:
    ///////

    // Always return world
    return worldPhys;
}

// }
