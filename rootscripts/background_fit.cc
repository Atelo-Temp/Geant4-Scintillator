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
TCanvas *canvas = nullptr;

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int background_fit() {
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
std::string const check_path(std::string const& path) { // std::string check_path(std::string const path) {
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
int load_ascii(std::string path) {
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
int plot (std::string fileName) {
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
 * Display relevant fit values in top right info box post-fit (chi^2, centroid, etc)
 */
int drawFitStats() {
    // Format the output SetOptFit(pcev)
    gStyle->SetOptFit(111); // NOTE: param is a bit-mask (4-digit integer)
    // p = chi2 probability
    // c = chi2 & number of degrees of freedom (NDF)
    // e = Errors (standard deviations of the fitted parameters)
    // v = Values (name/values of params)
    // NOTE: Putting a leading 0 makes the compiler interpret the number as octal (base 8),
    // note decimal, which can cause unexpected bit settings, using 111 treats its as a decimal,
    // so avoid SetOptFit(0111), and use SetOptFit(111) instead.
    
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
 * 
 * TODO: Likely could one or a couple big loops instead of a bunch of sequential ones,
 * but the performance difference is negligible for current purposes
 */
std::optional<TFitResultPtr> fit_peak(int const& roughCentroid, int const& roughFWHM) {
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
    std::cout << "Pre-fit Sigma: " << roughSigma << "\n";
    
    // Pass the parameters required for the gaussian fit function
    prefitFn->SetParameters(roughAmplitude, roughMean, roughSigma);
    
    // Define names for each parameter
    prefitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    // NOTE: [0] = Amplitude, [1] = Mean, [2] = Sigma
    
    // NOTE: If we defined "gaus(0) + gaus(3)", we would need to pass separate params, i.e.:
    // fitFn->SetParameters(roughAmplitude1, roughMean1, sigma1, roughAmplitude2, roughMean2, sigma2);
    // hence, params: [0], [1], [2], are used for the first gaussian function,
    // and params: [3], [4], [5], are used for the second gaussian function
    // the same applies for "gaus(0) + pol1(3)",
    // where params: [4] & [5] are then the intercept and slope for the poly fit
    
    // TEST - Set parameter limits for finding centroid
    prefitFn->SetParLimits(1, roughLow, roughHigh); // mean is arg[1]
    // NOTE: May want to consder limits for amplitude and sigma, but not sure its as
    // relevant for those (maybe wrong on that though)
    // TEST
    
    // Call the histograms fit method, passing the fit function and histogram fitting options string
    TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = attempts to improve the fit quality
    // "L" = use log likelihood method (default chi-square), for use with counts histograms
    // "+" = adds this new fitted func to list of fitted funcs (default is delete previous keep last)
    
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
    
    // Define the fit function which will take refined parameters
    auto refitFn = new TF1("refitFn", "gaus", low, high);
    
    // Perform a refit using the fitted params
    refitFn->SetParameters(prefitAmplitude, prefitCentroid, prefitSigma);
    refitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    
    // Calculate the results of the refit
    TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS"); // overwrite function list
    
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
std::vector<double> sideband_avg(TAxis const* xAxis, const double& xStart, const double& xEnd) {
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
 * ...
 */
int fit_background() {
    // ...
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
    // 1) Fit to the photopeak
    std::optional<TFitResultPtr> result = fit_peak(roughCentroid, roughFWHM);
    
    if (!result.has_value()) {
        std::cerr << "\nError: Failed to fit peak\n";
        return 1;
    }
    
    TFitResultPtr const resultValue = result.value();
    
    // 2) ...
    
    // ...
    // drawFitStats();
    
    // 3) Use the fitted photopeak params to define an exclusion zone
    
    // Grab photopeak centroid and fwhm from fit results
    double const fittedCentroid = resultValue->Parameter(1); // centroid = [1]
    double const fittedFwhm = resultValue->Parameter(2) * 2.355; // sigma = [2]
    
    // Band to the the left of the photopeak (from point where gaussian ~return to 0, to 2*FWHM)
    double const leftLow = fittedCentroid - (2 * fittedFwhm);
    double const leftHigh = fittedCentroid - (1.5 * fittedFwhm);
    // NOTE: 
    
    // Band to the the right of the photopeak (from point where gaussian ~return to 0, to 2*FWHM)
    double const rightLow = fittedCentroid + (1.5 * fittedFwhm);
    double const rightHigh = fittedCentroid + (2 * fittedFwhm);
    
    // 4) Use sideband averaging to evaluate mean background in channels immediately
    // to the left and right of the region of interest (the photopeak), and interpolate across the peak
    
    TAxis const* xAxis = hpx->GetXaxis();
    
    std::cout << "\nLeft sideband\n";
    std::vector<double> const left = sideband_avg(xAxis, leftLow, leftHigh);
    
    std::cout << "\nRight sideband\n\n";
    std::vector<double> const right = sideband_avg(xAxis, rightLow, rightHigh);
    
    // 5) Calculate initial estimate for first order polynomial params (slope & intercept)
    
    // slope = Δy / Δx
    double const deltaY = right[1] - left[1]; // [0] = xMean, [1] = yMean
    double const deltaX = right[0] - left[0];
    
    // ...
    double const slope = deltaY / deltaX; // gradient of the line

    // ...
    double const intercept = right[1] - slope * right[0]; // y-intercept (value of y when x = 0)
    // NOTE: y = mx+b => b = y - mx
    
    std::cout << "\nRough Slope: " << slope << " Rough Intercept: " << intercept << "\n";
    
    // 6) Draw the first order poly across the entire histogram range
    // NOTE: useful for debugging
    
    // auto poly = new TF1("poly", "pol1", leftLow, rightHigh);
    auto poly = new TF1("poly", "pol1", xAxis->GetXmin(), xAxis->GetXmax());
    // NOTE: ROOT defined as ([p0]+[p1]*x)
    
    // poly->Print("V");
    poly->SetParameters(intercept, slope);
    poly->SetParNames("Intercept", "Slope");
    poly->SetLineColor(kBlue);
    // poly->Draw("same");
    
    // 7) Perform the full fit
    
    // ...
    auto fullFitFn = new TF1("fullFit", "gaus(0) + pol1(3)", leftLow, rightHigh);
    
    fullFitFn->SetParameters(resultValue->Parameter(0), resultValue->Parameter(1), resultValue->Parameter(2), intercept, slope);
    fullFitFn->SetParNames("Amplitude", "Centroid", "Sigma", "Intercept", "Slope");
    
    // TFitResultPtr const fullFitResult = hpx->Fit(fullFitFn, "RS+0"); // Dont overwrite peak fit, dont draw by default
    TFitResultPtr const fullFitResult = hpx->Fit(fullFitFn, "RS0"); // Overwrite peak fit, dont draw by default
    
    // fullFitFn->SetLineColor(kBlue);
    fullFitFn->SetLineColor(kGreen);
    fullFitFn->Draw("same");
    
    // ...
    drawFitStats();
    
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
