// User lib
#include "DetectorMaterials.hh"

// G4 lib
#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "G4MaterialPropertiesTable.hh" // for optical photons
// #include "G4MaterialPropertyVector.hh" // can use instead of 2x std::vector

/*
 * ...
 */
void DetectorMaterials::DefineDetectorMats() {
    //////////////////////
    // GEOMETRY MATERIALS:
    //////////////////////

    // Get a pointer to the material manager instance
    G4NistManager* nist = G4NistManager::Instance();

    // World material
    fAir = nist->FindOrBuildMaterial("G4_AIR"); // Carbon, nitrogen, oxygen, argon
    
    G4Element* Na = nist->FindOrBuildElement("Na");
    G4Element* I = nist->FindOrBuildElement("I");
    G4Element* Tl = nist->FindOrBuildElement("Tl");

    // Scintillator material
    // G4Material* NaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE"); // (1 part Na, 1 part I), density = 3.667 g/cm^3
    // auto Na = new G4Element("Sodium", "Na", 11, 22.990 * g/mole);
    // auto I = new G4Element("Iodine", "I", 53, 126.90 * g/mole);
    // auto Tl = new G4Element("Thallium", "Tl", 81, 204.38 * g/mole);
    
    fNaI = new G4Material("NaI:Tl", 3.667 * g/cm3, 3);
    
    fNaI->AddElement(Na, 15.3035 * perCent);
    fNaI->AddElement(I, 84.5603 * perCent);
    // NaI->AddElement(Na, 0.1362 * perCent); // TODO: I have been running tests with Na instead of Tl
    fNaI->AddElement(Tl, 0.1362 * perCent);
    // TODO: NaI:Tl blend
    
    // Scintillation light reflector material (Alumina - Al2O3)
    // fAl2O3 = nist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE"); // (2 part Al, 3 part O), density = 3.97 g/cm^3
    // fAl2O3 = nist->BuildMaterialWithNewDensity("Al2O3", "G4_ALUMINUM_OXIDE", 1.2 * g/cm3);
    fAl2O3 = nist->BuildMaterialWithNewDensity("Al2O3", "G4_ALUMINUM_OXIDE", 2.0 * g/cm3);
    
    // Scintillator can material (Aluminium)
    fAl = nist->FindOrBuildMaterial("G4_Al"); // density = 2.699 g/cm^3
    
    // G4Element* H = nist->FindOrBuildElement("H"); // TODO: ................................................................................................................
    
    // Optical grease (Silicone gel) - NOTE: Synthetic "silicone" (polymer), not natural "Silicon"
    // auto H = new G4Element("Hydrogen", "H", 1, 1.0078 * g/mole); // Z=1, A=1, density = 8.3748E-5 g/cm^3
    // auto C = new G4Element("Carbon", "C", 6, 12.011 * g/mole); //  Z=6, A=12, density = 2 g/cm^3
    // auto O = new G4Element("Oxygen", "O", 8, 15.999 * g/mole); // Z=8, A=16, density = 0.00133151 g/cm^3
    // auto Si = new G4Element("Silicon", "Si", 14, 28.086 * g/mole); // Z=14, A=28, density = 2.33 g/cm^3
    
    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* O = nist->FindOrBuildElement("O");
    G4Element* Si = nist->FindOrBuildElement("Si");
    
    fPDMS = new G4Material("PDMS", 0.97 * g/cm3, 3);
    
    fPDMS->AddElement(Si, 1); // NOTE: MUST BE FRACTIONAL IF MATERIAL, PASS ELEMENT IF USING ATOMS
    fPDMS->AddElement(C, 2);
    fPDMS->AddElement(H, 6);
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
    fBorosilicate = new G4Material("BOROSILICATE_GLASS", 2.23 * g/cm3, 4); // 80.6% SiO2, , density = 2.23 g/cm^3 (NOTE: Some sources say 2.51 density)
    fBorosilicate->AddMaterial(SiO2, 80.6 * perCent); // Borosilicate ~81%, Soda lime is ~69% (Forms the basic structure of the glass)
    fBorosilicate->AddMaterial(B2O3, 13. * perCent); // Stated as 5-13% (Lowers melting temp, enhances chemical durability and thermal shock resistance)
    // 6.4% remaining
    fBorosilicate->AddMaterial(fAl2O3, 2.4 * perCent); // TODO: 2-3% (Enhances chemical durability and mechanical strength)
    // 4% remaining
    fBorosilicate->AddMaterial(Na2O, 4. * perCent); // TODO: ~4% alkali oxides (Na2O, K20 - one or the other, or both)
    // borosilicate->AddMaterial(K2O, 0. * perCent); // TODO ^^
    // NOTE: Potassium may cause unwanted noise, not all PMT designs use this in their borosilicate (while some do intentionally)
    // NOTE: SCHOTT BK7, SCHOTT 8250, SCHOTT 8337
    
    // Photocathode material (alkali metal)
    fLi = nist->FindOrBuildMaterial("G4_Li"); // Lithium (TODO: KCsSb)
    // TODO: Bialkali ? Although the material kinda doesnt matter in this sim, moreso the surface
    
    // Table material (MDF wood proxy, using generalised wood chemical composition)
    fWood = new G4Material("WOOD", 0.8 * g/cm3, 6); // NOTE: Solid cellular material of wood ~1.5 g/cm3, but lowered by air spaces
    
    // auto N = new G4Element("Nitrogen", "N", 7, 14.007 * g/mole); // Z=7, A=14, density = ...
    // auto Ca = new G4Element("Calcium", "Ca", 20, 40.078 * g/mole); // Z=20, A=40, density = ...
    // auto K = new G4Element("Potassium", "K", 19, 39.098 * g/mole); // Z=19, A=39, density = ...
    
    G4Element* N = nist->FindOrBuildElement("N");
    G4Element* Ca = nist->FindOrBuildElement("Ca");
    G4Element* K = nist->FindOrBuildElement("K");
    
    fWood->AddElement(C, 50. * perCent);
    fWood->AddElement(O, 42. * perCent);
    fWood->AddElement(H, 6. * perCent);
    fWood->AddElement(N, 1. * perCent);
    fWood->AddElement(Ca, 0.5 * perCent); // NOTE: Simplified by making last 1% Ca/K
    fWood->AddElement(K, 0.5 * perCent); // in reality, would also be sodium, magnesium, iron, sulfur, chlorine, silicon, phosphorus
    // NOTE: Composition from wikipedia, cellular density from britannica, MDF density from dover chemical
    
}

