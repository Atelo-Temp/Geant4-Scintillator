# About

Basic gamma spectroscopy scintillator detector, with output spectrum, and simulated radioactive decay

## Features

- An world volume filled with air, and a 3' sodium iodide scintillator crystal (and optional lead shielding)

- Electromagnetic physics

- Basic visualisation macro

- Sensitive detector for tracking energy deposited in the scintillator

- Run action for creating histogram

- Basic batch processing run macro

- Radioactive decay (i.e. for full 137 Cs spectrum, x-rays, etc... daughter emission for 44Ti, etc... )

- Isotropic source emissions

## Not Implemented

- Not currently seeing any x-rays in 137Cs spectrum (originating from 32 keV 137-Barium Ka transition - conversion electron) (NOTE: Can see x-rays for 207Bi as it undergoes electron capture, leading to shell transition to fill the gap left by the electron)
^ this is also related to detector efficiency actually (as lower energy photons have higher chance of interaction => lower energy, better efficiency, vice versa) ... may need to explore if i need to implement this (to make spectrum more realistic)

- Scintillation photons & photocathode

- Volumetric source (instead of point source)

- Consider the definition of the source material (does a portion need to be set as the daughter product)

## Relevant Examples

- basic/B3

- ...

## Components

...

### Scintillator Crystal

NaI is hygroscopic (absorbs water from atmosphere) so must be sealed in airtight can with glass window

### Scintillator Can

...

### Optical Window

...

This also serves the purpose of preventing electrons (and alpha/beta particles as a whole) from interacting with the crystal ...

### Optical Photon Reflector

The pulse height resolution of the detection system depens upon collecting as many of the photons created by the incident gamma-ray as possible.

The scintillation photons are emitted by the thallium ions in all directions, so a high efficiency reflector is used to surround the crystal (Al_{2}O_{3} and teflon), in order to reflect as much light as possible towards the photocathode (and subsequently PMT).

### Photomultipler Tube (PMT)

Window materials:

- Borosilicate glass

The most common window material, containing silica (SiO2) and boron trioxide (B2O3).

... ratio, density, etc, needed (TODO)

Transmits radiation from 300 nm to infrared (IR).

Doesnt transmit ultraviolet (UV) light effectively.

- Quartz glass

Also known as fused silica (SiO2).

Transmits radiation from deep UV to near IR (NIR) wavelengths.

Is used for applications demanding UV sensitivity.

### Photocathode

The Photomultipler Tube is evacuated glass vessel, with light entering through glass window, and inside surface of glass window is coated with an electropositive material - the photocathode.

Since scintillation photons are typically low energy (~10-15 eV for NaI), the photocathode must be comprised of a mixture of alkali metals (Li/Na/K/Cs), so that the photoelectric effect can take place, producing photoelectrons in the PMT.

Alkali metals have low electron binding energies (ionisation energies), as they have a single, weakly bound valence electron that is easily lost due to their large atomic size and low effective nuclear charge (a property that makes them highly reactive).

As you move down the alkali metal group, the atoms become larger, placing the valence electron at a greater distance from the nucleus.

> NOTE: The photocathode implemented is purely for counting scintillation photons.

> NOTE: Energy is not conserved with optical photons in Geant4, so they cannot be used for energy deposition applications, however they can still be counted, and to keep the model true to design, the photocathode will still use an appropriate material.

### Source Casing

Stops alpha and beta particles ...

If the scintillator crystal is exposed to the source directly (with no medium between the source and the crystal to block them), the alpha/beta particle energy will be superimposed on the output spectrum.

## Run Structure

Runs -> Events -> Steps

## Class Defintions (API)

### DectectorConstruction

One of the three mandatory classes ...

- Construct()

This method handles material specification, geometry definitions, visualiser coloring

- ConstructSDandField()

This method instantiates the extended "SensitiveDetector" (SD) class.

Assigns the SD to the scoring region.

Registers the sensitive detector instance with the Sensitive Detector Manager (singleton manager)

### PhysicsList

One of the three mandatory classes ...

- PhysicsList()

The constructor registers:

Electromagnetic physics.

Radioactive decay physics.

Decay physics.

### PrimaryGenerator

This class handles particle generation ...

Extends "G4VUserPrimaryGeneratorAction" ...

- PrimaryGenerator()

