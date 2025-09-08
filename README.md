# About

Simple projects acting as precursors to provided examples such as B1, and beyond.

## Project Ordering

- 1) Visualise

Minimal main() function to open the visualiser.

> NOTE: The "DetectorConstruction", "ActionInitialization", and "PhysicsList" classes, are mandatory in practice to see anything on the visualiser

- 2) Template

Defining an empty world (filled with air) with no nested geometry, a minimalist electromagnetic (EM) physics list, and a particle generator (particle gun, 1 MeV positron).

Assignment of the aforementioned classes to the "G4RunManager" will allow the visualiser to display the world, and observe the particle being shot in an event.

> NOTE: The main() function is not as comprehensive as subsequent "analysis", "decay", "scintillator" projects.

- 3) Geometry

Same as template, with a nested cylindrical tracker volume.

> NOTE: The main() function is not as comprehensive as subsequent "analysis", "decay", "scintillator" projects.

- 4) Sensitive

Implements a scoring region to track energy deposited in a medium, for each step, and the total for the event.

The positron from previous examples is changed to a 662 keV gamma-ray photon.

> NOTE: The main() function is not as comprehensive as subsequent "analysis", "decay", "scintillator" projects.

- 5) Analysis

Implements histogramming and nTuples, building on the sensitive detector functionality.

Features updates to main() function, to allow command line swap between batch processing and interactive mode.

- 6) Decay

Implements radioactive decay for a chosen isotope, replacing the monoenergetic particle shot by the particle gun in previous projects.

Particle is modelled as a point source, embedded within a spherical source geometry (which has been given the appropriate material).

Features addition to main() function, to allow longer lived (>1y) isotopes to decay.

> NOTE: May also wish to model the particle as a volume source, assigned to the spherical geometry.

- 7) Scintillator

Implements scintillation light (optical photons), in response to energy deposition in a scintillator crystal medium.

> NOTE: Reverts back to a simple 662 keV gamma shot from the particle gun, to keep the focus on the scintillation process.

- ...

## Other

- Starter: Just following the "Getting Started" section of the documentation, and making notes on library functionality (this will not run)

- Dose: Is just example B1 with slightly altered geometry (intending to explore dosimetry more at a later date)

## Final Project

- Spectroscopy: Brings all of the former concepts together to simulate gamma-ray spectroscopy using a scintillator detector (TODO: May leave this as a more comprehensive version of decay, with no optical photon generation)
^may rename as just "detector"

TODO: follow on project from scintillator, introducing histogramming, decay, and full geometry
