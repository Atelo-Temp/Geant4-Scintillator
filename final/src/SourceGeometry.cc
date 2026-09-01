// User lib
#include "SourceGeometry.hh"
#include "SourceMaterials.hh"

// G4 lib
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4PVPlacement.hh"

/*
 * ...
 */
SourceGeometry::SourceGeometry(SourceMaterials& sourceMaterials) : fSourceMaterials(sourceMaterials) {}

/*
 * ...
 */
G4VPhysicalVolume* SourceGeometry::BuildSource(
    G4LogicalVolume* worldLog,
    G4double const tableTopY,
    G4double const detectorFaceZ,
    G4double const detectorX,
    G4double const fSourceDetectorDistance,
    bool const fCheckOverlaps
) {
    //////////
    // SOURCE:
    //////////
    
    // Source geometry specification
    // G4double const sourceRadius = 0.5 * cm; // 1cm diameter
    // G4double const sourceThickness = 0.1 * cm; // 1mm thickness
    // G4double const sourceRadius = (2.54 * 0.5) * cm; // 1" -> 2.54cm diameter -> 1.27cm radius (typical of disk sources)
    // NOTE: Lab sources must have smaller diameters as this wouldnt fit casing (they are made in house tbf)
    // G4double const sourceRadius = (1.905 * 0.5) * cm; // 1" -> 2.54cm diameter -> 1.27cm radius (typical of disk sources)
    // G4double const sourceThickness = 0.3175 * cm; // 0.125" -> 3.175mm thickness (typical of disk sources)
    G4double const activeRadius = 0.15 * cm; // 3mm diameter
    G4double const activeThickness = 0.01 * cm; // 0.1mm thickness
    
    // Source geometry definition (modelled as cylinder)
    auto sourceActive = new G4Tubs(
        "SourceActive", // name
        0., // inner radius (0 = not hollow)
        activeRadius, // outer radius
        activeThickness * 0.5, // thickness
        0. * deg, // start angle
        360 * deg // end angle (full span here)
    );
    
    // ...
    // G4Material* sourceMat = fSourceMaterials.Create137Cs(); // TODO: This needs to be dependent on the macro command
    G4Material* sourceMat = fSourceMaterials.Polystyrene();
    // TODO: Maybe an if clause to check if material already made inside of Create137Cs
    // this is both a creator and getter, unlike other material methods
    
    // Define the radioactive source with the created material
    auto sourceActiveLog = new G4LogicalVolume(sourceActive, sourceMat, "SourceActive");
    
    
    ///////////////////
    // SOURCE RETAINER:
    ///////////////////
    
    // ...
    G4double const retainerInnerRadius = 0.25 * cm; // 5mm inner diameter
    G4double const retainerOuterRadius = 0.35 * cm; // 7mm outer diameter
    G4double const retainerThickness = 0.3 * cm; // 3mm thickness
    
    // ...
    auto sourceRetainer = new G4Tubs(
        "SourceRetainer", // name
        retainerInnerRadius, // inner radius (hollow)
        retainerOuterRadius, // outer radius
        retainerThickness * 0.5, // thickness
        0. * deg, // start angle
        360 * deg // end angle (full span here)
    );
    
    // ..
    G4Material* Al = fSourceMaterials.Aluminium();
    
    // ...
    auto sourceRetainerLog = new G4LogicalVolume(sourceRetainer, Al, "SourceRetainer");
    

    /////////////////
    // SOURCE WINDOW:
    /////////////////
    
    // ...
    G4double const retainerWindowRadius = retainerInnerRadius; // 5mm diameter
    G4double const retainerWindowThickness = 0.0125 * cm; // 0.125mm or 125um thickness
    
    // ...
    auto sourceRetainerWindow = new G4Tubs(
        "SourceRetainerWindow", // name
        0., // inner radius (0 = not hollow)
        retainerWindowRadius, // outer radius
        retainerWindowThickness * 0.5, // thickness
        0. * deg, // start angle
        360 * deg // end angle (full span here)
    );
    
    // Polyethylene terephthalate (PET) aka Mylar
    G4Material* mylar = fSourceMaterials.Mylar();
    
    // ...
    auto sourceRetainerWindowLog = new G4LogicalVolume(sourceRetainerWindow, mylar, "SourceRetainerWindow");
    
    
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
    
    // ..
    auto casingCut = new G4Box("SourceCasingCut", casingCutSizeX * 0.5, casingCutSizeY * 0.5, casingCutSizeZ * 0.5);
    
    // Create new solid with cut subtracted from base
    auto sourceCasing = new G4SubtractionSolid(
        "SourceCasing", // name
        casingBase, // the solid to subtract from
        casingCut, // the volume to subtract
        nullptr, // no rotation
        G4ThreeVector(0., 0., 0.) // cut translation (relative to base solid, not world)
    );
    
    // Acrylic
    G4Material* PMMA = fSourceMaterials.PMMA();
    
    // Assign a material to the casing solid
    auto casingLog = new G4LogicalVolume(
        sourceCasing, // subtraction solid acts same as any other geometry here
        PMMA, // casing material (acrylic - PMMA) (was opaque)
        "SourceCasing"
    );
    
    /////////////////////////
    // SOURCE CASING WINDOWS:
    /////////////////////////
    
    // Two of these will be placed, one on the front of the casing and one on the back
    G4double const casingWindowSizeZ = 0.15 * cm;
    
    // ...
    auto sourceCasingWindow = new G4Box("SourceCasingWindow", casingSizeX * 0.5, casingSizeY * 0.5, casingWindowSizeZ * 0.5);
    
    // ...
    auto casingWindowLog = new G4LogicalVolume(
        sourceCasingWindow,
        PMMA, // casing material (acrylic - PMMA) (was transparent)
        "SourceCasingWindow"
    );
    
    
    /////////////////
    // SOURCE HOLDER:
    /////////////////
    
    // Base geometry which will be cut
    G4double const holderSizeX = 4. * cm; // 4cm wide
    G4double const holderSizeY = 2. * cm; // 2cm high
    G4double const holderSizeZ = 1.95 * cm; // 1.95cm thick
    
    // ...
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
    
    // ...
    auto holderCut2 = new G4Box("SourceHolderCut2", cut2SizeX * 0.5, cut2SizeY * 0.5, cut2SizeZ * 0.5);
    
    // Translation for 2nd cut relative to base:
    // 
    
    // G4double const holderCut2TransY = 0.75 * holderSizeY;
    G4double const holderCut2TransY = (0.5 * holderSizeY) - ((0.5 * cut2SizeY) - (0.1 * cm));
    auto const holderCut2Trans = G4ThreeVector(0., holderCut2TransY, 0.);
    
    // Make the 2nd cut
    auto sourceHolder = new G4SubtractionSolid(
        "SourceHolder",
        holderSubtracted, // base solid to make cut in
        holderCut2, // cut to make in base
        nullptr,
        holderCut2Trans
    );
    
    // ...
    G4Material* PLA = fSourceMaterials.PLA();
    // the source holder looked 3D printed (and distinctly different from source casing and windows which were opaque and transparent acrylic respectively), 
    // likely PLA or ABS (going for PLA as it just the more commonly used material)
    
    // Logical volume
    auto holderLog = new G4LogicalVolume(
        sourceHolder,
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
    
    // ...
    G4double const holderY = tableTopY + (0.5 * holderSizeY);
    
    // ...
    auto const holderTrans = G4ThreeVector(detectorX, holderY, sourceZ);
    
    // ...
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
    
    // Place the radioactive source
    G4VPhysicalVolume* sourcePhys = new G4PVPlacement(
        nullptr,
        sourceTrans,
        sourceActiveLog,
        "SourceActive",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    ////
    // Source Retainer
    ////
    
    // ...
    G4VPhysicalVolume* sourceRetainerPhys = new G4PVPlacement(
        nullptr,
        sourceTrans,
        sourceRetainerLog,
        "SourceRetainer",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    ////
    // Source Retainer Windows
    ////
    
    // ...
    G4double const frontRetainerWindowZ = sourceZ + (0.5 * activeThickness) + (0.5 * retainerWindowThickness);
    auto const frontRetainerWindowTrans = G4ThreeVector(detectorX, casingTransY, frontRetainerWindowZ);
    
    // ...
    G4VPhysicalVolume* frontRetainerWindowPhys = new G4PVPlacement(
        nullptr,
        frontRetainerWindowTrans,
        sourceRetainerWindowLog,
        "FrontRetainerWindow",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    // ...
    G4double const backRetainerWindowZ = sourceZ - (0.5 * activeThickness) - (0.5 * retainerWindowThickness);
    auto const backRetainerWindowTrans = G4ThreeVector(detectorX, casingTransY, backRetainerWindowZ);
    
    // ...
    G4VPhysicalVolume* backRetainerWindowPhys = new G4PVPlacement(
        nullptr,
        backRetainerWindowTrans,
        sourceRetainerWindowLog,
        "BackRetainerWindow",
        worldLog,
        false,
        0,
        fCheckOverlaps
    );
    
    ////
    // Casing Windows
    ////
    
    // ...
    G4double const frontCasingWindowZ = sourceZ + (0.5 * casingSizeZ) + (0.5 * casingWindowSizeZ);
    auto const frontCasingWindowTrans = G4ThreeVector(detectorX, casingTransY, frontCasingWindowZ);
    
    // ...
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
    
    // ...
    G4double const backCasingWindowZ = sourceZ - (0.5 * casingSizeZ) - (0.5 * casingWindowSizeZ);
    auto const backCasingWindowTrans = G4ThreeVector(detectorX, casingTransY, backCasingWindowZ);
    
    // ...
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
    
    // fSourceVolume = sourceLog; // NOTE: TEMP UNTIL PLACING PHYSICAL VOLUME
    // fSourceCoords = sourceTrans; // NOTE: TEMP UNTIL PLACING PHYSICAL VOLUME
    // fSourceVolume = sourcePhys;
    
    return sourcePhys;
}
