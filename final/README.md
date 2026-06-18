# About

Basic gamma spectroscopy scintillator detector, with simulated radioactive decay, scintillation photon generation/transport, and output spectrum/ntuples

Note, this version uses the GPS volume source instead of a point source

This version also features improvements to the C++ code (such as extracting code blocks out to helper methods, etc)

New features include:
- ability to randomise simulations via seeds (G4 defaults to a repeatable seed for development purposes)
- automated run time tracking
- 2048 channel binning (via per-event optical photons detected Ntuple output)
- expanded statistics (distance travelled by detected photons, distance travelled by bulk absorbed photons, time of flight for optical photons, number of reflections before detection, etc)
- ability to merge ntuples generated for each thread (although perhaps better to use hadd)

Potential features:
- implementation of proper 1024 channel binning (converting photons detected to a channel number) (NOTE: May actually leave this to postprocessing, just output per-event data)
- potentially introducing PMT statistics ?? (NOTE: May actually leave this to postprocessing, just output raw per-event data)
- integration window for optical photon detection at the photocathode ?? (may reduce the photopeaks slight exponential tail, where sometimes, without it, many photons are eventually reaching photocathode and being detected)

## Geometry

- A world volume filled with air
- A wooden tabletop

Detector:

- A 3' thallium doped sodium iodide (NaI:Tl) scintillator crystal
- An alumina reflector surrounding the crystal (on all faces except the photodetector output)
- An aluminium enclosure
- A thin layer of silicone optical grease for (crystal->window coupling)
- A borosillicate glass optical window (representing PMT interface)
- A K-Cs-Sb photocathode on the inside of the optical window

Source:

- A PVC source casing
- A cylindrical source

## Features

- Electromagnetic physics

- Basic visualisation macro

- Run action for creating histogram/nutples

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

E = (hc) / λ

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
mkdir build # NOTE: If it doesnt yet exist, or has been deleted
cd build
cmake -DGeant4_DIR=~/geant4/geant4-v11.3.2/install/lib64/cmake/Geant4/ ../
make -j 3 final
```

### Updating Build On Changes

I.e., on ".cc" file change

```bash
cd build # make sure you are in the build dir
make -j 3 final # call to cmake can be omitted, and no need to clean build dir
```

NOTE: If you change/edit the CMakeLists.txt, such as adding a new cpp file to the build, you will need to do a fresh initial build

## Running

### Batch Mode

```bash
./final run.mac
```

```bash
./final 137Cs.mac
```

### Visualiser

```bash
./final
```

Then in the visualiser command prompt:

```bash
/control/execute test.mac
```

Centre camera on the detector:

```bash
/vis/viewer/centreOn Scintillator
```

Set view vector:

```bash
# Sets view vector from (0,0,0) to (x,y,z)
# /vis/viewer/set/viewpointVector -10 0 0 # vector from (0,0,0) to (-10,0,0)
/vis/viewer/set/viewpointVector -180 0 90
```

Set view angle:

```bash
/vis/viewer/set/viewpointThetaPhi 330
```

Set the zoom:

```bash
/vis/viewer/zoom 4
```

Produce a section view (cut) of the geometry:

```bash
# Crystal origin is at x = 0, y = 0, z = 10 cm:
/vis/viewer/addCutawayPlane 0 0 10 cm
```

TODO: Partial section cut of geometry:

```bash
/vis/viewer/addCutawayPlane 0 0 0 cm 1 0 0 # cut along the X-axis
/vis/viewer/addCutawayPlane 0 0 0 cm 0 -1 0 # cut along the Y-axis
```

Change rotation style:

```bash
/vis/viewer/set/rotationStyle freeRotation
```

Hide the world box:

```bash
/vis/geometry/set/visibility World 0 false
```

Set light to come from directly above the geometry

```bash
/vis/viewer/set/lightsVector 0 1 0
```

TODO: CUSTOM VIS MACROS FOR SCREENSHOTS

HALF-CUT

```bash
/vis/viewer/centreOn Scintillator
/vis/viewer/zoom 6
/vis/viewer/set/viewpointVector -180 90 90
/vis/geometry/set/visibility Table 0 false
/vis/geometry/set/visibility Casing 0 false
/vis/geometry/set/visibility World 0 false
/vis/viewer/addCutawayPlane 0 0 10 cm
```

QUARTER-CUT

```bash
/vis/viewer/centreOn Scintillator
/vis/viewer/zoom 6
/vis/viewer/set/viewpointVector -180 90 90
/vis/geometry/set/visibility Table 0 false
/vis/geometry/set/visibility Casing 0 false
/vis/geometry/set/visibility World 0 false
/vis/viewer/addCutawayPlane 0 0 0 cm 1 0 0 # cut along the X-axis
/vis/viewer/addCutawayPlane 0 0 0 cm 0 -1 0 # cut along the Y-axis
```

## Cleaning Outfiles

If youre playing around with the visualiser, you may end up with a large number of ".root" outfiles, to clean these:

```bash
find . -name "*.root" -type f # Ensure you will remove the correct files, inspect the output
find . -name "*.root" -type f -delete # Then run this command to remove those files
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

