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


// TODO: Probably wanna use consistent units throughout (cm probably easiest to adhere to)

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
    NaI->AddElement(Na, 0.1362 * perCent);
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
    
    // Wavelength range listed for NaI (refractiveindex.info)
    // std::vector<G4double> energy = {1.9587*eV, 2.3991*eV, 2.8437*eV}; // Wavelength (~436nm - 633nm)    TODO: This could be refined to emi range
    // std::vector<G4double> energy = {1.239841939*eV/0.633, 1.239841939*eV/0.436}; // 436 nm - 633 nm (smallest must go first)
    // NOTE: Visible light ranges from ~400 nm (violet) to ~700 nm (red)
    // TODO: See EMI notes
    std::vector<G4double> energy = {2.25425 * eV, 2.98756 * eV, 3.81488 * eV}; // (550 nm, 415 nm, 325 nm) - (green, violet, long wavelength ultraviolet) (hence most of the spectrum in blue-violet range)
    // 3 sources say max @ 415 nm
    //
    // One says:
    // 325-525nm, max @ 410 nm
    // 
    // Another says:
    // 340-520nm, with max @ 410nm
    
    // Refractive index (n) - The ratio of speed of light in air/vaccum (c) to SOL in medium (v) (NOTE: n = (c / v))
    // std::vector<G4double> rindex = {1.7779, 1.8043, 1.8391}; // A function of wavelength (~436nm - 633nm)
    // NOTE: Added a central curve value to show non-linear trend
    std::vector<G4double> rindex = {1.85, 1.85, 1.85}; // 1.85 @ emission max (415 nm)

    // Properties that depend on energy
    // NOTE: Vector lengths must be the same, 1st vector is energy, 2nd is property value at that energy
    //
    // Refractive index as a function of wavelength
    MPTCrystal->AddProperty("RINDEX", energy, rindex);
    
    // The energy spectrum of the emitted scintillation photons
    // NOTE: This is essential to generate the correct number of photons (25156 for 662 keV, instead of 5-10)
    // std::vector<G4double> emission = {1., 1., 1.}; // same amount of photons for each wavelength
    std::vector<G4double> emission = {0.1, 1., 0.1}; // emission max @ 415 nm
    // std::vector<G4double> emission = {0.01, 1., 0.01}; // NOTE: No different to 0.1 upper/lower
    MPTCrystal->AddProperty("SCINTILLATIONCOMPONENT1", energy, emission); // "Fast component"
    // NOTE: Tells Geant4 how many photons for each wavelength (or energy)
    // The scintillation photons will have a spectrum, depending on wavelength,
    // may have more photons in red spectrum than blue spectrum
    // TODO: NaI(Tl) actually emits light in ~340-520nm wavelength region, peaking at ~410, 415, or 420nm (gaussian shape)
    // mostly blue-violet light
    
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
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 0.); // no fluctuation
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 1.); // 1. to start, tune later
    MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 3.5); // Miller et al (2024)
    // MPTCrystal->AddConstProperty("RESOLUTIONSCALE", 10.); // more gaussian
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
     * reflection probability must be set by "REFLECTIVITY" property.
     * 
     * Front painted finishes prevent refraction (no transmission), but still allow for absorption:
     * - PolishedFrontPainted (specular spike reflection)
     * - GroundFrontPainted (lambertian reflection - diffuse)
     * 
     * NOTE: Both are suitable (as they prevent refraction, incident light will only absorb or reflect),
     * 
     * Eliminating possibility for visible light photons to penetrate alumina reflector seems suitable (painted options).
     * 
     * Back painted finishes prevent refraction (no transmission), but allow for custom reflection mechanics:
     * - PolishedBackPainted (specular spike, specular lobe, backscatter, lambertian)
     * - GroundBackPainted (specular spike, specular lobe, backscatter, lambertian)
     * 
     * SigmaAlpha to specify surface roughness...
     * 
     * NOTE: RINDEX must be specified (for surface) for back painted afaik (tested with and without)
     * 
     * Back painted allows for; setting sigma alpha, and individual reflection constants
     * I.e. ground front painted only lambertian, but ground back painted may be one of four
     * 
     * NOTE: If reflectivity = 1 -> no absorption at painted boundary
     * If reflectivity = 0.9 -> 0.1 absorption at boundary ?
     * 
     * NOTE: With a "painted" surface finish, it seems the rindex, or an MPT,
     * doesnt need to be assigned to the Al203 material, spectrum remains the same either way.
     * 
     * NOTE: SigmaAlpha only seems to have an effect on BackPainted (not FrontPainted)
     */

    // Reflector MPT (definining probability of reflection, or else absorption)
    auto MPTReflectorSurf = new G4MaterialPropertiesTable();
    
    // Unified model, polished front painted so refraction does not occur, dielectric-dielectric interface
    // auto reflectorSurface = new G4OpticalSurface("ReflectorSurface", unified, polishedfrontpainted, dielectric_dielectric);
    // NOTE: Reflectors are typically diffuse (ground) for better results, will see how much difference it makes in sim
    //
    // auto reflectorSurface = new G4OpticalSurface("ReflectorSurface", unified, groundfrontpainted, dielectric_dielectric);
    //
    auto reflectorSurface = new G4OpticalSurface("ReflectorSurface", unified, groundbackpainted, dielectric_dielectric);
    
    // Specify surface roughness (For back painted surface)
    reflectorSurface->SetSigmaAlpha(0.1); // Almost polished (specular)
    // reflectorSurface->SetSigmaAlpha(0.25); // Ground polished (partially diffuse)
    // reflectorSurface->SetSigmaAlpha(0.35);
    // reflectorSurface->SetSigmaAlpha(0.4);
    // reflectorSurface->SetSigmaAlpha(0.5); // Very Matte / Rough powder (strongly diffuse)
    // reflectorSurface->SetSigmaAlpha(0.75); // Rough powder (strongly diffuse)
    // reflectorSurface->SetSigmaAlpha(1); // Strongly diffuse
    
    // Reflection probability as a function of wavelength (1 = all photons will be reflected (Default val))
    // std::vector<G4double> reflectivityReflector = {0.9, 0.9, 0.9}; // Rough est val
    // std::vector<G4double> reflectivityReflector = {0.94, 0.94, 0.94}; // 96% alumina -> 94% reflectance
    std::vector<G4double> reflectivityReflector = {0.96, 0.96, 0.96}; // 99.7% alumina -> 96% reflectance @500-2000nm
    // NOTE: Need to specify reflectivity != 1. else no absorption in reflector (perfect reflector, not realistic)
    
    // Refractive index (back painted)
    // std::vector<G4double> rindexReflector = {1.78, 1.78, 1.78}; // Al2O3
    std::vector<G4double> rindexReflector = {1., 1., 1.}; // Air (dry packed causes layer of air)
    // std::vector<G4double> rindexReflector = {1.46, 1.46, 1.46}; // Silicone optical grease
    // NOTE: Levin 1996 (UNIFIED) says r = 1 when tape like coating (i.e. for powders) due to air gap,
    // and r = optical expoxy (i.e. silicone gel) for Al/Ti/Mg oxide powders mixed with epoxy
    
    // Reflection constants (back painted)
    std::vector<G4double> slcReflector = {0.05, 0.05, 0.05}; // Specular lobe constant
    std::vector<G4double> bsReflector = {0.01, 0.01, 0.01}; // Backscatter constant
    // NOTE: Defaults to specular spike = 0, specular lobe = 0, backscatter = 0, lambertian = 1
    // if other values are set to non-0, lambertial will be 1 - sum of other components
    
    // Assign property to MPT, and MPT to surface
    MPTReflectorSurf->AddProperty("REFLECTIVITY", energy, reflectivityReflector); // NOTE: No difference when using this energy or energyAl203
    MPTReflectorSurf->AddProperty("RINDEX", energy, rindexReflector);
    
    // MPTReflectorSurf->AddProperty("SPECULARLOBECONSTANT", energy, slcReflector);
    // MPTReflectorSurf->AddProperty("BACKSCATTERCONSTANT", energy, bsReflector);
    
    reflectorSurface->SetMaterialPropertiesTable(MPTReflectorSurf);

    
    /*
     * TRANSMISSION SURFACES ...
     * 
     * NOTE: If a surface is not specified, but the two volumes on either side of the border
     * have a refractive index defined, the surface is taken to be perfectly smooth,
     * and both materials are taken to be dielectric.
     * 
     * i.e. will default to polished, dielectric-dielectric surface
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
    
    // Optical grease (interface between crystal and PMT window)
    auto MPTGrease = new G4MaterialPropertiesTable();
    
    // Dielectric polished surface, allowing refraction
    // auto greaseSurface = new G4OpticalSurface("GreaseSurface", unified, polished, dielectric_dielectric);
    
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
    
    // PMT Glass MPT (Optical window)
    auto MPTWindow = new G4MaterialPropertiesTable();
    
    // Dielectric polished surface, allowing refraction
    // auto windowSurface = new G4OpticalSurface("WindowSurface", unified, polished, dielectric_dielectric);
    
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
    
    // NOTE: Leaning towards bialkali photocathode (K--Cs, i.e. K2CsSb or K-Cs-Sb)
    // Reflectivity: ~21% @500nm
    // QE: 0.08 @550nm, ~0.27 @415nm, ~0.26 @325nm 
    std::vector<G4double> reflectivityScoring = {0.21, 0.21, 0.21};
    std::vector<G4double> efficiencyScoring = {0.08, 0.27, 0.26}; // TODO: PC polished w/ just new R, QE
    
    // NOTE: Using same energy as other MPTS has no effect on output spectrum at all (vs "energyScoring")
    MPTPhotocathode->AddProperty("REFLECTIVITY", energy, reflectivityScoring); // 1 minus the absorption coeffcient
    MPTPhotocathode->AddProperty("EFFICIENCY", energy, efficiencyScoring); // Chance of an absorbed photon to be detected
    
    photocathodeSurface->SetMaterialPropertiesTable(MPTPhotocathode);
    
    // NOTE: Only seeing ~1% of the total optical photons being "DETECTED" on full energy deposition
    // Actually ranges from ~1% to 7% seemingly
    // update: photopeak centroid typically at ~5-6% of total photons
    
    // Bialkali material has a broad spectra response from 170-560 nm
    // photocathode spectral response should match the emission spectrum of the scintillator used
    
    
    /*
     * HERMETIC SEAL
     * 
     * NOTE: Aluminium is rarely used as an uncoated reflective surface,
     * as the mechanical and environmental properties are poor,
     * however in this case it will only really be exposed to
     * a small fraction of long wavelength UV and blue-violet visible photons
     */
    
    // Hermetic seal MPT
    auto MPTSeal = new G4MaterialPropertiesTable();
    auto sealSurface = new G4OpticalSurface("HermeticSeal", unified, polished, dielectric_metal);
    std::vector<G4double> reflectivitySeal = {0.9, 0.9, 0.9};
    MPTSeal->AddProperty("REFLECTIVITY", energy, reflectivitySeal);
    sealSurface->SetMaterialPropertiesTable(MPTSeal);
    
    
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
        
    // ... the optical grease will then be pressed against the PMT window
    
    // Optical grease (transmitting incident optical photons to the PMT window)
    auto grease = new G4Tubs(
        "OpticalGrease",
        crystalInnerRad, // 0cm
        crystalOuterRad, // same radius as crystal (slots into reflector)
        reflectorThickness * 0.5, // same thickness as reflector (will be multiplied by 2 on placement)
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // PDMS (proxy for silicone gel)
    auto greaseLog = new G4LogicalVolume(grease, PDMS, "OpticalGrease");
    
    // Translation along Z axis (relative to crystal origin)
    G4double greaseZ = crystalZ + (crystalHeight * 0.5) + (reflectorThickness * 0.5);
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

    // NOTE: Will likely just be the window (OPTICAL WINDOW)
    
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
    G4double windowZ = greaseZ + (reflectorThickness * 0.5) + (windowThick * 0.5);
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
        
    // Enclosure is 3.225' outer diameter according to ortec spec, and 0.2' thickness
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
    G4double sealZ = windowZ;
    
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
    
    // TODO: PVC material
    
    // ...
    G4double casingSizeX = 3. * cm;
    G4double casingSizeY = enclosureOuterRad * 2; // Same diameter as enclosure
    G4double casingSizeZ = 1. * cm;
    
    // ...
    auto casingBase = new G4Box("Table", casingSizeX * 0.5, casingSizeY * 0.5, casingSizeZ * 0.5);
    
    // ...
    auto casingCut = new G4Sphere(
        "CasingBase", // name
        0., // minmum radius (0 = not hollow),
        sourceRadius, // maximum radius
        0. * deg, // minimum phi angle
        360. * deg, // maximum phi angle (NOTE: Assuming this is like span angle ?)
        0. * deg, // minimum theta angle
        180. * deg // maximum theta angle (NOTE: What are these last two for ?)
    );
    
    // Create new solid with cut subtracted from base
    auto casing = new G4SubtractionSolid(
        "CasingCut", // name
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
    // G4VPhysicalVolume* casingPhys = new G4PVPlacement(
    //     nullptr, // no rotation
    //     sourceTrans, // same position as crystal
    //     casingLog, // logical volume
    //     "Casing", // name
    //     worldLog, // mother volume (logical)
    //     false, // no boolean ops
    //     0, // one copy
    //     checkOverlaps
    // );
    
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

    // Define the border between the optical window and the hermetic seal
    auto windowSealBorder = new G4LogicalBorderSurface("WindowToSeal", windowPhys, sealPhys, sealSurface);
    
    // Define the border between the optical window and the photocathode
    auto windowPhotocathodeBorder = new G4LogicalBorderSurface("WindowToPhotocathode", windowPhys, photocathodePhys, photocathodeSurface);
    
    // NOTE: Crystal->Grease, and Grease->Window surfaces are not explicitly needed when rindex of each is passed
    // will default to polished dielectric_dielectric
    
    
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
