# About

Simple projects acting as precursors to provided examples such as B1, and beyond.

Centred around developing a simulated scintillator detector.

> NOTE: Some of the earlier projects are left in an intentionally underdeveloped state to act as a development progression log

## Project Ordering

- 1) Visualise

Minimal main() function to open the visualiser.

> NOTE: The "DetectorConstruction", "ActionInitialization", and "PhysicsList" classes, are mandatory in practice to see anything on the visualiser

- 2) Template

Defining an empty world (filled with air) with no nested geometry, a minimalist electromagnetic (EM) physics list, and a particle generator (particle gun, 1 MeV positron).

Assignment of the aforementioned classes to the "G4RunManager" will allow the visualiser to display the world, and observe the particle being shot in an event.

> NOTE: The main() function is not as comprehensive as subsequent "analysis" project and those that come after it.

- 3) Geometry

Same as template, with a nested cylindrical tracker volume.

> NOTE: The main() function is not as comprehensive as subsequent "analysis" project and those that come after it.

- 4) Sensitive

Implements a scoring region to track energy deposited in a medium, for each step, and the total for the event.

The positron from previous examples is changed to a 662 keV gamma-ray photon.

> NOTE: The main() function is not as comprehensive as subsequent "analysis" project and those that come after it.

- 5) Analysis

Implements histogramming and nTuples, building on the sensitive detector functionality.

Features updates to main() function, to allow command line swap between batch processing and interactive mode.

- 6) Decay      (alt: Radioactivity)

Implements radioactive decay for a chosen isotope, replacing the monoenergetic particle shot by the particle gun in previous projects.

Particle is modelled as a point source, embedded within a spherical source geometry (which has been given the appropriate material).

Features addition to main() function, to allow longer lived (>1y) isotopes to decay.

> NOTE: May also wish to model the particle as a volume source, assigned to the spherical geometry.

- 7) Scintillator       (alt: Scintillation)

Implements scintillation light (optical photons), in response to energy deposition in a scintillator crystal medium, also defines reflective and detection (absorption) surfaces.

> NOTE: Reverts back to a simple 662 keV gamma shot from the particle gun, to keep the focus on the scintillation process, also no radioactive decay or histogramming.

- 8) Optics

Same as "Scintillator", but reintroduces histogramming (and optical photon detection/absorption positions)

TODO: Count edep from gammas, compare with how many optical photons generated (as well as detected, absorbed, etc - SEE: LXeHistoManager.cc)

- 9) Radioactivity      (PREVIOUSLY NAMED: Spectroscopy)

Same as "Optics", but reintroduces radioactive decay and source geometry

- 10) Spectroscopy

Same as "Radioactivity", but models full detector geometry

Brings all of the former concepts together to simulate gamma-ray spectroscopy using a scintillator detector (TODO: May leave this as a more comprehensive version of decay, with no optical photon generation)
^may rename as just "detector"

- 11) Deposition        (PREVIOUSLY NAMED: Spectroscopy) (TECHNICALLY THE FIRST DRAFTING OF THIS CAME AFTER DECAY SO THIS IS MORE LIKE 6.5, BUT MAY WORK ON IT IN FUTURE AS EDEP RATHER THAN OPTICAL PHOTON)

Same as "Decay", but introduces complete detector geometry beyond just the crstal.
Same as "Radioactivity", but no scintillation light (may do it, may not).

(TODO: Also swaps sensitive detector for stepping action and event action ? Or do this in a subsequent one)

TODO: Also use subtraction solids

> NOTE: Will remain as is, simulating gamma-ray spectroscopy via energy deposition, and not by counting scintillation photons.

- 12) GPS

Uses general particle source (GPS) for volumetric source with isotope assigned to volume, instead of particle gun point source

> NOTE: Extension of "Spectroscopy"

- 13) FINAL

...

## Other

- Starter: Just following the "Getting Started" section of the documentation, and making notes on library functionality (this will not run)

- Dose: Is just example B1 with slightly altered geometry (intending to explore dosimetry more at a later date)

## ROOT Scripts

A variety of ROOT macros, following a similar progression from beginner to more comprehensive, for use with lab and simulated data

> NOTE: See README in rootscripts directory
