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
#include "MaterialDefinitions.hh"

#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Color.hh"

#include "G4MaterialPropertiesTable.hh" // for optical photons
// #include "G4MaterialPropertyVector.hh" // can use instead of 2x std::vector

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4SurfaceProperty.hh"

#include "G4Types.hh"


// NOTE: Uses consistent units throughout (cm probably easiest to adhere to)

// TODO: Add tiny bit of Tl doping to the crystal material ?

// TODO: 

// TODO: There is overhang of the encapsulation at the back of the crystal 
// (of thickness = encapsulation thickness, as that was added to front for beta shield)

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
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR"); // Carbon, nitrogen, oxygen, argon

    // Scintillator material
    // G4Material* NaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE"); // (1 part Na, 1 part I), density = 3.667 g/cm^3
    auto Na = new G4Element("Sodium", "Na", 11, 22.990 * g/mole);
    auto I = new G4Element("Iodine", "I", 53, 126.90 * g/mole);
    auto Tl = new G4Element("Thallium", "Tl", 81, 204.38 * g/mole);
    auto NaI = new G4Material("NaI:Tl", 3.667 * g/cm3, 3);
    NaI->AddElement(Na, 15.3035 * perCent);
    NaI->AddElement(I, 84.5603 * perCent);
    // NaI->AddElement(Na, 0.1362 * perCent); // TODO: I have been running tests with Na instead of Tl
    NaI->AddElement(Tl, 0.1362 * perCent);
    // TODO: NaI:Tl blend
    
    // Scintillation light reflector material (Alumina - Al2O3)
    G4Material* Al2O3 = nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE"); // (2 part Al, 3 part O), density = 3.97 g/cm^3
    
    // Scintillator can material (Aluminium)
    G4Material* Al = nist->FindOrBuildMaterial("G4_Al"); // density = 2.699 g/cm^3
    
    // Optical grease (Silicone gel) - NOTE: Synthetic "silicone" (polymer), not natural "Silicon"
    auto H = new G4Element("Hydrogen", "H", 1, 1.0078 * g/mole); // Z=1, A=1, density = 8.3748E-5 g/cm^3
    auto C = new G4Element("Carbon", "C", 6, 12.011 * g/mole); //  Z=6, A=12, density = 2 g/cm^3
    auto O = new G4Element("Oxygen", "O", 8, 15.999 * g/mole); // Z=8, A=16, density = 0.00133151 g/cm^3
    auto Si = new G4Element("Silicon", "Si", 14, 28.086 * g/mole); // Z=14, A=28, density = 2.33 g/cm^3
    auto PDMS = new G4Material("PDMS", 0.97 * g/cm3, 3);
    PDMS->AddElement(Si, 1); // NOTE: MUST BE FRACTIONAL IF MATERIAL, PASS ELEMENT IF USING ATOMS
    PDMS->AddElement(C, 2);
    PDMS->AddElement(H, 6);
    // PDMS->AddElement(O, 1) // NOTE: Oxygen is missing (although a chain of 3 would only have 2 oxygen atoms)
    // NOTE: Using polydimethylsiloxane (PDMS) as a proxy for the gel, which has repeating monomer units.
    // Monomer unit: [Si(CH3)2-O]
    // Formula for PDMS: CH3 [Si(CH3)2-O]n Si(CH3)3 (NOTE: Where n is the number of repeating monomer units)
    // TODO: Maybe do PDMS as chain of 3: [(Si, 3) (C, 6), (H, 18), (O, 2)]
    // Or could do as percentages
    // TODO: JUST ADD 1 O TO BLEND, AVERAGES OUT OVER LONG CHAIN LENGTH (100-1000+)
    
    // Borosilicate glass (Pyrex is in mat lib, but maybe soda lime, not borosilicate as pyrex started out in 1915)
    // NOTE: High optical clarity in visible range (400-700 nm) (... and beyond 300-2500 nm), and resistant to breaking due to changes in temperature
    // NOTE: Ideally should match emission spectrum of NaI(Tl) (~300-550 nm or 320-550 nm ... some say ~340-520 nm)
    G4Material* SiO2 = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"); // Silica - (1 part Si, 2 part O), density = 2.32 g/cm^3
    G4Material* B2O3 = nist->FindOrBuildMaterial("G4_BORON_OXIDE"); // Boron Trioxide - (2 part B, 3 part O), density = 1.812 g/cm^3
    // NOTE: Not sure why theyve named "boron trioxide" as blanket boron oxide term
    //
    // Alkali Oxides
    G4Material* Na2O = nist->FindOrBuildMaterial("G4_SODIUM_MONOXIDE"); // Sodium Monoxide (Soda) - (2 part Na, 1 part O), density = 2.27 g/cm^3
    // G4Material* K2O = nist->FindOrBuildMaterial("G4_POTASSIUM_OXIDE"); // Potasium Monoxide - (2 part K, 1 part O), density = 2.32 g/cm^3
    // NOTE: 
    //
    auto borosilicate = new G4Material("BOROSILICATE_GLASS", 2.23 * g/cm3, 4); // 80.6% SiO2, , density = 2.23 g/cm^3 (NOTE: Some sources say 2.51 density)
    borosilicate->AddMaterial(SiO2, 80.6 * perCent); // Borosilicate ~81%, Soda lime is ~69% (Forms the basic structure of the glass)
    borosilicate->AddMaterial(B2O3, 13. * perCent); // Stated as 5-13% (Lowers melting temp, enhances chemical durability and thermal shock resistance)
    // 6.4% remaining
    borosilicate->AddMaterial(Al2O3, 2.4 * perCent); // TODO: 2-3% (Enhances chemical durability and mechanical strength)
    // 4% remaining
    borosilicate->AddMaterial(Na2O, 4. * perCent); // TODO: ~4% alkali oxides (Na2O, K20 - one or the other, or both)
    // borosilicate->AddMaterial(K2O, 0. * perCent); // TODO ^^
    // NOTE: Potassium may cause unwanted noise, not all PMT designs use this in their borosilicate (while some do intentionally)
    // NOTE: SCHOTT BK7, SCHOTT 8250, SCHOTT 8337
    
    // Photocathode material (alkali metal)
    G4Material* Li = nist->FindOrBuildMaterial("G4_Li"); // Lithium
    // TODO: Bialkali ? Although the material kinda doesnt matter in this sim, moreso the surface
    
    // Table material (MDF wood proxy, using generalised wood chemical composition)
    auto wood = new G4Material("WOOD", 0.8 * g/cm3, 6); // NOTE: Solid cellular material of wood ~1.5 g/cm3, but lowered by air spaces
    auto N = new G4Element("Nitrogen", "N", 7, 14.007 * g/mole); // Z=7, A=14, density = ...
    auto Ca = new G4Element("Calcium", "Ca", 20, 40.078 * g/mole); // Z=20, A=40, density = ...
    auto K = new G4Element("Potassium", "K", 19, 39.098 * g/mole); // Z=19, A=39, density = ...
    wood->AddElement(C, 50. * perCent);
    wood->AddElement(O, 42. * perCent);
    wood->AddElement(H, 6. * perCent);
    wood->AddElement(N, 1. * perCent);
    wood->AddElement(Ca, 0.5 * perCent); // NOTE: Simplified by making last 1% Ca/K
    wood->AddElement(K, 0.5 * perCent); // in reality, would also be sodium, magnesium, iron, sulfur, chlorine, silicon, phosphorus
    // NOTE: Composition from wikipedia, cellular density from britannica, MDF density from dover chemical
    
    
    ////////////////////
    // SOURCE MATERIALS:
    ////////////////////

