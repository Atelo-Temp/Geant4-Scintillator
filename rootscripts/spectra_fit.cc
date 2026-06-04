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
#include <string>
// #include <algorithm>

// Global root object variables
std::ifstream in;
TH1* hpx = nullptr;
TCanvas* canvas = nullptr;

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int spectra_fit() {
    // Usage
    std::cout << "\n-----------------------------------------------------------------------\n";
    std::cout << "\nConvert ASCII to Root Histogram.\n";
    std::cout << "\nTo get started, call: plot(\"~/dir/filename.Spe\"), passing path to ASCII (.Spe) file as param.\n";
    std::cout << "\nTo fit the plotted histogram, call fit().\n";
    std::cout << "\n-----------------------------------------------------------------------\n";
    
    // fin
    return 0;
}

/*
 * Takes path as arg, validates string is valid, updates it if needed, calls draw_histo
 * 
 * NOTE: Can pass file extension check by going "./somefile"
 * TODO: rfind("/"), to get just the file name, before attempting to find file extension,
 * makes checking for file extensions bit cleaner
 * 
 * TODO: Handle filenames such as .gitignore ?
 * else if (extDelimiterIdx == 0) {} 
 * NOTE: Kinda dont need to with ext check tho
 * 
 * TODO: Not sure about updating path var directly in tilde expansion, maybe just return 
 * new string in that enclosure
 * 
 * NOTE: arg[0]: std::string const path (so its immutable)
 */
std::string check_path(std::string const& path) { // std::string check_path(std::string const path) {
    // Print path to stdout
    std::cout << "\nUser provided path: " << path << "\n";
    
    // Check if string is empty (returns true if string is empty)
    if (path.empty()) {
        // Error message
        std::cerr << "\nError [check_path()]: Empty string.\n";
        
        // Error value
        return "";
    }
    
    // Directory delimiter
    std::string const dirDelimiter = "/"; // char const
    
    // Find index of last directory delimiter
    size_t const dirDelimiterIdx = path.rfind(dirDelimiter);
    // NOTE: rfind works from right to left
    
    // ...
    std::string fileName;
    
    // If path provided had no directory delimiter (i.e. file in same dir as pwd)
    if (dirDelimiterIdx == -1) {
        // Use full path for extension check
        fileName = path;
    } 
    // If path had directory delimiter
    else {
        // Use only the file name for extension check
        fileName = path.substr(dirDelimiterIdx);
    }
    
    // File extension delimiter
    std::string const extDelimiter = "."; // char const
    
    // Get index of delimiter
    size_t const extDelimiterIdx = fileName.rfind(extDelimiter);
    // NOTE: "find()" searches from left to right, so if the path contains any period
    // in directory names or earlier in the file name, it will return the index of that 
    // instead of the desired period before the file extension, to solve this:
    // "rfind()" searches from right to left, and so always returns index of final period
    
    // Ensure file extension delimiter is present in path (find will return -1 if not found)
    if (extDelimiterIdx == -1) {
        // Write to stdout
        std::cerr << "\nError: No file extension.\n";
        
        // Error value
        return "";
    }
    
    // Get substring from index of delimiter to the end of the string
    std::string const token = fileName.substr(extDelimiterIdx); // (extDelimiterIdx, path.size())
    // NOTE: Can omit path.size() as second param, defaults to end of string
    // NOTE: 0, delimiterIdx would get everything prior to delimiter
    
    // Debug
    // std::cout << token << std::endl;
    
    // Acceptable file extensions
    std::string const spe = ".Spe"; // const char*
    std::string const root = ".root"; // const char*
    
    // Check path ends with valid extension, reject invalid file type
    if (token != spe) {
        // Write to stdout
        std::cerr << "\nError: Invalid extension.\n";
        
        // Error value
        return "";
    }

    // Replace tilde if passed
    std::string const tilde = "~"; // char tilde[2]
    
    // Mutable path
    std::string returnPath = path;

    // If reference to character at [0] is tilde character
    if (path[0] == tilde) {
        // Get the home path (~) from the environment variable
        char const* home = getenv("HOME");
        // std::cout << home << std::endl; // debug
        
        // Trim "~" from the start of the string (start at idx = 1, as "~" at 0)
        std::string const trimmedPath = path.substr(1, path.size());
        // std::cout << trimmedPath << std::endl; // debug
        
        // Update the path, replacing "~" with "/home/user"
        returnPath = home + trimmedPath;
        
        // Tilde expansion was successful
        std::cout << "\nPath has been expanded: " << returnPath << "\n";
        // NOTE: This is now an absolute path
    }
    
    // Confirmation status
    std::cout << "\nPath is valid.\n";
    
    // No errors, all good
    return returnPath;
}

/*
 * Validate .Spe file can be opened/exists, load it into local memory if so
 * 
 * TODO: Probably wanna validate is expected ASCII format too:
 * line 1: $SPEC_ID:
 */