/*
 * ...
 */
void DetectorMaterials::DefineOpticalProperties() {
    /////////////////
    // SCINTILLATION:
    /////////////////
    
    // Instantiate a new material properties table, to be assigned to the scintillator material
    auto MPTCrystal = new G4MaterialPropertiesTable();
    // NOTE: Need at least: refractive index, emission spectrum, absorption length, yield, decay time
    
    // Energy range of NaI:Tl emission spectrum
    // NOTE: Visible light ranges from ~400 nm (violet) to ~700 nm (red)
    // TODO: See EMI notes
    std::vector<G4double> const energy = {2.25425 * eV, 2.98756 * eV, 3.81488 * eV}; // (550 nm, 415 nm, 325 nm) // TODO: Add an intermediary wavelength between 550->415, and 415->325
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
    std::vector<G4double> const rindexNaI = {1.85, 1.85, 1.85}; // 1.85 @ emission max (415 nm)

    // Properties that depend on energy
    // NOTE: Vector lengths must be the same, 1st vector is energy, 2nd is property value at that energy
    //
    // Refractive index as a function of wavelength
    MPTCrystal->AddProperty("RINDEX", energy, rindexNaI);
    
    // The energy spectrum of the emitted scintillation photons
    // NOTE: This is essential to generate the correct number of photons (25156 for 662 keV, instead of 5-10)
    // std::vector<G4double> emission = {1., 1., 1.}; // same amount of photons for each wavelength
    // std::vector<G4double> emission = {0.1, 1., 0.1}; // emission max @ 415 nm (NOTE: No different to 0. upper/lower)
    std::vector<G4double> const emission = {0., 1., 0.}; // emission max @ 415 nm (NOTE: No different to 0.1 upper/lower)
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
    std::vector<G4double> const absorptionLengthNaI = {56.234 * cm, 100 * cm, 0.794 * cm}; // TEST: Establish mean distance travelled before detection at PC
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
    fNaI->SetMaterialPropertiesTable(MPTCrystal);
    
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
    fReflectorSurface = new G4OpticalSurface("ReflectorSurface", unified, groundbackpainted, dielectric_dielectric);
    // NOTE: Back painted implies the presence of a tiny gap between the two media, i.e.
    // an air gap for dry packed powders or teflon, or 
    // NOTE: Lambertian reflector attached to scintillator crystal surface
    
    // NOTE: Reflectors are typically diffuse (ground), as it promotes greater light collection
    
    // Specify surface roughness (For back painted surface)
    fReflectorSurface->SetSigmaAlpha(0); // No specular lobe constant, so ...
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
    std::vector<G4double> const reflectivityReflector = {0.98, 0.98, 0.98};
    // std::vector<G4double> reflectivityReflector = {0.99, 0.99, 0.99};
    // std::vector<G4double> reflectivityReflector = {1., 1., 1.};
    // std::vector<G4double> reflectivityReflector = {0.1, 0.1, 0.1};
    // std::vector<G4double> reflectivityReflector = {0., 0., 0.};
    // NOTE: Need to specify reflectivity != 1. else no absorption in reflector (perfect reflector, not realistic)
    // TEST: There is actually very little difference when removing reflectivity from reflector surface
    // i.e. when it defaults to 1, versus 0.96
    
    // Refractive index (back painted)
    // std::vector<G4double> rindexReflectorSurface = {1.78, 1.78, 1.78}; // Al2O3
    std::vector<G4double> const rindexReflectorSurface = {1., 1., 1.}; // Air (dry packed causes layer of air)
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
    fReflectorSurface->SetMaterialPropertiesTable(MPTReflectorSurf);
    
    //////////////////////////////
    // ALUMINA MATERIAL PROPERTIES
    //////////////////////////////
    
    // TEST: 4OpBoundaryProcessStatus::NoRINDEX is being flagged at "Reflector" volume
    // auto MPTReflector = new G4MaterialPropertiesTable();
    
    // std::vector<G4double> const rindexAlumina = {1.78, 1.78, 1.78}; // Al2O3
    // MPTReflector->AddProperty("RINDEX", energy, rindexAlumina); // NOTE: THIS DOES CHANGE SPECTRA EVER SO SLIGHTLY
    
    // NOTE: Dont add reflectivity to the material, just the surface
    
    // NOTE: By adding RINDEX to Al2O3, NoRINDEX now being flagged at "Hermetic Seal" & "Casing"
    // but they shouldnt be getting that far ...
    
    // TODO: Set this high (is infinite currently)
    // std::vector<G4double> absorptionLengthAlumina = {400 * cm, 400 * cm, 400 * cm};
    // MPTReflector->AddProperty("ABSLENGTH", energy, absorptionLengthAlumina);
    // NOTE: Dont need this... photons dont enter the medium
    
    // TEST
    // Al2O3->SetMaterialPropertiesTable(MPTReflector);
    
    // BUG: ADDING THIS TO THE ALUMINA MATERIAL CAUSES HIGH FREQUENCY SINGLE PHOTON DETECTED EVENTS
    // ground back painted surface with rindex air (for dry packed powder air gap) 
    // and reflectivity of alumina is sufficient to model the physics
    // NOTE: there doesnt need to be a rindex assigned to the material itself, 
    // since reflection is almost entirely diffuse, so snells law, fresnel eqs, etc
    // doesnt need to be strictly calculated to determine angle of reflection
    
    // TODO: REMOVE THIS SECTION ^^
    
    
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
    
    // TODO: THIS SECTION CAN BE REMOVED ^^
    // dielectric_metal interface doesnt need rindex of material, or absorption length
    // subsequent aluminium surface mpt is fine
    
    ///////////////////////////////////////////////
    // ENCLOSURE & HERMETIC SEAL SURFACE PROPERTIES
    ///////////////////////////////////////////////
    
    auto MPTAlSurface = new G4MaterialPropertiesTable();
    
    fAluminiumSurface = new G4OpticalSurface("Aluminium", unified, polished, dielectric_metal);
    
    std::vector<G4double> const reflectivityAl = {0.9, 0.9, 0.9};
    MPTAlSurface->AddProperty("REFLECTIVITY", energy, reflectivityAl);
    
    std::vector<G4double> rindexAl = {0.59062, 0.33593, 0.22053};                                // TODO: ADD RINDEX TO SURFACE (IS NEEDED FOR REFLECTION CALCS, BUT ON OTHER HAND CURRENTLY NO PHOTONS REACH ALUMINIUM)
    MPTAlSurface->AddProperty("RINDEX", energy, rindexAl);
    
    fAluminiumSurface->SetMaterialPropertiesTable(MPTAlSurface);
    
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
    fGreaseSurface = new G4OpticalSurface("GreaseSurface", unified, polished, dielectric_dielectric);
    // NOTE: This is default surface, can be omitted without any change to spectra
    
    // Refractive index of optical grease (1.46 @ 589.3 nm)
    std::vector<G4double> const rindexGrease = {1.46, 1.46, 1.46}; // TODO: Refractive index matching ...
    // TODO: Refine this across 300-550 nm emission range
    
    // High absorption length in the medium
    std::vector<G4double> const abslengthGrease = {420. * cm, 420. * cm, 420. * cm};
    
    // Assign the values to the MPT
    MPTGrease->AddProperty("RINDEX", energy, rindexGrease);
    MPTGrease->AddProperty("ABSLENGTH", energy, abslengthGrease);
    
    // Assign the refractive index and absorption length to the optical grease material
    fPDMS->SetMaterialPropertiesTable(MPTGrease);
    
    // TEST TEST TEST
    // ...
//     auto MPTGreaseSurface = new G4MaterialPropertiesTable();
// 
//     // Refractive index of optical grease (1.46 @ 589.3 nm)
//     std::vector<G4double> const rindexGrease = {1.46, 1.46, 1.46}; // TODO: Refractive index matching ...
//     // TODO: Refine this across 300-550 nm emission range
//     
//     // ...
//     MPTGreaseSurface->AddProperty("RINDEX", energy, rindexGrease);
//     
//     // Dielectric polished surface, allowing refraction
//     auto windowSurface = new G4OpticalSurface("WindowGreaseSurface", unified, polished, dielectric_dielectric);
//     
//     // ...
//     windowSurface->SetMaterialPropertiesTable(MPTGreaseSurface);
    // TEST TEST TEST

    
    /////////////////////////////////////
    // OPTICAL WINDOW MATERIAL PROPERTIES
    /////////////////////////////////////
    
    // PMT Glass MPT (Optical window)
    auto MPTWindow = new G4MaterialPropertiesTable();
    
    // Dielectric polished surface, allowing refraction
    fWindowSurface = new G4OpticalSurface("WindowSurface", unified, polished, dielectric_dielectric);
    // NOTE: This is default surface, can be omitted without any change to spectra
    
    // Refractive index of borosilicate glass (1.53024 @ 404.7 nm - SCHOTT BK7 Datasheet)
    std::vector<G4double> const rindexBorosilicate = {1.51872, 1.53024, 1.54272}; // NOTE: From SCOTT BK7 datasheet
    // NOTE: Closest indices for (550 nm, 415 nm, 325 nm) => (546.1 nm, 404.7 nm, 334.1 nm)
    
    // High absorption length in the medium
    std::vector<G4double> const abslengthBorosilicate = {420. * cm, 420. * cm, 420. * cm};
    
    // Assign the values to the MPT
    MPTWindow->AddProperty("RINDEX", energy, rindexBorosilicate);
    MPTWindow->AddProperty("ABSLENGTH", energy, abslengthBorosilicate);
    
    // Assign the refractive index and absorption length to the borosilicate glass material
    fBorosilicate->SetMaterialPropertiesTable(MPTWindow);
    
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
    fPhotocathodeSurface = new G4OpticalSurface("Photocathode", unified, ground, dielectric_metal);
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
    std::vector<G4double> const efficiencyScoring = {0.08, 0.27, 0.26}; // QE: 0.08 @550nm, ~0.27 @415nm, ~0.26 @325nm (KNOLL)
    // TODO: PC polished w/ just new R, QE
    
    // TODO: Real/imaginary rindices instead of reflectivity? Takes into account incident angle.
    
    // TODO: IQE vs EQE (this is EQE afaik)
    // TODO: Anti-reflective coating? So likely much less than 21% reflectivity?
    // TODO: Transmittance = 33% @500nm (Harmer et al [2012])
    
    // NOTE: Using same energy as other MPTS has no effect on output spectrum at all (vs "energyScoring")
    // MPTPhotocathode->AddProperty("REFLECTIVITY", energy, reflectivityScoring); // 1 minus the absorption coeffcient
    MPTPhotocathode->AddProperty("EFFICIENCY", energy, efficiencyScoring); // Chance of an absorbed photon to be detected
    
    
    // TEST TEST TEST TEST Motta (2004) - Optical properties of bialkali photocathodes
    std::vector<G4double> const realRindexScoring = {3.20, 2.38, 1.92}; // n (@550nm, @415nm, @325nm)
    std::vector<G4double> const imaginaryRinexScoring = {0.63, 1.71, 1.69}; // k
    // (@545nm, @410nm, @380nm) <- closest available datapoints (NOTE: 325 pretty far off 380, but bulk absorption high here)
    // maybe see if there is another dateset for confluence, or interpolate for a val closer to 325nm
    MPTPhotocathode->AddProperty("REALRINDEX", energy, realRindexScoring);
    MPTPhotocathode->AddProperty("IMAGINARYRINDEX", energy, imaginaryRinexScoring);
    // TEST TEST TEST TEST
    
    
    // ...
    fPhotocathodeSurface->SetMaterialPropertiesTable(MPTPhotocathode);
    
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
    std::vector<G4double> const rindexAir = {1., 1., 1.}; // MPT2->AddProperty("RINDEX", "Air") NOTE: Default available
    MPTAir->AddProperty("RINDEX", energy, rindexAir);
    // fAir->SetMaterialPropertiesTable(MPTAir);
    
    // TODO: Test removing this (to identify any air gaps)
}