//     // Source material (define Cesium-18 isotope)
//     auto sourceIsotope = new G4Isotope(
//         "137Cs", // name
//         55, // num protons (Z)
//         137, // Atomic mass (num nucleons) (A),
//         136.907089 * g / mole // Molar mass (grams per molecule) (~18g per mol)
//     );
//     // NOTE: 1 mol contains avogadros number of particles (6.022 x 10^23)
//     
//     // Define an element from the isotope
//     auto sourceElement = new G4Element("Cesium-137", "137Cs", 1); // name, symbol, num isotopes
//     
//     // Assign the defined isotope to the element
//     sourceElement->AddIsotope(sourceIsotope, 100.0 * perCent); // isotope, no other isotopes so 100%
//     
//     // Because isotope and element have no direct interaction in G4, need to create a material to assign to logical volume
//     auto sourceMat = new G4Material("137Cs", 1.886 * g / cm3, 1); // name, density (g/cm^3), phase (solid, liquid, gas)
//     // NOTE: Density is estimate, in reality it wont usually be a pure 137Cs source,
//     // usually embedded in a matrix, encapsulated (stainless steel), or a mixed compound (cesium chloride),
//     // i.e. cesium oxide ceramic matrix (more like 1.47 g/cm3)
//     
//     // Assign the element to the G4 material
//     sourceMat->AddElement(sourceElement, 100.0 * perCent); // element, amount of element in material (100%)
//     // TODO: In practice there would be non-zero amount of the daughter isotope too,
//     // based on how old the source was (after 30y half of a "new" 137Cs source would be 137Ba)
    
    // Cesium-137 (137Cs) source, 50% barium (137Ba) daughter product
    auto sourceHandler = new MaterialDefinitions();
    auto sourceMat = sourceHandler->Create137Cs();
    
    // Source Casing
    G4Material* PVC = nist->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"); // density = 1.3 g/cm^3
    // NOTE: 2 part carbon (C), 3 part hydrogen (H), 1 part chlorine (Cl)
    
    // TODO: Should "MaterialDefinitions -- i.e. sourceHandler" be deleted...
    
    
    /////////////////
    // SCINTILLATION:
    /////////////////
    
    // Instantiate a new material properties table, to be assigned to the scintillator material
    auto MPTCrystal = new G4MaterialPropertiesTable();
    // NOTE: Need at least: refractive index, emission spectrum, absorption length, yield, decay time
    
    // Energy range of NaI:Tl emission spectrum
    // NOTE: Visible light ranges from ~400 nm (violet) to ~700 nm (red)
    // TODO: See EMI notes
    std::vector<G4double> energy = {2.25425 * eV, 2.98756 * eV, 3.81488 * eV}; // (550 nm, 415 nm, 325 nm) // TODO: Add an intermediary wavelength between 550->415, and 415->325
    // NOTE: (green, violet, long wavelength ultraviolet) (hence most of the spectrum in blue-violet range)
    //
    // One says:
    // 325-550nm, max @ 415 nm // TODO: STATE REF
    //
    // Another says:
    // 325-525nm, max @ 410 nm // TODO: STATE REF
    // 
    // Another says:
    // 340-520nm, with max @ 410nm // TODO: STATE REF
    //
    // NOTE: 3 sources say max @ 415 nm
    
    // Refractive index (n) - The ratio of speed of light in air/vaccum (c) to SOL in medium (v) (NOTE: n = (c / v))
    // std::vector<G4double> rindex = {1.7779, 1.8043, 1.8391}; // A function of wavelength (~436nm - 633nm)
    // NOTE: Added a central curve value to show non-linear trend
    std::vector<G4double> rindexNaI = {1.85, 1.85, 1.85}; // 1.85 @ emission max (415 nm)

    // Properties that depend on energy
    // NOTE: Vector lengths must be the same, 1st vector is energy, 2nd is property value at that energy
    //
    // Refractive index as a function of wavelength
    MPTCrystal->AddProperty("RINDEX", energy, rindexNaI);
    
    // The energy spectrum of the emitted scintillation photons
    // NOTE: This is essential to generate the correct number of photons (25156 for 662 keV, instead of 5-10)
    // std::vector<G4double> emission = {1., 1., 1.}; // same amount of photons for each wavelength
    // std::vector<G4double> emission = {0.1, 1., 0.1}; // emission max @ 415 nm (NOTE: No different to 0. upper/lower)
    std::vector<G4double> emission = {0., 1., 0.}; // emission max @ 415 nm (NOTE: No different to 0.1 upper/lower)
    MPTCrystal->AddProperty("SCINTILLATIONCOMPONENT1", energy, emission); // "Fast component"
    // NOTE: Tells Geant4 how many photons for each wavelength (or energy)
    // The scintillation photons will have a spectrum, depending on wavelength,
    // may have more photons in red spectrum than blue spectrum
    // TODO: NaI(Tl) actually emits light in ~340-520nm wavelength region, peaking at ~410, 415, or 420nm (gaussian shape)
    // mostly blue-violet light
    
    // Absorption length is the average distance travelled by a photon before being absorbed by the medium 
    // (i.e. it is the mean free path returned by the GetMeanFreePath method)
    // std::vector<G4double> absorptionLengthNaI = {30.*cm, 30.*cm, 30.*cm};
    // std::vector<G4double> absorptionLengthNaI = {50.*cm, 50.*cm, 50.*cm}; // increased collection at the photocathode
    // std::vector<G4double> absorptionLengthNaI = {56.234 * cm, 31.623 * cm, 0.794 * cm}; // Brown (2021) (corrigendum) & Miller et al (2024)
    std::vector<G4double> absorptionLengthNaI = {56.234 * cm, 100 * cm, 0.794 * cm}; // TEST: Establish mean distance travelled before detection at PC
    // ..
    MPTCrystal->AddProperty("ABSLENGTH", energy, absorptionLengthNaI); // NOTE: Trivial in that the process merely kills the particle
    // NOTE: This has effect on air too (WITHOUT SPECIFYING THIS, SIM WILL HANG INDEFINITELY, WHEN AIR RINDEX SPECIFIED)
    // NOTE: At the ultraviolet threshold, the self-absorption edge is hit, where the
    // NaI:Tl crystal strongly absorbs its own light, dropping the transmission distance
    // by orders of magnitude.
    
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
    
    // TODO: Maybe add slow component
    // MPTCrystal->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 1500. * ns);
    // 96% Fast (may need to set ratios for constants 1 & 2)
    // 4% Slow
    // Brown (2021)
    
    // Factor to vary width of yield distribution 
    // NOTE: (sigma^2 = RESOLUTIONSCALE * mean)
    // hence a resolution scale of 1 implies perfect poisson statistics
    // i.e. if res scale = 1: sigma = sqrt(mean), where mean = 38000./MeV
    // but if res scale = 0: optical photon yield will always be exactly 38000./MeV
    // 
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 0.); // no fluctuation
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 1.); // 1. to start, tune later
    //
    // TEST: Res scale 3.5 = 112.69 FWHM, needs to be 62.25 FWHM to match lab, so seeing if can calc res scale via: 112.69/62.25 = 1.81
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 1.8);
    // NOTE: ^^^^ No, not direct conversion at all, 1.8 = ~105.35 FWHM (so ~51% reduction of res scale = 6.5% reduction of FWHM)
    //
    // TODO: RUN SIM WITH RES SCALE = 1
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 1); // NOTE: True val probably 1 +/- some %
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 0.5);
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 0);
    // NOTE: Zero might be excessive, and not very realistic as it implies no variance
    // TODO: To match lab spectra, might have to choose a value between 0 and 1 (but non-zero, and likely closer to 1)
    // ^^ if its still too broad, potentially reduce gaussian smearing in post-processing
    // ^^ in post-processing, reducing from:
    // sigma = sqrt(n)
    // to:
    // sigma = 0.5 * sqrt(n)
    // reduces FWHM even more, from ~105.35 FWHM (at 1.8 res scale), to 91.16 (still at 1.8 res scale)
    //
    MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 3.5); // NOTE: Miller et al (2024)
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 10.); // more gaussian
    // NOTE: A resolution scale of ZERO produces no fluctuation in optical photons generated
    // (sigma = sqrt of mean photons for step * RESOLUTIONSCALE)
    // NOTE: val > 0. broadens intrinsic Poisson stats (captures non-proportionality-ish behaviour)
    // Res scales of: 0. -> 1. -> 2. produce a nearly identical spectrum
    // 10. produces something actually resembling a gaussian photopeak (rather than an almost exponential peak)
    
    // ...
    // MPTCrystal->AddConstProperty("SCINTILLATIONYIELD1", 1.); // 100% in the single component (NOTE: idk what this is)

    // TODO: Rayleigh Scattering ? Mie scattering ?
    // MPTCrystal->AddConstProperty("RAYLEIGH", ...)
    // NOTE: No energy is lost, but might have an impact on light collection?
    
    // Assign the defined material properties to the sodium iodide material
    NaI->SetMaterialPropertiesTable(MPTCrystal);
    
    /*
     * >>> REFLECTOR SURFACE & MATERIAL DEFINITIONS:
     * 
     * These are the defaults for the optical surface:
     * G4OpticalSurface("ReflectorSurface", glisur, polished, dielectric_dielectric)
     * 
     * The "GLISUR" and "UNIFIED" models appear to be almost identical in all cases below (tested),
     * leaning towards "UNIFIED" as there is a chart in docs explicitly stating functionality.
     * 
     * The "dielectric_dielectric" interface is appropriate for crystal->alumina interface,
     * "dielectric_lut" is also available, however the look up tables are currently limited,
     * and need downloading.
     * 
     * Hence, all following finishes listed will be using:
     * G4OpticalSurface("name", unified, X, dielectric_dielectric || dielectric_metal)
     * 
     * 
     * >>> SURFACE FINISHES:
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
     * NOTE: Also, the use of a "polished" or "ground" surface here, without giving the
     * reflector material itself a refractive index, means optical photons will be killed
     * instead of reflected.
     * 
     * With any of the painted finishes, the only interaction mechanisms will be:
     * - Reflection
     * - Absorption
     * 
     * NOTE: No refraction, suitable for a reflector material, reflection probability must 
     * be set by "REFLECTIVITY" property.
     * 
     * NOTE: I have tested each of the 6 surface finishes. No paint allows refraction,
     * painted does not.
     * 
     * Front painted finishes:
     * - PolishedFrontPainted (specular spike reflection)
     * - GroundFrontPainted (lambertian reflection - diffuse)
     * 
     * NOTE: These are for paints that are physically bound to the crystal, i.e., white paint ??
     * 
     * Back painted finishes:
     * - PolishedBackPainted (specular spike, specular lobe, backscatter, lambertian)
     * - GroundBackPainted (specular spike, specular lobe, backscatter, lambertian)
     * 
     * NOTE: These are for reflective surfaces that have an air gap (i.e. teflon, dry 
     * packed powder reflectors), or a binding medium (i.e. optical epoxy to bind powder
     * reflectors to the crystal)
     * 
     * 
     * NOTE: With a "painted" surface finish, it seems the rindex, or an MPT,
     * doesnt need to be assigned to the Al203 material, spectrum remains the same either way.
     * 
     * 
     * 
     * >>> REFLECTIVITY / TRANSMITTANCE:
     * 
     * Reflectivity is defined as:
     * 
     * REFLECTIVITY = (1 - ABSORPTION);
     * 
     * So:
     * 
     * ABSORPTION = 1 - REFLECTIVITY;
     * 
     * REFLECTIVITY = 1; (by default)
     * ABSORPTION = 0; (by default)
     * TRANSMITTANCE = 0; (by default)
     * 
     * Geant4 rolls a random number between 0.0 and 1.0.
     * 
     * - 1) If the roll is GREATER than (REFLECTIVITY + TRANSMITTANCE), the photon dies immediately
     * (it is absorbed at the surface)
     * 
     * i.e. (REFLECTIVITY = 1, TRANSMITTANCE = 0): No photons killed
     * i.e. (REFLECTIVITY = 0.96, TRANSMITTANCE = 0): 4% chance of photon being killed at boundary
     * 
     * - 2) If the roll is LESS than REFLECTIVITY, the photon survives and goes on to the physics 
     * calculations
     * 
     * i.e. (REFLECTIVITY = 1, TRANSMITTANCE = 0): All photons undergo physics calculations
     * i.e. (REFLECTIVITY = 0.96, TRANSMITTANCE = 0): 96% chance to go to calcs
     * 
     * - 3) If the roll is between REFLECTIVITY and (REFLECTIVITY + TRANSMITTANCE), the photon
     * undergoes forced transmission (passing straight through to the next volume, completely
     * ignoring Fresnel reflections/refraction)
     * 
     * i.e. if TRANSMITTANCE is non zero AND if REFLECTIVITY < 1:
     * (REFLECTIVITY = 0.96, TRANSMITTANCE = 0.04)
     * ^ REFLECTIVITY + TRANSMITTANCE = 1, so any roll between 0.96 and 1 passes this check
     * So if roll = 0.97, the photon ignores physics and is transmitted to next volume
     * 
     * 
     * Hence, for a dielectric_dielectric surface, setting reflectivity to 0 means most 
     * photons will be killed on contact with the surface, whereas reflectivity of 1 means
     * almost all photons will undergo physics calculations.
     * 
     * NOTE: Shouldnt all photons be killed on contact with surface w/ 0?
     * ^ yet when it is zero, im still getting photons lost (when no rindex set for aluminium)
     * 
     * 
     * >>> SURFACE REFLECTION CONSTANTS:
     * 
     * Defaults to:
     * - specular spike constant = 0
     * - specular lobe constant = 0
     * - backscatter constant = 0
     * - diffuse lobe constant (lambertian) = 1
     * 
     * If any of the other constants are set to non-0 values:
     * - diffuse lobe constant (lambertian) = (1 - sum of other constants)
     * 
     * - Specular lobe constant:
     * Represents the reflection probability about the normal of a micro facet (local normal).
     * 
     * - Specular spike constant:
     * The probability of reflection about the average surface normal.
     * 
     * - Diffuse lobe constant:
     * The probability of internal Lambertian reflection.
     * 
     * - Back-scatter spike constant:
     * The probability of several reflections within a deep groove with the end result
     * being exact back-scattering.
     * 
     * NOTE: Powder reflections scatter deep within grain structures, destroying any 
     * directional memory relative to the macro-surface, hence "specular lobe constant"
     * should be kept near or at zero.
     * 
     * NOTE: Powders are composed of millions of microscopic crystalline grains; they
     * cannot act as mirror surfaces. Hence, "specular spike constant" should also be 
     * kept near or at zero.
     * 
     * 
     * >>> SIGMA ALPHA:
     * 
     * SigmaAlpha is used specify surface roughness of the exit medium.
     * 
     * NOTE: This is only applicable to:
     * - Ground
     * - Polished Back Painted
     * - Ground Back Painted
     * 
     * The facet normal is chosen from a gaussian distribution with this sigma.
     * 
     * NOTE: Unit is radians.
     * 
     * NOTE: Estimating this is very difficult, so may be best omitted
     * 
     * For back painted options:
     * 
     * If defining a dry packed reflector such as Al2O3, which has a tiny air gap between 
     * the crystal and the reflector, sigma alpha refers to the crystal->air interface,
     * or the outside of the crystal. Not the roughness of the reflector surface itself.
     * 
     * 
     * >>> REFRACTIVE INDICES:
     * 
     * RINDEX must be specified both for the surface, and the secondary medium, for the
     * back painted finishes. I.e., set rindex of air to the surface, and rindex of Al2O3 
     * to the material itself.
     * 
     * (Of materials beyond the scintillator)
     * 
     * NOTE: Scintillation photons will be "killed" when attempting to leave the crystal
     * if the medium it is entering has no refractive index defined, hiding the need for 
     * a reflector material
     * 
     * 
     * >>> ABSORPTION LENGTH:
     * 
     * When giving surrounding media a rindex (beyond the crystal), if absorption 
     * length is not specified, it defaults to a near infinite value, 
     * 
     * NOTE: In a geometry of only: crystal->air this will cause sim to hang almost 
     * indefinitely
     * 
     * 
     * >>> MATERIAL CHOICE
     * 
     * Rather than just having a polished aluminium reflector, reflectors such as alumina 
     * powders are often used.
     * 
     * This is because while aluminium can be highly reflective, it has an extremely short
     * attenuation length for visible light (10-20 nm). Whereas alumina powders have an 
     * extremely low bulk absorption coefficient. This allows the optical photons (not 
     * reflected at or very close to the surface) to undergo extensive scattering in the 
     * reflector medium (in the granular matrix)
     * 
     * While teflon may provide greater reflectivity than alumina in the visible and 
     * near-UV spectrums, prolonged exposure to high-energy UV or radiation, may cause it 
     * to slightly discolor or degrade over time. Is is also very soft (low hardness), so 
     * can be prone to scratching.
     * 
     * Whereas alumina powder will not degrade or yellow under intense UV light or high 
     * heat, and it is also extremely hard (scratch resistant) (Mohs hardness of 9.0).
     */
    
    ///////////////////////////////
    // REFLECTOR SURFACE PROPERTIES
    ///////////////////////////////

    // Reflector MPT (definining probability of reflection, or else absorption)
    auto MPTReflectorSurf = new G4MaterialPropertiesTable();
    
    // NOTE: The polished and ground finishes allow for refraction
    // NOTE: Front painted do not allow refraction to occur
    // NOTE: These are for paints that are physically bound to the crystal
    
    // NOTE: Back painted do not allow refraction to occur
    // UNIFIED model, ground back painted (same as ground but with a back-paint), dielectric-dielectric interface (crystal->reflector)
    auto reflectorSurface = new G4OpticalSurface("ReflectorSurface", unified, groundbackpainted, dielectric_dielectric);
    // NOTE: Back painted implies the presence of a tiny gap between the two media, i.e.
    // an air gap for dry packed powders or teflon, or 
    // NOTE: Lambertian reflector attached to scintillator crystal surface
    
    // NOTE: Reflectors are typically diffuse (ground), as it promotes greater light collection
    
    // Specify surface roughness (For back painted surface)
    reflectorSurface->SetSigmaAlpha(0); // No specular lobe constant, so ...
    // reflectorSurface->SetSigmaAlpha(0.023); // Mechanically polished - Janecek et al (2010)
    // reflectorSurface->SetSigmaAlpha(0.1); // Almost polished (specular)
    // reflectorSurface->SetSigmaAlpha(0.2); // Ground - Janecek et al (2010) (~12 deg)
    // reflectorSurface->SetSigmaAlpha(0.25); // Ground polished (partially diffuse)
    // reflectorSurface->SetSigmaAlpha(0.35);
    // reflectorSurface->SetSigmaAlpha(0.4);
    // reflectorSurface->SetSigmaAlpha(0.5); // Very Matte / Rough powder (strongly diffuse)
    // reflectorSurface->SetSigmaAlpha(0.75); // Rough powder (strongly diffuse)
    // reflectorSurface->SetSigmaAlpha(1); // Strongly diffuse
    
    // TODO: IM PRETTY SURE SIGMA ALPHA IS SURFACE ROUGHNESS OF CRYSTAL, NOT REFLECTOR
    // ^ at Crystal->air gap interface, it probably is fairly rough
    // WOULD REFLECTION CONSTANTS ALSO BE THE SAME (crystal->air gap interface)
    
    // Reflection probability as a function of wavelength 
    // NOTE: Default value: 1 = all photons will undergo reflection/refraction calculation
    // std::vector<G4double> reflectivityReflector = {0.9, 0.9, 0.9}; // Rough est val
    // std::vector<G4double> reflectivityReflector = {0.94, 0.94, 0.94}; // 96% alumina -> 94% reflectance
    // std::vector<G4double> reflectivityReflector = {0.96, 0.96, 0.96}; // 99.7% alumina -> 96% reflectance @500-2000nm
    std::vector<G4double> reflectivityReflector = {0.98, 0.98, 0.98};
    // std::vector<G4double> reflectivityReflector = {0.99, 0.99, 0.99};
    // std::vector<G4double> reflectivityReflector = {1., 1., 1.};
    // std::vector<G4double> reflectivityReflector = {0.1, 0.1, 0.1};
    // std::vector<G4double> reflectivityReflector = {0., 0., 0.};
    // NOTE: Need to specify reflectivity != 1. else no absorption in reflector (perfect reflector, not realistic)
    // TEST: There is actually very little difference when removing reflectivity from reflector surface
    // i.e. when it defaults to 1, versus 0.96
    
    // Refractive index (back painted)
    // std::vector<G4double> rindexReflectorSurface = {1.78, 1.78, 1.78}; // Al2O3
    std::vector<G4double> rindexReflectorSurface = {1., 1., 1.}; // Air (dry packed causes layer of air)
    // std::vector<G4double> rindexReflectorSurface = {1.46, 1.46, 1.46}; // Silicone optical grease
    // NOTE: Levin 1996 (UNIFIED) says r = 1 when tape like coating (i.e. for powders) due to air gap,
    // and r = optical expoxy (i.e. silicone gel) for Al/Ti/Mg oxide powders mixed with epoxy
    
    // Reflection constants (back painted)
    // std::vector<G4double> slcReflector = {0.05, 0.05, 0.05}; // Specular lobe constant
    // std::vector<G4double> slcReflector = {1., 1., 1.};
    // std::vector<G4double> bsReflector = {0.01, 0.01, 0.01}; // Backscatter constant
    // NOTE: Defaults to specular spike = 0, specular lobe = 0, backscatter = 0, diffuse lobe (lambertian) = 1
    // if other values are set to non-0, diffuse lobe constant (lambertian) will be 1 - sum of other components
    
    // TODO: I THINK IT MAY BE BEST TO GO FOR HIGH SPECULAR LOBE ? IF IT IS FOR Crystal->Air gap INTERFACE ???
    // NO, THIS IS FOR polished CRYSTAL->PMT WINDOW INTERFACE (sigma alpha = 0, SL = 1)
    
    // NOTE: JUST LEAVE IT AT: LAMBERTIAN = 1, see notes in jsdoc comment block above
    
    // TEST: EXPLICIT LAMBERTIAN = 1
    // std::vector<G4double> dlcReflector = {1., 1., 1.}; // Diffuse lobe constant (lambertian)
    // MPTReflectorSurf->AddProperty("DIFFUSELOBECONSTANT", energy, dlcReflector);
    // TEST
    
    // TEST
    // std::vector<G4double> efficiencyReflector = {0., 0., 0.};
    // MPTReflectorSurf->AddProperty("EFFICIENCY", energy, efficiencyReflector);
    // TEST ^^^ this has zero impact (as one would expect)
    
    // Assign property to MPT, and MPT to surface
    MPTReflectorSurf->AddProperty("REFLECTIVITY", energy, reflectivityReflector); // NOTE: No difference when using this energy or energyAl203
    MPTReflectorSurf->AddProperty("RINDEX", energy, rindexReflectorSurface);
    
    // MPTReflectorSurf->AddProperty("SPECULARLOBECONSTANT", energy, slcReflector);
    // MPTReflectorSurf->AddProperty("BACKSCATTERCONSTANT", energy, bsReflector);
    
    // ...
    reflectorSurface->SetMaterialPropertiesTable(MPTReflectorSurf);
    
    //////////////////////////////
    // ALUMINA MATERIAL PROPERTIES
    //////////////////////////////
    
    // TEST: 4OpBoundaryProcessStatus::NoRINDEX is being flagged at "Reflector" volume
    auto MPTReflector = new G4MaterialPropertiesTable();
    
    std::vector<G4double> rindexAlumina = {1.78, 1.78, 1.78}; // Al2O3
    MPTReflector->AddProperty("RINDEX", energy, rindexAlumina); // NOTE: THIS DOES CHANGE SPECTRA EVER SO SLIGHTLY
    
    // NOTE: Dont add reflectivity to the material, just the surface
    
    // NOTE: By adding RINDEX to Al2O3, NoRINDEX now being flagged at "Hermetic Seal" & "Casing"
    // but they shouldnt be getting that far ...
    
    // TODO: Set this high (is infinite currently)
    // std::vector<G4double> absorptionLengthAlumina = {400 * cm, 400 * cm, 400 * cm};
    // MPTReflector->AddProperty("ABSLENGTH", energy, absorptionLengthAlumina);
    // NOTE: Dont need this... photons dont enter the medium
    
    // TEST
    Al2O3->SetMaterialPropertiesTable(MPTReflector);
    
    // NOTE: UNCOMMENT ME ^^^^^^^^^^^^^^^^^^^
    
    
    /*
     * ENCAPSULATION MATERIAL & SURFACE (ENCLOSURE & HERMETIC SEAL)
     * 
     * Alumina powder does not act as a simple specular mirror. Instead it is a random
     * scattering medium.
     * 
     * Light undergoes a "random walk", governed by the transport mean free path, which
     * is the average distance a photon travels before its direction becomes completely
     * randomised.
     * 
     * Alumina has a relatively high refractive index (~1.78 for visible light), so for
     * light arriving at angles less than the critical angle, it refracts into the alumina
     * grains (entering the material).
     * 
     * A portion of the scintillation light will penetrate the reflector material, and
     * then undergo refraction into the individual alumina grains.
     * 
     * Alumina has an incredibly low bulk absorption coefficient in the visible spectrum,
     * hence the light undergoes multiple internal reflections within the reflector powder
     * granular matrix.
     * 
     * Due to this low bulk absorption coefficient, and since the powder grains scatter
     * light, a photons path is a statistical distribution.
     * 
     * While the probability heavily favours the photon scattering backward into the high
     * rindex scintillator crystal (i.e. NaI:Tl ~1.85), a fraction of the photons will
     * migrate forward, eventually reaching the outer boundary of the powder layer, via
     * diffuse transmission.
     * 
     * Light leakage through the reflector via transmission is a real and well-documented
     * physical phenomenon. In an unencapsulated system, roughly 1-5% of the scintillation
     * photons can scatter all the way through a standard powder layer and escape.
     * 
     * This is another reason why scintillator detectors require an opaque outer housing,
     * such as an aluminium cannister.
     * 
     * In a properly designed detector, the escaping photons hit the polished inner wall of
     * the aluminium housing and are redirected back toward the crystal (via the reflector).
     * 
     * The housing also helps to block external ambient light, since these same statistics
     * could let the external light permeate through the reflector.
     * 
     * The bare aluminium housing of a scinitillator is rarely left untreated. To maximise
     * reflectivity and prevent light leakage, the internal surfaces are typically treated
     * with one of the following methods:
     * - Mirror polishing of the aluminium itself (excellent specular reflection for optical photons)
     * - Highly reflective polymer films
     * - White reflective paint (diffuse TiO2 or BaSO4)
     * 
     * REFLECTIVITY
     * 
     * Polished aluminium can achieve a visible light reflectivity of 88% to 92%.
     * 
     * ABSORPTION LENGTH
     * 
     * Aluminium has an absorption length of roughly 10 to 20 nm in the visible range, as
     * free electrons quickly attenuate electromagnetic waves.
     */
    
    ////////////////////////////////
    // ALUMINIUM MATERIAL PROPERTIES
    ////////////////////////////////
    
    // auto MPTAl = new G4MaterialPropertiesTable();
    // std::vector<G4double> rindexAl = {0.59062, 0.33593, 0.22053};
