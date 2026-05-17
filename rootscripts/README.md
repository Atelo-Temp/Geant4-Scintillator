# About

...

## Project Ordering

NOTE: ascii_to_root.cc is pulled from root.cern tutorials, hence not listed here

1) canvas.cc

Creates a basic canvas and plots a sin function

2) ascii_canvas.cc

Read ascii data from ".Spe" file produced by Maestro, store it in a local histogram, and plot it on a canvas

3) load_root.cc

Read data from ".root" file produced by G4 simulations, store it in a local histogram, and plot it on a canvas

4) hist_zoom.cc

Same as load_root.cc, with added functionality to zoom in on a region of interest

5) basic_fit.cc

Same as hist_zoom.cc, with very basic peak fitting functionality (automatically finds centroid and sigma)

6) fit_stats.cc

Implements custom statistics box output

Also requires manual implementation of rough FWHM (more versatile for merged peaks, etc), and calculates accurate FWHM using fit

7) refit.cc

Rquires manual implementation of both centroid and sigma (more versatile for merged peaks, etc)
Will perform an initial rough fit, then a secondary fit using the parameters from the first fit for convergence

8) counts.cc

Integrates area under the fit curve to determine counts

>>> TODOS

8.5) ...

Write custom data to the statistics box

9) lab_fit.cc

Fit a lab spectrum using a gaussian + poly fit

10) downsampling.cc

Convert a 2048 bin lab spectrum to 1024 bins

11) any_fit.cc

Hybrid fitting, able to handle both ascii files (lab) and root files (simulation)
^ maybe also able to determine whether it needs a gaussian or gaus + pol fit

12) multi_fit.cc

Able to fit multiple peaks (i.e. 60Co, or even 133Ba)

13) ...

Full functionality of all previous fitting capabilities, plus final touches

...
fitter.cc
fit_root.cc

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

- Example 4 (CeBr 1' 137Cs):

```c++
get_path("~/Maestro/CeBr/CeBr_300s_sources/137Cs_CeBr_-1000v_3cm.Spe")
get_path("~/Maestro/CeBr/CeBr_300s_sources/60Co_CeBr_-1000v_3cm.Spe")
```
