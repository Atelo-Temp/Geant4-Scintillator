# About

NOTE: Code in many of the earlier macros (and even some of the later macros) is being left 
intentionally rough, poorly designed, non-refactored, etc, for reference. The final versions
will be where i clean up the code significantly.

## ASCII (.Spe) Format

SPE files are Block Structured ASCII (BSA) files that can be viewed or modified with any available text editor.

A BSA-file is divided into blocks. Each block is identified by a string that starts with a dollar sign ($) and ends with a colon (:).

Thus, $BLOCK_NAME:.

The structure of the data in each block is uniquely defined; the order in which blocks may appear in the file is, however, not always defined.

These ROOT macros are designed to handle BSA-files exported from Maestro spectroscopy software, and follow the format:

- Spectrum ID

```text
$SPEC_ID:
<description>
```

- Spectrum Remarks

```text
$SPEC_REM:
<remark1>
<remark2>
...
<remarkN>
```

- Measurement Date

```text
$DATE_MEA: (start date of measurement)
<month/day/year> <hour:min:sec>
```

- Measurement time

```text
$MEAS_TIM: (spectrum measurement time in seconds)
<livetime> <truetime>
```

- Data

```text
$DATA: (spectral data)
<lower channel number> <upper channel number>
<data line 1>
...
<data line n>
```

- ROI

```text
$ROI: (regions of interest)
num_roi
lower_chan_no(1) upper_chan_no(1)
lower_chan_no(n) upper_chan_no(n)
```

- Presets

```text
$PRESETS:
...
```

- ...

```text
$ENER_FIT:
offset slope quadratic
```

- ...

```text
$MCA_CAL:
num_coefficients
```

- ...

```text
$SHAPE_CAL:
```

- Example format

NOTE: Channel data condensed to ellipsis for brevity.

```text
$SPEC_ID:
DeT_2
$SPEC_REM:
DET# 1
DETDESC# PHY-R11-02 Easy-MCA-2k SN 12319717
AP# Maestro Version 7.01
$DATE_MEA:
11/01/2024 16:27:06
$MEAS_TIM:
600 600
$DATA:
0 2047
...
$ROI:
1
1550 1773
$PRESETS:
Live Time
600
0
$ENER_FIT:
0.000000 0.000000
$MCA_CAL:
3
0.000000E+000 0.000000E+000 0.000000E+000 
$SHAPE_CAL:
3
0.000000E+000 0.000000E+000 0.000000E+000
```

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

Implements statistics box output

Also requires manual implementation of rough FWHM (more versatile for merged peaks, etc), and calculates accurate FWHM using fit

7) refit.cc

Rquires manual implementation of both centroid and sigma (more versatile for merged peaks, etc)
Will perform an initial rough fit, then a secondary fit using the parameters from the first fit for convergence

8) counts.cc

Integrates area under the fit curve to determine counts

9) custom_stats.cc

Write custom data to the statistics box

Also integrates counts into the fitting pipeline, rather than as a separate method to be called manually as in counts.cc

10) plot_ntuples.cc (alt: ntuple_binning.cc)

convert per-event photons detected ntuples to 2048 bin histogram

11) ntuple_smearing.cc

Apply a gaussian smearing to per-event photon ntuples, in order to fix aliasing issue seen when plotting 2048 bin histogram directly

> NOTE: This smearing represents PMT statistics / shot noise

12) ntuple_fit.cc (alt: fit_ntuple.cc, fit_root.cc)

Integrate ntuple handling into custom_stats.cc (i.e. the previous histogram pipeline)

13) ascii_fit.cc

Fit a single photopeak in a lab ASCII spectrum using a gaussian fit function

> NOTE: Doesnt consider multi-peak scenarios or the background component

14) hybdrid_plot.cc

Introduces user filename input, similar to ascii_canvas.cc, but more comprehensive

Also disambiguates between ROOT and ASCII input files, handling each via dedicated helper functions

> NOTE: Currently only works with ROOT Ntuples (not ROOT hists, etc), also has hardcoded tree/branch 
names (will visit solutions for this in a new macro soon)

To avoid adding unneccessary code to the core principle im trying to address with this macro, 
am omitting writing loaded hist to outfile, and fitting hist (albeit they are simple additions)

15) save_hist.cc (alt: write_histo.cc, write_hist.cc, save_histo.cc)

Gives ability to save histograms to root files (handy for saving ROOT Ntuples as smaller files)

> NOTE: Literally just adds: save() method to plot_any.cc

