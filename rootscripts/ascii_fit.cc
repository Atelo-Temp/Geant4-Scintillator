// Load an ASCII (.Spe) file into memory, fill a histogram, and display it on a canvas,
// then zoom in on a peak, and fit it with a user defined function

// ROOT lib
#include <TFile.h> // TODO: This is unused w/ current format, but will be used for omni_fit.cc
#include <TH1.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TROOT.h> // TODO: I think this is unused ?
#include <TStyle.h>
#include <TFitResult.h>
#include <TPaveStats.h>
#include <TLatex.h>
#include <TRandom.h> // TODO: This is unused w/ current format, but will be used for omni_fit.cc

// C lib
#include <fstream>
#include <sstream>
#include <optional>

// Global root object variables
std::ifstream in;
TH1 *hpx = nullptr;
TCanvas *c = nullptr;

/*
 * Load in plotting and fitting functions
 */
int ascii_fit() {
    // Usage
    std::cout << "\nASCII to Root Histogram\n\nTo get started, call: plot_ascii(\"path.Spe\"), passing path to ASCII (.Spe) file as param.\n\n";
    
    // fin
    return 0;
}

/*
 * Takes path as arg, validates string is valid, updates it if needed, calls draw_histo
 * 
 * TODO: Doesnt handle references to current dir, i.e. "../xyz.Spe", as delimiter is found at start
 * NOTE: Just check if first char is ".", go until "/" is found, repeat until no "." at start, continue with delimiter
 */
std::string get_path(std::string path) {
    // Print path to stdout
    std::cout << path << "\n";
    
    // Check if string is empty (returns true if string is empty)
    if (path.empty()) {
        // Error message
        std::cout << "Error: Empty string.\n";
        
        // Error value
        return "";
    }
    
    // TODO: Check if relative path is used, i.e. "./", "../", "../../", etc
    // ...
    // NOTE: Can pass file extension check by going "./somefile"
    
    // File extension delimiter
    char delimiter[2] = ".";
    
    // Get index of delimiter
    auto delimiterIdx = path.find(delimiter);
    
    // Ensure file extension delimiter is present in path (find will return -1 if not found)
    if (delimiterIdx == -1) {
        // Write to stdout
        std::cout << "Error: No file extension.\n";
        
        // Error value
        return "";
    }
    
    // Get substring from index of delimiter to the end of the string
    auto token = path.substr(delimiterIdx, path.size());
    // NOTE: 0, delimiterIdx would get everything prior to delimiter
    
    // Debug
    // std::cout << token << std::endl;
    
    // Check file ends with ".Spe", reject invalid file type
    if (token != ".Spe") {
        // Write to stdout
        std::cout << "Error: Invalid extension.\n";
        
        // Error value
        return "";
    }
    
    // THIS DOES NOT WORK
    //
    // const char* tilde = "~"; // const, as C++ does not allow conversion from string literal to char*
    // const char* char0 = &path.at(0); // cast from "char" to "const char *" with ampersand (&)
    // 
    // // Replace tilde if passed
    // if (std::strcmp(char0, tilde) == 0) { ... }
    // NOTE: char0 will be full string in this case, hence clause will not trigger

    // THIS WORKS
    //
    // char tilde[2] = "~";
    // 
    // // Get reference to character at [0], compare it to
    // if (path.at(0) == *tilde) { ... }
    
    // THIS ALSO WORKS
    //
    // std::string tilde = "~";
    // 
    // // Get reference to character at [0], compare it to
    // if (path.at(0) == tilde) { ... }

    // THIS ALSO ALSO WORKS (and prolly cleanest tbh ?)
    std::string const tilde = "~";

    // If reference to character at [0] is tilde character
    if (path[0] == tilde) {
        // Get the home path (~) from the environment variable
        char* const home = getenv("HOME");
        // std::cout << home << std::endl; // debug
        
        // Trim "~" from the start of the string (start at idx = 1, as "~" at 0)
        auto trimmedPath = path.substr(1, path.size());
        // std::cout << trimmedPath << std::endl; // debug
        
        // Update the path, replacing "~" with "/home/user" (NOTE: Not sure if this is "okay" to do), but is simple
        path = home + trimmedPath; // TODO: Not sure about updating path var directly, maybe just return new string here
        std::cout << path << "\n";
    }
    
    // No errors, all good
    return path;
}

/*
 * Validate .Spe file can be opened/exists, load it into local memory if so
 * 
 * TODO: Probably wanna validate is expected ASCII format too:
 * line 1: $SPEC_ID:
 */
