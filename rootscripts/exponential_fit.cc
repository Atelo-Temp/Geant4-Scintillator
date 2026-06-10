// Load an ASCII (.Spe) or ROOT (.root) file into memory, fill a histogram, and display it on a canvas

// ROOT lib
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h> // gStyle
#include <TF1.h>
#include <TFitResult.h>

// C lib
#include <iostream> // cerr, cin, cout

// Global root object variables
TFile* inROOT = nullptr;
TTree* nTuple = nullptr;
TBranch* branch = nullptr;
TLeaf* leaf = nullptr;
TH1* hpx = nullptr;
TCanvas* canvas = nullptr;

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
 * Get file extension
 */
std::string get_extension(std::string const& path) {
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
    if (dirDelimiterIdx == std::string::npos) {
        // Use full path for extension check
        fileName = path;
    } 
    // If path had directory delimiter
    else {
        // Use only the file name for extension check
        fileName = path.substr(dirDelimiterIdx + 1);
        // NOTE: +1, since otherwise it would be "/filename.ext"
    }
    
    std::cout << "\nFilename: " << fileName << "\n";
    
    // File extension delimiter
    std::string const extDelimiter = "."; // char const
    
    // Get index of delimiter
    size_t const extDelimiterIdx = fileName.rfind(extDelimiter);
    // NOTE: "find()" searches from left to right, so if the path contains any period
    // in directory names or earlier in the file name, it will return the index of that 
    // instead of the desired period before the file extension, to solve this:
    // "rfind()" searches from right to left, and so always returns index of final period
    
    // Ensure file extension delimiter is present in path (find will return -1 if not found)
    if (extDelimiterIdx == std::string::npos) {
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
    // std::cout << token << "\n";
    
    return token;
}

/*
 * Check if tilde expansion needed, perform it if so
 * 
 * TODO: if (path[0] != tilde) return path; is likely cleaner tbh
 */
std::string expand_path(std::string const& path) {
    // Replace tilde if passed
    std::string const tilde = "~"; // char tilde[2]
    
    // Mutable path
    std::string returnPath = path;

    // If reference to character at [0] is tilde character
    if (path[0] == tilde) {
        // Get the home path (~) from the environment variable
        char const* home = getenv("HOME");
        // std::cout << home << "\n"; // debug
        
        // Handle null envariable
        if (!home) {
            std::cerr << "\nError: Failed to get home path.\n";
            return "";
        }
        
        // Trim "~" from the start of the string (start at idx = 1, as "~" at 0)
        std::string const trimmedPath = path.substr(1, path.size());
        // std::cout << trimmedPath << "\n"; // debug
        
        // Update the path, replacing "~" with "/home/user"
        returnPath = home + trimmedPath;
        
        // Tilde expansion was successful
        std::cout << "\nPath has been expanded: " << returnPath << "\n";
        // NOTE: This is now an absolute path
    }
    
    return returnPath;
}

/*
 * Takes path as arg, validates string is valid, updates it if needed
 * 
 * NOTE: arg[0]: const (so its immutable)
 * 
 * NOTE: Uses rfind("/"), to isolate the file name, before attempting to find file extension,
 * makes checking for file extensions bit cleaner
 * 
 * TODO: Handle filenames such as .gitignore ?
 * else if (extDelimiterIdx == 0) {} 
 * NOTE: Kinda dont need to with ext check tho
 */
std::string check_path(std::string const& path) {
    // Print path to stdout
    std::cout << "\nUser provided path: " << path << "\n";
    
    // Get file extension
    std::string const token = get_extension(path);
    
    // Acceptable file extensions
    std::string const spe = ".Spe"; // const char*
    std::string const root = ".root"; // const char*
    
    // Check path ends with valid extension, reject invalid file type
    if ((token != root)) {
        // Write to stdout
        std::cerr << "\nError: Invalid extension.\n";
        
        // Error value
        return "";
    }

    // Check if tilde expansion needed, perform it if so
    std::string const returnPath = expand_path(path);
    
    // Confirmation status
    std::cout << "\nPath is valid.\n";
    
    // No errors, all good
    return returnPath;
}

/*
 * To be called on error, or after data processing complete
 * 
 * NOTE: Since the TBranch is owned by the TTree, and the TTree is owned by the TFile
 * (assuming TTree->SetDirectory(nullptr) hasnt been called), it is not necessary to
 * manually call "delete" on the TBranch or TTree
 * 
 * NOTE: Closing the TFile causes destructors for each of the other objects to be called
 * 
 * NOTE: Hence, calling DELETE on TTREE after already DELETING TFILE causes a segfault
 * (assuming TTree->SetDirectory() wasnt called)
 * 
 * NOTE: Calling DELETE on TBRANCH after already DELETING TTREE causes a segfault
 * 
 * NOTE: Calling DELETE on BRANCH after FILE IS CLOSED also causes a segfault
 * 
 * NOTE: Calling TTree->SetDirector(nullptr) is unwise
 */
void root_cleanup() {
    // Call the TFile destructor
    delete inROOT; // NOTE: Calls: inROOT->Close(); via TFile class destructor incase its zombie
    // NOTE: If inROOT = nullptr still, calling delete is safe (no-op)
    
    // Nullify pointers
    inROOT = nullptr;
    nTuple = nullptr;
    branch = nullptr;
    leaf = nullptr;
    // NOTE: No harm in nullifying already nulled pointers
}

/*
 * Open ROOT file containing ROOT objects, load it into local memory, check its not empty 
 * (i.e., contains at least one ROOT object)
 */
int load_root_file(std::string const& path) {
    // Convert: std::string, to: char const*
    char const* charPath = path.c_str();
    
    // Fetch and open root file
    inROOT = TFile::Open(charPath, "READ"); // readonly mode
    
    // Handle incorrect path
    if (!inROOT || inROOT->IsZombie()) {
        std::cerr << "\nError: ROOT file not found!\n";
        root_cleanup();
        return 1;
    }
    
    // Handle no ROOT objects being found (i.e. TTree, TH1D, etc)
    if (inROOT->GetNkeys() == 0) {
        std::cerr << "\nError: ROOT file is empty, closing file.\n";
        root_cleanup();
        return 1;
    }
    
    // Success message
    std::cout << "\nROOT file has been loaded into memory.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Populate the TTree pointer with the specified TTree
 * 
 * NOTE: DO NOT: Detach Ntuple object from input file, then close input file
 * i.e.: nTuple->SetDirectory(nullptr); delete inROOT;
 * UNLIKE WITH HISTOGRAMS, CLOSING THE ROOT FILE BEFORE WORKING WITH NTUPLE DATA IS UNWISE
 * Could have millions of entries which would need to be fully loaded into memory
 * ^^ Usually the TTree just becomes unusable
 * 
 * NOTE: If for some reason you must detach it, and you dont call:
 * TTree->SetDirector(nullptr);
 * before closing the ROOT file, when trying to call methods such as:
 * GetListOfBranches(); 
 * it will cause a segfault and crash program
 * Is best to just wait until the end of the root object handling pipeline to close TFile
 * 
 * NOTE: I seemingly dont need to append ";1" to tree names like i had been doing
 * But not sure if maybe i should though?
 * i.e.:
 * char const treeName[16] = "TrackData;1"; // << How i was doing it
 */
int cache_tree(char const* treeName) {
    // Handle incorrect path
    if (!inROOT) {
        std::cerr << "\nError: File not found!\n";
        return 1;
    }
    
    // Get the TTree from the root file and assign it to the TTree pointer
    inROOT->GetObject<TTree>(treeName, nTuple); // NOTE: Also works: TTree* nTuple = in->Get<TTree>(treeName);
    
    // NOTE: SEEMINGLY NOT NEEDED
    // std::string name = treeName;
    // std::string fullTreeName = name + ";1";
    // char const* theTreeName = fullTreeName.c_str();
    // inROOT->GetObject<TTree>(theTreeName, nTuple);
    
    // Handle missing ntuple (incorrect tree name, etc)
    if (!nTuple) {
        std::cerr << "\nError: Couldnt load TTree!\n";
        root_cleanup();
        return 1;
    }
    
    // Success message
    std::cout << "\nTree: \"" << treeName << "\" has been loaded into memory.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Caches the TBranch pointer in the global variable
 * 
 * TBranch* const eventData = nTuple->GetBranch(branchName); // NOTE: Does work (but this is different to typical pointer)
 * TBranch const* eventData = nTuple->GetBranch(branchName); // NOTE: Doesnt work
 */
int cache_branch(std::string const& selectedBranch) {
    // ...
    if (!nTuple) {
        std::cerr << "\nError: Ntuple not found, closing root file.\n";
        root_cleanup();
        return 1;
    }
    
    // ...
    if (selectedBranch.empty()) {
        std::cerr << "\nError: Invalid branch name. Closing root file and deconstructing Ntuple.\n";
        root_cleanup();
        return 1;
    }
    
    // ...
    char const* branchName = selectedBranch.c_str();
    
    // Get the TBranch of interest from the TTree, and assign it to pointer
    branch = nTuple->GetBranch(branchName);
    
    // Handle invalid branch name
    if (!branch) {
        std::cerr << "\nError: TTree branch: " << branchName << " not found. Closing root file and deconstructing Ntuple.\n";
        root_cleanup();
        return 1;
    }
    
    // TODO: Should this check be done here or in fill_hist
    if (branch->GetEntries() == 0) {
        std::cerr << "\nError: Selected TTree branch contains no entries! Closing root file, and deconstructing Ntuple/branch.\n";
        root_cleanup();
        return 1;
    }
    
    // Success message
    std::cout << "\nTTree Branch: \"" << branchName << "\" successfully loaded.\n";
    
    // ...
    return 0;
}

/*
 * Caches a pointer to a leaf in the active TTree branch
 * 
 * NOTE: Saves making multiple get leaf requests later for branch data type querying
 */
int cache_leaf() {
    // Handle invalid branch name
    if (!branch) {
        std::cerr << "\nError: TBranch pointer not found. Closing root file...\n";
        root_cleanup();
        return 1;
    }
    
    // Query chosen branch name
    char const* branchName = branch->GetName();
    
    // Cache a pointer to a leaf in this branch (for data type access later)
    leaf = branch->GetLeaf(branchName);
    
    // Fallback (incase branch name and name required by GetLeaf() differ)
    if (!leaf) leaf = static_cast<TLeaf*>(branch->GetListOfLeaves()->At(0));
    // NOTE: Static cast is safe as we know list of leaves is not empty from 
    // load pipeline, and were calling get leaves
    
    // If leaf still not set, throw
    if (!leaf) {
        std::cerr << "\nError: Failed to cache leaf pointer\n";
        root_cleanup();
        return 1;
    }
    
    std::cout << "\nLeaf pointer cached.\n";
    
    return 0;
}

/*
 * Load Ntuple object (TTree) from ROOT input file and cache pointers
 */
int load_root_tree(char const* treeName, char const* branchName) {
    // Cache TTree pointer
    int const loadTreeError = cache_tree(treeName);
    
    if (loadTreeError) {
        std::cerr << "\nError: Failed to cache TTree pointer.\n";
        return 1;
    }
    
    // Cache TBranch pointer
    int const loadBranchError = cache_branch(branchName);
    
    if (loadBranchError) {
        std::cerr << "\nError: Failed to cache TBranch pointer.\n";
        return 1;
    }
    
    // Cache TLeaf pointer
    int const cacheLeafError = cache_leaf();
    
    if (cacheLeafError) {
        std::cerr << "\nError: Failed to cache TLeaf pointer.\n";
        return 1;
    }
    
    return 0;
}


/*
 * Instantiate a ROOT histogram object
 * 
 * @nbins // number of channels (bins)
 * @xmin // min channel value (i.e., usually 0, but maybe non-zero, or negative)
 * @xmax // max channel value (i.e., 3500 photons, 2000 mm, 30 ns, etc)
 * 
 * NOTE: Sets title based on input type
 * i.e., ASCII = "Energy Spectrum"
 * ROOT Ntuple = name of ntuple
 * (this function wont be called with ROOT hist, so dont need to handle that)
 * 
 * NOTE: Chooses TH1 type based on input type
 * i.e., ASCII = TH1I
 * ROOT Ntuple = TH1I, OR, TH1D
 * 
 * TODO: Have user specify whether hist params should be automatically calculated,
 * via finding max value from dataset (+say 5-10% for xmax), and either calulating 
 * nbins, or leaving at 1024-4096 bins or if theyd like a specific setup (i.e.,
 * 2048 channels)
 * 
 * TODO: Im calling this in fill_hist_ntuple & fill_hist_ascii:
 * hpx->SetDirectory(nullptr);
 * Is it not better to just call it at the end of this function?
 * 
 * TODO: nbins, xmin, xmax, args are currently useless at the moment, largely just 
 * placeholders for when i implement replotting, etc
 */
int create_hist(int nbins = -1, double xmin = -1., double xmax = -1.) {
    // Histogram args
    std::string title;
    std::string legendTitle;

    // Will be used to store data type of chosen TBranch if ROOT TTree is selected
    std::string_view leafType; // std::string leafType;
    
    // Handle ROOT Ntuple
    
    // Get the TTree and TBranch names
    std::string const nTupleName = nTuple->GetName();
    char const* branchName = branch->GetName();
    
    // Name the histogram after the ROOT object
    title = nTupleName + "Hpx"; // NOTE: Using the TTree name itself causes ROOT to think the histogram already exists
    legendTitle = branchName;
    
    // Get the minimum/maximum values in the chosen TBranch
    double const branchMin = nTuple->GetMinimum(branchName);
    double const branchMax = nTuple->GetMaximum(branchName);
    
    // If histogram args havent been passed, auto bin
    if (nbins == -1 && xmin == -1. && xmax == -1.) {
        // Set xmin to zero or branch minimum, whichever is lower, and xmax to max + 10%
        nbins = 2048; // TODO: Dynamic binning
        xmin = ((branchMin < 0.) ? branchMin : 0.); // should be zero, unless negative axis
        xmax = (branchMax) * 1.1; // +10% (NOTE: Using max is very succeptible to outliers)
    }
    // NOTE: If they have been passed, use them
    
    // Get TTree data type by reading the leaves
    leafType = leaf->GetTypeName();
    // std::cout << "LEAF TYPE: " << leafType << "\n";
    
    
    std::cout << "\nHistogram args set to:\n";
    std::cout << ">>> Num Bins: " << nbins << " XMIN: " << xmin << " XMAX: " << xmax << "\n";
    
    // Reject invalid histogram args
    if (title.empty() || legendTitle.empty() || nbins == -1 || xmin == -1 || xmax == -1) {
        std::cerr << "\nError: Failed to define histogram args.\n";
        root_cleanup();
        return 1;
    }
    
    // Handle ROOT TBranches containing doubles
    std::cout << "\n>>> Creating TH1D...\n";
    
    // Create a histogram (TH1D = double)
    hpx = new TH1D(
        title.c_str(), // "hpx", // Legend title
        legendTitle.c_str(), // "distance travelled", // Histo title
        nbins, // num bins
        xmin, // x low
        xmax // x up
    );
    
    // Handle missing histogram (failed instantiation for any reason)
    if (!hpx) {
        std::cerr << "\nError (create_hist()): Histogram not found!\n";
        // Need to close file (could be ascii or root)
        root_cleanup();
        return 1;
    }
    
    // X-axis title
    // hpx->SetXTitle("Distance (mm)");
    
    // Detach the histogram from the current open ROOT TFile
    // hpx->SetDirectory(nullptr);
    
    std::cout << "\nHistogram instantiated.\n";
    
    // No errors, all good
    return 0;
}


/*
 * Iterate through tree branch, populating histogram with per-event values
 * 
 * TODO: Make post-processing optional
 * 
 * TODO: Make it so branch or branch name is actually passed in as param
 */
int fill_hist_ntuple() {
    // Handle invalid branch name
    if (!branch) {
        std::cerr << "\nError: TTree branch not found. Couldnt fill histogram. Closing root file and deconstructing Ntuple.\n";
        root_cleanup();
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (fill_hist_ntuple()): Histogram not found!\n";
        root_cleanup();
        return 1;
    }
    
    // Query the cached TBranch for its name
    char const* branchName = branch->GetName();
    
    // std::cout << "\nDisabling non-essential branches...\n";
    
    // Disable all branches from being read by TTree->GetEntry()
    nTuple->SetBranchStatus("*", false);
    // NOTE: It is recommended to only read the branches actually needed
    
    // Enable only the branch we need
    nTuple->SetBranchStatus(branchName, true);
    // NOTE: For iteration over multiple branches, we would just enable those too
    
    // ...
    std::cout << "\nNon-essential branches have been disabled.\n";
    
    // ...
    if (!leaf) {
        std::cerr << "\nError [fill_hist_ntuple()]: Pointer to leaf in current TTree branch not found!\n";
        root_cleanup();
        return 1;
    }
    
    // Define branch entry variables
    double doubleEntry;
    
    std::cout << "\nSetting branch address for: " << branchName << "\n";
    
    // To read a tree, neeed to associate variables with the trees branches
    nTuple->SetBranchAddress(branchName, &doubleEntry);
    // NOTE: When loading a tree entry, the tree will set the variables to the 
    // branches value as read from the storage
    
    // NOTE: Multiple variables can be set to different branches here, and TTree->GetEntry(i) 
    // will update all variables to the current index
    
    // ...
    std::cout << "\nBranch address set to: " << branchName << "\n";

    // Get the number of entries in the branch (i.e., length for iteration limit)
    long long const numEntries = branch->GetEntries();
    // long long const numEntries = 0; // TEST: For debugging (triggering the following clause)
    
    // ...
    std::cout << "\nGot num entries: " << numEntries << "\n";
    
    // Reject empty branch
    if (numEntries == 0) {
        std::cerr << "\nError: Selected TTree branch contains no entries! Closing root file, and deconstructing Ntuple/branch.\n";
        root_cleanup();
        return 1;
    }
    
    // ...
    std::cout << "\nFilling histogram from TTree branch...\n";
    
    // Read all entries in the branch
    for (long long i = 0; i < numEntries; i++) {
        // Load the data for the given tree entry
        // branch->GetEntry(i); // returns bytes read, not the actual val
        // NOTE: The "entry" variable will now be updated
        
        // Load the data for the given tree entry (TODO: Compare speed)
        nTuple->GetEntry(i); // returns bytes read, not the actual val
        // NOTE: The "entry" variable will now be updated
        
        // NOTE: Calling "GetEntry" on the TTree itself is more flexible and scalable,
        // if working with multiple branches at the same time, it ensures the indices
        // match up during iteration, and saves calling GetEntry on each individual branch
        
        // std::cout << entry; // debug
        
        // Add a count to the appropriate bin for that value
        hpx->Fill(doubleEntry);
    }
    
    // ...
    std::cout << "\nBranch iteration complete.\n";
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    root_cleanup(); 
    // NOTE: Now that histogram has been populated via nTuple data, the ROOT infile
    // can safely be closed, doing so beforehand may cause undefined behaviour
    // NOTE: There may be some future usecase where this is undesireable, i.e.,
    // keeping the ntuple available for another reason, although in that case the
    // ROOT file itself would also need to stay open
    
    // ...
    std::cout << "\nROOT input file closed.\n";
    
    // Success message
    std::cout << "\nHistogram filled from ROOT Ntuple.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Instantiates a canvas object, populating the global variable
 */
int create_canvas() {
    std::cout << "\nCreating canvas...\n";
    
    // Canvas args
    Int_t const winX = 0; // Top left of screen
    Int_t const winY = 0; // Top left of screen
    Int_t const width = 1200;
    Int_t const height = 800;
    
    // Create a canvas display
    canvas = new TCanvas("canvas", "Histogram Viewer", winX, winY, width, height);
    
    // Handle error creating canvas
    if (!canvas) {
        std::cerr << "\nError (create_canvas()): Couldnt create canvas!\n";
        return 1;
        
        // NOTE: Both ASCII and ROOT files (and objects) should already be cleaned
        // by this point, the only thing that may be floating around in memory is
        // the histogram
        
        // TODO: Maybe:
        // delete hpx;
        // hpx = nullptr;
    }
    
    std::cout << "\nCanvas created.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Renders the populated histogram object on to the instantiated canvas
 * 
 * NOTE: May not always want to setOptStat(0), is useful for energy spectra,
 * but for exponentials etc, having a way to leave it enabled is useful
 */
int render_hist(bool hideDefaultStats = false) {
    // Handle missing canvas
    if (!canvas) {
        std::cerr << "\nError (render_hist()): Couldnt find canvas!\n";
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (render_hist()): Histogram not found!\n";
        // NOTE: All files and objects should already be closed/deconstructed at this point
        return 1;
    }
    
    // ...
    std::cout << "\nRendering histogram to canvas...\n";
    
    // Draw histogram to the canvas with default option
    hpx->Draw(); // NOTE: With histos filled from ASCII & ntuples, "HIST" no longer needed
    
    // ...
    canvas->Update(); // NOTE: Afaik, this is not needed
    
    // Clean the default histogram statistics box (498.4, 291.1)
    if (hideDefaultStats) gStyle->SetOptStat(0); // default = 1111 (NOTE: 000001111 with zeros removed)
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
 * Validate file path, load file into memory, instantiate histogram, fill histogram,
 * instantiate canvas, render histogram
 * 
 * TODO: Maybe move away from global objects, and lean into functional a little more
 * 
 * plot("../final/build/output0.root", "TrackData", "DetectionDistance", 4096, 0, 2500)
 * 
 * plot("../final/build/output0.root", "TrackData", "TimeOfFlight", 4096, 0, 20)
 */
int plot(
    std::string const userPath,
    std::string const treeName = "",
    std::string const branchName = "",
    int const nbins = -1,
    double const xmin = -1.,
    double const xmax = -1.
) {
    // Error handlers to catch bad program state (these should all have been cleared)
    if (leaf) {
        std::cout << "\nFound existing Ntuple branch leaf, clearing...\n";
        return 1;
    }
    if (branch) {
        std::cout << "\nFound existing Ntuple branch, clearing...\n";
        return 1;
    }
    if (nTuple) {
        std::cout << "\nFound existing Ntuple, clearing...\n";
        return 1;
    }
    if (inROOT) {
        std::cout << "\nFound existing ROOT file, clearing...\n";
        return 1;
    }
    // NOTE: The inASCII & inROOT cases should never really flag true now (closed after loading 
    // TH1D or Ntuple, respectively, or on error trying to load them)
    
    // Incase plot will be called multiple times in succession, ensure histo cleared each time
    if (hpx) {
        std::cout << "\nFound existing histogram, clearing...\n";
        // hpx->Delete(); // TODO: Not sure if this is needed (no im pretty sure it causes seg fault when trying to delete on next line lol)
        delete hpx;
        hpx = nullptr;
    }
   
    // NOTE: There may be a way to keep existing canvas, but only creating canvas if (!canvas) not working as intended, see create_canvas() notes below
    if (canvas) {
        std::cout << "\nFound existing canvas, clearing...\n";
        delete canvas;
        canvas = nullptr;
        // canvas->Clear(); // NOTE: This allows for if (!canvas) to work as intended
    }
    
    // Check provided path is valid (will return empty string if not valid)
    std::string const path = check_path(userPath);
    
    if (path.empty()) {
        std::cerr << "\nAborting: Invalid path error!\n";
        return 1;
    }
    
    // Attempt to open the ROOT file
    int const loadFileError = load_root_file(path);
    
    if (loadFileError) {
        std::cerr << "\nAborting: Load file error!\n";
        return 1;
    }
    
    // Attempt to cache ROOT TTree, TBranch, and TLeaf
    int const loadRootTreeError = load_root_tree(treeName.c_str(), branchName.c_str());
    
    if (loadRootTreeError) {
        std::cerr << "\nAborting: Load file error!\n";
        return 1;
    }
    
    // Attempt to instantiate histogram object
    int const histError = create_hist(nbins, xmin, xmax);
    
    if (histError) {
        std::cerr << "\nAborting: Create hist error!\n";
        return 1;
    }
    
    // Attempt to populate histogram from ASCII or ROOT file
    int const fillError = fill_hist_ntuple();
    
    if (fillError) {
        std::cerr << "\nAborting: Fill hist error!\n";
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
    
    return 0;
}

/*
 * ...
 */
TF1* exponential_decay() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
    std::cout << "Peak X: " << peakX << " Peak Y: " << peakY << "\n";
    
     // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    // (it’s a fit window, not a Gaussian width parameter)
    // The Gaussian itself mathematically extends to infinity.    
    double const low = hpx->GetXaxis()->GetXmin();
    double const high = hpx->GetXaxis()->GetXmax();
    
    // Automate decay constant (tau_fall)
    // NOTE: Find the point where the signal drops to 37% (1/e) of its peak after the max
    int fallBin = maxBin;
    
    // While current bin less than max bins, and current bin value still greater than 1/e of the peak
    while ((fallBin < hpx->GetNbinsX()) && (hpx->GetBinContent(fallBin) > peakY * 0.368)) {
        fallBin++;
    }
    
    std::cout << "1/e Bin: " << fallBin << "\n";
    
    // ...
    double const x_fall = hpx->GetBinCenter(fallBin);
    double estimated_tau_fall = x_fall - peakX; // TODO: Comments
    
    if (estimated_tau_fall <= 0) estimated_tau_fall = 10.0; // Fallback safety
    
    // Define the fit function
    // auto fitFn = new TF1("fitFn", "expo", low, high);
    // auto fitFn = new TF1("fitFn", "[0]*exp(-x/[1])", low, high); // exponential decay
    auto fitFn = new TF1("fitFn", "[0]*exp(-x/[1])", peakX, high); // set window low bound to peak
    // NOTE: "expo" is built-in ROOT shorthand for [0]*exp(-x/[1])
    // [A]*exp(-x/[tau_fall])
    // Where: A = amplitude, tau_fall = decay time constant
    
    // ..
    fitFn->SetParameters(peakY, estimated_tau_fall);
    fitFn->SetParNames("Amplitude", "Decay Constant");
    
    return fitFn;
}

/*
 * Exponential decay with a fast and slow component
 * 
 * AKA: biexponential decay OR double exponential decay
 * 
 * y(t) = [A_1]*exp(-x/[tau_fall_1]) + [A_2]*exp(-x/[tau_fall_2]) + C
 * 
 * Where:
 * - y(t) = Quantity or signal remaining at time "t"
 * - A_1 = amplitude (fast component)
 * - tau_fall_1 = decay time constant (fast component)
 * - A_2 = amplitude (slow component)
 * - tau_fall_2 = decay time constant (slow component)
 * - C = Constant baseline or plateau (0 in this case with no background component)
 */
TF1* exponential_decay_two_phase() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
    std::cout << "Peak X: " << peakX << " Peak Y: " << peakY << "\n";
    
    // Define the fit window (low & high)
    // NOTE: the region of the histogram ROOT is allowed to use for the fit.
    double const low = hpx->GetXaxis()->GetXmin();
    double const high = hpx->GetXaxis()->GetXmax();
    
    
    
    
    // ...
    // double const constant = 0.;
    
    // Define the fit function
    // std::string const fnString = "[0]*exp(-x/[1]) + [2]*exp(-x/[3]) + [4]";
    std::string const fnString = "[0]*exp(-x/[1]) + [2]*exp(-x/[3])";
    
    // Instantiate the fit object
    // auto fitFn = new TF1("fitFn", fnString.c_str(), low, high); // using 0 as min causes fit error (as no rise component)
    auto fitFn = new TF1("fitFn", fnString.c_str(), peakX, high); // set window low bound to peak
    
    // Assign fit params
    fitFn->SetParameters(amplitude_fast, tau_fall_fast, amplitudeSlow, tau_fall_slow);
    fitFn->SetParNames("Amplitude (fast)", "Decay Constant (fast)", "Amplitude (slow)", "Decay Constant (slow)");
    
    // ...
    return fitFn;
}

/*
 * Automatically find exponential centroid, derive hist low/high, fit convolution of 
 * exponential rise and decay to the data, and display the fit
 * 
 * TODO: This didnt quite fit to my data, but it may come in handy at a later date
 * 
 * TODO: This needs updating with some of the updated error handling from ascii_fit.cc
 */
TF1* exponential_rise_exponential_decay() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
    std::cout << "Peak X: " << peakX << " Peak Y: " << peakY << "\n";
    
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
    double const x_rise = hpx->GetBinCenter(riseBin); // grab center of rise bin
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
    double const x_fall = hpx->GetBinCenter(fallBin);
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
    
    return pulseFit;
}

/*
 * TODO ...
 */
int rise_and_decay() {
    return 1;
}

/*
 * TODO ...
 */
int fit_log_normal () {
    return 1;
}

/*
 * TODO ...
 */
int ex_gaussian () {
    return 1;
}

/*
 * TODO ...
 */
int gamma_dist () {
    return 1;
}

/*
 * TODO: Define a top level: fit(), method, which directs you to exponential, double_exponential, etc, etc
 * 
 * TODO: Needs faster rise time to fit to the photon distance travelled data
 * 
 * 1) Log normal
 * 2) Gamma Distribution
 * 3) ExGaussian
 */
int fit(std::string const function) {
    // ...
    TF1* fitFn = nullptr;
    
    // ...
    if (function == "exp_decay"){
        fitFn = exponential_decay();
    }
    if (function == "double_exp_decay"){
        fitFn = exponential_decay_two_phase();
    }
    else if (function == "exp_rise_decay") {
        fitFn = exponential_rise_exponential_decay();
    }
    
    // ...
    if (!fitFn) {
        std::cerr << "Aborting: Failed to get fit function.\n";
        return 1;
    }
    
    // ...
    TFitResultPtr const result = hpx->Fit(fitFn, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = improves the fit quality
    
    // Draw the fit line (ROOT internally stores the fit function with the histogram after fitting)
    hpx->GetFunction("fitFn")->Draw("SAME");
    // NOTE: The "HIST" option suppresses drawing associated functions (including fits),
    // hence why "hpx->Draw()" works here instead of drawing the fit fn (but we lose the histogram view),
    // and why "hpx->Draw("HIST")" doesnt work alone, so calling draw on the stored fn is the way,
    // it is also not enough to just call Modified() & Update().
    
    // TODO: Display relevant fit values in top right info box post-fit (chi^2, centroid, etc)
    
    return 0;
}