Read ASCII file, OR, per-event Ntuple data, and plot a ROOT histogram, then save the histogram (will have to apply smearing to Ntuples still) (saves storing 10+GB .root files in local data, and can reference prior runs/configurations easier)
^ can use load_root.cc (custom_stats.cc) esque pipeline to load it back for post-processing

16) root_explorer.cc (alt: plot_cli.cc)

Implements:
- reading root file for available objects
- multiple root object type handling
- prompting user with object choice
- providing filtered list of names matching that object choice
- loading and then plotting said named object choice
- able to swap between "int" and "double" when reading from TTree branch entries (i.e., int for num photons, but double for distances/times) (NOTE: May be better to just do double for both tbh)

NOTE: Was briefly named "exponential_fit.cc", but going to separate out file loading and exponential fitting into two separate macros (was trying to introduce too many new features at once).

NOTE: Was then briefly named "omnit_fit.cc", but arguably the introduction of dynamic root object selection should be its own small endeavour, trying to introduce that while also keeping the ASCII/ROOT file type logic from hybrid_fit.cc feels a bit much (but then combining the dynamic file handling of hybrid_fit.cc and dynamic object handling of this macro feels like a natural progression from here).

NOTE: Have stripped ASCII handling features back, so that its now just an interactive CLI for selecting ROOT objects (rather than hardcoded tree/branch names).

17a) multi_fit_a.cc

> NOTE: Extension of ascii_fit.cc

Able to fit multiple peaks (i.e. 60Co, or even 133Ba) for lab spectra (ASCII - .Spe)

Performs full fit first, extracts params, then performs fits on indidual peaks 

17b) multi_fit_b.cc

> NOTE: Extension of ascii_fit.cc

Able to fit multiple peaks (i.e. 60Co, or even 133Ba) for lab spectra (ASCII - .Spe)

Performs fits on indidual peaks first, extracts params, then performs full fit

18) background_fit.cc

> NOTE: Extension of ascii_fit.cc (multi-peak fitting is intentionally omitted from this one to focus on the linear component)

Fit a single peak in an ASCII lab spectrum using a gaussian + 1st order polynomial background fit function

> NOTE: The idea here isnt to fit to the entire spectra besides the peak (i.e., x-rays, compton region, backscatter, compton edge, etc...),
instead the chi2/ndf of the photopeak fit can be improved tenfold by introducing a small linear background component 
(the lower energy tail is typically visually higher then the higher energy tail, creating a sort of slanted gaussian), 
which ends up being a good first order approximation when considering the immediate vicinity to the left and right of the photopeak.

19) spectra_fit.cc (alt: spectrum_fit.cc, lab_fit.cc)

Combines multi-peak fitting (from multi_fit_b.cc), and background component addition (from background_fit.cc).

20) read_spe.cc

The ASCII (.Spe) file reading in previous macros has been rudimentary. This simply improves upon the "fill_hist()" or "fill_hist_ascii()" methods seen prior.

> NOTE: This parses BSA-style header blocks ($...:), instead of relying on arbitrary line numbers, which in turn also allows for extracting parameters such as live time (relevant for background subtraction), and channel numbers from the file.

## TODOS

21) background_subtract.cc

Fill a ROOT TH1 with an ASCII (.Spe) lab spectrum (recorded with a source), then subtracts ASCII (.Spe) background spectrum (recorded with no source)

NOTE: Likely best to populate two histograms hpx1 = source spectrum, hpx2 = background spectrum, use: TH1::Add(hpx1, hpx2, 1, -1);

Potentially then saving the resultant spectra as a .root file.

> NOTE: To account for differences in live times between the spectra recorded with a source (typically has greater
dead time, leading to live time being shorter than real time, due to high decays/s of sources and electronics signal 
processing time), and background only (very little, if any dead time), the background spectra is scaled using the
live time value listed in the ASCII file.

22) plot_any.ccc (alt: onmi_plot.cc, load_any.cc)

NOTE: This should have been named plot_any.cc tbh (maybe rename plot_any to something else, as it kinda doesnt plot any as is) (also omni feels like it should be saved for final fitting macro)

Combines:
- ROOT object browser CLI (root_explorer.cc)
- Updated ASCII handling (read_spe.cc)

20) exponential_fit.cc

NOTE: The prior work done on this has been extracted out to omni_plot.cc, as handling multiple root object inputs, and tree/branch names, is quite an involved process, and dont want to implement too many new features into a single