/*
 * TODO: Not a detector material
 */
G4Material* DetectorMaterials::Air() const {
    return fAir;
};

/*
 * TODO: Not a detector material
 */
G4Material* DetectorMaterials::Wood() const {
    return fWood;
};

/*
 * ...
 */
G4Material* DetectorMaterials::NaI() const {
    return fNaI;
};

/*
 * ...
 */
G4Material* DetectorMaterials::Al2O3() const {
    return fAl2O3;
};

/*
 * ...
 */
G4Material* DetectorMaterials::Al() const {
    return fAl;
};

/*
 * ...
 */
G4Material* DetectorMaterials::PDMS() const {
    return fPDMS;
};

/*
 * ...
 */
G4Material* DetectorMaterials::Borosilicate() const {
    return fBorosilicate;
};

/*
 * ...
 */
G4Material* DetectorMaterials::Li() const {
    return fLi;
};

/*
 * ...
 */
G4OpticalSurface* DetectorMaterials::ReflectorSurface() const {
    return fReflectorSurface;
};

/*
 * ...
 */
G4OpticalSurface* DetectorMaterials::AluminiumSurface() const {
    return fAluminiumSurface;
};

/*
 * ...
 */
G4OpticalSurface* DetectorMaterials::GreaseSurface() const {
    return fGreaseSurface;
};

/*
 * ...
 */
G4OpticalSurface* DetectorMaterials::WindowSurface() const {
    return fWindowSurface;
};

/*
 * ...
 */
G4OpticalSurface* DetectorMaterials::PhotocathodeSurface() const {
    return fPhotocathodeSurface;
};

/*
 * ...
 */
G4Material* DetectorMaterials::GetReflectorMaterial(ReflectorMaterial const material) const {
    switch (material) {
        case ReflectorMaterial::Al2O3:
            return fAl2O3;
        default:
            G4cerr << "Error: Unrecognised reflector material." << G4endl;
            return nullptr;
    }
};

/*
 * ...
 */
G4Material* DetectorMaterials::GetEnclosureMaterial(EnclosureMaterial  const material) const {
    switch (material) {
        case EnclosureMaterial::Aluminium:
            return fAl;
        case EnclosureMaterial::StainlessSteel:
            return G4NistManager::Instance()->FindOrBuildMaterial("G4_STAINLESS-STEEL"); // TODO: Temporary instantiation in here, remove
        default:
            G4cerr << "Error: Unrecognised enclosure material." << G4endl;
            return nullptr;
    }
};