int load_ascii(std::string path) {
    // Open the ASCII file with validated .Spe extension
    in.open(path);
    
    // Ensure file was found, exit with error if its not
    // NOTE: No need to reprompt, user can just call the function again
    if (!in.is_open()) {
        // Error message
        std::cout << "Error: File not found.\n";
        
        // Error
        return 1;
    };
    
    // No errors, all good
    return 0;
}

/*
 * Instantiate a ROOT histogram object
 */
int create_hist() {
    // Histogram args
    int const nbins = 2048; // 2048 channels (bins)
    int const xmin = 0; // min channel
    int const xmax = 2048; // max channel (3500 photons)
    
    // Create a histogram (TH1I = integer - channel/counts both ints)
    hpx = new TH1I(
        "hpx", // Legend title
        "optical photons", // Histo title
        nbins, // num bins
        xmin, // x low
        xmax // x up
    );
    // NOTE: TH1I works while num photons is int, but may need long64 (TH1L) for gain applied num photons,
    // or TH1F (float - 4 bytes) / TH1D (double - 8 bytes) if using floating point values
    
    // Handle missing histogram
    if (!hpx) {
        printf("Error (draw_hist()): Histogram not found!\n");
        return 1;
    }
    
    // X-axis title
    hpx->SetXTitle("Channels");
    
    // No errors, all good
    return 0;
}

/*
 * Iterate through ASCII file, populating histogram with per-bin values
 * 
 * TODO: Dont start from arbitrary line 13 and go until line 2060 
 * (parse the infile header for start, $DATA, then skip next line, then the following line is bin 0)
 * (when you read $ROI, break)
 */
int draw_hist() {
    // Handle missing input file
    if (!in) {
        printf("Error: No infile to read!\n");
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        printf("Error (draw_hist()): Histogram not found!\n");
        return 1;
    }
    
    // Line counter
    int currentLine = 0;

    // ...
    std::string line;
    
    // Histo bin counter
    int currentBin = 0;
    
    // Get line reads a line from input stream into a string, until end of stream encountered
    while (std::getline(in, line)) {
        // Print line number
        // std::cout << nlines << std::endl;
        
        // Print each line
        // std::istringstream iss(line);
        // std::string a;
        // if (!(iss >> a)) break;
        // std::cout << a << std::endl;        
       
        // Increment line counter
        currentLine++;
        
        // Print
        std::istringstream stringStream(line);
        std::string lineContent; // contains current line string
        
        // ...
        if (!(stringStream >> lineContent)) break;
        
        // Only parse lines 13-2060 (TODO: FIX THIS HARDCODED SLOP)
        if (currentLine >= 13 && currentLine <= 2060) {
            // Debug
            // std::cout << a << std::endl;
            // std::cout << stoi(a) << std::endl;
            
            // Convert string to integer
            int const converted = stoi(lineContent);
            
            // Set current bin to the integer value on current line
            hpx->SetBinContent(currentBin, converted);
            // NOTE: Dont use h->Fill(converted), Instead of filling bin 0 with line 0,
            // its filling bin 0 every time 0 is encountered
            
            // Increment bin counter
            currentBin++;
        }
    }
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    in.close();
    
    // No errors, all good
    return 0;
}

/*
 * ...
 */
int create_canvas() {
    // Canvas args
    Int_t const winX = 0; // Top left of screen
    Int_t const winY = 0; // Top left of screen
    Int_t const width = 1200;
    Int_t const height = 800;
    
    // Create a canvas display
    c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    
    // Handle error creating canvas
    if (!c) {
        printf("Error: Couldnt create canvas!\n");
        return 1;
    }
    
    // TODO: \/\/\/\/\/\/\/\/\/ MAYBE EXTRACT THIS OUT TBH
    
    // Handle missing histogram
    if (!hpx) {
        printf("Error (create_canvas()): Histogram not found!\n");
        return 1;
    }
    
    // Draw histogram to the canvas with default option
    // hpx->Draw("HIST");
    hpx->Draw(); // NOTE: With ntuples, "HIST" no longer needed afaik
    
    // ...
    c->Update(); // NOTE: Afaik, this is not needed
    
    // Clean the default histogram statistics box (498.4, 291.1)
    gStyle->SetOptStat(0); // default = 1111 (NOTE: 000001111 with zeros removed)
    // 0 = hides the statistics box entirely (leaving only fit box when fitted)
    // 10 = only number of entries
    // 110 = entries and mean
    // NOTE: Prefix zeros must be removed, as "01" is treated as octal number
    
    // No errors, all good
    return 0;
}