TODO: Make a copy of omni_plot.cc when its done, then just reintroduce fitting
^ or potentially keep it simpler and just focus on exponential fitting first, then combine the two later

TODO: Currently just whipping up a rough draft of fit(), need to update it to match refined fit() methodology in ascii_fit()

21) any_fit.cc (alt: fit_any.cc, fitter.cc)

Hybrid fitting, able to handle both ascii files (lab) and root files (simulation)

<!-- NOTE: Just reintroduces fitting to save_hist.cc -->
NOTE: ^^^ reintroducing peak fitting (from spectra_fit.cc) to omni_plot.cc is probably better

NOTE: Rather than trying to expanding on fitting capabilities etc, previous fitting macros 
havent had the user specified path functionality, or save functionality, so integrating 
that is a good stepping stone to a comprehensive fitting macro

- Takes .root / .Spe filename as argument rather than hardcoded
- Parses the filename to identify whether its .root or .Spe, runs histogramming pipeline for respective file type

22) hybrid_fit.cc (alt: smart_fit.cc)

Hybrid fitting, able to handle both ascii files (lab) and root files (simulation)
^ maybe also able to determine whether it needs a gaussian or gaus + pol fit
^ takes .root / .Spe filename as argument rather than hardcoded
^ parses the filename to identify whether its .root or .Spe, runs histogramming pipeline for respective file type

NOTE: I think this main functionality should be determining whether to use gaus or gaus + pol
^ maybe even whether to use exponential etc
^ sinice 17) kinda ticks most of these other boxes, but trying to fit smart fitting would overcrowd 17)

23) omni_fit.cc

Full functionality of all previous fitting capabilities, plus final touches

TODO: Maybe think of a way to determine if input data has aliasing issue, and needs gaussian smearing

24) dimension_plotter.cc

Introduce 2D and 3D histogramming

24) dimension_plotter.cc

Introduce 2D and 3D fitting

XX) downsampling.cc

Convert a 2048 bin lab spectrum to 1024 bins

NOTE: Im not sure this is a good idea honestly, merged peaks will become even more merged, etc, likely better to find workaround to G4 1024 bin limit

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

### ntuple_fit.cc

```c++
range(500,1100) // xlow, xhigh
fit(800,100) // rough centroid, rough fwhm
```

### ascii_fit.cc

```c++
plot("~/Maestro/LaBr/LaBr_300s_sources/137Cs_LaBr_750v_10coarse_3cm.Spe")
fit(800, 50)
```

```c++
plot("~/Maestro/NaI/NaI_2inch_300s_sources/137Cs_NaI_800v_20coarse_3cm.Spe")
fit(800, 50)
```

### plot_any.cc

```c++
// From ./rootscripts
plot("../data/21_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_1-8res_1000000event.root")
```

```c++
// Absolute pathing
plot("~/geant4/geant4-v11.3.2/project/data/21_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_1-8res_1000000event.root")
```

### save_hist.cc

```c++
// From ./rootscripts
plot("../data/21_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_1-8res_1000000event.root")
save("../data/21_hist.root")
```

```c++
// Absolute pathing
plot("~/geant4/geant4-v11.3.2/project/data/21_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_1-8res_1000000event.root")
save("~/geant4/geant4-v11.3.2/project/data/21_hist.root")
```

### omni_plot.cc

ASCII file

```c++
// Absolute pathing
plot("~/Maestro/LaBr/LaBr_300s_sources/137Cs_LaBr_750v_10coarse_3cm.Spe")
```

ROOT file (containing Ntuples & Histogram)

```c++
// From ./rootscripts
plot("../final/build/output0.root")
```

```c++
// From ./rootscripts
plot("../data/21_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_1-8res_1000000event.root")
```

```c++
// Absolute pathing
plot("~/geant4/geant4-v11.3.2/project/data/21_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_1-8res_1000000event.root")
```

ROOT file (containing only a histogram)

```c++
plot("../data/21_hist.root")
```

Save plotted histogram

```c++
save("~/geant4/geant4-v11.3.2/project/data/21_hist.root") // absolute pathing
save("../data/21_hist.root") // from ./rootscripts
```

### multi_fit.cc

To open ASCII file (Spe format):

```c++
plot("/path/to/ASCII.Spe")
```

#### multi_fit_a.cc

