// Load an ASCII (.Spe) or ROOT (.root) file into memory, fill a histogram, and display it on a canvas

// ROOT lib
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TROOT.h> // TODO: I think this is unused ?
#include <TStyle.h>
#include <TFitResult.h>
#include <TPaveStats.h>
#include <TLatex.h>
#include <TRandom.h>

#include <TKey.h>

// C lib
#include <fstream>
#include <sstream>

#include <stdexcept>
#include <unordered_set>

// Global root object variables
std::ifstream inASCII;
TFile* inROOT = nullptr;
TTree* nTuple = nullptr;
TH1 *hpx = nullptr;
TCanvas *c = nullptr;

// Accepted file types
enum class FileType {
    ROOT,
    ASCII
};

// Active file type flag
FileType fileType;

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int exponential_fit() {
    // Usage
    std::cout << "\n-----------------------------------------------------------------------\n";
    std::cout << "\nConvert ASCII & ROOT Ntuples to Root Histogram.\n\nTo get started, call: plot(\"path.ext\"), passing path to ASCII (.Spe) or ROOT (.root) file as param.\n";
    std::cout << "\nTo save the plotted histogram, call save(\"dir/out.root\").\n";
    std::cout << "\n-----------------------------------------------------------------------\n";
    
    // fin
    return 0;
}

/*
 * Automatically find exponential centroid, derive hist low/high, fit convolution of 
 * exponential rise and decay to the data, and display the fit
 * 
 * TODO: This didnt quite fit to my data, but it may come in handy at a later date
 * 
 * TODO: Define a top level: fit(), method, which directs you to exponential, double_exponential, etc, etc
 * 
 * TODO: This needs updating with some of the updated error handling from ascii_fit.cc
 */