/*
 * Executes automatically on script start (NOTE: Choose another function name if you wish to manually call it instead) 
 */
int plot_ascii (std::string fileName) {
    // Check provided path is valid (will return empty string if not valid)
    std::string path = get_path(fileName);
    
    if (path.empty()) {
        printf("Aborting: Invalid path error!\n");
        return 1;
    }
    
    // Attempt to load the ASCII into memory
    int const fileError = load_ascii(path);
    
    if (fileError) {
        printf("Aborting: Load file error!\n");
        return 1;
    }
    
    // Attempt to instantiate histogram object
    int const histError = create_hist();
    
    if (histError) {
        printf("Aborting: Create hist error!\n");
        return 1;
    }
    
    // Attempt to populate histogram from ASCII file
    int const drawError = draw_hist();
    
    if (drawError) {
        printf("Aborting: Draw hist error!\n");
        return 1;
    }
    
    // Attempt to create canvas and paint the histogram
    int const canvasError = create_canvas();
    
    if (canvasError) {
        printf("Aborting: Create canvas error!\n");
        return 1;
    }
    
    // No errors, all good
    return 0;
}

/*
 * Zoom in on a specific range
 */
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
    
    // No errors, all good
    return 0;
}

/*
 * Zoom back out to the full view histogram (NOTE: Could also call range(0,0))
 */
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
    
    // No errors, all good
    return 0;
}

/*
 * Calculate integrated counts under the fit curve, and errors
 */
std::optional<std::vector<double>> getCounts() {
    // Handle missing histogram
    if (!hpx) {
        printf("Error (getCounts()): Histogram not found!\n");
        return std::nullopt;
    }
    
    // Retrieve the fit function
    TF1* fit = hpx->GetFunction("refitFn");
    
    // Handle missing function
    if (!fit) {
        printf("Error: No such fit function found.\n");
        return std::nullopt;
    }
    
    // Retrieve the fit window set during fitting
    double xmin;
    double xmax;
    fit->GetRange(xmin, xmax);
    // NOTE: These will be the lower/upper bounds of the integration
    
    // Get bin width for bin 1 (NOTE: could be any of the 1024 bins, theyre all same width)
    double binWidth = hpx->GetBinWidth(1);
    // NOTE: ROOT histograms store counts per bin, the integral of a function is in
    // units of: Energy * Counts (technically: photons * counts, with current setup), 
    // dividing by the Energy/Bin (kev/Bin), or in this case photons/Bin, converts
    // it back to pure counts
    
    // Calculate integral for the area under the fitted gaussian curve
    double area = fit->Integral(xmin, xmax); // NOTE: Is num photons in current form
    double totalCounts = area / binWidth;
    
    // Get the error (calculated as: sqrt(totalCounts), in counting statistics)
    double countsError = fit->IntegralError(xmin, xmax) / binWidth;
    // NOTE: Must also be divided by bin width, otherwise it would be:
    // +/- num photons, rather than counts
    
    printf("COUNTS: %.2f, ERROR [SQRT(COUNTS)]: +/-%.2f\n", totalCounts, countsError);
    
    std::vector<double> countResults = {totalCounts, countsError};

    return countResults;
    
    // return std::vector<double> {totalCounts, countsError};
    
    // TODO: Total area - background area (for lab spectra with background)
}

/*
 * Display relevant fit values in top right info box post-fit (chi^2, centroid, etc)
 */