- Example 1 (NaI 2' 60Co 2 Merged Peaks, 133Ba 4 Merged Peaks):

```c++
plot("~/Maestro/NaI/NaI_2inch_300s_sources/60Co_NaI_800v_20coarse_3cm.Spe")
plot("~/Maestro/NaI/NaI_2inch_300s_sources/133Ba_NaI_800v_20coarse_3cm.Spe") // 3000 counts amplitude, but the two small shoulder peaks barely noticeable due to poor resolution
```

- Example 2 (NaI 1' 60Co 2 Merged Peaks, 133Ba 4 Merged Peaks):

```c++
plot("~/Maestro/NaI/NaI_1inch_300s_sources/60Co_NaI1_800v_20coarse_3cm.Spe")
plot("~/Maestro/NaI/NaI_1inch_300s_sources/133Ba_NaI1_800v_20coarse_3cm.Spe")
```

- Example 3 (LaBr 1.5' 60Co 2 Merged Peaks, 108mAg 2 Merged Peaks, 133Ba 4 Merged Peaks):

```c++
plot("~/Maestro/LaBr/LaBr_300s_sources/60Co_LaBr_750v_10coarse_3cm.Spe") // best resolution
// fit(...)
```

```c++
plot("/home/user/Maestro/LaBr/LaBr_300s_sources/108mAg_LaBr_750v_10coarse_3cm.Spe")
range(600,950)
fit({700,850}, 50) // intentionally off by a bit
```

```c++
plot("~/Maestro/LaBr/LaBr_300s_sources/133Ba_LaBr_750v_10coarse_3cm.Spe")
// fit(...)
```

> NOTE: LaBr arguably best resolution 

- Example 4 (CeBr 1' 60Co 2 Merged Peaks, 133Ba 4 Merged Peaks):

```c++
plot("~/Maestro/CeBr/CeBr_300s_sources/60Co_CeBr_-1000v_3cm.Spe")
plot("~/Maestro/CeBr/CeBr_300s_sources/133Ba_CeBr_-1000v_3cm.Spe")
```

#### multi_fit_b.cc

- Example 3 (LaBr 1.5' 60Co 2 Merged Peaks, 108mAg 2 Merged Peaks, 133Ba 4 Merged Peaks):

```c++
plot("/home/user/Maestro/LaBr/LaBr_300s_sources/108mAg_LaBr_750v_10coarse_3cm.Spe")
// range(600,950) // NOTE: fit() will zoom anyways
fit({710,840}, 50) // intentionally off by a bit
```

### background_fit.cc

```c++
plot("~/Maestro/LaBr/LaBr_300s_sources/137Cs_LaBr_750v_10coarse_3cm.Spe") // 137Cs - LaBr
fit(800, 50) // ~662 keV photopeak
```

```c++
plot("~/Maestro/NaI/NaI_2inch_300s_sources/137Cs_NaI_800v_20coarse_3cm.Spe") // 137Cs - NaI2'
fit(800, 50) // ~662 keV photopeak
```

```c++
plot("/home/user/Maestro/NaI/NaI_2inch_300s_sources/22Na_NaI_800v_20coarse_3cm.Spe") // 22Na - NaI2'
fit(1450, 50) // ~1275 keV photopeak
fit(600, 50) // ~511 keV photopeak
```

### spectra_fit.cc

```c++
plot("~/Maestro/NaI/NaI_2inch_300s_sources/137Cs_NaI_800v_20coarse_3cm.Spe") // 137Cs - NaI2'
fit({800}, 50) // ~662 keV photopeak
```

```c++
plot("/home/user/Maestro/NaI/NaI_2inch_300s_sources/22Na_NaI_800v_20coarse_3cm.Spe") // 22Na - NaI2'
fit({1450}, 50) // ~1275 keV photopeak
fit({600}, 50) // ~511 keV photopeak
```

```c++
plot("/home/user/Maestro/LaBr/LaBr_300s_sources/108mAg_LaBr_750v_10coarse_3cm.Spe") // 108mAg - LaBr
// range(600,950) // NOTE: fit() will zoom anyways
fit({710,840}, 50) // intentionally off by a bit
// NOTE: ~624 keV & ~722 keV photopeaks
```

```c++
plot("~/Maestro/LaBr/LaBr_300s_sources/60Co_LaBr_750v_10coarse_3cm.Spe") // 60Co - LaBr
fit({1350,1550},50) // ~1170 keV & ~1330 keV
```

```c++
plot("~/Maestro/LaBr/LaBr_300s_sources/133Ba_LaBr_750v_10coarse_3cm.Spe") // 133Ba - LaBr
fit({320,350,420,450},50) // ~(276 keV, 303 keV, 356 keV & 384 keV)
```