int rise_and_decay() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
    // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    // (it’s a fit window, not a Gaussian width parameter)
    // The Gaussian itself mathematically extends to infinity.    
    double const low = hpx->GetXaxis()->GetXmin();
    double const high = hpx->GetXaxis()->GetXmax();
    
    // Automate rise time (tau_rise)
    // NOTE: Estimate rise time by looking backwards to 10% of the peak
    int riseBin = maxBin;
    
    // While current bin greater than min bin, and current bin value still greater than 10% of the peak
    while ((riseBin > 1) && (hpx->GetBinContent(riseBin) > (peakY * 0.1))) {
        riseBin--;
    }
    
    // ...
    double x_rise = hpx->GetBinCenter(riseBin); // grab center of rise bin
    double estimated_tau_rise = (peakX - x_rise) * 0.5; // Rise is usually sharper // TODO: Comments
    
    if (estimated_tau_rise <= 0) estimated_tau_rise = 1.; // Fallback safety
    
    // Automate decay constant (tau_fall)
    // NOTE: Find the point where the signal drops to 37% (1/e) of its peak after the max
    int fallBin = maxBin;
    
    // While current bin less than max bins, and current bin value still greater than 1/e of the peak
    while ((fallBin < hpx->GetNbinsX()) && (hpx->GetBinContent(fallBin) > peakY * 0.368)) {
        fallBin++;
    }
    
    // ...
    double x_fall = hpx->GetBinCenter(fallBin);
    double estimated_tau_fall = x_fall - peakX; // TODO: Comments
    
    if (estimated_tau_fall <= 0) estimated_tau_fall = 10.0; // Fallback safety
    
    // Define and initialise double exponential fit (fast rise component & slow fall component)
    auto pulseFit = new TF1("fitFn", "[0]*(exp(-(x-[1])/[3]) - exp(-(x-[1])/[2]))", low, high);
    // A*(exp(-(x-[x_rise])/[tau_fall]) - exp(-(x-[x_rise])/[tau_rise]))
    // Where: A = amplitude, x_rise = delay time, tau_rise = rise time constant, tau_fall = decay time constant
    
    // ...
    // pulseFit->SetParameters(peakY, x_rise, estimated_tau_rise, estimated_tau_fall);
    pulseFit->SetParameters(peakY, 0, estimated_tau_rise, estimated_tau_fall);
    pulseFit->SetParNames("Amplitude", "t0", "tau_rise", "tau_fall");

    // Enforce limits so the fitter doesnt swap rise and fall constants
    pulseFit->SetParLimits(2, 0.01, estimated_tau_fall);
    pulseFit->SetParLimits(3, estimated_tau_fall * 0.1, estimated_tau_fall * 10.);

    // ...
    auto result = hpx->Fit(pulseFit, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = improves the fit quality
    
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
 * Automatically find exponential centroid, derive hist low/high, fit convolution of 
 * exponential rise and decay to the data, and display the fit
 * 
 * TODO: This didnt quite fit to my data, but it may come in handy at a later date
 * 
 * TODO: Define a top level: fit(), method, which directs you to exponential, double_exponential, etc, etc
 * 
 * TODO: This needs updating with some of the updated error handling from ascii_fit.cc
 */
int double_exponential_fit() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
    // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    // (it’s a fit window, not a Gaussian width parameter)
    // The Gaussian itself mathematically extends to infinity.    
    double const low = hpx->GetXaxis()->GetXmin();
    double const high = hpx->GetXaxis()->GetXmax();
    
    
    // Automate rise time (tau_rise)
    // NOTE: Estimate rise time by looking backwards to 10% of the peak
    int riseBin = maxBin;
    
    // While current bin greater than min bin, and current bin value still greater than 10% of the peak
    while ((riseBin > 1) && (hpx->GetBinContent(riseBin) > (peakY * 0.1))) {
        riseBin--;
    }
    
    // ...
    double x_rise = hpx->GetBinCenter(riseBin); // grab center of rise bin
    double estimated_tau_rise = (peakX - x_rise) * 0.5; // Rise is usually sharper // TODO: Comments
    
    if (estimated_tau_rise <= 0) estimated_tau_rise = 1.; // Fallback safety
    
    
    // Automate decay constant (tau_fall)
    // NOTE: Find the point where the signal drops to 37% (1/e) of its peak after the max
    int fallBin = maxBin;
    
    // While current bin less than max bins, and current bin value still greater than 1/e of the peak
    while ((fallBin < hpx->GetNbinsX()) && (hpx->GetBinContent(fallBin) > peakY * 0.368)) {
        fallBin++;
    }
    
    // ...
    double x_fall = hpx->GetBinCenter(fallBin);
    double estimated_tau_fall = x_fall - peakX; // TODO: Comments
    
    if (estimated_tau_fall <= 0) estimated_tau_fall = 10.0; // Fallback safety
    
    // Define and initialise double exponential fit (fast rise component & slow fall component)
    auto pulseFit = new TF1("fitFn", "[0]*(exp(-(x-[1])/[3]) - exp(-(x-[1])/[2]))", low, high);
    // A*(exp(-(x-[x_rise])/[tau_fall]) - exp(-(x-[x_rise])/[tau_rise]))
    // Where: A = amplitude, x_rise = delay time, tau_rise = rise time constant, tau_fall = decay time constant
    
    // ...
    pulseFit->SetParameters(peakY, x_rise, estimated_tau_rise, estimated_tau_fall);
    pulseFit->SetParNames("Amplitude", "t0", "tau_rise", "tau_fall");

    // Enforce limits so the fitter doesnt swap rise and fall constants
    pulseFit->SetParLimits(2, 0.01, estimated_tau_fall);
    pulseFit->SetParLimits(3, estimated_tau_fall * 0.1, estimated_tau_fall * 10.);

    // ...
    auto result = hpx->Fit(pulseFit, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = improves the fit quality
    
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
 * ...
 */
int fit_log_normal () {
    return 1;
}

/*
 * ...
 */
int ex_gaussian () {
    return 1;
}

/*
 * ...
 */
int gamma_dist () {
    return 1;
}

/*
 * TODO: Needs faster rise time to fit to the photon distance travelled data
 * 
 * 1) Log normal
 * 2) Gamma Distribution
 * 3) ExGaussian
 */
int fit() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
    // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    // (it’s a fit window, not a Gaussian width parameter)
    // The Gaussian itself mathematically extends to infinity.    
    double const low = hpx->GetXaxis()->GetXmin();
    double const high = hpx->GetXaxis()->GetXmax();
    
    
    // Automate rise time (tau_rise)
    // NOTE: Estimate rise time by looking backwards to 10% of the peak
    int riseBin = maxBin;
    
    // While current bin greater than min bin, and current bin value still greater than 10% of the peak
    while ((riseBin > 1) && (hpx->GetBinContent(riseBin) > (peakY * 0.1))) {
        riseBin--;
    }
    
    // ...
    double x_rise = hpx->GetBinCenter(riseBin); // grab center of rise bin
    double estimated_tau_rise = (peakX - x_rise) * 0.5; // Rise is usually sharper // TODO: Comments
    
    if (estimated_tau_rise <= 0) estimated_tau_rise = 1.; // Fallback safety
    
    
    // Automate decay constant (tau_fall)
    // NOTE: Find the point where the signal drops to 37% (1/e) of its peak after the max
    int fallBin = maxBin;
    
    // While current bin less than max bins, and current bin value still greater than 1/e of the peak
    while ((fallBin < hpx->GetNbinsX()) && (hpx->GetBinContent(fallBin) > peakY * 0.368)) {
        fallBin++;
    }
    
    // ...
    double x_fall = hpx->GetBinCenter(fallBin);
    double estimated_tau_fall = x_fall - peakX; // TODO: Comments
    
    if (estimated_tau_fall <= 0) estimated_tau_fall = 10.0; // Fallback safety
    
    
    // Define the fit function
    // auto fitFn = new TF1("fitFn", "expo", low, high);
    // auto fitFn = new TF1("fitFn", "[0]*exp(-x/[1])", low, high); // exponential decay
    // NOTE: "gaus" is built-in ROOT shorthand for [0]*exp(-x/[1])
    
    // ..
    // fitFn->SetParameters(peakY, 20.0);
    // fitFn->SetParNames("Amplitude", "Decay Constant");
    
    
    // Define and initialise double exponential fit (fast rise component & slow fall component)
    auto pulseFit = new TF1("fitFn", "[0]*(exp(-(x-[1])/[3]) - exp(-(x-[1])/[2]))", low, high);
    
    // ...
    pulseFit->SetParameters(peakY, x_rise, estimated_tau_rise, estimated_tau_fall);
    pulseFit->SetParNames("Amplitude", "t0", "tau_rise", "tau_fall");

    // Enforce limits so the fitter doesnt swap rise and fall constants
    pulseFit->SetParLimits(2, 0.01, estimated_tau_fall);
    pulseFit->SetParLimits(3, estimated_tau_fall * 0.1, estimated_tau_fall * 10.);

    // ...
    auto result = hpx->Fit(pulseFit, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = improves the fit quality
    
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