The class constructor instantiates the particle gun, defines particle coordinate position, and momentum.

- GeneratePrimaries()

This method defines the isotope, using the "IonTable".

Then passes this definition to the particle gun, as well as the kinetic energy and charge of the particle.

Lastly the particle guns "GeneratePrimaryVertex" method is called to generate an event.

### RunAction

This class provides start & end of event handlers

- RunAction()

The class constructor creates a histogram (for energy deposited), and nTuples (for monte carlo truths)

- BeginOfRunAction()

...

- EndOfRunAction()

...

### SensitiveDetector

This class extends the base SensitiveDetector

- SensitiveDetector()

The class constructor initialises the total energy deposited in the detector medium (for the current event) as zero.

- Initialize()

This method resets the total energy deposited in the detector medium to zero between events. 

- EndOfEvent()

This method writes energy deposited to G4 stdout, and adds to the appropriate histogram bin, at the end of each event.

- ProcessHits()

For each step (interaction) in an event, if energy was deposited in the detector medium, add it to the total energy deposited in the medium for the current event.

NOTE: Also handles NTuple generation, getting coordinates, event id, and global time at each step (may revise this for final build though, generating monte carlo truths are nice, but spectra is primary concern for now)

### ActionInitialization

One of the three mandatory classes ...

- Build()

This method registers user actions:

The particle generator (PrimaryGenerator)

The run handler (RunAction).

## Opening The Histogram

```bash
root output0.root --web=off
new TBrowser
```

> NOTE: Can be run without specifying the --web flag, but is much quicker with web browser disabled

Double click to open the histogram

When open, type "HIST" in the "Draw Option" input, and hit enter

Double click to reopen the histogram (makes photopeak visible for some reason)

"SetLogy" To make the y-axis log scaled, showing the compton region more clearly

## Exploring The NTuples

Additionally, step information is written in nTuples.

These are additional monte-carlo informations, called monte-carlo truth.

In the real experiment we would not be able to get this information,
i.e. how many photons interact with detector, xyz of interaction.

(Except using a detector with high granularity, divided in bins in xyz)

Would also not be able to get information about global time, as we dont know
when photon was emitted from the source

Similarly we would not be able to get the wavelength

To view these histograms:

```bash
root output0_T0.root --web=off
new TBrowser
```

The energy-counts histogram will no longer be included, 
but histograms for the nTuples described in "RunAction" & "SensitiveDetector" will be.

i.e.:

- iEvent: 

The number of hits per event, in 100 bins (x10^3 = 100,000 events), so 1000 events per bin

We would expect 1000 hits per bin (if every photon interacted once), 
but what we see is a range from 0-2500,
implying 0 hits, or multiple hits for a single photon, or somewhere inbetween.

NOTE: Perhaps electrons liberated creating additional photons too, etc.

- X, Y, Z, Positions: 

Most of the photons enter the detector at (0, 0, )

X & Y:
Since the photon is shot in a straight line along Z, there is a noticeable peak at X=0 & Y=0,
with a slight distribution around these coordinates, in both positive and negative directions,
implying photons are interacting mostly in the centre of the detector (X=0, Y=0),
however, some are scattering off from (0, 0) and interacting at non-zero (X, Y) coordinates

Z:
There is a noticeable peak at the Z coordinate corresponding to the face of the scintillator,
implying most photons interact with the scintillator as soon as they enter,
then, it follows a distribution that declines with distance,
impling more photons are interacting closer to the face of the cryal, 
and fewer near the back of the crystal,
until zero are recorded at Z > end of crystal

NOTE: Should look at both pre-step and post-step to get a more precise picture

NOTE: Should also identify the interaction mechanism, 
photoelectric effect (full deposition in one step),
or compton scattering (partial deposition in one step)

> NOTE: A 3D heatmap of detector interactions can also be seen by typing the following in the root stdin with a TBrowser open:

```C++
// compare (x,y,z), no cut, surface plot
Photons->Draw("fX:fY:fZ", "", "surf");
```

- Global Time: 

Time of interaction, typically peaks similarly to Z position chart,
the shorter times represent the particle first entering the detector,
trailing off in a similar manor, but slightly differently due to changes in X & Y too

- Wavelength:

On the order of 10 pico meters, which is typical for x-ray and gamma rays,
seems to line up with 662 keV converted to wavelength
