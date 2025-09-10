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
    G4Material* Li = nist->FindOrBuildMaterial("G4_Li"); // lithium photocathode
    
    
    /////////////////
    // SCINTILLATION:
    /////////////////
    
    // Instantiate a new material properties table, to be assigned to the scintillator material
    auto MPTCrystal = new G4MaterialPropertiesTable();
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
    MPTCrystal->AddProperty("RINDEX", energy, rindex);
    
    // The energy spectrum of the emitted scintillation photons
    // NOTE: This is essential to generate the correct number of photons (25156 for 662 keV, instead of 5-10)
    std::vector<G4double> emi = {1., 1., 1.}; // same amount of photons for each wavelength
    MPTCrystal->AddProperty("SCINTILLATIONCOMPONENT1", energy, emi); // "Fast component"
    // NOTE: Tells Geant4 how many photons for each wavelength (or energy)
    // The scintillation photons will have a spectrum, depending on wavelength,
    // may have more photons in red spectrum than blue spectrum
    
    // Absorption length is the average distance travelled by a photon before being absorbed by the medium 
    // (i.e. it is the mean free path returned by the GetMeanFreePath method)
    std::vector<G4double> absorption = {30.*cm, 30.*cm, 30.*cm};
    // std::vector<G4double> absorption = {100.*cm, 100.*cm, 100.*cm};
    MPTCrystal->AddProperty("ABSLENGTH", energy, absorption); // NOTE: Trivial in that the process merely kills the particle
    // NOTE: This has effect on air too (WITHOUT SPECIFYING THIS, SIM WILL HANG INDEFINITELY, WHEN AIR RINDEX SPECIFIED)
    
    // Properties independent of energy (const property)
    //
    // The number of scintillation photons generated per unit energy deposited in the medium
    MPTCrystal->AddConstProperty("SCINTILLATIONYIELD", 38000. / MeV); // 38 photons per keV deposited
    // MPTCrystal->AddConstProperty("SCINTILLATIONYIELD", 10. / MeV); // 38 photons per keV deposited
        
    // Rise time (defaults to zero)
    // MPTCrystal->AddConstProperty("SCINTILLATIONRISETIME1", 0. * ns);
    // NOTE: If a non-zero rise time is wanted, set the optical parameter "setFiniteRiseTime" to true
    
    // Exponential decay time constant (some say 230-250) (at room temperature, increasing at lower temps)
    MPTCrystal->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 250. * ns);
    // NOTE: Number of photons emitted follows an exponential function,
    // creation time of photons is chosen from a distribution with these characterisics
    
    // Factor to vary width of yield distribution
    MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 10.); // 1. to start, tune later
    // NOTE: A resolution scale of ZERO produces no fluctuation in optical photons generated
    // (sigma = sqrt of mean photons for step * RESOLUTIONSCALE)
    // NOTE: val > 0. broadens intrinsic Poisson stats (captures non-proportionality-ish behaviour)
    // Res scales of: 0. -> 1. -> 2. produce a nearly identical spectrum
    // 10. produces something actually resembling a gaussian photopeak (rather than an almost exponential peak)
    
    // ...
    // MPTCrystal->AddConstProperty("SCINTILLATIONYIELD1", 1.); // 100% in the single component (NOTE: idk what this is)

    // TODO: Rayleigh Scattering ?
    // MPTCrystal->AddConstProperty("RAYLEIGH", ...)
    
    // Assign the defined material properties to the sodium iodide material
    NaI->SetMaterialPropertiesTable(MPTCrystal);
    
    /*
     * REFRACTIVE INDICES
     * 
     * (Of materials beyond the scintillator)
     * 
     * NOTE: Scintillation photons will be "killed" when attempting to leave the crystal
     * if the medium it is entering has no refractive index defined, 
     * hiding need for reflector material
     * 
     * NOTE: If absorption length is not specified above, giving air a rindex will cause sim to hang indefinitely
     */
    
    // Assign a refractive index to air, using the same energy vector as above
    auto MPTAir = new G4MaterialPropertiesTable();
    std::vector<G4double> rindexAir = {1., 1., 1.}; // MPT2->AddProperty("RINDEX", "Air") NOTE: Default available
    MPTAir->AddProperty("RINDEX", energy, rindexAir);
    air->SetMaterialPropertiesTable(MPTAir);
    
    /*
     * REFLECTOR SURFACE DEFINITIONS:
     * 
     * These are the defaults for the optical surface:
     * G4OpticalSurface("ReflectorSurface", glisur, polished, dielectric_dielectric)
     * 
     * The "GLISUR" and "UNIFIED" models appear to be almost identical in all cases below (tested),
     * leaning towards "UNIFIED" as there is a chart in docs explicitly stating functionality.
     * 
     * NOTE: In cases where "UNIFIED" is specified, but reflection probability constants are not,
     * the default becomes Lambertian reflection.
     * 
     * The "dielectric_dielectric" interface is appropriate for crystal->alumina interface,
     * "dielectric_lut" is also available, however the look up tables are currently limited, and need downloading.
     * 
     * Hence, all following finishes listed will be using:
     * G4OpticalSurface("ReflectorSurface", unified, X, dielectric_dielectric)
     * 
     * With the non-painted finishes:
     * - Polished
     * - Ground
     *
     * Any of the three interaction mechanisms may take place:
     * - Refraction
     * - Reflection
     * - Absorption
     * 
     * NOTE: The mechanism will be decided based on the supplied energy dependent vectors,
     * but refraction is not very desireable for a reflector material as light will escape.
     * 
     * NOTE: Also, the use of a "polished" or "ground" surface here, 
     * without giving the reflector material itself a refractive index,
     * means optical photons will be killed instead of reflected.
     * 
     * With the painted finishes, the only interaction mechanisms will be:
     * - Reflection
     * - Absorption
     * 
     * NOTE: No refraction, suitable for a reflector material, 
     * reflection probability must be set by "REFLECIVITY" property.
     * 
     * Front painted finishes prevent refraction, but still allow for absorption:
     * - PolishedFrontPainted (specular spike reflection)
     * - GroundFrontPainted (lambertian reflection)
     * 
     * NOTE: Both are suitable (as they prevent refraction, incident light will only absorb or reflect),
     * 
     * Eliminating possibility for visible light photons to penetrate alumina reflector seems suitable (painted options).
     * 
     * Back painted finishes prevent refraction, but also prevent absorption:
     * - PolishedBackPainted (specular spike, specular lobe, backscatter, lambertian)
     * - GroundBackPainted (specular spike, specular lobe, backscatter, lambertian)
     * 
     * NOTE: Afaik, the backpainted choices work, but will only reflect, no absorption,
     * which is not as realistic? (i could be mistaken about absorption though)
     * 
     * NOTE: With a "painted" surface finish, it seems the rindex, or an MPT,
     * doesnt need to be assigned to the Al203 material, spectrum remains the same either way.
     */

    // Reflector MPT (definining probability of reflection, or else absorption)
    auto MPTReflectorSurf = new G4MaterialPropertiesTable();
    
    // Unified model, polished front painted so refraction does not occur, dielectric-dielectric interface
    auto reflectorSurface = new G4OpticalSurface("ReflectorSurface", unified, polishedfrontpainted, dielectric_dielectric);
    
    // Reflection probability as a function of wavelength
    std::vector<G4double> reflectivity = {0.9, 0.9, 0.9}; // 1 = all photons will be reflected (reflectance of 0.3% in practice)
    
    // Assign property to MPT, and MPT to surface
    MPTReflectorSurf->AddProperty("REFLECTIVITY", energy, reflectivity); // NOTE: No difference when using this energy or energyAl203
    reflectorSurface->SetMaterialPropertiesTable(MPTReflectorSurf);

    /*
     * DETECTOR SURFACE DEFINITION
     * 
     * Spectrum appears essentially identical with "UNIFIED" instead of default "GLISUR",
     * again leaning toward "UNIFIED" here due to clear documentation.
     * 
     * When the "dielectric_metal" interface is chosen, the only interaction mechanisms become:
     * - Reflection
     * - Absorption
     * 
     * NOTE: No refraction.
     * 
     * If the photon is "absorbed" it can be "detected" according to the photoelectron efficiency of the metal.
     * 
     * Additionally, the only surface finishes for "dielectric_metal" become:
     * - Polished
     * - Ground
     * 
     * The "polished" finish will only undergo specular spike reflection.
     * NOTE: Acts as "PolishedFrontPainted" for "dielectric_dielectric".
     * 
     * The "ground" finish will be undergo one of four reflection mechanisms (decided by assigned probabilities),
     * it does not behave the same as its "dielectric_dielectric" counterpart.
     * NOTE: May need to provide probabilities in this case.
     * 
     * Refractive index may not be needed on either the surface, or the material itself, 
     * as dielectric_metal only reflects/absorbs.
     * 
     * The mandatory properties for a dielectric_metal surface are those pertaining to:
     * - Reflectivity (1 minus the absorption coeffcient)
     * - Efficiency (chance of an absorbed photon being detected)
     * 
     * NOTE: As these are the only interaction mechanisms at this surface.
     * 
     * NOTE: Afaik, since the absorption/reflection interactions will mostly happen at the surface, 
     * the MPT only needs assigning to the optical surface, the material itself does not need an MPT.
     * (it does not work if solely applied to material)
     * (Also, its only applied to the surface in "examples/extended/optical/LXe")
     */
    
    // Scoring MPT (Lithium Photocathode)
    auto MPTPhotocathode = new G4MaterialPropertiesTable();
    
    // Unified model, polished surface finish, dielectric->metal interface
    auto photocathodeSurface = new G4OpticalSurface("Photocathode", unified, polished, dielectric_metal);

    // std::vector<G4double> energyScoring = {1.239841939*eV / 0.700, 1.239841939*eV / 0.551, 1.239841939*eV / 0.400}; // 400 nm - 700 nm (visible range)
    // std::vector<G4double> reflectivityScoring = {0.9, 0.9, 0.9}; // Li apparently 90% reflectivity between 400-700 nm
    std::vector<G4double> reflectivityScoring = {0.05, 0.05, 0.05}; // NOTE: But that massively decreases efficiency ...
    // NOTE: There must be a methodology used to decrease reflectivity in this application
    
    //
    std::vector<G4double> efficiency = {0.25, 0.25, 0.25}; // 25% QE starter (flat efficiency)
    
    // NOTE: Using same energy as other MPTS has no effect on output spectrum at all (vs "energyScoring")
    MPTPhotocathode->AddProperty("REFLECTIVITY", energy, reflectivityScoring); // 1 minus the absorption coeffcient
    MPTPhotocathode->AddProperty("EFFICIENCY", energy, efficiency); // Chance of an absorbed photon to be detected
    
    photocathodeSurface->SetMaterialPropertiesTable(MPTPhotocathode);
    
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
    // is used to surround the crystal (Al_{2}O_{3} and teflon), on all sides except the back,
    // to increase the amount of photons reaching the the photocathode
    
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
    
    // Photocathode (absorbing or detecting incident optical photons)
    auto photocathode = new G4Tubs(
        "Photocathode",
        crystalInnerRad,
        crystalOuterRad,
        reflectorThickness * 0.5, // NOTE: Probably want much thinner photocathode
        startAngle,
        endAngle
    );
    
    // Lithium (alkali metal due to low binding energy of outer shell electron)
    auto photocathodeLog = new G4LogicalVolume(photocathode, Li, "Photocathode");
    
    // Placed on the back side of the crystal
    G4VPhysicalVolume* photocathodePhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(crystalX, crystalY, crystalZ + (0.5 * crystalHeight) + (0.5 * reflectorThickness)), // Translation
        photocathodeLog, // The logical volume
        "Photocathode", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        checkOverlaps
    );
    
    // NOTE: This geometry will be replaced by optical grease (silicon gel) in full model, can leave as current thickness,
    // the optical grease will then be pressed against the PMT window, with the photocathode coating the other side
    
    
    ////////////
    // SURFACES:
    ////////////
    
    // Define the border between the crystal and the reflector
    auto crystalReflectorBorder = new G4LogicalBorderSurface("CrystalToReflector", crystalPhys, reflectorPhys, reflectorSurface);
    
    // Define the border between the crystal and the photocathode
    auto crystalScoringBorder = new G4LogicalBorderSurface("CrystalToPhotocathode", crystalPhys, photocathodePhys, photocathodeSurface);
    
    
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
    
    // Scoring region
    auto photocathodeVisAtt = new G4VisAttributes(G4Color(1., 0., 0., 0.5)); // red
    photocathodeVisAtt->SetForceSolid(true);
    photocathodeLog->SetVisAttributes(photocathodeVisAtt);
    
    
    ///////////
    // SCORING:
    ///////////

    // Will need to access logical scintillator volume outside of this methods scope (for scoring)
    // fDetectorVolume = scintillatorLog;
    // NOTE: Assigned to the class property defined in header file
    
    // NOTE: May want to double check that "DETECTED" flag matches photocathode geometry
    // fScoringVolume = photocathodeLog;


    ///////
    // ...:
    ///////

    // Always return world
    return worldPhys;
}

// }
