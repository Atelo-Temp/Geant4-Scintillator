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

// C lib
#include <fstream>
#include <sstream>

// Global root object variables
std::ifstream inASCII;
TFile* inROOT = nullptr;
TTree* nTuple = nullptr;
TH1 *hpx = nullptr;
TCanvas *c = nullptr;

// ...
enum class FileType {
    ROOT,
    ASCII
};

// ...
FileType fileType;

/*
 * Load in plotting and fitting functions
 */
int plot_any() {
    // Usage
    std::cout << "\nASCII to Root Histogram\n\nTo get started, call: plot(\"path.ext\"), passing path to ASCII (.Spe) or ROOT (.root) file as param.\n\n";
    
    // fin
    return 0;
}

/*
 * TODO: ...
 */
int save() { return 1; }

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
    
    // Directory delimiter
    // char const dirDelimiter[2] = "/";
    std::string const dirDelimiter = "/";
    
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
    // char const extDelimiter[2] = ".";
    std::string const extDelimiter = ".";
    
    // Get index of delimiter
    // auto delimiterIdx = path.find(delimiter);
    // size_t extDelimiterIdx = path.rfind(extDelimiter);
    size_t const extDelimiterIdx = fileName.rfind(extDelimiter);
    // NOTE: "find()" searches from left to right, so if the path contains any period
    // in directory names or earlier in the file name, it will return the index of that 
    // instead of the desired period before the file extension, to solve this:
    // "rfind()" searches from right to left, and so always returns index of final period
    
    // Ensure file extension delimiter is present in path (find will return -1 if not found)
    if (extDelimiterIdx == -1) {
        // Write to stdout
        std::cout << "Error: No file extension.\n";
        
        // Error value
        return "";
    }
    
    // Get substring from index of delimiter to the end of the string
    // auto token = path.substr(delimiterIdx, path.size());
    // std::string token = path.substr(extDelimiterIdx);
    std::string const token = fileName.substr(extDelimiterIdx);
    // NOTE: Can omit path.size() as second param, defaults to end of string
    // NOTE: 0, delimiterIdx would get everything prior to delimiter
    
    // Debug
    // std::cout << token << std::endl;
    
    // Acceptable file extensions
    // const char* spe = ".Spe";
    // const char* root = ".root";
    std::string const spe = ".Spe";
    std::string const root = ".root";
    
    // Check path ends with valid extension, reject invalid file type
    if (token != spe && token != root) {
        // Write to stdout
        std::cout << "Error: Invalid extension.\n";
        
        // Error value
        return "";
    }
    // If its an ASCII file extension, log it and set ASCII flag
    else if (token == spe) {
        std::cout << "\nASCII file detected.\n\n";
        fileType = FileType::ASCII;
    }
    // If its a ROOT file extension, log it and set ROOT flag
    else if (token == root) {
        std::cout << "\nRoot file detected.\n\n";
        fileType = FileType::ROOT;
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
        // char* const home = getenv("HOME");
        char const* home = getenv("HOME");
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
    inASCII.open(path);
    
    // Ensure file was found, exit with error if its not
    // NOTE: No need to reprompt, user can just call the function again
    if (!inASCII.is_open()) {
        // Error message
        std::cout << "Error: File not found.\n";
        
        // Error
        return 1;
    };
    
    // No errors, all good
    return 0;
}

/*
 * Open root file containing TTree (ntuple), load it into local memory
 */
int load_root(std::string path) {
    // Convert: std::string, to: char const*
    char const *charPath = path.c_str();
    
    // Fetch and open root file
    inROOT = TFile::Open(charPath);
    
    // Handle incorrect path
    if (!inROOT) {
        printf("Error: File not found!\n");
        return 1;
    }
    
    // No errors, all good
    return 0;
}

/*
 * TODO: Have treeName passed as param
 * ^ if calling plot() as:
 * plot(fileName) - do ascii/root check (and assume root histo in file)
 * plot(fileName, branchName) - do ascii/root check (but assume root) (and assume ntuple in file)
 * 
 * TODO: May want to consider interactive prompt, i.e. check for tree names in root file,
 * then ask user for input to choose said tree
 * ^ same for branches
 */
