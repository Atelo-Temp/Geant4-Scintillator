//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Box.hh"
// #include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
// #include "G4Trd.hh"

namespace B1 {

    G4VPhysicalVolume* DetectorConstruction::Construct() {
        // Get nist material manager
        G4NistManager* nist = G4NistManager::Instance();

        // Envelope parameters
        //
        G4double env_sizeXY = 100 * cm, env_sizeZ = 150 * cm;
        G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");

        // Option to switch on/off checking of volumes overlaps
        //
        G4bool checkOverlaps = true;

        /////////
        // World:
        /////////

        G4double world_sizeXY = 1.2 * env_sizeXY;
        G4double world_sizeZ = 1.2 * env_sizeZ;
        G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

        auto solidWorld =
        new G4Box("World",  // its name
                    0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);  // its size

        auto logicWorld = new G4LogicalVolume(solidWorld,  // its solid
                                            world_mat,  // its material
                                            "World");  // its name

        auto physWorld = new G4PVPlacement(nullptr,  // no rotation
                                        G4ThreeVector(),  // at (0,0,0)
                                        logicWorld,  // its logical volume
                                        "World",  // its name
                                        nullptr,  // its mother  volume
                                        false,  // no boolean operation
                                        0,  // copy number
                                        checkOverlaps);  // overlaps checking

        ////////////
        // Envelope:
        ////////////

        auto solidEnv = new G4Box("Envelope",  // its name
                                0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);  // its size

        auto logicEnv = new G4LogicalVolume(solidEnv,  // its solid
                                            env_mat,  // its material
                                            "Envelope");  // its name

        new G4PVPlacement(nullptr,  // no rotation
                        G4ThreeVector(),  // at (0,0,0)
                        logicEnv,  // its logical volume
                        "Envelope",  // its name
                        logicWorld,  // its mother  volume
                        false,  // no boolean operation
                        0,  // copy number
                        checkOverlaps);  // overlaps checking


        ////////////
        // TABLETOP:
        ////////////

        // TODO: Add wood? tabletop under the scintillator crystal


        ////////////////////////
        // SCINTILLATOR CRYSTAL:
        ////////////////////////

        G4Material* sctintillation_mat = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
        G4ThreeVector crystalPos = G4ThreeVector(0, -1 * cm, 7 * cm);

        // Define the cylinder dimensions (radii, height, span)
        G4double innerRadiusCrystal = 0.*cm; // prevents it being hollow, a non-zero and less than outer radius would have centre hole
        G4double outerRadiusCrystal = 7.62*cm; // what is the "*" suffix ??
        G4double hzCrystal = 3.81*cm;
        G4double startAngle = 0.*deg;
        G4double spanningAngle = 360.*deg; // Full circumference cylinder (i.e. 0 - 180 give half cylider)

        // Create a cylinder named "Scintillator", using the G4Tubs class
        G4Tubs* solidScintillator = new G4Tubs(
            "Scintillator",
            innerRadiusCrystal,
            outerRadiusCrystal,
            hzCrystal,
            startAngle,
            spanningAngle
        );
        // NOTE: Cylinder has radius of 7.62 cm (3 inches) and length 7.62 cm (the "hz" param is also a half length)

        // ...
        auto logicScintillator = new G4LogicalVolume(
            solidScintillator,  // its solid
            sctintillation_mat,  // its material
            "Scintillator" // its name
        );

        // ...
        new G4PVPlacement(
            nullptr,  // no rotation
            crystalPos,  // at position
            logicScintillator,  // its logical volume
            "Scintillator",  // its name
            logicEnv,  // its mother  volume
            false,  // no boolean operation
            0,  // copy number
            checkOverlaps // overlaps checking
        );

        // Set Scintillator Crystal as scoring volume
        //
        fScoringVolume = logicScintillator;


        /////////////////
        // ALUMINIUM CAN:
        /////////////////

        G4Material* can_mat = nist->FindOrBuildMaterial("G4_Al");

        // Define the cylinder dimensions (radii, height, span)
        G4double innerRadiusCan = 7.62*cm; // same as outer radius of crystal
        G4double outerRadiusCan = 7.72*cm; // 1mm thickness
        G4double hzCan = 3.81*cm; // THIS NEEDS TO ACTUALLY BE SLIGHTLY LONGER DUE TO THE WINDOW
        // G4double startAngle = 0.*deg;
        // G4double spanningAngle = 360.*deg; // Same as crystal

        // Create a cylinder named "Scintillator", using the G4Tubs class
        G4Tubs* solidCan = new G4Tubs(
            "Can",
            innerRadiusCan, // this could be "outerRadiusCrystal" also
            outerRadiusCan,
            hzCan,
            startAngle,
            spanningAngle
        );
        // NOTE: Can has hole with radius of 7.62 cm (3 inches) and length 7.62 cm (the "hz" param is also a half length)

        // ...
        auto logicCan = new G4LogicalVolume(
            solidCan,  // its solid
            can_mat,  // its material
            "Can" // its name
        );

        // ...
        new G4PVPlacement(
            nullptr,  // no rotation
            crystalPos,  // at position
            logicCan,  // its logical volume
            "Can",  // its name
            logicEnv,  // its mother  volume
            false,  // no boolean operation
            0,  // copy number
            checkOverlaps // overlaps checking
        );


        //////////////////
        // OPTICAL WINDOW:
        //////////////////

        // TODO ...

        // NOTE: Before i go making the geometry realistic, probably best to get the crystal working as intended


        //
        // always return the physical World
        //
        return physWorld;
    }
}
