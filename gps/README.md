# About

Basic gamma spectroscopy scintillator detector, with output spectrum, and simulated radioactive decay

Note, this version uses the GPS volume source instead of a point source

## Features

- An world volume filled with air, a 3' sodium iodide scintillator crystal, an alumina reflector covering the crystal, and a lithium photocathode attached to the back of the crystal

- Electromagnetic physics

- Basic visualisation macro

- Run action for creating histogram

- Basic batch processing run macro

- Radioactive decay (i.e. for full 137 Cs spectrum, x-rays, etc... daughter emission for 44Ti, etc... )

- Isotropic source emissions

- Scintillation photons & photocathode

- Volumetric source (instead of point source)

## Not Implemented

- Consider the definition of the source material (does a portion need to be set as the daughter product)

## Relevant Examples

- basic/B3

- extended/optical/LXe

## Scintillation Parameters

### Refractive Index

The refractive index (n) of NaI is dependent on photon wavelength

A relationship between wavelength and n is established by a Sellmeier equation,
which expresses 'n' as a function of wavelength (λ).

Utilising the connection between photon energy and wavelength:

E = (hc)/λ

Where:

E = photon energy
h = Planck's constant (6.6261 * 10^-34 J s)
c = Speed of light (2.9979 * 10^8 m/s)

Hence, higher photon energy corresponds to shorter wavelengths (i.e. E is inverseley proportional to λ).

[Refractive Indices](refractiveindex.info)

Since we need to pass the energy that accompanies the refractive index vector in electron volts (eV),
convert Planck's constant from "J s" to "eV s":

> NOTE: 1 eV = 1.602176634 * 10^-19 J

So Planck's constant as a fraction of an eV (h / 1 eV):

h (eV s) = (6.6261 * 10^-34 J s) / (1.602176634 * 10^-19 J)

Therefore:

hc = ((6.6261 * 10^-34 J s) / (1.602176634 * 10^-19 J)) * (2.9979 * 10^8 m/s)
   = 1.239837404 * 10^-6 eV m

Units: eV s * m s^-1 = eV m

So to convert a wavelength in metres to eV:

E (eV) = (1.239837404 * 10^-6 eV m) / λ m

For wavelength in nm (1 * 10^-9 m) to eV:

E (eV) = 1239.837404 / λ nm

## Implementation

- Physics List

Include the physics list header file

```C++
#include "G4OpticalPhysics.hh"
```

Constructor should register optical physics

```C++
RegisterPhysics(new G4OpticalPhysics());
```

- Detector Construction

Need to define the following values for the scintillator medium: refractive index, emission spectrum, yield, decay time, absorption length

## Building

### Initial Build

```bash
# rm -rf build # NOTE: If it already exists and you want to do a clean build
mkdir build
cd build
cmake -DGeant4_DIR=~/geant4/geant4-v11.3.2/install/lib64/cmake/Geant4/ ../
make -j 3 gps
```

### Updating Build On Changes

I.e., on ".cc" file change

```bash
cd build # make sure you are in the build dir
make -j 3 gps # call to cmake can be omitted, and no need to clean build dir
```

NOTE: If you change/edit the CMakeLists.txt, such as adding a new cpp file to the build, you will need to do a fresh initial build

## Running

### Batch Mode

```bash
./gps run.mac
```

### Visualiser

```bash
./gps
```

## Analysis

### Opening The Histogram

```bash
root output0.root --web=off
new TBrowser
```

> NOTE: Can be run without specifying the --web flag, but is much quicker with web browser disabled

Double click to open the histogram

When open, type "HIST" in the "Draw Option" input, and hit enter

Double click to reopen the histogram (makes photopeak visible for some reason)

"SetLogy" To make the y-axis log scaled, showing the compton region more clearly

### Exploring The NTuples

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