int load_ascii(std::string const& path) {
    // Open the ASCII file with validated .Spe extension
    in.open(path);
    
    // Ensure file was found, exit with error if its not
    // NOTE: No need to reprompt, user can just call the function again
    if (!in.is_open()) {
        // Error message
        std::cerr << "\nError: File not found.\n";
        
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
        "Energy Spectrum", // Histo title
        nbins, // num bins
        xmin, // x low
        xmax // x up
    );
    // NOTE: TH1I works while num photons is int, but may need long64 (TH1L) for gain applied num photons,
    // or TH1F (float - 4 bytes) / TH1D (double - 8 bytes) if using floating point values
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (create_hist()): Histogram not found!\n";
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
int fill_hist() {
    // Handle missing input file
    if (!in.is_open()) {
        std::cerr << "\nError: No infile to read!\n";
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (fill_hist()): Histogram not found!\n";
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
    in.clear();
    
    // No errors, all good
    return 0;
}

/*
 * Instantiates a canvas object, caches the address in a global pointer
 */
int create_canvas() {
    // Canvas args
    Int_t const winX = 0; // Top left of screen
    Int_t const winY = 0; // Top left of screen
    Int_t const width = 1200;
    Int_t const height = 800;
    
    // Create a canvas display
    canvas = new TCanvas("c", "Spectrum", winX, winY, width, height);
    
    // Handle error creating canvas
    if (!canvas) {
        std::cerr << "\nError: Couldnt create canvas!\n";
        return 1;
    }
    
    // No errors, all good
    return 0;
}

/*
 * Renders the populated histogram object on to the instantiated canvas
 */
int render_hist() {
    // Handle error creating canvas
    if (!canvas) {
        std::cerr << "\nError (render_hist()): Couldnt find canvas!\n";
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (render_hist()): Histogram not found!\n";
        return 1;
    }
    
    // ...
    std::cout << "\nRendering histogram to canvas...\n";
    
    // Draw histogram to the canvas with default option
    hpx->Draw(); // NOTE: "HIST" not needed when instantiating TH1 ourselves

    // ...
    canvas->Update(); // NOTE: Afaik, this is not needed
    // gPad->Update(); // Make sure the statistics box is created
    // NOTE: Without gPad update, FindObject("stats") may sometimes return null pointer (leading to undefined behaviour)
    
    // Clean the default histogram statistics box (498.4, 291.1)
    gStyle->SetOptStat(0); // default = 1111 (NOTE: 000001111 with zeros removed)
    // 0 = hides the statistics box entirely (leaving only fit box when fitted)
    // 10 = only number of entries
    // 110 = entries and mean
    // NOTE: Prefix zeros must be removed, as "01" is treated as octal number
    
    // ...
    std::cout << "\nHistogram rendered to canvas.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Executes automatically on script start (NOTE: Choose another function name if you wish to manually call it instead) 
 */
int plot (std::string const fileName) {
    // Check provided path is valid (will return empty string if not valid)
    std::string path = check_path(fileName);
    
    if (path.empty()) {
        std::cerr << "\nAborting: Invalid path error!\n";
        return 1;
    }
    
    // Attempt to load the ASCII into memory
    int const fileError = load_ascii(path);
    
    if (fileError) {
        std::cerr << "\nAborting: Load file error!\n";
        return 1;
    }
    
    // Attempt to instantiate histogram object
    int const histError = create_hist();
    
    if (histError) {
        std::cerr << "\nAborting: Create hist error!\n";
        return 1;
    }
    
    // Attempt to populate histogram from ASCII file
    int const drawError = fill_hist();
    
    if (drawError) {
        std::cerr << "\nAborting: Draw hist error!\n";
        return 1;
    }
    
    // Attempt to create canvas
    int const canvasError = create_canvas();
    
    if (canvasError) {
        std::cerr << "\nAborting: Create canvas error!\n";
        return 1;
    }
    
    // Attempt to draw the histogram to the canvas
    int const renderError = render_hist();
    
    if (renderError) {
        std::cerr << "\nAborting: Render histogram error!\n";
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
    if (!hpx || !canvas) {
        std::cerr << "\nError: Histogram or Canvas not found!\n";
        return 1;
    }
    
    // Set lower/upper canvas view range
    hpx->GetXaxis()->SetRangeUser(start, end);
    
    // Notify canvas of update
    canvas->Modified();
    canvas->Update();
    
    std::cout << "Zoomed to: " << start << " - " << end << "\n";
    
    // No errors, all good
    return 0;
}

/*
 * Zoom back out to the full view histogram (NOTE: Could also call range(0,0))
 */
int reset () {
    // Handle missing histogram/canvas
    if (!hpx || !canvas) {
        std::cerr << "\nError: Histogram or Canvas not found!\n";
        return 1;
    }
    
    // Resets canvas view range
    hpx->GetXaxis()->UnZoom();
    
    // Notify canvas of update
    canvas->Modified();
    canvas->Update();
    
    // No errors, all good
    return 0;
}

/*
 * Manually input estimated photopeak centroid and FWHM values, fit a gaussian 
 * to it, and display the fit
 * 
 * NOTE: While errors may not be reduced via the refit, it is not necessarily 
 * redundant, as it still ensures the low/high window is centred on the centroid
 * (instead of +/- the rough centroid passed as a param to the fn)
 * 
 * TODO: This is likely insufficient for closely merged peaks, unless accurate
 * "rough" centroid & fwhm is passed, consider ways to refine it
 * 
 * TODO: Not sure about using "+" flag on calls to fit(), will end up with a lot
 * of functions on the hist object, but perhaps not a big deal, and maybe useful
 * in the future for one reason or another
 * 
 * TODO: Explore log likelihood fit method ("L" flag passed to hpx->fit()), is 
 * potentially better for energy spectra
 */
std::optional<TFitResultPtr> fit_individual(int const& roughCentroid, int const& roughFWHM, int const& peakNum) {
    // Log input params to stdout
    std::cout << "Rough Centroid Arg: " << roughCentroid << " Rough FWHM: " << roughFWHM << "\n";
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (fit_individual()): Histogram not found!\n";
        return std::nullopt;
    }
    
    // Get the centre of the centroid channel, and number of counts in centroid bin
    double const roughMean = hpx->GetXaxis()->GetBinCenter(roughCentroid); // get the x-axis location of max counts bin
    double const roughAmplitude = hpx->GetBinContent(roughCentroid); // get the y-axis number of counts for max bin, i.e. amplitude
    // NOTE: Since bin center takes actual bin, need the conversion from photons to bins
    // but centroid & FWHM values will reflect the photons
    
    std::cout << "Rough Centroid: " << roughMean << " Rough Amplitude: " << roughAmplitude << "\n";
    
    // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    // (it’s a fit window, not a Gaussian width parameter)
    // The Gaussian itself mathematically extends to infinity.
    double const roughLow = roughMean - (2.5 * roughFWHM); // NOTE: Generous 2.5 for centroid finding
    double const roughHigh = roughMean + (2.5 * roughFWHM); // TODO: ^ May want to be more conservative if fitting overlapping peaks though
    // TODO: FWHM (roughMean / 2 => gives half maximum => iterate outwards from centre until bin val below half maximum) ?
    // ^ but this wont work for merged peaks, etc
    
    std::cout << "Rough Lower Bound: " << roughLow << " Rough Upper Bound: " << roughHigh << "\n";
    
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
    
    // TEST
    std::string const prefitFuncName = "prefitFn" + std::to_string(peakNum);
    // NOTE: If overwriting functions (no "+" on call to fit), this maybe not needed,
    // but if adding each function to the list (as is currently), this is mandatory
    // TEST
    
    // Define the fit function
    // auto prefitFn = new TF1("prefitFn", "gaus", roughLow, roughHigh);
    auto prefitFn = new TF1(prefitFuncName.c_str(), "gaus", roughLow, roughHigh);
    // NOTE: "gaus" is built-in ROOT shorthand for [0]*exp(-0.5*((x-[1])/[2])^2)
    
    // NOTE: "gaus(0)" is functionally the same, and can be abbreviated to "gaus" 
    // when using a single fitting function, however, if you were to use two functions
    // "gaus(0) + gaus(3)" ensures that the subsequent call to set parameters assigns
    // passed parameters to the correct function (see below)

    // Instead of relying on automatic RMS, which is not reliable for merged peaks etc,
    // require the user to state a rough FWHM value deduced by eye, and derive sigma from it
    double const roughSigma = roughFWHM / 2.355;
    std::cout << "Pre-fit Sigma: " << roughSigma << "\n";
    
    // TEST - Prefix arguent names
    std::string const arg0Name = std::to_string(peakNum) + "-Amplitude";
    std::string const arg1Name = std::to_string(peakNum) + "-Centroid";
    std::string const arg2Name = std::to_string(peakNum) + "-Sigma";
    // NOTE: I dont think that this actually needs doing
    // TEST
    
    // Pass the parameters required for the gaussian fit function
    prefitFn->SetParameters(roughAmplitude, roughMean, roughSigma);
    
    // Define names for each parameter
    // prefitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    prefitFn->SetParNames(arg0Name.c_str(), arg1Name.c_str(), arg2Name.c_str()); // TEST - use prefix names (NOTE: again not sure this actually needed)
    // NOTE: [0] = Amplitude, [1] = Mean, [2] = Sigma
    
    // NOTE: If we defined "gaus(0) + gaus(3)", we would need to pass separate params, i.e.:
    // fitFn->SetParameters(roughAmplitude1, roughMean1, sigma1, roughAmplitude2, roughMean2, sigma2);
    // hence, params: [0], [1], [2], are used for the first gaussian function,
    // and params: [3], [4], [5], are used for the second gaussian function
    // the same applies for "gaus(0) + pol1(3)",
    // where params: [4] & [5] are then the intercept and slope for the poly fit
    
    // TEST - Set parameter limits for finding centroid
    // double const prefitCentroidLimitLow = roughMean - (1.5 * roughFWHM);
    // double const prefitCentroidLimitHigh = roughMean + (1.5 * roughFWHM);
    // prefitFn->SetParLimits(1, prefitCentroidLimitLow, prefitCentroidLimitHigh); // mean is arg[1]
    // double const prefitSigmaLimitLow = roughSigma * 0.5;
    // double const prefitSigmaLimitHigh = roughSigma * 1.5;
    // prefitFn->SetParLimits(2, prefitSigmaLimitLow, prefitSigmaLimitHigh); // sigma is arg[2]
    // NOTE: May want to consder limits for amplitude and sigma, but not sure its as
    // relevant for those (maybe wrong on that though)
    // TEST
    
    // Call the histograms fit method, passing the fit function and histogram fitting options string
    // TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS");
    TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS+0");
    // TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS+0L");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = attempts to improve the fit quality
    // "L" = use log likelihood method (default chi-square), for use with counts histograms
    // "+" = adds this new fitted func to list of fitted funcs (default is delete previous keep last)
    // "0" = does not draw fitted function after fitting
    
    // Handle fit error (NOTE: success = 0)
    if (initialResult->Status() != 0) {
        std::cerr << "\nError: Failed to perform initial fit!\n";
        return std::nullopt;
    }
    
    // Extract the initial fit results
    double const prefitAmplitude = initialResult->Parameter(0);
    double const prefitCentroid = initialResult->Parameter(1);
    double const prefitSigma = initialResult->Parameter(2);
    double const prefitFWHM = prefitSigma * 2.355;
    
    // Base the fit window around the true centroid to avoid lopsidedness in the fit
    double const low = prefitCentroid - (2 * prefitFWHM);
    double const high = prefitCentroid + (2 * prefitFWHM);
    
    std::cout << "Refined Lower Bound: " << low << " Refined Upper Bound: " << high << "\n";
    
    // TEST
    std::string const refitFuncName = "refitFn" + std::to_string(peakNum);
    // NOTE: Again this may only be needed for non-overwriting functions
    // TEST
    
    // Define the fit function which will take refined parameters
    // auto refitFn = new TF1("refitFn", "gaus", low, high);
    auto refitFn = new TF1(refitFuncName.c_str(), "gaus", low, high);
    
    // Perform a refit using the fitted params
    refitFn->SetParameters(prefitAmplitude, prefitCentroid, prefitSigma);
    
    // refitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    refitFn->SetParNames(arg0Name.c_str(), arg1Name.c_str(), arg2Name.c_str()); // TEST - see prefit name comment above
    // Define names for each parameter
    
    // TEST - Set parameter limits for finding centroid
    // double const refitCentroidLimitLow = prefitCentroid - (1 * prefitFWHM);
    // double const refitCentroidLimitHigh = prefitCentroid + (1 * prefitFWHM);
    // prefitFn->SetParLimits(1, refitCentroidLimitLow, refitCentroidLimitHigh); // mean is arg[1]
    // double const refitSigmaLimitLow = prefitSigma * 0.5;
    // double const refitSigmaLimitHigh = prefitSigma * 1.5;
    // prefitFn->SetParLimits(2, refitSigmaLimitLow, refitSigmaLimitHigh); // sigma is arg[2]
    // NOTE: May want to consder limits for amplitude and sigma, but not sure its as
    // relevant for those (maybe wrong on that though)
    // TEST
    
    // Calculate the results of the refit
    // TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS"); // overwrite function list
    TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS+0");
    // TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS+0L");
    // NOTE: Append to function list ("+"), disable auto draw ("0")
    
    // Handle refit error
    if (refitResult->Status() != 0) {
        std::cerr << "\nError: Failed to perform refit!\n";
        return std::nullopt;
    }
    
    // ...
    return refitResult;
}

/*
 * Get average x and y values for a specific range of the histogram
 * 
 * Utilising a weighted average for sideband averaging accounts for the fact that not
 * all sidebins hold equal significance. It helps to extract accurate center frequencies
 * or signal properties while minimising noise.
 * 
 * x = Σ(x_i * w_i) / Σ(w_i)
 * 
 * Where:
 * - x_i = value of the specific sideband
 * - w_i = weighting factor for that sideband
 * 
 * NOTE: For mean x value in the sideband, we use the bin contents (y value for that bin),
 * as the weighting factor
 * 
 * x = Σ(x_i * y_i) / Σ(y_i)
 * 
 * y = Σ(y_i) / N
 * 
 * Where:
 * - N = number of bins + 1
 */
std::vector<double> sideband_avg(TAxis const* xAxis, double const& xStart, double const& xEnd) {
    // ...
    std::cout << "Band Start: " << xStart << " - Band End: " << xEnd << "\n";
    
    int const xStartBin = hpx->FindFixBin(xStart);
    int const xEndBin = hpx->FindFixBin(xEnd);
    
    std::cout << "Band Start Bin: " << xStartBin << " - Band End Bin: " << xEndBin << "\n";
    
    // ..
    double ySum = 0; // Σ(y_i)
    double xSumWeighted = 0; // Σ(x_i * y_i)
    
    // ...
    for (int i = xStartBin; i < xEndBin; i++) {
        // ...
        const double xBinCentre = xAxis->GetBinCenter(i); // x_i
        const double yVal = hpx->GetBinContent(i); // y_i
        
        // ...
        xSumWeighted += xBinCentre * yVal;
        ySum += yVal;
    }
    
    // ...
    const double xMean = xSumWeighted / ySum; // Σ(x_i * y_i) / Σ(y_i)
    // const double range = (xEnd - xStart) + 1; // N
    const double range = (xEndBin - xStartBin) + 1; // N
    const double yMean = ySum / range; // Σ(y_i) / N
    
    // ...
    std::cout << "X Mean: " << xMean << " - Y Mean: " << yMean << "\n";
    
    // Σ(y_i)
    // const double ySum2 = hpx->Integral(xStart, xEnd);
    // const double range2 = (xEnd - xStart) + 1;
    // const double yMean2 = ySum2 / range2;
    // std::cout << "X Mean: " << xMean << " - Y Mean 2: " << yMean2 << "\n";
    // NOTE: Exactly equivalent to above
    
    // ...
    return std::vector<double> { xMean, yMean };
}

/*
 * Generate a function string for the TF1 constructor, based on number of peaks
 * 
 * 1 Peak: "gaus(0)"
 * 2 Peaks: "gaus(0) + gaus(3)"
 * 3 Peaks: "gaus(0) + gaus(3) + gaus(6)"
 * etc ...
 * 
 * NOTE: Spaces not relevant, i.e.:
 * "gaus(0)+gaus(3)" == "gaus(0) + gaus(3)"
 * 
 * std::format("gaus({})") // NOTE: C++ 20 feature ...
 * 
 * TODO: Will need slight adjustment to accomodate background function
 * i.e.: "gaus(0) + gaus(3) + pol1(6)"
 */
std::string fit_string(int const& numPeaks) {
    // ....
    std::string result;
    
    // ...
    for (int i = 0; i < numPeaks; i++) {
        result += "gaus(";
        int const paramStartIdx = i * 3;
        std::string const stringified = std::to_string(paramStartIdx);
        result += stringified;
        result += ")";
        // if (i != numPeaks - 1) result += "+";
        if (i != numPeaks - 1) result += " + ";
    }
    
    return result;
}

/*
 * Assign parameters to the full fit function using individual peak fit results
 * 
 * TODO: Maybe check params arent 0 after setting or such
 */
int assign_peak_params(TF1* fitFn, std::vector<TFitResultPtr> const& fitResultsVec) {
    // ...
    for (int i = 0; i < fitResultsVec.size(); i++) {
        std::cout << "Setting params for peak: " << i << "\n";
        
        // Fit parameter indices
        int const gausArg0Start = i * 3; // 0, 3, 6, etc
        int const gausArg1Start = gausArg0Start + 1; // 1, 4, 7, etc
        int const gausArg2Start = gausArg0Start + 2; // 2, 5, 8, etc
        
        // Prefix parameter names
        std::string const arg0Name = std::to_string(i) + "-Amplitude"; // 0-Amplitude, 1-Amplitude, 2-Amplitude, etc
        std::string const arg1Name = std::to_string(i) + "-Centroid"; // 0-Centroid, 1-Centroid, 2-Centroid, etc
        std::string const arg2Name = std::to_string(i) + "-Sigma"; // 0-Sigma, 1-Sigma, 2-Sigma, etc
        // NOTE: Without this prefix there would be a naming conflict when fitting
        // for more than one peak
        
        // Set full fit function parametes using individual peak fit results
        fitFn->SetParameter(gausArg0Start, fitResultsVec[i]->Parameter(0)); // amplitude
        fitFn->SetParName(gausArg0Start, arg0Name.c_str());
        std::cout << "Set par " << gausArg0Start << " - " << arg0Name << " to " << fitResultsVec[i]->Parameter(0) << "\n";
        
        fitFn->SetParameter(gausArg1Start, fitResultsVec[i]->Parameter(1)); // mean
        fitFn->SetParName(gausArg1Start, arg1Name.c_str());
        std::cout << "Set par " << gausArg1Start << " - " << arg1Name << " to " << fitResultsVec[i]->Parameter(1) << "\n";
        
        fitFn->SetParameter(gausArg2Start, fitResultsVec[i]->Parameter(2)); // sigma
        fitFn->SetParName(gausArg2Start, arg2Name.c_str());
        std::cout << "Set par " << gausArg2Start << " - " << arg2Name << " to " << fitResultsVec[i]->Parameter(2) << "\n";
    }
    
    // ...
    return 0;
}

// TODO
struct CountsResult {
    double const totalCounts;
    double const countsError;
};

/*
 * Calculate integrated counts under the fit curve, and errors
 * 
 * NOTE: Could take func name as arg and query hpx for the pointer, but kinda just feels 
 * more natural to take fit function as arg instead of querying
 * 
 * NOTE: Takes "draw-only" copy of individual peak fits
 * 
 * NOTE: TFitResultPtr from the individual initial peak fits is required to avoid 
 * IntegralError failing due to last fitter "not being compatible" with current TF1
 * 
 * TODO: Total area - background area (for lab spectra with background)
 */
std::optional<std::vector<double>> get_counts(TF1* fitFn, TFitResultPtr const result) {
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (getCounts()): Histogram not found!\n";
        return std::nullopt;
    }
    
    // Retrieve the fit function
    // TF1* fitFn = hpx->GetFunction("refitFn");
    // TF1* fitFn = hpx->GetFunction(funcName);
    
    // Handle missing function
    if (!fitFn) {
        std::cerr << "\nError: No such fit function found.\n";
        return std::nullopt;
    }
    
    // Retrieve the fit window set during fitting
    double xmin;
    double xmax;
    fitFn->GetRange(xmin, xmax);
    // NOTE: These will be the lower/upper bounds of the integration
    
    // Get bin width for bin 1 (NOTE: could be any of the 1024 bins, theyre all same width)
    double const binWidth = hpx->GetBinWidth(1);
    // NOTE: ROOT histograms store counts per bin, the integral of a function is in
    // units of: Energy * Counts (or: photons * counts, with simulated spectra), dividing
    // by the Energy/Bin (kev/Bin), or photons/Bin, converts it back to pure counts
    
    // Calculate integral for the area under the fitted gaussian curve
    double const area = fitFn->Integral(xmin, xmax); // NOTE: Is energy * counts (or num photons * counts) in current form
    double const totalCounts = area / binWidth; // NOTE: Now is just counts
    
    // Get the error (calculated as: sqrt(totalCounts), in counting statistics)
    // double const countsError = fit->IntegralError(xmin, xmax) / binWidth; // TODO: This requires the actually fitted function, not the drawing only copy
    double const countsError = fitFn->IntegralError(xmin, xmax, fitFn->GetParameters(), result->GetCovarianceMatrix().GetMatrixArray()) / binWidth;
    // NOTE: Must also be divided by bin width, otherwise it would be:
    // +/- num photons, rather than counts
    // NOTE: Feeding the covariance matrix
    
    std::cout << "COUNTS: " << totalCounts << ", ERROR [SQRT(COUNTS)]: +/-" << countsError << "\n";
    
    // std::vector<double> countResults = { totalCounts, countsError };

    // return countResults;
    
    return std::vector<double> { totalCounts, countsError };
    
    // return { totalCounts, countsError };
}

/*
 * Extract fit statistics for individual peaks, create and populate a new line for
 * each chosen statistic, then add them to the full list of lines
 * 
 * NOTE: TFitResultPtr is already lightweight, reference maybe not needed
 * 
 * TODO: Add prefix boolean flag (multipeak = true/false), or just pass number of peaks:
 * if only one peak fitted, no integer prefix
 * if >1 peak fitted, add peak number prefix, i.e.:
 * 1-Centroid, 1-FWHM, 1-Counts
 * 2-Centroid, 2-FWHM, 2-Counts
 * NOTE: Would likely have to take peak number as param to
 */
int get_stats_lines(TFitResultPtr const &result, std::vector<double> const &counts, TList* listOfLines) {    
    // Retrieve the fit chi squared & n.d.f
    double const chi2 = result->Chi2();
    double const ndf = result->Ndf();
    
    // Calculate the goodness of fit
    double const goodFit = chi2 / ndf;
    
    std::cout << "Goodness of Fit: " << goodFit << "\n";
    
    // Extract the fitted photopeak centroid and error on the result
    double const fittedCentroid = result->Parameter(1);
    double const fittedCentroidError = result->Error(1);
    
    // Calculate the updated FWHM, based on fitted sigma
    double const fittedSigma = result->Parameter(2);
    double const fittedFWHM = fittedSigma * 2.355;
    
    std::cout << "POST-FIT SIGMA: " << fittedSigma << "\n";
    std::cout << "POST-FIT FWHM: " << fittedFWHM << "\n";
    
    // Calculate the error on the fitted FWHM, based on fitted sigma error
    double const fittedSigmaError = result->Error(2);
    double const fittedFWHMError = fittedSigmaError * 2.355;
    
    // Get integrated photopeak counts and error on counts from input vector
    double const countsVal = counts[0];
    double const countsErr = counts[1];
    
    // Add centroid (+/- error) to the stats box
    char const* text1 = Form("Centroid = %.2f #pm %.2f", fittedCentroid, fittedCentroidError); // Format the entry (#pm generates +/-)
    auto newLine1 = new TLatex(0, 0, text1); // <- may have to do Form() for string
    newLine1->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
    newLine1->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
    listOfLines->Add(newLine1); // append the fwhm value & error to the fit stats

    // Add FWHM (+/- error) to the stats box
    char const* text2 = Form("FWHM = %.2f #pm %.2f", fittedFWHM, fittedFWHMError); // Format the entry (#pm generates +/-)
    auto newLine2 = new TLatex(0, 0, text2); // <- may have to do Form() for string
    newLine2->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
    newLine2->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
    listOfLines->Add(newLine2); // append the fwhm value & error to the fit stats
    
    // Add counts (+/- error) to the stats box
    char const* text3 = Form("Counts = %.2f #pm %.2f", countsVal, countsErr);
    auto newLine3 = new TLatex(0, 0, text3);
    newLine3->SetTextFont(gStyle->GetStatFont());
    newLine3->SetTextSize(gStyle->GetStatFontSize());
    listOfLines->Add(newLine3);
    
    // No errors, all good
    return 0;
}

/*
 * Display relevant fit values in statistics box post-fit (chi^2, centroid, etc)
 * 
 * NOTE: Uses default stats object located in top right of the screen post-fit
 * 
 * TODO: Not a fan of using the existing stats box object just to clear it and
 * refill it, likely better to create my own stats box from scratch
 */
int draw_fit_stats(TList* listOfLines) {
    // Format the output SetOptFit(pcev)
    gStyle->SetOptFit(111); // NOTE: param is a bit-mask (4-digit integer)
    // p = chi2 probability
    // c = chi2 & number of degrees of freedom (NDF)
    // e = Errors (standard deviations of the fitted parameters)
    // v = Values (name/values of params)
    // NOTE: Putting a leading 0 makes the compiler interpret the number as octal (base 8),
    // note decimal, which can cause unexpected bit settings, using 111 treats its as a decimal,
    // so avoid SetOptFit(0111), and use SetOptFit(111) instead.
    
    // TEST
    // c->Update(); // NOTE: This can be omitted with the working combo
    // gPad->Update(); // ^ same
    // hpx->SetStats(1); // NOTE: Tried using this instead of SetOptFit, doesnt work, causes stats box not found
    // hpx->SetStats(0); // this is called later, might be wiggle room in location, but if it aint broke...
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (drawFitStats()): Histogram not found!\n";
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
        std::cerr << "\nError: Stats box object not found.\n";
        // Error("stats", "Stats not found");
        return 1;
    }
    
    // ps->SetX1NDC(0);
    // ps->SetX2NDC(1000);
    
    // Detach from root auto management (need to use when doing set stats 0 and ps draw)
    ps->SetName("mystats"); // NOTE: Without this, get segmentation violation (segfault)
    
    // ...
    ps->Clear();
    
    // Get existing statistics box content
    TList* existingLines = ps->GetListOfLines();
    
    // ...
    // existingLines->Clear();
    
    // ...
    for (int i = 0; i < listOfLines->GetSize(); i++) {
        existingLines->Add(listOfLines->At(i));
    }
    
    // Display the custom statistics box
    hpx->SetStats(0); // Disable auto future stats regeneration
    ps->Draw(); // Redraw custom box
    // c->Update(); // this can actually be omitted too
    
    ////////////////////////////////////////////////////////////////////////////
    
    // auto statsBox = new TPaveStats();
    
//     for (int i = 0; i < listOfLines->GetSize(); i++) {
//         // TObject* newLine = listOfLines->At(i);
//         auto newLine = static_cast<TLatex*>(listOfLines->At(i));
//         // We know its a list of TLatex*, not TObject*, as it was populated in pipeline
//         
//         // statsBox->AddText(newLine);
//         // statsBox->InsertText();
//     }
    
    // No errors, all good
    return 0;
}

/*
 * Main entry point for peak fitting
 * 
 * TODO: Likely could one or a couple big loops instead of a bunch of sequential ones,
 * but the performance difference is negligible for current purposes
 * 
 * TODO: Extract processes out to handlers
 */
int fit(std::initializer_list<int> const centroids, int const roughFWHM) {
    // 1) Copy initialiser list contents to vector, check if theyre in ascending order
    const int* centroid = centroids.begin(); // pointer to first address in initializer_list
    const int numPeaks = centroids.size();
    std::vector<int> centroidVec = {};
    bool isAscending = true; // sorted centroids flag
    
    std::cout << "Num peaks: " << numPeaks << "\n";
    
    // ...
    for (int i = 0; i < numPeaks; i++) {
        std::cout << "Centroid" << i << ": " << centroid[i] << "\n";
        centroidVec.push_back(centroid[i]);
        
        // If vector is found to be in non-ascending order (unsorted, descending, etc), flag it
        if (i >= 1 && (centroid[i] < centroid [i - 1])) isAscending = false;
    }
    
    std::cout << "Num peaks: " << centroidVec.size() << "\n";
    
    // 2) If vector contents not sorted in ascending order, sort them
    if (!isAscending) {
        std::cout << "Sorting centroid vector...\n";
        // Ensure centroids are in ascending order (i.e. 1100, 1300)
        std::sort(centroidVec.begin(), centroidVec.end());
    }
    
    // 3) Perform fits for each individual peak selected, cache the result pointer in a vector
    std::vector<TFitResultPtr> fitResults = {};
    
    for (int i = 0; i < centroidVec.size(); i++) {
        auto fitResult = fit_individual(centroidVec[i], roughFWHM, i);
        if (!fitResult.has_value()) {
            std::cerr << "\nError: Failed to fit peak " << i << "\n";
            return 1;
        }
        fitResults.push_back(fitResult.value());
    }
    
 
    
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 3.1) Use the fitted photopeak params to define an exclusion zone
    
    // Grab lowest energy photopeak centroid and fwhm from fit results
    double const lowEnergyCentroid = fitResults[0]->Parameter(1); // centroid = [1]
    double const lowEnergyFWHM = fitResults[0]->Parameter(2) * 2.355; // sigma = [2]
    
    // Band to the the left of the photopeak
    double const leftLow = lowEnergyCentroid - (2 * lowEnergyFWHM);
    double const leftHigh = lowEnergyCentroid - (1.5 * lowEnergyFWHM);
    // NOTE: from -1.5*FWHM where gaussian returns to ~0 (0.2%), to -2*FWHM (0.0...%)
    
    // Grab highest energy photopeak centroid and fwhm from fit results
    double const highEnergyCentroid = fitResults[numPeaks - 1]->Parameter(1); // centroid = [1]
    double const highEnergyFWHM = fitResults[numPeaks - 1]->Parameter(2) * 2.355; // sigma = [2]
    
    // Band to the the right of the photopeak
    double const rightLow = highEnergyCentroid + (1.5 * highEnergyFWHM);
    double const rightHigh = highEnergyCentroid + (2 * highEnergyFWHM);
    
    // NOTE: In the case of a single fitted peak, low energy and high energy centroid/fwhm
    // will be the same
    
    // 3.2) Use sideband averaging to evaluate mean background in channels immediately
    // to the left and right of the region of interest (the photopeak), and interpolate across the peak
    
    TAxis const* xAxis = hpx->GetXaxis();
    
    std::cout << "\nLeft sideband\n";
    std::vector<double> const left = sideband_avg(xAxis, leftLow, leftHigh);
    
    std::cout << "\nRight sideband\n\n";
    std::vector<double> const right = sideband_avg(xAxis, rightLow, rightHigh);
    
    // 3.3) Calculate initial estimate for first order polynomial params (slope & intercept)
    
    // slope = Δy / Δx
    double const deltaY = right[1] - left[1]; // [0] = xMean, [1] = yMean
    double const deltaX = right[0] - left[0];
    double const slope = deltaY / deltaX; // gradient of the line
    
    // intercept = y - (slope * x)
    double const intercept = right[1] - slope * right[0]; // y-intercept (value of y when x = 0)
    // NOTE: y = mx+b => b = y - mx
    
    std::cout << "\nRough Slope: " << slope << " Rough Intercept: " << intercept << "\n";
    ////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    
    
    
    // 4) Generate a full fit function string based on the number of peaks to fit
    std::string const fitString = fit_string(numPeaks);
    
    std::cout << "Fit function string: " << fitString << "\n";
    
    
    
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 4.1) Append polynomial to the fit string
    std::string const fullFitString = fitString + " + pol1(" + std::to_string(numPeaks * 3) + ")";
    // NOTE: I.e., "gaus(0) + gaus(3) + gaus(3)" -> "gaus(0) + gaus(3) + gaus(6) + pol1(9)"
    std::cout << "Full Fit function string: " << fullFitString << "\n";
    // return 1;
    ////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    
    
    
    // 5) Establish lower/upper bounds for full fit window
    double const rangeLow = fitResults[0]->Parameter(1) - (2.5 * roughFWHM); // NOTE: Generous 2.5
    double const rangeHigh = fitResults[numPeaks - 1]->Parameter(1) + (2.5 * roughFWHM);
    // NOTE: Using smallest rough low and biggest rough high
    // NOTE: These can be fed to first order polynomial when its introduced
    
    std::cout << "Full fit low: " << rangeLow << " - Full fit high: " << rangeHigh << "\n";
    
    range(rangeLow, rangeHigh); // TEST: Auto zoom on fit window
    
    // 6) Instantiate the full fit function
    // auto fullFitFn = new TF1("fullPrefitFn", fitString.c_str(), rangeLow, rangeHigh);
    
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 6.1) Instantiate the full fit function
    auto fullFitFn = new TF1("fullPrefitFn", fullFitString.c_str(), rangeLow, rangeHigh);
    ////////////////////////////////////////////////////////////////////////////////////////////

    
    
    
    // 7) Assign parameters to the full fit function
    assign_peak_params(fullFitFn, fitResults);
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 7.1) Assign linear component parameters to the full fit function
    int const polArg0IDX = numPeaks * 3;
    int const polArg1IDX = polArg0IDX + 1; // (numPeaks * 3) + 1)
    fullFitFn->SetParameter(polArg0IDX, intercept);
    fullFitFn->SetParameter(polArg1IDX, slope);
    fullFitFn->SetParName(polArg0IDX, "Intercept");
    fullFitFn->SetParName(polArg1IDX, "Slope");
    // intercept, slope
    ////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    
    // 8) Attempt the full fit, and abort on unsuccessful fit
    // TFitResultPtr const fullFitResult = hpx->Fit(fullFitFn, "RS+");
    // TFitResultPtr const fullFitResult = hpx->Fit(fullFitFn, "RS+0");
    TFitResultPtr const fullFitResult = hpx->Fit(fullFitFn, "RS+0L");
    // NOTE: Adding to function list instead of replacing previous ("+")
    // NOTE: Disabling automatic drawing ("0")
    // NOTE: "L", i.e., log likelihood fitting method instead of chi2 method, 
    // produces very marginally better results for "high" counts peaks, i.e., 
    // 22Na 511 keV annihilation photopeak:
    // (chi2/ndf = 370.3 / 229) => (chi2/ndf = 369.177 / 229)
    // but for "low" counts peaks, i.e., 22Na 1275 keV photopeak:
    // (chi2/ndf = 467 / 363) => (chi2/ndf = 415.912 / 363)
    // ^ quite a substantial improvement
    // TODO: Need to test this across a range of spectra and energies
    // TODO: Test "M" flag too (attempt to improve local minimum)

    // Handle fit error (NOTE: success = 0)
    if (fullFitResult->Status() != 0) {
        std::cerr << "\nError: Failed to perform initial fit!\n";
        return 1;
    }
    
    // 9) Render the full fit line to the histogram
    
    // \/\/\/\/\/\/\/\/\/\/\/\/ TODO: render_fit()
    
    // Draw the fit line (ROOT internally stores the fit function with the histogram after fitting)
    // hpx->GetFunction("fullPrefitFn")->Draw("same");
    // fullFitFn->SetLineColor(kBlue);
    fullFitFn->Draw("same");
    // NOTE: The "HIST" option suppresses drawing associated functions (including fits),
    // hence why "hpx->Draw()" works here instead of drawing the fit fn (but we lose the histogram view),
    // and why "hpx->Draw("HIST")" doesnt work alone, so calling draw on the stored fn is the way,
    // it is also not enough to just call Modified() & Update().
    
    // 10) Grab the individual parameters from the full fit, storing them in a vector
    // NOTE: 1 peak = 3 params, 2 peaks = 6 params, etc
    
