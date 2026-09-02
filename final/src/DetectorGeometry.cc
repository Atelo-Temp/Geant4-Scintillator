// User lib
#include "DetectorGeometry.hh"
#include "DetectorMaterials.hh"

// G4 lib
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"

/*
 * ...
 */
DetectorGeometry::DetectorGeometry(DetectorMaterials& detectorMaterials) : fDetectorMaterials(detectorMaterials) {}

/*
 * ...
 */
DetectorBuild DetectorGeometry::BuildDetector(
    G4LogicalVolume* worldLog,
    G4double const tableTopY,
    G4double const fCrystalDiameter,
    bool const fCheckOverlaps
) {
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
    G4Material* NaI = fDetectorMaterials.NaI();
    
    // Define the scintillator crystal material as sodium iodide
    auto scintillatorLog = new G4LogicalVolume(scintillator, NaI, "Scintillator");
    
    // TEST: Create a region for the crystal (for 100 um cuts only in detector volume)
    auto crystalRegion = new G4Region("Scintillator"); // NOTE: Havent imported this ?
    scintillatorLog->SetRegion(crystalRegion);
    crystalRegion->AddRootLogicalVolume(scintillatorLog);
    // TODO: Is this redundant?
    
    
    ////////////////////
    // RADIAL REFLECTOR:
    ////////////////////
    
    // The scintillation photons are emitted in all directions, so a high efficiency reflector
    // is used to surround the crystal (Al_{2}O_{3} and teflon), on all sides except the back,
    // to increase the amount of photons reaching the the photocathode
    
    // Inner rad can (4.04495 cm) - outer rad crystal (3.81 cm) => 0.23495 cm reflector thickness
    G4double const reflectorRadialThickness = 0.23495 * cm;
    G4double const reflectorRadialOuterRad = crystalOuterRad + reflectorRadialThickness;
    // NOTE: Same as crystal height, with reflector thickness added to both ends
    
    // Base volume which will be cut
    auto reflectorSolidRadial = new G4Tubs(
        "ReflectorSolidRadial",
        crystalOuterRad, // inner rad (hollow)
        reflectorRadialOuterRad, // outer rad
        crystalHeight * 0.5, // height 
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // ...
    G4Material* Al2O3 = fDetectorMaterials.Al2O3();
    
    // Assign a material to the reflector solid
    auto reflectorRadialLog = new G4LogicalVolume(
        reflectorSolidRadial,
        Al2O3, // reflector material
        "ReflectorRadial"
    );
    
    // NOTE: This current setup works, but leaves an overhang of reflector,
    // this isnt really an issue and maybe worth keeping,
    // but may also want reflector to stop in line with back of crystal,
    // just need to give "ReflectorSolid" (crystalheight + 0.5 * reflectorThick),
    // but it also kinda needs to 
    
    
    ///////////////////
    // AXIAL REFLECTOR:
    ///////////////////
    
    // G4double const reflectorHeight = crystalHeight + (reflectorThickness * 2);
    // G4double const reflectorAxialOuterRad = crystalOuterRad + reflectorThickness;
    G4double const reflectorAxialThickness = 0.23495 * cm;
    
    // ...
    auto reflectorSolidAxial = new G4Tubs(
        "ReflectorSolidAxial",
        0., // inner rad (no hole)
        reflectorRadialOuterRad, // outer rad
        reflectorAxialThickness * 0.5, // height 
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // ...
    auto reflectorAxialLog = new G4LogicalVolume(
        reflectorSolidAxial,
        Al2O3, // reflector material
        "ReflectorAxial"
    );

    ////////////
    // NEOPRENE:
    ////////////
    
    // TODO
    
    
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
    G4Material* PDMS = fDetectorMaterials.PDMS();
    
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
    G4Material* borosilicate = fDetectorMaterials.Borosilicate();
    
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
    G4Material* Li = fDetectorMaterials.Li(); // TODO: KCsSb by atoms (not that it really matters due to surface interaction)
    
    // ...
    auto photocathodeLog = new G4LogicalVolume(photocathode, Li, "Photocathode");
    
    
    ///////////////////////
    // ALUMINIUM ENCLOSURE:
    ///////////////////////
        
//     // Enclosure is 3.225' outer diameter according to ortec spec, and 0.2' thickness (0.508mm)
//     G4double const enclosureThick = 0.0508 * cm; // NOTE: 0.02' => 0.508 mm (added to both sides in Z direction)
//     // G4double const enclosureThick = 0.08 * cm; // 0.8mm thickness used w/ stainless steel enclosure from: scionix.nl/standard/
//     
//     // Derive outer radius and length from reflector radius & length plus enclosure thickness
//     G4double const enclosureOuterRad = reflectorRadialOuterRad + enclosureThick; // NOTE: 3.225' dia => 8.1915cm dia => 4.09575 cm outer rad
//     // G4double const enclosureLength = reflectorHeight + (enclosureThick * 2); // NOTE: Same height as reflector, with thickness added to both ends
//     G4double const enclosureLength = crystalHeight + (reflectorRadialThickness * 2) + (enclosureThick * 2); // NOTE: Same height as reflector, with thickness added to both ends
//     // NOTE: 4.04495 cm inner rad
//     
//     // Base volume which will be cut
//     auto enclosureSolid = new G4Tubs(
//         "EnclosureSolid",
//         0. * cm, // inner rad (no hole, as cut will handle it in this case)
//         enclosureOuterRad, // outer rad
//         enclosureLength * 0.5, // height
//         startAngle, // 0 deg
//         endAngle // 360 deg (full span)
//     );
//     
//     // The section to cut from the base volume
//     auto enclosureCut = new G4Tubs(
//         "EnclosureCut",
//         0. * cm, // inner rad (no hole, solid cut)
//         reflectorRadialOuterRad, // outer rad (cut a section with same rad as reflector)
//         enclosureLength * 0.5, // height (cut has same height as base)
//         startAngle, // 0 deg
//         endAngle // 360 deg (full span)
//     );
//     // NOTE: To be a "perfect cut", could do +0.5*thickness & translate by same amount too,
//     // currently after translation part of this solid is cutting nothing, but tbh thats fine,
//     // i think this approach is more readable
//     
//     // Create new solid with cut subtracted from base
//     auto enclosure = new G4SubtractionSolid(
//         "Enclosure", // name
//         enclosureSolid, // the solid to subtract from
//         enclosureCut, // the volume to subtract
//         nullptr, // no rotation
//         G4ThreeVector(0., 0., enclosureThick) // cut translation (relative to base solid, not world)
//     );
//     
//     // ...
//     G4Material* Al = fDetectorMaterials.Al();
//     
//     // Assign a material to the enclosure solid
//     auto enclosureLog = new G4LogicalVolume(
//         enclosure, // subtraction solid acts same as any other geometry here
//         Al, // enclosure material (aluminium)
//         "Enclosure"
//     );
    
    //////////////////////////////
    // RADIAL ALUMINIUM ENCLOSURE:
    //////////////////////////////
    
    // Enclosure is 3.225' outer diameter according to ortec spec, and 0.2' thickness (0.508mm)
    G4double const enclosureThick = 0.0508 * cm; // NOTE: 0.02' => 0.508 mm (added to both sides in Z direction)
    // G4double const enclosureThick = 0.08 * cm; // 0.8mm thickness used w/ stainless steel enclosure from: scionix.nl/standard/
    
    // Derive outer radius and length from reflector radius & length plus enclosure thickness
    G4double const enclosureOuterRad = reflectorRadialOuterRad + enclosureThick; // NOTE: 3.225' dia => 8.1915cm dia => 4.09575 cm outer rad
    // G4double const enclosureLength = reflectorHeight + (enclosureThick * 2); // NOTE: Same height as reflector, with thickness added to both ends
    // G4double const enclosureLength = crystalHeight + (reflectorRadialThickness * 2) + (enclosureThick * 2); // NOTE: Same height as reflector, with thickness added to both ends
    G4double const enclosureLength = crystalHeight + reflectorAxialThickness; // TODO: + neoprene thickness ??
    
    // ...
    auto enclosureRadialSolid = new G4Tubs(
        "EnclosureRadialSolid",
        reflectorRadialOuterRad, // inner rad (hollow)
        enclosureOuterRad, // outer rad
        enclosureLength * 0.5, // height
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // ...
    G4Material* Al = fDetectorMaterials.Al();
    
    // Assign a material to the enclosure solid
    auto enclosureRadialLog = new G4LogicalVolume(
        enclosureRadialSolid, // subtraction solid acts same as any other geometry here
        Al, // enclosure material (aluminium)
        "EnclosureRadial"
    );
    
    /////////////////////////////
    // AXIAL ALUMINIUM ENCLOSURE:
    /////////////////////////////
    
    // ...
    // ...
    auto enclosureAxialSolid = new G4Tubs(
        "EnclosureAxialSolid",
        0., // inner rad (no hole)
        enclosureOuterRad, // outer rad
        enclosureThick * 0.5, // height
        startAngle, // 0 deg
        endAngle // 360 deg (full span)
    );
    
    // Assign a material to the enclosure solid
    auto enclosureAxialLog = new G4LogicalVolume(
        enclosureAxialSolid, // subtraction solid acts same as any other geometry here
        Al, // enclosure material (aluminium)
        "EnclosureAxial"
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
    G4double const sealLength = greaseThickness + windowThick;
    G4double const sealOuterRad = reflectorRadialOuterRad;
    
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
        seal,
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
    
    // Place the reflector radial volume
    G4VPhysicalVolume* reflectorRadialPhys = new G4PVPlacement(
        nullptr, // no rotation
        detectorOrigin, // same position as crystal
        reflectorRadialLog, // logical volume
        "ReflectorRadial", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        fCheckOverlaps
    );
    
    // ...
    // ...
    
    // ...
    G4double const axialReflectorZ = detectorZ - (0.5 * crystalHeight) - (0.5 * reflectorAxialThickness);
    auto const axialReflectorOrigin = G4ThreeVector(detectorX, detectorY, axialReflectorZ);
    
    // Place the reflector axial volume
    G4VPhysicalVolume* reflectorAxialPhys = new G4PVPlacement(
        nullptr, // no rotation
        axialReflectorOrigin, // same position as crystal
        reflectorAxialLog, // logical volume
        "ReflectorAxial", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        fCheckOverlaps
    );
    
    // ...
    // ...
    
    // ...
    G4double const radialEnclosureZ = detectorZ - (0.5 * reflectorAxialThickness);
    auto const radialEnclosureOrigin = G4ThreeVector(detectorX, detectorY, radialEnclosureZ);
    // NOTE: Since the length of the radial enclosure is that of the crystal + reflector thickness,
    // to centre it on the midpoint of the two components, it needs to be placed on the crystal origin
    // then shifted in the negative Z direction by half the reflector thickness
    
    // Place the enclosure
    G4VPhysicalVolume* enclosureRadialPhys = new G4PVPlacement(
        nullptr, // no rotation
        radialEnclosureOrigin, // same position as crystal
        enclosureRadialLog, // logical volume
        "EnclosureRadial", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        fCheckOverlaps
    );
    
    // ...
    // ...
    
    // ...
    G4double const axialEnclosureZ = axialReflectorZ - (0.5 * reflectorAxialThickness) - (0.5 * enclosureThick);
    auto const axialEnclosureOrigin = G4ThreeVector(detectorX, detectorY, axialEnclosureZ);
    
    // ...
    G4VPhysicalVolume* enclosureAxialPhys = new G4PVPlacement(
        nullptr, // no rotation
        axialEnclosureOrigin, // same position as crystal
        enclosureAxialLog, // logical volume
        "EnclosureAxial", // name
        worldLog, // mother volume (logical)
        false, // no boolean ops
        0, // one copy
        fCheckOverlaps
    );
    
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
    // G4double const sealZ = detectorZ + (crystalHeight * 0.5) + reflectorThickness + (sealLength * 0.5); // TEST
    G4double const sealZ = detectorZ + (crystalHeight * 0.5) + (sealLength * 0.5);
    
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
    // DefineOpticalInterfaces(crystalPhys, reflectorRadialPhys, reflectorAxialPhys, enclosureRadialPhys, enclosureAxialPhys, greasePhys, windowPhys, photocathodePhys, sealPhys);
    DefineOpticalInterfaces(crystalPhys, reflectorRadialPhys, reflectorAxialPhys, greasePhys, windowPhys, sealPhys, photocathodePhys);
    
    // ...
    G4double detectorFaceZ = detectorZ - ((crystalHeight * 0.5) + reflectorAxialThickness + enclosureThick);
    
    // ...
    return { detectorFaceZ, detectorX };
}

/*
 * ....
 */
void DetectorGeometry::DefineOpticalInterfaces(
    G4VPhysicalVolume* crystalPhys,
    G4VPhysicalVolume* reflectorRadialPhys,
    G4VPhysicalVolume* reflectorAxialPhys,
    // G4VPhysicalVolume* enclosureRadialPhys,
    // G4VPhysicalVolume* enclosureAxialPhys,
    G4VPhysicalVolume* greasePhys,
    G4VPhysicalVolume* windowPhys,
    G4VPhysicalVolume* sealPhys,
    G4VPhysicalVolume* photocathodePhys
) {
    // Define the border between the crystal and the reflector
    auto crystalReflectorRadialBorder = new G4LogicalBorderSurface("CrystalToRadialReflector", crystalPhys, reflectorRadialPhys, fDetectorMaterials.ReflectorSurface());
    auto crystalReflectorAxialBorder = new G4LogicalBorderSurface("CrystalToAxialReflector", crystalPhys, reflectorAxialPhys, fDetectorMaterials.ReflectorSurface());

    // NOTE: Crystal->Grease, and Grease->Window surfaces are not explicitly needed when rindex of each is passed
    // will default to: GLISUR, polished, dielectric_dielectric
    // ^ maybe do it just to explicitly pick UNIFIED
    auto crystalGreaseBorder = new G4LogicalBorderSurface("CrystalToGrease", crystalPhys, greasePhys, fDetectorMaterials.GreaseSurface());
    auto greaseWindowBorder = new G4LogicalBorderSurface("GreaseToWindow", greasePhys, windowPhys, fDetectorMaterials.WindowSurface());
    // NOTE: After testing, adding these two borders produces an identical spectrum to just 
    // leaving these two borders as default
    
    
    // TEST
    // auto crystalWindowBorder = new G4LogicalBorderSurface("CrystalToWindow", crystalPhys, windowPhys, fDetectorMaterials.greaseSurface());
    // auto crystalWindowBorder = new G4LogicalBorderSurface("CrystalToWindow", crystalPhys, windowPhys, fDetectorMaterials.windowSurface());
    // TEST
    
    
    // TEST \/\/\/\/
    // Define the border between the reflector and enclosure
    // auto reflectorEnclosureBorder = new G4LogicalBorderSurface("ReflectorToEnclosure", reflectorPhys, enclosurePhys, fDetectorMaterials.aluminiumSurface());
    //
    // Define the border between the reflector and hermetic seal
    // auto reflectorSealBorder = new G4LogicalBorderSurface("ReflectorToSeal", reflectorPhys, sealPhys, fDetectorMaterials.aluminiumSurface());
    // TEST ^^^^^^^
    
    // NOTE: ^^^ These are not needed, as groundbackpainted prevents refraction
    
    // TODO: Border back from: (enclosurePhys -> reflectorPhys) & (sealPhys - reflectorPhys)
    // auto enclosureReflectorBorder = new G4LogicalBorderSurface("EnclosureToReflector", enclosurePhys, reflectorPhys, fDetectorMaterials.reflectorSurface());
    // auto enclosureSealBorder = new G4LogicalBorderSurface("SealToReflector", sealPhys, reflectorPhys, fDetectorMaterials.reflectorSurface());
    // TODO: Border back from reflectorPhys->crystalPhys
    // auto reflectorCrystalBorder = new G4LogicalBorderSurface("ReflectorToCrystal", reflectorPhys, crystalPhys, fDetectorMaterials.reflectorSurface());
    
    // NOTE: ^^^ I DONT THINK THESE ARE ACTUALLY NEEDED (borders work both ways as is)
    
    // Define the border between the optical grease and the reflector
    // auto greaseReflectorBorder = new G4LogicalBorderSurface("GreaseToReflector", greasePhys, reflectorRadialPhys, fDetectorMaterials.ReflectorSurface()); // TEST (FIXES LOST PHOTONS)
    auto greaseSealBorder = new G4LogicalBorderSurface("GreaseToSeal", greasePhys, sealPhys, fDetectorMaterials.AluminiumSurface()); // TEST (FIXES LOST PHOTONS)
    // TODO: Is the crystal->reflector surface sufficient here? Not really as the sigma alpha value is for crystal surface, and uses rindex air for gap
    
    // ...
    // auto windowReflectorBorder = new G4LogicalBorderSurface("WindowToReflector", windowPhys, reflectorRadialPhys, fDetectorMaterials.ReflectorSurface()); // TEST (for 25 um grease geom)
    // Define the border between the optical window and the hermetic seal
    auto windowSealBorder = new G4LogicalBorderSurface("WindowToSeal", windowPhys, sealPhys, fDetectorMaterials.AluminiumSurface()); // NOTE: UNCOMMENT ME
    
    // Define the border between the optical window and the photocathode
    auto windowPhotocathodeBorder = new G4LogicalBorderSurface("WindowToPhotocathode", windowPhys, photocathodePhys, fDetectorMaterials.PhotocathodeSurface());
}
