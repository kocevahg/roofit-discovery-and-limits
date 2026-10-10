# Signal-plus-Background Inference with RooFit and RooStats

ROOT macros that fit a Breit–Wigner (signal) plus Gaussian (background) model to a one-dimensional distribution and then run the standard inference steps on it: an extended likelihood fit, a workspace with background-only and signal-plus-background hypotheses, a profile-likelihood interval, an asymptotic discovery significance, and a CLs upper limit on a toy example.

> **Status.** A cleaned-up collection of exercises.

## Overview

The repository walks through one analysis chain, from a plain fit to a hypothesis test:

```
fit_sig_bkg.C                     simple fit, signal fraction                  (testtree.root)

fit_extended.C ──► make_modelconfig.C ──┬─► profile_likelihood.C               (testtree.root)
  extended fit       B and S+B models   └─► discovery_from_workspace.C
  + workspace

discovery_standalone.C            the whole chain in one macro                  (treeForHypoTest.root)

cls_limit_toy.C                   CLs upper limit on generated toy data         (no input file)
```

## Statistical model

All macros that use the data work with the observable `x6` (a branch of the input tree, range 0–9):

- **signal:** Breit–Wigner PDF with position `mean_bw` and width `sigma_bw`
- **background:** Gaussian PDF with `mean_g` and `sigma_g`

The components are combined either with a signal fraction (`fit_sig_bkg.C`) or as an *extended* sum with yields (`Ns`, `Nb` in `fit_extended.C`; `frac_sg`, `frac_b` in `discovery_standalone.C`, where the "frac" names are in fact yields).

## Macros

| Macro | What it does | Reads | Writes |
|---|---|---|---|
| `fit_sig_bkg.C` | Unbinned fit of Breit–Wigner + Gaussian with a signal fraction; plots data, total and components. | `testtree.root` | `fit_sig_bkg.pdf` |
| `fit_extended.C` | Extended fit (yields `Ns`, `Nb`, Minuit2); builds a `RooWorkspace` with data and model. | `testtree.root` | `fit_extended.pdf`, `workspace_extended_fit.root` |
| `make_modelconfig.C` | Defines `B_model` (Ns = 0) and `S+B_model` (Ns = 50) as `ModelConfig`s. Nuisance parameters: `mean_g`, `sigma_g`, `Nb`; `mean_bw` and `sigma_bw` are held constant. | `workspace_extended_fit.root` | `workspace_modelconfig.root` |
| `profile_likelihood.C` | 68 % profile-likelihood interval on `Ns` (`ProfileLikelihoodCalculator`). | `workspace_modelconfig.root` | `profile_likelihood.pdf` |
| `discovery_from_workspace.C` | Asymptotic one-sided discovery test (`AsymptoticCalculator`) from the stored `ModelConfig`s. | `workspace_modelconfig.root` | console output |
| `discovery_standalone.C` | Fit, `ModelConfig`s and asymptotic discovery test in one macro, on a binned copy of the data. | `treeForHypoTest.root` | `discovery_standalone.pdf`, console output |
| `cls_limit_toy.C` | Toy study: Landau "signal" plus Gaussian "background", 500 generated events, 95 % CLs upper limit with `HypoTestInverter` (asymptotic, one-sided). Uses generated data, not the trees. | none | console output |

The macros follow the structure of the RooFit and RooStats tutorials distributed with ROOT.

## Requirements

- [ROOT](https://root.cern) 6.x with RooFit and RooStats. The version used in the course is not recorded; any ROOT 6 release with RooStats is expected to work, but this has not been checked.
- The macros are meant to be run in the ROOT interpreter.

## Repository structure

```
.
├── macros/                 # the seven ROOT macros
└── data/                   # course-provided input trees

```

## How to run

The macros read their input from, and write their output to, the current directory, so run them from a separate working directory:

```bash
mkdir -p work && cd work
ln -s ../data/testtree.root ../data/treeForHypoTest.root .

root -l -b -q ../macros/fit_sig_bkg.C
root -l -b -q ../macros/fit_extended.C
root -l -b -q ../macros/make_modelconfig.C
root -l -b -q ../macros/profile_likelihood.C
root -l -b -q ../macros/discovery_from_workspace.C
root -l -b -q ../macros/discovery_standalone.C
root -l -b -q ../macros/cls_limit_toy.C
```

- `fit_extended` → `make_modelconfig` → `profile_likelihood` / `discovery_from_workspace` must run in this order, because each reads the workspace written by the previous step.
- Run the macros interpreted, without a trailing `+`; they rely on ROOT's automatic header loading for the RooStats classes.

## Examples


