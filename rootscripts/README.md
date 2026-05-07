# About

...

## Examples

To load in the macro:

```bash
root
.x ascii_canvas.cc
```

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
```

- Example 3 (LaBr ' 137Cs):

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