//     std::vector<double> fittedParams = {};
//     // NOTE: Using C array with (size = numPeaks * 3) causes error
//     
//     for (int i = 0; i < numPeaks * 3; i++) {
//         fittedParams.push_back(fullFitFn->GetParameter(i));
//     }
    // TEST - No need to deal with C style arrays etc due to using function methods
    std::vector<double> fittedParams = fullFitResult->Parameters();
    // TEST - Use TFitResultPtr method to return vector containing results ...
    
    // 11) Create "draw only" copies of the individual peak fits, populated from full fit, then render them
    // NOTE: Could also have step 3 (fit_individual) return refit functions
    // NOTE: Could also populate from "fitResults" vector, containing individual peak TFitResultPtr
    
    std::vector<TF1*> peakFunctions = {};
    
    for (int i = 0; i < numPeaks; i++) {
        int const peakArg0IDX = i * 3;
        
        double const amplitude = fittedParams[peakArg0IDX];
        double const mean = fittedParams[peakArg0IDX + 1];
        double const sigma = fittedParams[peakArg0IDX + 2];
        
        std::string const name = "peak" + std::to_string(i);
        double const peakLow = mean - (4 * sigma);
        double const peakHigh = mean + (4 * sigma);
        
        std::cout << name << " " << peakLow << " - " << peakHigh << "\n";
        std::cout << "Amplitude: " << amplitude << " Mean: " << mean << " Sigma: " << sigma << "\n";
        
        auto func = new TF1(name.c_str(), "gaus", peakLow, peakHigh);
        
        func->SetParameters(amplitude, mean, sigma);
        // NOTE: Not displaying in fit stats, so setting ParNames not really needed
        
        peakFunctions.push_back(func); // cache pointers for "draw-only" peak functions
        
        func->SetLineColor(kGreen - 3); // change the colour of the drawn function
        
        func->Draw("same"); // draw each individual peak
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 11.1)
    // Draw the first order poly across the entire histogram range (NOTE: useful for debugging)
    auto polyFitted = new TF1("polyFitted", "pol1", xAxis->GetXmin(), xAxis->GetXmax());
    polyFitted->SetParameters(fittedParams[polArg0IDX], fittedParams[polArg1IDX]); // fitted intercept & slope
    // polyFitted->SetParNames("Intercept", "Slope"); // NOTE: not displaying in fit stats, so not really needed
    polyFitted->SetLineColor(kBlue);
    polyFitted->SetLineStyle(kDot);
    polyFitted->Draw("same");
    ////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    
    // 12) Get integrated counts for individual fitted peaks (and errors)
    
    std::vector<std::vector<double>> countsResults = {}; // TODO: object return type
    
    for (int i = 0; i < numPeaks; i++) {
        // Calculate counts in the fitted photopeak via integration
        std::optional<std::vector<double>> countsResult = get_counts(peakFunctions[i], fitResults[i]);
        // NOTE: Counts is not a std::vector<double> yet, it is still optional type
        
        // Handle nullopt return
        if (!countsResult.has_value()) {
            std::cerr << "\nError: Failed to get counts!\n";
            return 1;
        }
        // NOTE: Again this does not tell the compiler it is not optional
        
        // ...
        countsResults.push_back(countsResult.value());
        // NOTE: Here *countsResults, or .value(), converts the optional type to a vector
    }
    
    // 13) Compose custom list of fit statistics
    
    auto listOfLines = new TList(); // TList*
    // auto listOfLines = new TPaveStats();
    // listOfLines->InsertText();
    // listOfLines->AddText();
    // listOfLines->
    
    // Get chi-square / n.d.f for full fit
    double const chi2 = fullFitResult->Chi2();
    double const ndf = fullFitResult->Ndf();
    
    // Create a line
    char const* text1 = Form("#chi^{2} / ndf = %.2f / %.2f", chi2, ndf); // Format the entry (#pm generates +/-)
    auto newLine1 = new TLatex(0, 0, text1); // <- may have to do Form() for string
    newLine1->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
    newLine1->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
    listOfLines->Add(newLine1); // append the fwhm value & error to the fit stats
    
    for (int i = 0; i < numPeaks; i++) {
        // Write custom statistics to list for each fitted peak
        int statsLinesError = get_stats_lines(fitResults[i], countsResults[i], listOfLines);
        
        // Handle statistics writing
        if (statsLinesError) {
            std::cerr << "\nError: Failed get fit statistics!\n";
            return 1;
        }
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////
    // 13.1) Append fitted linear component stats to the list
    char const* polyText1 = Form("Intercept = %.2f", fittedParams[polArg0IDX]); // Format the entry (#pm generates +/-)
    auto newLinePoly1 = new TLatex(0, 0, polyText1);
    newLinePoly1->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
    newLinePoly1->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
    listOfLines->Add(newLinePoly1); // append the fwhm value & error to the fit stats
    
    char const* polyText2 = Form("Slope = %.2f", fittedParams[polArg1IDX]); // Format the entry (#pm generates +/-)
    auto newLinePoly2 = new TLatex(0, 0, polyText2);
    newLinePoly2->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
    newLinePoly2->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
    listOfLines->Add(newLinePoly2); // append the fwhm value & error to the fit stats
    ////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    
    // 14) Render the fit statistics box containing custom stats
    
    // Handle statistics box and write custom value to it
    int statsDrawError = draw_fit_stats(listOfLines);
    
    // Handle statistics drawing error
    if (statsDrawError) {
        std::cerr << "\nFailed draw fit statistics!\n";
        return 1;
    }
    
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