int drawFitStats(TFitResultPtr const &result, std::vector<double> const &counts) {
    // Format the output SetOptFit(pcev)
    gStyle->SetOptFit(111); // NOTE: param is a bit-mask (4-digit integer)
    // p = chi2 probability
    // c = chi2 & number of degrees of freedom (NDF)
    // e = Errors (standard deviations of the fitted parameters)
    // v = Values (name/values of params)
    // NOTE: Putting a leading 0 makes the compiler interpret the number as octal (base 8),
    // note decimal, which can cause unexpected bit settings, using 111 treats its as a decimal,
    // so avoid SetOptFit(0111), and use SetOptFit(111) instead.
    
    // Retrieve the fit chi squared & n.d.f
    double const chi2 = result->Chi2();
    double const ndf = result->Ndf();
    
    // Calculate the goodness of fit
    double const goodFit = chi2 / ndf;
    
    printf("Goodness of Fit: %f\n", goodFit);
    
    // Calculate the updated FWHM, based on fitted sigma
    double const fittedSigma = result->Parameter(2);
    double const fittedFWHM = fittedSigma * 2.355;
    
    printf("POST-FIT SIGMA: %f\n", fittedSigma);
    printf("POST-FIT FWHM: %f\n", fittedFWHM);
    
    // Calculate the error on the fitted FWHM, based on fitted sigma error
    double const fittedSigmaError = result->Error(2);
    double const fittedFWHMError = fittedSigmaError * 2.355;
    
    // Get integrated photopeak counts and error on counts from input vector
    auto countsVal = counts[0];
    auto countsErr = counts[1];
    
    // TEST
    // c->Update(); // NOTE: This can be omitted with the working combo
    // gPad->Update(); // ^ same
    // hpx->SetStats(0); // this is called later, might be wiggle room in location, but if it aint broke...
    
    // Handle missing histogram
    if (!hpx) {
        printf("Error (drawFitStats()): Histogram not found!\n");
        return 1;
    }
    
    // Get a pointer to the stats box object
    // auto ps = (TPaveStats*)(hpx->FindObject("stats"));
    // auto ps = reinterpret_cast<TPaveStats*>(hpx->FindObject("stats"));
    auto ps = dynamic_cast<TPaveStats*>(hpx->FindObject("stats")); // NOTE: Works w/ combo
    // auto ps = dynamic_cast<TPaveStats*>(hpx->GetListOfFunctions()->FindObject("stats")); // NOTE: Works w/ combo
    // auto ps = dynamic_cast<TPaveStats*>(c->GetPrimitive("stats")); // NOTE: Works w/ combo
    // NOTE: Casting the ambiguous return type of find object (TObject*), to TPaveStats object
    
    // Handle missing stats box
    if (!ps) {
        printf("Error: Stats box object not found.\n");
        // Error("stats", "Stats not found");
        return 1;
    }
    
    // ps->SetX1NDC(0);
    // ps->SetX2NDC(1000);
    
    // Detach from root auto management (need to use when doing set stats 0 and ps draw)
    ps->SetName("mystats"); // NOTE: Without this, get segmentation violation (segfault)
    
    // Get existing statistics box content
    TList* listOfLines = ps->GetListOfLines();
    
    // Remove amplitude and sigma from the stat box? (kinda like amplitude being shown tho)
    auto line1 = ps->GetLineWith("Sigma");
    listOfLines->Remove(line1);

    // Add FWHM (+/- error) to the stats box
    char* text1 = Form("FWHM = %.2f #pm %.2f", fittedFWHM, fittedFWHMError); // Format the entry (#pm generates +/-)
    auto newLine1 = new TLatex(0, 0, text1); // <- may have to do Form() for string
    newLine1->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
    newLine1->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
    listOfLines->Add(newLine1); // append the fwhm value & error to the fit stats
    
    // Add counts (+/- error) to the stats box
    char* text2 = Form("Counts = %.2f #pm %.2f", countsVal, countsErr);
    auto newLine2 = new TLatex(0, 0, text2);
    newLine2->SetTextFont(gStyle->GetStatFont());
    newLine2->SetTextSize(gStyle->GetStatFontSize());
    listOfLines->Add(newLine2);
    
    // Display the custom statistics box
    hpx->SetStats(0); // Disable auto future stats regeneration
    ps->Draw(); // Redraw custom box
    // c->Update(); // this can actually be omitted too
    
    // No errors, all good
    return 0;
}

/*
 * Manually input estimated photopeak centroid and FWHM values, fit a gaussian to it, and display the fit
 * 
 * NOTE: While errors may not be reduced via the refit, it is not necessarily redundant,
 * as it still ensures the low/high window is centred on the centroid
 * (not the rough centroid passed as a param to the fn)
 */
