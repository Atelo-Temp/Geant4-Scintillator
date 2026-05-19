// Load a histogram into memory from a '.root' file, zoom in on a peak, and fit user defined function

#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TF1.h>

#include <TROOT.h>
#include <TStyle.h>
#include <TFitResult.h>

// Global histo/canvas variables
TH1 *hpx = nullptr;
TCanvas *c = nullptr;

// Executes automatically on script start (NOTE: Choose another function name if you wish to manually call it instead) 
int fit_root () {
    // Fetch and open root file
    // auto *in = TFile::Open("~/geant4/geant4-v11.3.2/project/gps/build/output0.root");
    TFile* in = TFile::Open("~/geant4/geant4-v11.3.2/project/data/17_2_NaI-Tl_gpsvolsrc_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_3-5res_500000event.root");
    
    // Get the histogram from the root file and assign it to the TH1 pointer
    in->GetObject("Photons;2", hpx); // hpx = in->Get("Photons;2");
    
    // Handle missing histogram
    if (!hpx) {
        printf("Error: Histogram not found!\n");
        return 1;
    }
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    in->Close();
    
    // Canvas args
    Int_t winX = 0; // Top left of screen
    Int_t winY = 0; // Top left of screen
    Int_t width = 1200;
    Int_t height = 800;
    
    // Create a canvas display
    c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    
    // Handle error creating canvas
    if (!c) {
        printf("Error: Couldnt create canvas!\n");
        return 1;
    }
    
    // Draw histogram with default option
    hpx->Draw("HIST");
    
    // Clean the histogram statistics box
    gStyle->SetOptStat(0); // default = 1111 (NOTE: 000001111 with zeros removed)
    // 0 = hides the statistics box entirely (leaving only fit box when fitted)
    // 10 = only number of entries
    // 110 = entries and mean
    
    return 0;
}

// Zoom in on a specific range
int range (double start, double end) {
    // Handle missing histogram/canvas
    if (!hpx || !c) {
        printf("Error: Histogram or Canvas not found!\n");
        return 1;
    }
    
    // Set lower/upper canvas view range
    hpx->GetXaxis()->SetRangeUser(start, end);
    
    // Notify canvas of update
    c->Modified();
    c->Update();
    
    printf("Zoomed to %.2f - %.2f\n", start, end);
    
    return 0;
}

// Zoom back out to the full view histogram (NOTE: Could also call range(0,0))
int reset () {
    // Handle missing histogram/canvas
    if (!hpx || !c) {
        printf("Error: Histogram or Canvas not found!\n");
        return 1;
    }
    
    // Resets canvas view range
    hpx->GetXaxis()->UnZoom();
    
    // Notify canvas of update
    c->Modified();
    c->Update();
    
    return 0;
}

