// Load a histogram into memory from a '.root' file, zoom in on a peak, and fit user defined function

#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TF1.h>

// Global histo/canvas variables
TH1 *hpx = nullptr;
TCanvas *c = nullptr;

// Executes automatically on script start (NOTE: Choose another function name if you wish to manually call it instead) 
int basic_fit () {
    // Fetch and open root file
    auto *in = TFile::Open("~/geant4/geant4-v11.3.2/project/gps/build/output0.root");
    
    // Get the histogram from the root file and assign it to the TH1 pointer
    // TH1 *hpx = nullptr;
    in->GetObject("Photons;2", hpx);
    // hpx = in->Get("Photons;2");
    
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
    // auto c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    
    // Handle error creating canvas
    if (!c) {
        printf("Error: Couldnt create canvas!\n");
        return 1;
    }
    
    // Draw histogram with default option
    // hpx->DrawCopy("HIST");
    // copy = hpx->DrawCopy("HIST"); // NOTE: Could store copy in global variable and ref in range()
    hpx->Draw("HIST");
    
    return 0;
}

// Zoom in on a specific range
int range (double start, double end) {
    // Handle missing histogram/canvas
    if (!hpx || !c) {
        printf("Error: Histogram or Canvas not found!\n");
        return 1;
    }
    
    hpx->GetXaxis()->SetRangeUser(start, end);
    
    // hpx->DrawCopy("HIST"); // NOTE: If working with copy, need to call draw copy again
    
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
    
    hpx->GetXaxis()->UnZoom();
    
    c->Modified();
    c->Update();
    
    return 0;
}

// ...
int fit (int FWHM) {
    // Find the tallest point in the current histogram range (NOTE: Zoom in on peak of interest first)
    int bin = hpx->GetMaximumBin();
    double peakX = hpx->GetXaxis()->GetBinCenter(bin); // get the x-axis location of max counts bin
    double peakY = hpx->GetBinContent(bin); // get the y-axis number of counts for max bin, i.e. amplitude
    
    // Get root mean squared
    double rms = hpx->GetRMS(); 
    // NOTE: Returns the RMS of the entire visible histogram range,
    // So for:
    // - Multiple peaks
    // - Compton background
    // - long continuum tails
    // - escape peaks
    // RMS may become much larger than the actual peak width.
    // For a zoomed-in isolated peak though, is reasonable.
    
    // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    // (it’s a fit window, not a Gaussian width parameter)
    // The Gaussian itself mathematically extends to infinity.

    // double low = peakX - (3 * rms);
    // double high = peakX + (3 * rms);

    // TODO: FWHM (peakX / 2 => gives half maximum => iterate outwards from centre until bin val below half maximum) ?
    // ^ but this wont work for merged peaks, etc
    double low = peakX - (2 * FWHM);
    double high = peakX + (2 * FWHM);
    
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
    
    auto fitFn = new TF1("fitFn", "gaus", low, high);

    // ...
    double sigma = FWHM / 2.355;
    
    fitFn->SetParameters(peakY, peakX, sigma);
    // fitFn->SetParameters(peakY, peakX, rms/2.0);
    // fitFn->SetParNames("Constant", "Centroid", "FWHM");
    fitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    
    auto result = hpx->Fit(fitFn, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    
    // Draw the fit line (ROOT internally stores the fit function with the histogram after fitting)
    hpx->GetFunction("fitFn")->Draw("SAME");
    // NOTE: The "HIST" option suppresses drawing associated functions (including fits),
    // hence why "hpx->Draw()" works here instead of drawing the fit fn (but we lose the histogram view),
    // and why "hpx->Draw("HIST")" doesnt work alone, so calling draw on the stored fn is the way,
    // it is also not enough to just call Modified() & Update().
    
    printf("Peak X: %f, Peak Y: %f", peakX, peakY);
    
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
