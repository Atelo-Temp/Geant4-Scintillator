# About

Basic gamma spectroscopy scintillator detector with output spectrum

Radioactive decay can be simulated, but this will only focus on the particle gun shooting specific gammas
Subsequent project will simulate decay

## Features

- An world volume filled with air, and a 3' sodium iodide scintillator crystal (and optional lead shielding)

- A gamma-ray photon as the default particle (662 keV - 137 Cs), emitted along z-axis

- Electromagnetic physics

- Basic visualisation macro

- Sensitive detector for tracking energy deposited in the scintillator

- Run action for creating histogram

## Not Implemented

- Isotropic source emissions

- Radioactive decay (i.e. for full 137 Cs spectrum, x-rays, etc... daughter emission for 44Ti, etc... )

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

- Global Time: 

Time of interaction, typically peaks similarly to Z position chart,
the shorter times represent the particle first entering the detector,
trailing off in a similar manor, but slightly differently due to changes in X & Y too

- Wavelength:

On the order of 10 pico meters, which is typical for x-ray and gamma rays,
seems to line up with 662 keV converted to wavelength