// Manually input estimated photopeak centroid and FWHM values, fit a gaussian to it, and display the fit
int fit (int const FWHM) {
    // Find the tallest point in the current histogram range (NOTE: Zoom in on peak of interest first)
    int const bin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(bin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(bin); // get the y-axis number of counts for max bin, i.e. amplitude
    
    // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    // (it’s a fit window, not a Gaussian width parameter)
    // The Gaussian itself mathematically extends to infinity.
    double const low = peakX - (2 * FWHM);
    double const high = peakX + (2 * FWHM);
    // TODO: FWHM (peakX / 2 => gives half maximum => iterate outwards from centre until bin val below half maximum) ?
    // ^ but this wont work for merged peaks, etc
    
    // NOTE: You usually want the fit window to extend well into the tails/background
    // because the fitter needs tail information to constrain sigma properly.
    // If the window is too tight:
    // - sigma gets underestimated
    // - tails look clipped
    // - centroid can shift
    // - fit becomes unstable/noisy
    // You generally want:
    // - enough tails for sigma estimation
    // - some background included
    // - but not neighboring peaks
    
    // Define the fit function
    auto fitFn = new TF1("fitFn", "gaus", low, high);
    // NOTE: "gaus" is built-in ROOT shorthand for [0]*exp(-0.5*((x-[1])/[2])^2)
    
    // NOTE: "gaus(0)" is functionally the same, and can be abbreviated to "gaus" 
    // when using a single fitting function, however, if you were to use two functions
    // "gaus(0) + gaus(3)" ensures that the subsequent call to set parameters assigns
    // passed parameters to the correct function (see below)

    // Instead of relying on automatic RMS, which is not reliable for merged peaks etc,
    // require the user to state a rough FWHM value deduced by eye, and derive sigma from it
    double const sigma = FWHM / 2.355;
    
    printf("PRE-FIT SIGMA: %f\n", sigma);
    
    // Pass the parameters required for the gaussian fit function
    fitFn->SetParameters(peakY, peakX, sigma);
    fitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    // NOTE: [0] = Amplitude, [1] = Mean, [2] = Sigma
    
    // NOTE: If we defined "gaus(0) + gaus(3)", we would need to pass separate params, i.e.:
    // fitFn->SetParameters(peakY1, peakX1, sigma1, peakY2, peakX2, sigma2);
    // hence, params: [0], [1], [2], are used for the first gaussian function,
    // and params: [3], [4], [5], are used for the second gaussian function
    // the same applies for "gaus(0) + pol1(3)",
    // where params: [4] & [5] are then the intercept and slope for the poly fit
    
    // Call the histograms fit method, passing the fit function and histogram fitting options string
    TFitResultPtr result = hpx->Fit(fitFn, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = attempts to improve the fit quality
    // "L" = use log likelihood method (default chi-square), for use with counts histograms
    // "+" = adds this new fitted func to list of fitted funcs (default is delete previous keep last)
    
    // Draw the fit line (ROOT internally stores the fit function with the histogram after fitting)
    hpx->GetFunction("fitFn")->Draw("SAME");
    // NOTE: The "HIST" option suppresses drawing associated functions (including fits),
    // hence why "hpx->Draw()" works here instead of drawing the fit fn (but we lose the histogram view),
    // and why "hpx->Draw("HIST")" doesnt work alone, so calling draw on the stored fn is the way,
    // it is also not enough to just call Modified() & Update().
    
    // Format the output SetOptFit(pcef)
    gStyle->SetOptFit(111); // NOTE: param is a bit-mask (4-digit integer)
    // p = chi2 probability
    // c = chi2 and number of degrees of freedom (NDF)
    // e = Errors (standard deviations of the fitted parameters)
    // v = Values 
    // NOTE: Putting a leading 0 makes the compiler interpret the number as octal (base 8),
    // note decimal, which can cause unexpected bit settings, using 111 treats its as a decimal,
    // so avoid SetOptFit(0111), and use SetOptFit(111) instead.
    
    // Retrieve the fit chi squared & n.d.f
    double const chi2 = result->Chi2();
    double const ndf = result->Ndf();
    
    // Calculate the goodness of fit
    double const goodFit = chi2 / ndf;
    
    // Calculate the updated FWHM, based on fitted sigma
    double const fittedSigma = result->Parameter(2);
    double const fittedFWHM = fittedSigma * 2.355;
    
    printf("Peak X: %f, Peak Y: %f\n", peakX, peakY);
    printf("Goodness of Fit: %f\n", goodFit);
    
    printf("POST-FIT SIGMA: %f\n", fittedSigma);
    printf("POST-FIT FWHM: %f\n", fittedFWHM);
    
    // TODO: Display relevant fit values in top right info box post-fit (chi^2, centroid, etc)
    
    return 0;
}

/*
 * TODO:
 * 
 */
// custom centroid + FWHM for shoulders / merged peaks
// 
// is exactly how real spectroscopy software evolves.
// 
// Because eventually:
// 
// GetMaximumBin() fails for merged peaks
// automatic RMS fails on asymmetric structures
// background biases sigma
// overlapping peaks require multi-Gaussian fits
// 
// At that stage people usually move toward:
// 
// gaus(0) + gaus(3) + pol1(6)
// 
// style composite fits.



// NOTE: In spectroscopy, a very common workflow is actually:
// 
// User zooms near a peak
// Find maximum bin
// Estimate FWHM locally
// Use fit window around:
// ±2σ
// ±3σ
// or ~1.5×FWHM
// 
// That gives stable fits without too much background contamination.



// NOTE: Instead of:
// 
// hpx->GetRMS()
// 
// you may eventually prefer a local RMS estimate around the peak.
// 
// Or even simpler:
// 
// estimate sigma directly from half-max crossings.
// 
// That’s actually very spectroscopy-ish and often surprisingly robust.
// 
// Something like:
// 
// find left half-max crossing
// find right half-max crossing
// 
// FWHM = right - left
// sigma = FWHM / 2.355
// 
// Then:
// 
// fit window = peakX ± (2 * FWHM)
