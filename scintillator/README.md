# About ...

## Scintillation Parameters

### Refractive Index

The refractive index (n) of NaI is dependent on photon wavelength

A relationship between wavelength and n is established by a Sellmeier equation,
which expresses 'n' as a function of wavelength (λ).

Utilising the connection between photon energy and wavelength:

E = (hc)/λ

Where:

E = photon energy
h = Planck's constant
c = Speed of light

Hence, higher photon energy corresponds to shorter wavelengths.

[Refractive Indices](refractiveindex.info)

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