int load_tree() {
    // Handle incorrect path
    if (!inROOT) {
        printf("Error: File not found!\n");
        return 1;
    }
    
    // TODO: Have this passed as param
    char const treeName[16] = "EventData;1";
    
    // Get the TTree from the root file and assign it to the TTree pointer
    inROOT->GetObject(treeName, nTuple); // NOTE: Also works: TTree* nTuple = in->Get<TTree>(treeName);
    
    // Handle missing ntuple (incorrect tree name, etc)
    if (!nTuple) {
        printf("Error: Couldnt load TTree!\n");
        return 1;
    }
    
    // No errors, all good
    return 0;
}

/*
 * Executes ASCII or ROOT file procedures based on file type flag
 * 
 * TODO: Assumes Ntuple in root file
 */
int load_file(std::string path) {
    // Success status
    int status;
    
    // Attempt to load the ASCII file into memory
    if (fileType == FileType::ASCII) {
        status = load_ascii(path);
        std::cout << "\nASCII file has been loaded into memory.\n\n";
    }
    // Attempt to load the ROOT file into memory
    else if (fileType == FileType::ROOT) {
        status = load_root(path);
        std::cout << "\nROOT file has been loaded into memory.\n\n";
    }
    // Reject invalid usage
    else {
        std::cout << "Error (load_file()): File type not set.\n";
        return 1;
    }
    
    // TODO: TEMP (assumes ntuples, not root histo, in .root) (need to make it flexible)
    if ((status == 0) && (fileType == FileType::ROOT)) {
        status = load_tree();
    }
    
    return status;
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
        printf("Error (create_hist()): Histogram not found!\n");
        return 1;
    }
    
    // X-axis title
    hpx->SetXTitle("Channels");
    
    // No errors, all good
    return 0;
}

/*
 * NOTE: With the higher resolution (2048 bins vs 1024 bins previously),
 * aliasing is seen when plotting the Ntuples data in a histogram,
 * to account for the higher resolution, we can apply a gaussian smearing
 * to reduce the jagged edges
 */
double const post_processing(int entry) {
// int const post_processing(int entry) {
// TODO: int const post_processing(int entry, int nbins = 2048, int xmax = 3500) {
    // In counting statistics: sigma = sqrt(N)
    double const sigma = std::sqrt(entry);
    // TODO: This assumes pure Poisson statistics, in a real detector system,
    // resolution is a combination of:
    // sigma_scintillator + sigma_transfer + sigma_PMT
    // in geant4 RESOLUTIONSCALE covers sigma_scintillator,
    // geometry and photocathode efficiency covers sigma_transfer,
    // and this sigma covers sigma_PMT (but doesnt neccesarily model is accurately)
    
    // double const sigma = 0.5 * std::sqrt(entry);
    // double const sigma = entry * (0.08 / 2.355);
    // NOTE: ^ 1.8 res scale, n * (res lab / 2.355) = 114.02 FWHM
    
    // Apply gaussian smearing to the photons detected in this event
    double const smeared = gRandom->Gaus(entry, sigma);
    // TODO: potentially reduce gaussian smearing, reducing from:
    // sigma = sqrt(n)
    // to:
    // sigma = 0.5 * sqrt(n)
    // reduces FWHM from ~105.35 FWHM (at 1.8 res scale), to 91.16 (still at 1.8 res scale)
    // NOTE: Impacts FWHM more than i initially thought
    
    // Conversion factor from num optical photons "detected" to 0-2048 channel number
    double const conversion = 2048. / 3500.;
    // double const conversion = 1024. / 3500.;
    // NOTE: 3500 photons is arbitrary currently, in practice, this value should
    // reflect the upper window limit for the energy region of interest, i.e.:
    // 0 - 2 MeV
    
    // TODO: ^^ potentially make nbins & xmax global variables, set prior to fit,
    // or pass them in as params to this fn
    
    // Convert entry to channel number
    // int const channel = conversion * smeared; // int channel = std::floor(conversion * entry);
    double const channel = conversion * smeared;
    // NOTE: Let H1 handle binning doubles rather than flooring
    
    // NOTE: Can also apply a gain factor (but would likely want to establish 
    // this value accurately from the physical detector rather than using estimate):
    // int const gain = 1e6;
    // int const nTotal = entry * gain;
    // double const sigma = gain * std::sqrt(entry);
    // double const smeared = gRandom->Gaus(nTotal, sigma);
    // double const conversion = 2048. / (3500. * gain);
    // double const channel = conversion * smeared;
    
    return channel;
}

