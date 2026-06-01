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
NOTE: This smearing represents PMT statistics / shot noise

12) ntuple_fit.cc (alt: fit_ntuple.cc)

Integrate ntuple handling into custom_stats.cc (i.e. the previous histogram pipeline)

13) ascii_fit.cc

Fit a lab ASCII spectrum using a gaussian fit function

14) plot_any.cc (alt: load_any.cc)

Introduces user filename input, similar to ascii_canvas.cc, but more comprehensive

Also disambiguates between ROOT and ASCII input files, handling each via dedicated helper functions

NOTE: Currently only works with ROOT Ntuples (not ROOT hists, etc), also has hardcoded tree/branch names (will visit solutions for this in a new macro soon)

To avoid adding unneccessary code to the core principle im trying to address with this macro, am omitting writing loaded hist to outfile, and fitting hist (albeit they are simple additions)

15) save_hist.cc (alt: write_histo.cc, write_hist.cc, save_histo.cc)

Gives ability to save histograms to root files (handy for saving ROOT Ntuples as smaller files)

NOTE: Literally just adds: save() method to plot_any.cc

Read ASCII file, OR, per-event Ntuple data, and plot a ROOT histogram, then save the histogram (will have to apply smearing to Ntuples still) (saves storing 10+GB .root files in local data, and can reference prior runs/configurations easier)
^ can use load_root.cc (custom_stats.cc) esque pipeline to load it back for post-processing

16) onmi_plot.cc (alt: hybrid_plot.cc,  plot_cli.cc)

NOTE: This should have been named plot_any.cc tbh (maybe rename plot_any to something else, as it kinda doesnt plot any as is) (also omni feels like it should be saved for final fitting macro)

Was briefly named exponential fit, but going to separate out file loading and exponential fitting into two separate macros

Implements:
- multiple root object type handling
- reading root file for available objects
- prompting user with object choice
- providing filtered list of names matching that object choice
- loading and then plotting said named object choice

- able to swap between "int" and "double" when reading from TTree branch entries (i.e., int for num photons, but double for distances/times)

17) multi_fit.cc

NOTE: Extension of ascii_fit.cc

Able to fit multiple peaks (i.e. 60Co, or even 133Ba) for lab spectra

>>> TODOS

18) background_fit.cc (alt: lab_fit.cc)

Fit an ASCII lab spectrum using a gaussian + poly fit function

hpx->GetXaxis()->GetBinLowEdge(...GetXaxis()->GetFirst())
...GetXaxis()->GetBinUpEdge(...GetXaxis()->GetLast())

NOTE: Lab spectra often have lower level discriminator, so GetFirst() will likely return 0 when trying to automate intercept

17) exponential_fit.cc

NOTE: The prior work done on this has been extracted out to omni_plot.cc, as handling multiple root object inputs, and tree/branch names, is quite an involved process, and dont want to implement too many new features into a single

TODO: Make a copy of omni_fit.cc when its done, then just reintroduce fitting

TODO: Currently just whipping up a rough draft of fit(), need to update it to match refined fit() methodology in ascii_fit()

17) any_fit.cc (alt: fit_any.cc)

Hybrid fitting, able to handle both ascii files (lab) and root files (simulation)

NOTE: Just reintroduces fitting to save_hist.cc

NOTE: Rather than trying to expanding on fitting capabilities etc, previous fitting macros havent had the user specified path functionality, or save functionality,
so integrating that is a good stepping stone to a comprehensive fitting macro

- Takes .root / .Spe filename as argument rather than hardcoded
- Parses the filename to identify whether its .root or .Spe, runs histogramming pipeline for respective file type

19) any_fit.cc (alt: smart_fit.cc)

Hybrid fitting, able to handle both ascii files (lab) and root files (simulation)
^ maybe also able to determine whether it needs a gaussian or gaus + pol fit
^ takes .root / .Spe filename as argument rather than hardcoded
^ parses the filename to identify whether its .root or .Spe, runs histogramming pipeline for respective file type

NOTE: I think this main functionality should be determining whether to use gaus or gaus + pol
^ maybe even whether to use exponential etc
^ sinice 17) kinda ticks most of these other boxes, but trying to fit smart fitting would overcrowd 17)

21) omni_fit.cc

Full functionality of all previous fitting capabilities, plus final touches

TODO: Maybe think of a way to determine if input data has aliasing issue, and needs gaussian smearing

22) dimension_plotter.cc

Introduce 2D and 3D histogramming and fitting

XX) downsampling.cc

Convert a 2048 bin lab spectrum to 1024 bins

NOTE: Im not sure this is a good idea honestly, merged peaks will become even more merged, etc, likely better to find workaround to G4 1024 bin limit

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

### ntuple_fit.cc

```c++
range(500,1100) // xlow, xhigh
fit(800,100) // rough centroid, rough fwhm
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

- Example 3 (LaBr 1.5' 60Co 2 Merged Peaks, 133Ba 4 Merged Peaks):

```c++
plot("~/Maestro/LaBr/LaBr_300s_sources/60Co_LaBr_750v_10coarse_3cm.Spe") // best resolution
plot("~/Maestro/LaBr/LaBr_300s_sources/133Ba_LaBr_750v_10coarse_3cm.Spe")
```

> NOTE: LaBr arguably best resolution 

- Example 4 (CeBr 1' 60Co 2 Merged Peaks, 133Ba 4 Merged Peaks):

```c++
plot("~/Maestro/CeBr/CeBr_300s_sources/60Co_CeBr_-1000v_3cm.Spe")
plot("~/Maestro/CeBr/CeBr_300s_sources/133Ba_CeBr_-1000v_3cm.Spe")
```

### ...