//     std::vector<G4double> absLengthAl = {15 * nm, 15 * nm, 15 * nm};
    // MPTAl->AddProperty("RINDEX", energy, rindexAl);
    // Al->SetMaterialPropertiesTable(MPTAl);
    
    // NOTE: No photons lost to NoRINDEX with rindex omitted here, but accurate rindex
    // likely impacts reflection
    
    // NOTE: Absorption length not needed with dielectric_metal interface, photon can only
    // be absorbed or reflected (not refracted)
    
    ///////////////////////////////////////////////
    // ENCLOSURE & HERMETIC SEAL SURFACE PROPERTIES
    ///////////////////////////////////////////////
    
    auto MPTAlSurface = new G4MaterialPropertiesTable();
    auto aluminiumSurface = new G4OpticalSurface("Aluminium", unified, polished, dielectric_metal);
    std::vector<G4double> reflectivityAl = {0.9, 0.9, 0.9};
    MPTAlSurface->AddProperty("REFLECTIVITY", energy, reflectivityAl);
    aluminiumSurface->SetMaterialPropertiesTable(MPTAlSurface);
    
    // TODO: USE REAL/IMAGINARY RINDICES INSTEAD OF FLAT VALUE (refractiveindex.info - Al)
    // ^ only relevant if hermetic seal is in contact with PMT window
    
    /*
     * TRANSMISSION SURFACES ...
     * 
     * NOTE: If a surface is not specified, but the two volumes on either side of the border
     * have a refractive index defined, the surface is taken to be perfectly smooth,
     * and both materials are taken to be dielectric.
     * 
     * i.e. will default to glisur, polished, dielectric-dielectric surface
     * 
     * Photons will undergo total internal reflection, refraction or reflection,
     * depending on the photon wavelength, angle of incidence, and refractive indices
     * on both sides of the boundary.
     * 
     * Hence, only a refractive index for the wavelength of photons entering the medium,
     * and a sizeable absorption length need to be specified, in order to model 
     * optical photon transmission through the optical grease and the optical window.
     * 
     * NOTE: The energy of visible light photons does not match the energy levels required
     * to excite electrons in the optical materials, preventing significant absorption,
     * hence absorption length is set high.
     * 
     * NOTE: Optical surfaces with transmission & reflectivity are not defined,
     * as this may bypass the Fresnel computation
     */
    
    /////////////////////////////////////
    // OPTICAL GREASE MATERIAL PROPERTIES
    /////////////////////////////////////
    
    // Optical grease (interface between crystal and PMT window)
    auto MPTGrease = new G4MaterialPropertiesTable();
    
    // Dielectric polished surface, allowing refraction
    auto greaseSurface = new G4OpticalSurface("GreaseSurface", unified, polished, dielectric_dielectric);
    
    // Refractive index of optical grease (1.46 @ 589.3 nm)
    std::vector<G4double> rindexGrease = {1.46, 1.46, 1.46}; // TODO: Refractive index matching ...
    // TODO: Refine this across 300-550 nm emission range
    
    // High absorption length in the medium
    std::vector<G4double> abslengthGrease = {420. * cm, 420. * cm, 420. * cm};
    
    // Assign the values to the MPT
    MPTGrease->AddProperty("RINDEX", energy, rindexGrease);
    MPTGrease->AddProperty("ABSLENGTH", energy, abslengthGrease);
    
    // Assign the refractive index and absorption length to the optical grease material
    PDMS->SetMaterialPropertiesTable(MPTGrease);
    
    /////////////////////////////////////
    // OPTICAL WINDOW MATERIAL PROPERTIES
    /////////////////////////////////////
    
    // PMT Glass MPT (Optical window)
    auto MPTWindow = new G4MaterialPropertiesTable();
    
    // Dielectric polished surface, allowing refraction
    auto windowSurface = new G4OpticalSurface("WindowSurface", unified, polished, dielectric_dielectric);
    
    // Refractive index of borosilicate glass (1.53024 @ 404.7 nm - SCHOTT BK7 Datasheet)
    // std::vector<G4double> rindexBorosilicate = {1.53, 1.53, 1.53};
    // TODO: Refine this across 300-550 nm emission range
    std::vector<G4double> rindexBorosilicate = {1.51872, 1.53024, 1.54272}; // NOTE: From SCOTT BK7 datasheet
    // NOTE: Closest indices for (550 nm, 415 nm, 325 nm) => (546.1 nm, 404.7 nm, 334.1 nm)
    
    // High absorption length in the medium
    std::vector<G4double> abslengthBorosilicate = {420. * cm, 420. * cm, 420. * cm};
    
    // Assign the values to the MPT
    MPTWindow->AddProperty("RINDEX", energy, rindexBorosilicate);
    MPTWindow->AddProperty("ABSLENGTH", energy, abslengthBorosilicate);
    
    // Assign the refractive index and absorption length to the borosilicate glass material
    borosilicate->SetMaterialPropertiesTable(MPTWindow);
    
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
    // auto photocathodeSurface = new G4OpticalSurface("Photocathode", unified, polished, dielectric_metal);
    auto photocathodeSurface = new G4OpticalSurface("Photocathode", unified, ground, dielectric_metal);
    // NOTE: Polished more typical for PC
    
    // ...
    // photocathodeSurface->SetSigmaAlpha(0.0175);

    // Reflectivity of the photocathode
    // std::vector<G4double> energyScoring = {1.239841939*eV / 0.700, 1.239841939*eV / 0.551, 1.239841939*eV / 0.400}; // 400 nm - 700 nm (visible range)
    // std::vector<G4double> reflectivityScoring = {0.9, 0.9, 0.9}; // Li apparently 90% reflectivity between 400-700 nm
    // std::vector<G4double> reflectivityScoring = {0.05, 0.05, 0.05}; // NOTE: But that massively decreases efficiency ...
    // NOTE: There must be a methodology used to decrease reflectivity in this application (yes, anti-reflective coatings)
    
    // Quantum efficiency
    // std::vector<G4double> efficiencyScoring = {0.25, 0.25, 0.25}; // 25% QE starter (flat efficiency)
    
    // Bialkali photocathode (K--Cs, i.e. K2CsSb or K-Cs-Sb)
    // 
    // std::vector<G4double> reflectivityScoring = {0.21, 0.21, 0.21}; // Reflectivity: ~21% @500nm (Harmer et al [2012])
    // std::vector<G4double> reflectivityScoring = {0.05, 0.05, 0.05};
    
     // std::vector<G4double> reflectivityScoring = {0., 0., 0.}; // TODO: TEST
     // std::vector<G4double> reflectivityScoring = {1., 1., 1.}; // TODO: TEST
    
    // std::vector<G4double> reflectivityScoring = {0.25, 0.18, 0.13}; // Optical properties of bialkali photocathodes - Motta (2004)
    std::vector<G4double> efficiencyScoring = {0.08, 0.27, 0.26}; // QE: 0.08 @550nm, ~0.27 @415nm, ~0.26 @325nm (KNOLL)
    // TODO: PC polished w/ just new R, QE
    
    // TODO: Real/imaginary rindices instead of reflectivity? Takes into account incident angle.
    
    // TODO: IQE vs EQE (this is EQE afaik)
    // TODO: Anti-reflective coating? So likely much less than 21% reflectivity?
    // TODO: Transmittance = 33% @500nm (Harmer et al [2012])
    
    // NOTE: Using same energy as other MPTS has no effect on output spectrum at all (vs "energyScoring")
    // MPTPhotocathode->AddProperty("REFLECTIVITY", energy, reflectivityScoring); // 1 minus the absorption coeffcient
    MPTPhotocathode->AddProperty("EFFICIENCY", energy, efficiencyScoring); // Chance of an absorbed photon to be detected
    
    
    // TEST TEST TEST TEST Motta (2004) - Optical properties of bialkali photocathodes
    std::vector<G4double> realRindexScoring = {3.20, 2.38, 1.92}; // n (@550nm, @415nm, @325nm)
    std::vector<G4double> imaginaryRinexScoring = {0.63, 1.71, 1.69}; // k
    // (@545nm, @410nm, @380nm) <- closest available datapoints (NOTE: 325 pretty far off 380, but bulk absorption high here)
    // maybe see if there is another dateset for confluence, or interpolate for a val closer to 325nm
    MPTPhotocathode->AddProperty("REALRINDEX", energy, realRindexScoring);
    MPTPhotocathode->AddProperty("IMAGINARYRINDEX", energy, imaginaryRinexScoring);
    // TEST TEST TEST TEST
    
    
    // ...
    photocathodeSurface->SetMaterialPropertiesTable(MPTPhotocathode);
    
    // NOTE: Only seeing ~1% of the total optical photons being "DETECTED" on full energy deposition
    // Actually ranges from ~1% to 7% seemingly
    // update: photopeak centroid typically at ~5-6% of total photons
    
    // Bialkali material has a broad spectra response from 170-560 nm
    // photocathode spectral response should match the emission spectrum of the scintillator used
    
    /*
     * ...
     */
    
    // Assign a refractive index to air, using the same energy vector as above
    auto MPTAir = new G4MaterialPropertiesTable();
    std::vector<G4double> rindexAir = {1., 1., 1.}; // MPT2->AddProperty("RINDEX", "Air") NOTE: Default available
    MPTAir->AddProperty("RINDEX", energy, rindexAir);
    // air->SetMaterialPropertiesTable(MPTAir);
    
    // TODO: Test removing this (to identify any air gaps)
    
    
    /////////
    // WORLD:
    /////////
    
    // The largest volume is the "World" volume (top level container)
    // NOTE: The world volume must contain all other volumes in the detector geometry (with some margin)
    // NOTE: A box is the most simple (and efficient) shape to describe the world

    // Define the world box dimensions (x, y, z)
    G4double world_hx = 100 * cm;
    G4double world_hy = 100 * cm;
    G4double world_hz = 100 * cm;

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
    G4double crystalInnerRad = 0. * cm; // No centre hole
    G4double crystalOuterRad = (7.62 * 0.5) * cm; // 3 inch = 7.62 cm diameter => (diameter / 2) = 3.81 cm outer radius
    G4double crystalHeight = 7.62 * cm; // 3 inch height (this arg will be doubled, so will need to * 0.5)
    G4double startAngle = 0. * deg;
    G4double endAngle = 360. * deg; // Full circumference cylinder

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
    G4double crystalX = 0. * cm;
    G4double crystalY = 0. * cm;
    G4double crystalZ = 10. * cm; // 10cm (maybe 3cm as i have a lot of data for that distance)
    
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
    
    // TEST: Create a region for the crystal (for 100 um cuts only in detector volume)
    auto crystalRegion = new G4Region("Scintillator"); // NOTE: Havent imported this ?
    scintillatorLog->SetRegion(crystalRegion);
    crystalRegion->AddRootLogicalVolume(scintillatorLog);
    
    
    /////////////
    // REFLECTOR:
    /////////////
    
    // The scintillation photons are emitted in all directions, so a high efficiency reflector
    // is used to surround the crystal (Al_{2}O_{3} and teflon), on all sides except the back,
    // to increase the amount of photons reaching the the photocathode
    
    // Inner rad can (4.04495 cm) - outer rad crystal (3.81 cm) => 0.23495 cm reflector thickness
    G4double reflectorThickness = 0.23495 * cm;
    G4double reflectorOuterRad = crystalOuterRad + reflectorThickness;
    G4double reflectorHeight = crystalHeight + (reflectorThickness * 2);
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
    
    
    //////////////////
    // OPTICAL GREASE:
    //////////////////
    
    // Silicon gel with ~same thickness as reflector (2.3495 mm as per schematics)
    // NOTE: Some sources say only 0.1 mm thickness is suggested though ^ (and less kinda makes sense)
    // NOTE: Some sources say only 10-50 um thickness
    // NOTE: OST Photonics 2" NaI schematic states 2mm gel thickness
        
    // ... the optical grease will then be pressed against the PMT window
    // G4double greaseThickness = reflectorThickness; // Same thickness as reflector (2.3495 mm)
    G4double greaseThickness = 25 * um; // TEST
    
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
    auto greaseLog = new G4LogicalVolume(grease, PDMS, "OpticalGrease");
    
    // Translation along Z axis (relative to crystal origin)
    G4double greaseZ = crystalZ + (crystalHeight * 0.5) + (greaseThickness * 0.5); // TEST
    // NOTE: Places it on crystal Z (centre of the crystal), 
    // translates it by half the crystal height (to account for it being centre of crystal),
    // due to 0.5 reflector thickness being placed either side of its origin,
    // need to shift it by 0.5 * its thickness
    
    // Placed on the back side of the crystal
    G4VPhysicalVolume* greasePhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(crystalX, crystalY, greaseZ), // Translation
        greaseLog, // The logical volume
        "OpticalGrease", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        checkOverlaps
    );
    
    
    ////////////////////////
    // PHOTOMULTIPLIER TUBE:
    ////////////////////////

    // NOTE: Just modelling the PMT window (OPTICAL WINDOW)
    
    G4double windowThick = 0.2 * cm; // 2mm according to hamamatsu handbook
    
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
    auto windowLog = new G4LogicalVolume(window, borosilicate, "OpticalWindow");
    
    // Translation along Z axis (relative to optical grease origin)
    G4double windowZ = greaseZ + (greaseThickness * 0.5) + (windowThick * 0.5); // TEST
    // NOTE: Places it on grease Z (centre of the grease),
    // translates it by half the grease height (to account for it being centre of grease),
    // due to 0.5 window thickness being placed either side of window origin,
    // need to shift it by 0.5 * its thickness
    
    // Placed against the optical grease
    G4VPhysicalVolume* windowPhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(crystalX, crystalY, windowZ), // Translation
        windowLog, // The logical volume
        "OpticalWindow", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        checkOverlaps
    );
    
    
    ////////////////
    // PHOTOCATHODE:
    ////////////////
    
    // Coating inside of PMT optical window
    
    // ...
    // G4double photocathodeThick = 0.1 * cm; // 1mm
    G4double photocathodeThick = 20 * nm; // 20nm
    
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
    auto photocathodeLog = new G4LogicalVolume(photocathode, Li, "Photocathode");
    
    // Translation along Z axis (relative to optical window origin)
    G4double photocathodeZ = windowZ + (windowThick * 0.5) + (photocathodeThick * 0.5);
    // NOTE: Places it on window Z (centre of the window),
    // translates it by half the window height (to account for it being centre of window),
    // due to 0.5 window thickness being placed either side of window origin,
    // need to shift it by 0.5 * its thickness
    
    // Placed on the inside of the PMT window
    G4VPhysicalVolume* photocathodePhys = new G4PVPlacement(
        nullptr, // No rotation
        G4ThreeVector(crystalX, crystalY, photocathodeZ), // Translation
        photocathodeLog, // The logical volume
        "Photocathode", // Name
        worldLog, // Mother volume (logical)
        false, // No boolean ops
        0, // Copy number
        checkOverlaps
    );
    
    
    ///////////////////////
    // ALUMINIUM ENCLOSURE:
    ///////////////////////
        
    // Enclosure is 3.225' outer diameter according to ortec spec, and 0.2' thickness (0.508mm)
    // G4double inchToCM = 2.54;
    G4double enclosureThick = 0.0508 * cm; // NOTE: 0.02' => 0.508 mm (added to both sides in Z direction)
    G4double enclosureOuterRad = ((3.225 * 2.54) / 2) * cm; // NOTE: 3.225' dia => 8.1915cm dia => 4.09575 cm outer rad
    G4double enclosureLength = reflectorHeight + (enclosureThick * 2); // NOTE: Same height as reflector, with thickness added to both end
    // NOTE: 4.04495 cm inner rad
    
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
    
    // Assign a material to the enclosure solid
    auto enclosureLog = new G4LogicalVolume(
        enclosure, // subtraction solid acts same as any other geometry here
        Al, // enclosure material (aluminium)
        "Enclosure"
    );
    
    // Place the enclosure
    G4VPhysicalVolume* enclosurePhys = new G4PVPlacement(
        nullptr, // no rotation
        crystalTrans, // same position as crystal
        enclosureLog, // logical volume
        "Enclosure", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        checkOverlaps
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
    G4double sealLength = windowThick;
    G4double sealOuterRad = reflectorOuterRad;
    
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
    
    // Translation along Z axis (relative to optical window origin)
    // G4double sealZ = windowZ;
    G4double sealZ = crystalZ + (crystalHeight * 0.5) + reflectorThickness + (sealLength * 0.5); // TEST
    
    // Place the seal
    G4VPhysicalVolume* sealPhys = new G4PVPlacement(
        nullptr, // no rotation
        G4ThreeVector(crystalX, crystalY, sealZ), // same position as crystal
        sealLog, // logical volume
        "HermeticSeal", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        checkOverlaps
    );

    
    //////////
    // SOURCE:
    //////////
    
    // Source geometry specification
    G4double sourceRadius = 0.1 * cm; // 1mm
    
    // Source geometry definition (modelled as sphere in "decay", but apparently is cylinder)
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
    auto sourceLog = new G4LogicalVolume(solidSource, sourceMat, "Source");
    
    // Define translation (offset from origin by 1cm => so 11cm from detector)
    // auto sourceTrans = G4ThreeVector(0 * cm, 0 * cm, -1. * cm);
    // NOTE: This Z should be crystalZ + (crystalHeight * 0.5) + (reflectorThick) + (enclosureThick) + (sourceDetectorDist)
    
    // NOTE: Face of the detector is 5.90425 cm from world origin (0, 0, 0)
    // so, source had been 6.90425 cm from face of detector in all prior sims
    
    // 3cm source-detector (face) distance, as it was in lab work (and my recorded spectra)
    G4double sourceDetectorDist = 3. * cm;
    G4double sourceZ = crystalZ - ((crystalHeight * 0.5) + reflectorThickness + enclosureThick) - sourceDetectorDist;

    // ...
    auto sourceTrans = G4ThreeVector(crystalX, crystalY, sourceZ);
    // NOTE: Source is placed exactly in line with crystal in (x, y) plane, and specified distance in z
    
    // TODO: I NEED THIS IN PRIMARY GENERATOR ACTION TOO ...
    // TODO: THE LAST SPECTRUM ONLY MOVED THE CASING LOL NOT THE SOURCE
    
    // Place the radioactive source 
    // G4VPhysicalVolume* sourcePhys = new G4PVPlacement(
    //     nullptr,
    //     sourceTrans,
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
    
    // Encapsulates alpha and beta radiation
    
    // Box base solid
    // Subtraction solid of same geometry as source
    // likely slightly different in reality but itll do
    
    // Source encapsulation dimensions
    G4double casingSizeX = 3. * cm;
    G4double casingSizeY = enclosureOuterRad * 2; // Same height as diameter of detector enclosure
    G4double casingSizeZ = 0.5 * cm;
    
    // Base geometry which will be cut
    auto casingBase = new G4Box("CasingBase", casingSizeX * 0.5, casingSizeY * 0.5, casingSizeZ * 0.5);
    
    // Cut to be made in base geometry
    auto casingCut = new G4Sphere(
        "CasingCut", // name
        0., // minmum radius (0 = not hollow),
        sourceRadius, // maximum radius
        0. * deg, // minimum phi angle
        360. * deg, // maximum phi angle (NOTE: Assuming this is like span angle ?)
        0. * deg, // minimum theta angle
        180. * deg // maximum theta angle (NOTE: What are these last two for ?)
    );
    
    // Create new solid with cut subtracted from base
    auto casing = new G4SubtractionSolid(
        "Casing", // name
        casingBase, // the solid to subtract from
        casingCut, // the volume to subtract
        nullptr, // no rotation
        G4ThreeVector(0., 0., 0.) // cut translation (relative to base solid, not world)
    );
    
    // Assign a material to the casing solid
    auto casingLog = new G4LogicalVolume(
        casing, // subtraction solid acts same as any other geometry here
        PVC, // casing material (aluminium)
        "Casing"
    );
    
    // Place the casing
    G4VPhysicalVolume* casingPhys = new G4PVPlacement(
        nullptr, // no rotation
        sourceTrans, // same position as crystal
        casingLog, // logical volume
        "Casing", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        checkOverlaps
    );
    
    // TODO: Could get bit spicy and add top section too
    
    
    ////////////
    // TABLETOP:
    ////////////

    // MDF style wood proxy, ~1' -> 2' thick
    
    // Table geometry parameters
    G4double tableSize = 50. * cm; // 1m probably better but dont wanna make world massive (also is rectangle not square)
    G4double tableHeight = 5. * cm; // 5cm ? TODO: spitballing, need to refine this
    
    // ...
    auto table = new G4Box("Table", tableSize * 0.5, tableSize * 0.5, tableHeight * 0.5);
    
    // Using wood as a proxy for MDF (which would actually be slightly different)
    auto tableLog = new G4LogicalVolume(table, wood, "Table");
    
    // Rotate about z-axis 90 degrees
    auto tableRot = new G4RotationMatrix();
    tableRot->rotateX(90. * deg);
    // NOTE: Could just change xyz lengths, but leaving this here as example of rotation matrix
    
    // Translate in -y direction by radius of can + half thickness of table
    G4double tableTransY = -1. * (enclosureOuterRad + (tableHeight * 0.5));
    // NOTE: So that bottom of enclosure rests on table
    
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
        checkOverlaps
    );

    
    ////////////
    // SURFACES:
    ////////////
    
    // Define the border between the crystal and the reflector
    auto crystalReflectorBorder = new G4LogicalBorderSurface("CrystalToReflector", crystalPhys, reflectorPhys, reflectorSurface);

    // NOTE: Crystal->Grease, and Grease->Window surfaces are not explicitly needed when rindex of each is passed
    // will default to: GLISUR, polished, dielectric_dielectric
    // ^ maybe do it just to explicitly pick UNIFIED
    auto crystalGreaseBorder = new G4LogicalBorderSurface("CrystalToGrease", crystalPhys, greasePhys, greaseSurface);
    auto greaseWindowBorder = new G4LogicalBorderSurface("GreaseToWindow", greasePhys, windowPhys, windowSurface);
    // NOTE: After testing, adding these two borders produces an identical spectrum to just 
    // leaving these two borders as default
    
    // TEST \/\/\/\/
    // Define the border between the reflector and enclosure
    // auto reflectorEnclosureBorder = new G4LogicalBorderSurface("ReflectorToEnclosure", reflectorPhys, enclosurePhys, aluminiumSurface);
    //
    // Define the border between the reflector and hermetic seal
    // auto reflectorSealBorder = new G4LogicalBorderSurface("ReflectorToSeal", reflectorPhys, sealPhys, aluminiumSurface);
    // TEST ^^^^^^^
    
    // NOTE: ^^^ These are not needed, as groundbackpainted prevents refraction
    
    // TODO: Border back from: (enclosurePhys -> reflectorPhys) & (sealPhys - reflectorPhys)
    // auto enclosureReflectorBorder = new G4LogicalBorderSurface("EnclosureToReflector", enclosurePhys, reflectorPhys, reflectorSurface);
    // auto enclosureSealBorder = new G4LogicalBorderSurface("SealToReflector", sealPhys, reflectorPhys, reflectorSurface);
    // TODO: Border back from reflectorPhys->crystalPhys
    // auto reflectorCrystalBorder = new G4LogicalBorderSurface("ReflectorToCrystal", reflectorPhys, crystalPhys, reflectorSurface);
    
    // NOTE: ^^^ I DONT THINK THESE ARE ACTUALLY NEEDED
    
    // Define the border between the optical window and the hermetic seal
    // auto windowSealBorder = new G4LogicalBorderSurface("WindowToSeal", windowPhys, sealPhys, aluminiumSurface); // NOTE: UNCOMMENT ME
    
    auto windowSealBorder = new G4LogicalBorderSurface("WindowToReflector", windowPhys, reflectorPhys, reflectorSurface); // TEST (for 25 um grease geom)
    
    // Define the border between the optical window and the photocathode
    auto windowPhotocathodeBorder = new G4LogicalBorderSurface("WindowToPhotocathode", windowPhys, photocathodePhys, photocathodeSurface);
    
    
    /////////////
    // COLOURING:
    /////////////

    // Set visualiser colouring (R, G, B, opacity), and assign colours to the detector geometry
    // NOTE: I think these could also be set via the visualiser init macro script
   
    // World
    auto worldVisAtt = new G4VisAttributes(G4Color(0., 0., 1., 0.1)); // blue (transparent)
    worldVisAtt->SetForceSolid(true); // ... (think this can be called w/ no arg for same effect)
    worldLog->SetVisAttributes(worldVisAtt); // assign to the logical volume

    // Scintillator crystal
    auto scintillatorVisAtt = new G4VisAttributes(G4Color(1., 1., 1., 0.5)); // white (part-transparent)
    scintillatorVisAtt->SetForceSolid(true);
    scintillatorLog->SetVisAttributes(scintillatorVisAtt);
    
    // Aluminium oxide reflector
    auto reflectorVisAtt = new G4VisAttributes(G4Color(0., 0., 1.0, 0.5)); // blue
    reflectorVisAtt->SetForceSolid(true);
    reflectorLog->SetVisAttributes(reflectorVisAtt);
    
    // Aluminium enclosure
    auto enclosureVisAtt = new G4VisAttributes(G4Color(0.8, 0.8, 0.8, 1.)); // mid-light gray (solid)
    enclosureVisAtt->SetForceSolid(true);
    enclosureLog->SetVisAttributes(enclosureVisAtt);
    
    // Optical grease
    auto greaseVisAtt = new G4VisAttributes(G4Color(1.0, 1.0, 0., 0.75)); // yellow
    greaseVisAtt->SetForceSolid(true);
    greaseLog->SetVisAttributes(greaseVisAtt);
    
    // Optical window
    auto windowVisAtt = new G4VisAttributes(G4Color(0.8, 0.8, 0.8, 0.25)); // mid-light gray (transparent)
    windowVisAtt->SetForceSolid(true);
    windowLog->SetVisAttributes(windowVisAtt);
    
    // Scoring photocathode
    auto photocathodeVisAtt = new G4VisAttributes(G4Color(1., 0., 0., 0.5)); // red
    photocathodeVisAtt->SetForceSolid(true);
    photocathodeLog->SetVisAttributes(photocathodeVisAtt);
    
    // Hermetic seal
    auto sealVisAtt = new G4VisAttributes(G4Color(0.9, 0.9, 0.9, 1.)); // light gray (opaque)
    sealVisAtt->SetForceSolid(true);
    sealLog->SetVisAttributes(sealVisAtt);
    
    // Source geometry
    auto sourceVisAtt = new G4VisAttributes(G4Color(0.0, 1.0, 0.0, 0.5)); // green
    sourceVisAtt->SetForceSolid(true);
    sourceLog->SetVisAttributes(sourceVisAtt);
    
    // Source casing geometry
    auto casingVisAtt = new G4VisAttributes(G4Color(0.8, 0.8, 0.8, 0.25)); // mid-light gray (transparent)
    casingVisAtt->SetForceSolid(true);
    casingLog->SetVisAttributes(casingVisAtt);
    
    // Tabletop geometry
    auto tableVisAtt = new G4VisAttributes(G4Color(0.95, 0.95, 0.95, 1.)); // light gray
    tableVisAtt->SetForceSolid(true);
    tableLog->SetVisAttributes(tableVisAtt);
    
    
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

// }