Similarly we would not be able to get the wavelength of each photon.

To view these histograms:

```bash
root output0_t0.root --web=off
new TBrowser
```

The energy-counts histogram will no longer be included, 
but histograms for the nTuples described in "RunAction" & "SensitiveDetector" will be.

Alternatively, by adding the following in the RunAction class constructor:

```c++
auto analysisManager = G4AnalysisManager::Instance();
analysisManager->SetNtupleMerging(true); // this is false by default
```

Geant4 will automatically stitch all the thread ntuples together into the single output0.root file.

Subseqeuently, we can access the Ntuples via the standard:

```bash
root output0.root --web=off
new TBrowser
```

> NOTE: Merging massive Ntuples can cause the simulation to run out of memory and crash at the end of the run, keeping merging false
ensures that the raw data is safely dumped to the hard drive on the fly, using virtually zero extra RAM. 

> NOTE: Additionally, if running a simulation across a huge number of threads (i.e., 32, 64, or more), having all those threads
attempt to feed data into a single merged structure at the end of the run can create an I/O bottleneck.

> NOTE: Most of the time it is fine to enable merging though, i.e., running on a local desktop or laptop, but its good to be aware
of these caveats.

If you want/need to keep thread files separate, the following command can be used for post-processing, via ROOT's build-in "hadd" utility:

```bash
hadd merged.root output0_t*.root # just ntuples
# hadd merged.root output0.root output0_t*.root # histogram and ntuples
```

Or with multi--threading:

```bash
hadd -j 3 merged.root output0_t*.root
```

Which can then be accessed via:

```bash
root merged.root --web=off
new TBrowser
```

i.e.:

- Optical Photons Detected Per Event:

This Ntuple contains the number of optical photons "detected" at the photocathode each event.

Storing this information in an Ntuple instead of the G4 H1 allows us to bypass the 1024 bin limit.

However, we cant just view it as a histogram immediately, as its a sequential list of photons detected for each event, i.e. 100000 events, 100000 entries.

To quickly view it as a histogram in the TBrowser, enter the following into the root terminal:

```C++
// 1024 bins, 0 photons lower bound, 3000 photons upper bound
EventData->Draw("NumPhotons >> hTotal(1024, 0, 3000", "", "")
// NOTE: EventData is the name of the ntuple, and NumPhotons is the name of the Ntuple column of interest
```

For low event counts (i.e. testing a change without a lengthy run time):

```c++
EventData->Draw("NumPhotons >> hTotal(256, 0, 3500", "", "")
```

Since we have per-event data, this can also be easily binned into a root histogram via a root macro, for further post-processing (i.e. fitting, etc).

- Distance Travelled By Detected Photons

```c++
TrackData->Draw("DetectionDistance >> hTotal(2048, 0, 3500", "", "") // distance is in mm
```

- Time of Flight

Time elapsed from optical photon birth, to optical photon detection at the photocathode.

```c++
TrackData->Draw("TimeOfFlight >> hTotal(4096, 0, 30", "", "") // time is in nanoseconds
```

- Distance Travelled By Bulk Absorbed Photons (non-boundary absorption):

```c++
TrackDataAbsorb->Draw("AbsorptionDistance >> hTotal(2048, 0, 4000", "", "") // distance is in mm
```

- iEvent: 

The number of hits per event, in 100 bins (x10^3 = 100,000 events), so 1000 events per bin

We would expect 1000 hits per bin (if every photon interacted once), 
but what we see is a range from 0-2500,
implying 0 hits, or multiple hits for a single photon, or somewhere inbetween.

NOTE: Perhaps electrons liberated creating additional photons too, etc.

- X, Y, Z, Positions:

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

Surface plot

```C++
// compare (x,y,z), no cut, surface plot
StepDataDetection->Draw("fX:fY:fZ", "", "surf"); // detection coords
StepDataAbsorption->Draw("aX:aY:aZ", "", "surf"); // absorption coords
```

Box plot

```C++
// compare (x,y,z), no cut, box plot
StepDataDetection->Draw("fX:fY:fZ", "", "box2"); // detection coords
StepDataAbsorption->Draw("aX:aY:aZ", "", "box2"); // absorption coords
```

Heatmap

```c++
// Heatmap of detection (x,y) on the photocathode surface plane
StepDataDetection->Draw("fX:fY", "", "colz");
// Heatmap of boundary absorption in the 2d plane
StepDataAbsorption->Draw("aX:aY", "", "colz");
```

- Global Time: 

Time of interaction, typically peaks similarly to Z position chart,
the shorter times represent the particle first entering the detector,
trailing off in a similar manor, but slightly differently due to changes in X & Y too

- Wavelength:

On the order of 10 pico meters, which is typical for x-ray and gamma rays,
seems to line up with 662 keV converted to wavelength
