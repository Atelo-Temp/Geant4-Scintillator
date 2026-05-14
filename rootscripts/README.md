# About

...

## Project Ordering

1) canvas.cc

Creates a basic canvas and plots a sin function

2) ascii_canvas.cc

Read ascii data from ".Spe" file produced by Maestro, store it in a local histogram, and plot it on a canvas

3) load_root.cc

Read data from ".root" file produced by G4 simulations, store it in a local histogram, and plot it on a canvas

4) hist_zoom.cc

Same as load_root.cc, with added functionality to zoom in on a region of interest

5) basic_fit.cc

Same as hist_zoom.cc, with very basic peak fitting functionality

6) ...

NOTE: ascii_to_root.cc is pulled from root.cern tutorials, hence not listed here

## Examples

### Execute Macros

To load in the macro:

```bash
root
.x ascii_canvas.cc
```

### ascii_canvas.cc

To open ASCII file (Spe format):

```c++
get_path("/path/to/ASCII.Spe")
```

- Example 1 (NaI 2' 137Cs):

```c++
get_path("~/Maestro/NaI/NaI_2inch_300s_sources/137Cs_NaI_800v_20coarse_3cm.Spe")
get_path("~/Maestro/NaI/NaI_2inch_300s_sources/60Co_NaI_800v_20coarse_3cm.Spe")
get_path("~/Maestro/NaI/NaI_2inch_300s_sources/241Am_NaI_800v_20coarse_3cm.Spe")
```

- Example 2 (NaI 1' 137Cs):

```c++
get_path("~/Maestro/NaI/NaI_1inch_300s_sources/137Cs_NaI1_800v_100coarse_3cm.Spe")
get_path("~/Maestro/NaI/NaI_1inch_300s_sources/60Co_NaI1_800v_20coarse_3cm.Spe")
```

- Example 3 (LaBr 1.5' 137Cs):

```c++
get_path("~/Maestro/LaBr/LaBr_300s_sources/137Cs_LaBr_750v_10coarse_3cm.Spe")
get_path("~/Maestro/LaBr/LaBr_300s_sources/60Co_LaBr_750v_10coarse_3cm.Spe")
get_path("~/Maestro/LaBr/LaBr_300s_sources/241Am_LaBr_750v_10coarse_3cm.Spe")
```

- Example 4 (CeBr 2' 137Cs):

```c++
get_path("~/Maestro/CeBr/CeBr_300s_sources/137Cs_CeBr_-1000v_3cm.Spe")
get_path("~/Maestro/CeBr/CeBr_300s_sources/60Co_CeBr_-1000v_3cm.Spe")
```