/*
 * Iterate through tree branch, populating histogram with per-event values
 * 
 * TODO: Make post-processing optional
 * 
 * TODO: Make it so branchname is actually passed in as param
 * 
 * TODO: Maybe query list of branchnames in tree
 */
int fill_hist_ntuple(char const branchName[16] = "NumPhotons") {
    // char const branchName[16] = "NumPhotons";
    
    // Get the TBranch of interest from the TTree, and assign it to pointer
    TBranch* const eventData = nTuple->GetBranch(branchName);
    
    // TODO: if (!eventData) ...
    
    // To read a tree, neeed to associate variables with the trees branches
    int entry;
    nTuple->SetBranchAddress(branchName, &entry);
    // NOTE: When loading a tree entry, the tree will set the variables to the branches value as read from the storage

    // Get the number of entries in the branch (i.e., length for iteration limit)
    long long const numEntries = eventData->GetEntries();
    
    // PMT gain factor
    // int const gain = 1e6;
    
    // TODO: Maybe explore for reproducability
    // gRandom->SetSeed(...);
    
    // Read all entries in the branch
    for (long long i = 0; i < numEntries; i++) {
        // Load the data for the given tree entry
        eventData->GetEntry(i); // returns bytes read, not the actual val
        // NOTE: The "entry" variable will now be updated
        
        // Convert number of detected photons into a channel number (psuedo ADC)
        int const channel = post_processing(entry);
        
        // Add a count to the channel number associated with the current event
        hpx->Fill(channel);
    }
    
    // Handle missing input file
    if (!inROOT) {
        printf("Error: No infile to close!\n");
        return 1;
    }
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    inROOT->Close();
    
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
int fill_hist_ascii() {
    // Handle missing input file
    if (!inASCII) {
        printf("Error (draw_hist_ascii()): No infile to read!\n");
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        printf("Error (draw_hist_ascii()): Histogram not found!\n");
        return 1;
    }
    
    // Line counter
    int currentLine = 0;

    // ...
    std::string line;
    
    // Histo bin counter
    int currentBin = 0;
    
    // Get line reads a line from input stream into a string, until end of stream encountered
    while (std::getline(inASCII, line)) {
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
    inASCII.close();
    
    // No errors, all good
    return 0;
}

/*
 * ...
 * 
 * TODO: draw_hist_root() <- From ROOT histogram
 */
int draw_hist() {
    // ...
    int status;
    
    // ...
    if (fileType == FileType::ASCII) {
        status = fill_hist_ascii();
    }
    // ...
    else if (fileType == FileType::ROOT) {
        status = fill_hist_ntuple();
    }
    
    // ...
    if (status == 0) {
        std::cout << "\nHistogram has been populated.\n\n";
    }
    
    // No errors, all good
    return status;
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
        printf("Error (create_canvas()): Couldnt create canvas!\n");
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
int plot(std::string fileName) {
    // Check provided path is valid (will return empty string if not valid)
    std::string path = get_path(fileName);
    
    if (path.empty()) {
        printf("Aborting: Invalid path error!\n");
        return 1;
    }
    
    // Attempt to load ASCII or ROOT file into memory
    int const fileError = load_file(path);
    
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
    
    // Attempt to populate histogram from ASCII or ROOT file
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