int fit (int const roughCentroid, int const roughFWHM) {
    // Log input params to stdout
    printf("Centroid Channel: %d, FWHM (channels): %d\n", roughCentroid, roughFWHM);
    
    // Handle missing histogram
    if (!hpx) {
        printf("Error (fit()): Histogram not found!\n");
        return 1;
    }
    
    // Get the centre of the centroid channel, and number of counts in centroid bin
    double const peakX = hpx->GetXaxis()->GetBinCenter(roughCentroid); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(roughCentroid); // get the y-axis number of counts for max bin, i.e. amplitude
    // NOTE: Since bin center takes actual bin, need the conversion from photons to bins
    // but centroid & FWHM values will reflect the photons
    
    printf("Peak X: %f, Peak Y: %f\n", peakX, peakY);
    
    // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    // (it’s a fit window, not a Gaussian width parameter)
    // The Gaussian itself mathematically extends to infinity.
    double const roughLow = peakX - (2.5 * roughFWHM); // NOTE: Generous 2.5 for centroid finding
    double const roughHigh = peakX + (2.5 * roughFWHM); // TODO: ^ May want to be more conservative if fitting overlapping peaks though
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
    auto prefitFn = new TF1("prefitFn", "gaus", roughLow, roughHigh);
    // NOTE: "gaus" is built-in ROOT shorthand for [0]*exp(-0.5*((x-[1])/[2])^2)
    
    // NOTE: "gaus(0)" is functionally the same, and can be abbreviated to "gaus" 
    // when using a single fitting function, however, if you were to use two functions
    // "gaus(0) + gaus(3)" ensures that the subsequent call to set parameters assigns
    // passed parameters to the correct function (see below)

    // Instead of relying on automatic RMS, which is not reliable for merged peaks etc,
    // require the user to state a rough FWHM value deduced by eye, and derive sigma from it
    double const roughSigma = roughFWHM / 2.355;
    printf("PRE-FIT SIGMA: %f\n", roughSigma);
    
    // Pass the parameters required for the gaussian fit function
    prefitFn->SetParameters(peakY, peakX, roughSigma);
    prefitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    // NOTE: [0] = Amplitude, [1] = Mean, [2] = Sigma
    
    // NOTE: If we defined "gaus(0) + gaus(3)", we would need to pass separate params, i.e.:
    // fitFn->SetParameters(peakY1, peakX1, sigma1, peakY2, peakX2, sigma2);
    // hence, params: [0], [1], [2], are used for the first gaussian function,
    // and params: [3], [4], [5], are used for the second gaussian function
    // the same applies for "gaus(0) + pol1(3)",
    // where params: [4] & [5] are then the intercept and slope for the poly fit
    
    // Call the histograms fit method, passing the fit function and histogram fitting options string
    TFitResultPtr initialResult = hpx->Fit(prefitFn, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = attempts to improve the fit quality
    // "L" = use log likelihood method (default chi-square), for use with counts histograms
    // "+" = adds this new fitted func to list of fitted funcs (default is delete previous keep last)
    
    // Handle fit error (NOTE: success = 0)
    if (initialResult->Status() != 0) {
        std::cerr << "Failed to perform initial fit!" << std::endl;
        return 1;
    }
    
    // Extract the initial fit results
    double const prefitAmplitude = initialResult->Parameter(0);
    double const prefitCentroid = initialResult->Parameter(1);
    double const prefitSigma = initialResult->Parameter(2);
    double const prefitFWHM = prefitSigma * 2.335;
    
    // Base the fit window around the true centroid to avoid lopsidedness in the fit
    double const low = prefitCentroid - (2 * prefitFWHM);
    double const high = prefitCentroid + (2 * prefitFWHM);
    
    // Define the fit function which will take refined parameters
    auto refitFn = new TF1("refitFn", "gaus", low, high);
    
    // Perform a refit using the fitted params
    refitFn->SetParameters(prefitAmplitude, prefitCentroid, prefitSigma);
    refitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    
    // Calculate the results of the refit
    TFitResultPtr refitResult = hpx->Fit(refitFn, "RS");
    
    // Handle refit error
    if (refitResult->Status() != 0) {
        std::cerr << "Failed to perform refit!" << std::endl;
        return 1;
    }
    
    // Draw the fit line (ROOT internally stores the fit function with the histogram after fitting)
    hpx->GetFunction("refitFn")->Draw("SAME");
    // NOTE: The "HIST" option suppresses drawing associated functions (including fits),
    // hence why "hpx->Draw()" works here instead of drawing the fit fn (but we lose the histogram view),
    // and why "hpx->Draw("HIST")" doesnt work alone, so calling draw on the stored fn is the way,
    // it is also not enough to just call Modified() & Update().
    
    // Calculate counts in the fitted photopeak via integration
    auto counts = getCounts();
    // NOTE: Counts is not a std::vector<double> yet, it is still optional type
    
    // Handle nullopt return
    if (!counts) {
        std::cerr << "Failed to get counts!" << std::endl;
        return 1;
    }
    // NOTE: Again this does not tell the compiler it is not optional
    
    // Handle statistics box and write custom value to it
    int statsError = drawFitStats(refitResult, *counts);
    // drawFitStats(refitResult, counts.value());
    // NOTE: Here *counts, or counts.value(), converts the optional type to a vector
    
    // Handle statistics drawing error
    if (statsError) {
        std::cerr << "Failed draw fit statistics!" << std::endl;
        return 1;
    }
    
    // No errors, all good
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
