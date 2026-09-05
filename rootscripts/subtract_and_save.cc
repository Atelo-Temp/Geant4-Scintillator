// Load an ASCII (.Spe) spectrum file into memory, fill a histogram, 
// load an ASCII (.Spe) background file into memory, fill another histogram,
// perform background subtraction by using live time values parsed from the Spe file,
// display the resultant spectrum on a canvas

// ROOT lib
#include <TDirectory.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TStyle.h> // gStyle
#include <TVirtualPad.h>
#include <TFile.h>

// C lib
#include <iostream> // std::cout, std::cerr
#include <fstream> // std::ifstream
#include <sstream> // std:: istringstream
#include <optional>

// Global root object variables
std::ifstream in;
TCanvas* canvas = nullptr;
TH1* hpx = nullptr;

// Metadata object type for ASCII (.Spe) files
struct SpeMetaData {
    // Will be assigned the live and real time of the detector
    int liveTime = 0;
    int realTime = 0;
    
    // Will be assigned start and end channel numbers (i.e., 0 and 2047)
    int start = 0;
    int end = 0;
}; // TODO: Will need a metadata object for each hist
// TODO: Maybe display each of the original, the background, and the subtracted on individual pads on the canvas

// Object containing file path and file extension
struct PathResult {
    std::string const path;
    std::string const token;
};

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int subtract_and_save() {
    // Usage
    std::cout << "\n-----------------------------------------------------------------------\n";
    std::cout << "\nConvert ASCII to ROOT histogram and perform background subtraction.\n";
    std::cout << "\nTo get started, call: subtract(\"~/dir/spectrum_filename.Spe\", \"~/dir/background_filename.Spe\"), passing path to ASCII (.Spe) file as params.\n";
    std::cout << "\nOnce thats done, call: save(\"~/dir/outfile.root\"), passing path to ROOT outfile as param.\n";
    std::cout << "\n-----------------------------------------------------------------------\n";
    
    // fin
    return 0;
}

/*
 * Get file extension from passed path
 * 
 * NOTE: Returns empty string on error, or if no file extension found
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
 * Validate that the file extension matches list of supported file extensions
 */
int assert_extension(std::string const& token) {
    // Acceptable file extensions
    std::string const spe = ".Spe"; // const char*
    std::string const root = ".root"; // const char*
    
    // Check path ends with valid extension, reject invalid file type
    if ((token != spe) && (token != root)) {
        // Write to stdout
        std::cerr << "\nError: Invalid extension.\n";
        
        // Error value
        return 1;
    }
    // If its an ASCII file extension, log it and set ASCII flag
    else if (token == spe) {
        std::cout << "\nASCII file detected.\n";
    }
    // If its a ROOT file extension, log it and set ROOT flag
    else if (token == root) {
        std::cout << "\nROOT file detected.\n";
    }
    
    return 0;
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
 * 
 * TODO: Maybe return { returnPath, token }
 * then do FileType assignment outside of this function (@callee)
 * currently introduces dependency on FileType, but for save(...), thats a hinderance
 * 
 * TODO: Maybe extract file extensions out to global variable, or struct
 */
PathResult check_path(std::string const& path) {
    // Print path to stdout
    std::cout << "\nUser provided path: " << path << "\n";
    
    // Get file extension
    std::string const token = get_extension(path);
    
    if (token.empty()) {
        std::cerr << "\nError: File extension not found.\n";
        return PathResult {};
    }
    
    // Check it matches supported extensions
    int const invalidExtension = assert_extension(token);
    
    if (invalidExtension) {
        std::cerr << "\nError: Extension assertion failed.\n";
        return PathResult {};
    }

    // Check if tilde expansion needed, perform it if so
    std::string const returnPath = expand_path(path);
    
    if (returnPath.empty()) {
        std::cerr << "\nError: Expanded path empty.\n";
        return PathResult {};
    }
    
    // Confirmation status
    std::cout << "\nPath is valid.\n";
    
    // No errors, all good
    return PathResult { returnPath, token };
}

/*
 * Validates .Spe file can be opened/exists, loading it into local memory in readonly mode if so
 */
int load_ascii(std::string const& path) {
    // Open the ASCII file with validated .Spe extension
    in.open(path, std::ios_base::in);
    // NOTE: Read-only mode (std::ios_base::in)
    
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
 * Read lines from the BSA-style ASCII file, extracting parameters from relevant blocks
 * 
 * Leaves the infile on the line just before channel 0 data
 * 
 * NOTE: If "$MEAS_TIM:" is encountered:
 * 1) Read the next line for detector times
 * 300 307
 * 2) Set
 * liveTime = 300;
 * realTime = 307;
 * 
 * Parses the infile header for start, $DATA, then skip next line, then the following line is bin 0
 * 
 * NOTE: If "$DATA:" is encountered:
 * 1) Read the next line for min/max channel numbers, i.e.:
 * 0 2047
 * 2) Parse the channel numbers and assign them to variables, i.e.:
 * start = 0;
 * end = 2047;
 * 3) Leaves the infile on the line before first data entry, so subsequently fill_hist
 * first call to getline(...) will immediately return data
 * 
 * TODO: Probably wanna validate is expected ASCII format too:
 * line 1: $SPEC_ID:
 * 
 * TODO: Since some blocks ($) come after the data block, could add a parse_footer() method down the line too,
 * but currently those blocks are not needed
 * 
 * TODO: Not currently worthwhile, as my BSA-style files all follow the same format, but
 * for scalability in the future, may want to have a line counter variable, and when
 * "$DATA:" is encountered, mark that line number, so if say the "$MEAS_TIM" block is
 * after the data block for whatever reason, then set line number back to that number at
 * the end of this method, so that fill_hist still begins at the correct spot
 * 
 * TODO: if (!in.good()) break;
 */
int parse_headers(SpeMetaData& metaData) {
    // Handle missing input file
    if (!in.is_open()) {
        std::cerr << "\nError: No infile to read!\n";
        return 1;
    }
    
    std::cout << "\nParsing ASCII headers...\n";
    
    // Buffer to store lines from the infile
    std::string buffer;
    
    // Loop control flags
    bool parsedTimeHeader = false;
    bool parsedDataHeader = false;
    // NOTE: Using two flags ensures regardless of which order the headers are encountered,
    // both will be parsed before loop completes
    
    // Execute until relevant headers have been parsed
    while (!parsedTimeHeader || !parsedDataHeader) {
        // std::cout << nlines << "\n"; // Print line number (NOTE: debug)
        // std::cout << buffer << "\n"; // Print line contents (NOTE: debug)
        
        // Get line reads a line from input stream into a string, until end of stream encountered
        if (!std::getline(in, buffer)) {
            std::cerr << "\nError: Failed to read line.\n";
            return 1;
        }
        // NOTE: Stores characters from current line of infile in the buffer, until "\n" is encountered
        // NOTE: Contents of buffer are erased at the start of next line before reading commences again
        // NOTE: The "\n" at the end of the line is not stored in the buffer, but "\r" etc may be
        
        // Remove trailing carriage return (lines contain hidden carriage return: "$MEAS_TIM:\r")
        if (!buffer.empty() && (buffer.back() == '\r')) buffer.pop_back();
        // NOTE: Single quote for single character delimiter, double quotes for string of chars
        
        // Block identifier containing detector time information (live time & real time)
        if (buffer == "$MEAS_TIM:") {
            // Go to next line, read it into the buffer
            if (!std::getline(in, buffer)) {
                std::cerr << "\nError: Failed to read line.\n";
                return 1;
            }
            // std::cout << buffer << "\n"; // NOTE: debug
            
            // Create a stream for the current line to parse individual values
            std::istringstream stringStream(buffer);
            // NOTE: Using string stream saves doing: lineContent.substr(whitespaceIdx); etc
            // also, will convert from string to integer automatically
            
            // Try to pipe the line into the two line contents variables
            // NOTE: The ">>" operator skips preceding whitespace, then reads chars until whitespace 
            // is encountered again, i.e., extracts one word, or one number, etc, at a time, so:
            if (!(stringStream >> metaData.liveTime >> metaData.realTime)) {
                std::cerr << "\nError: Failed to pipe line.\n"; // NOTE: Expecting: <livetime> <truetime>
                return 1;
            }
            // NOTE: The ">>" operator attempts to read data from the stream and parse it into
            // the variable. The expression "stream >> variable" returns a reference to the stream
            // itself. When placed inside an if statement, the stream is automatically evaluated as 
            // a boolean, returning true if read was successful or false if it failed.       
            
            // Debug
            std::cout << "\nLive Time: " << metaData.liveTime << " Real Time: " << metaData.realTime << "\n";
            
            // Fin 1/2
            parsedTimeHeader = true;
        }
        
        // This header marks the start of the data entries
        // NOTE: The next line will contain lower/upper channel numbers,
        // and then the line after that will be the channel 0 value
        else if (buffer == "$DATA:") {
            // Go to next line, try to read it into the buffer
            if (!std::getline(in, buffer)) {
                std::cerr << "\nError: Failed to read line.\n";
                return 1;
            }
            // std::cout << buffer << "\n"; // NOTE: debug
            
            // Create a stream for the current line
            std::istringstream stringStream(buffer);
            
            // Expecting: <lower channel number> <upper channel number>
            if (!(stringStream >> metaData.start >> metaData.end)) {
                std::cerr << "\nError: Failed to pipe line.\n";
                return 1;
            }
            
            // Debug
            std::cout << "\nStart: " << metaData.start << " End: " << metaData.end << "\n";
            
            // Ensure end > start
            if (metaData.start >= metaData.end) {
                std::cerr << "\nError: Malformed channel range.\n";
                return 1;
            }
            
            // Fin 2/2
            parsedDataHeader = true;
        }
    }
    
    std::cout << "\nParsed ASCII headers.\n";
    
    // ...
    return 0;
}

/*
 * Instantiate a ROOT histogram object
 * 
 * // Histogram args
 * @nbins // number of channels (bins)
 * @xmin // minimum channel
 * @xmax // maximum channel
 * 
 * TODO: Likely best to generate unique name by querying gDirectory
 */
std::optional<TH1*> create_hist(int const& nbins = 2048, int const& xmin = 0, int const& xmax = 2048) {
//     if (gDirectory->FindObject("hpx")) {
//         // append suffix to name to avoid naming conflicts
//         int i = 0;
//         
//         while (true) {
//             std::string title = "hpx";
//             std::string suffix = std::to_string(i);
//             if (!gDirectory->FindObject((title + suffix).c_str())) {
//                 ...
//                 break;
//             }
//         }
//     }
    
    // Create a histogram (TH1I = integer - channel/counts both ints)
    auto hpx = new TH1D(
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
        return std::nullopt;
    }
    
    // X-axis title
    hpx->SetXTitle("Channels");
    
    // No errors, all good
    return hpx;
}

/*
 * Iterate through ASCII file, populating histogram with per-bin values
 * 
 * NOTE: Assumes parse_headers() has been called first, so that next call to getline()
 * returns channel 0 data
 * 
 * TODO: May want a separate close_ascii() method, and call it from the top level,
 * rather than calling it inside of here (separation of responsibilities)
 * 
 * TODO: if (!in.good()) break;
 */
int fill_hist_ascii(TH1* hpx, int const& start, int const& end) {
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
    
    std::cout << "\nFilling histogram from ASCII file...\n";

    // Buffer to store lines from the infile
    std::string buffer;
    
    // Only try to parse once $DATA header encountered
    long long dataValue;

    // Only parse data values (i.e. next 2048 lines for 2048 channels)
    // Increment from start up to max channel number, i.e. 2047
    for (int i = start; i <= end; i++) {
        // std::cout << buffer << "\n"; // NOTE: debug
        
        // Go to next line, try to read it into the buffer
        if (!std::getline(in, buffer)) {
            std::cerr << "\nError: Failed to read line\n";
            return 1;
        }
        
        // Create a stream for the current line
        std::istringstream dataStream(buffer);
        
        // Convert string to integer
        if (!(dataStream >> dataValue)) {
            std::cerr << "\nError: Failed to pipe data\n";
            return 1;
        }
        // NOTE: Strips leading whitespace, which is desirable as data entries have format:
        // "       0", "    8010", etc
        // NOTE: std::from_chars doesnt strip leading whitespace, otherwise it would be preferred
        
        // Set current bin to the integer value on current line
        hpx->SetBinContent(i + 1, dataValue);
        // NOTE: Dont use h->Fill(converted), Instead of filling bin 0 with line 0,
        // its filling bin 0 every time 0 is encountered
        
        // NOTE: When instantiating a ROOT histogram with nbins = 2048, there are actually
        // 2050 bins created, with bin 0 being underflow, and bin 2050 being overflow, so we
        // want to increment i by 1 when writing to bins
    }
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    in.close();
    in.clear();
    
    std::cout << "\nHistogram filled.\n";
    
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
int render_hist(TH1* hpx) {
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
    // hpx->Draw(); // NOTE: "HIST" not needed when instantiating TH1 ourselves
    hpx->Draw("HIST"); // NOTE: When calling sumw2 ...

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
 * Validate file paths, load files into memory, instantiate histograms, fill histograms,
 * instantiate canvas, render subtracted histogram
 * 
 * TODO: Display each of:
 * - Original spectrum
 * - Background spectrum
 * - Subtracted spectrum
 * On individual TPads in the TCanvas
 */
int subtract(std::string const spectrumPath, std::string const backgroundPath) {
    // Caches
    std::vector<std::string> paths = { spectrumPath, backgroundPath };
    std::vector<TH1*> histograms = {};
    std::vector<SpeMetaData> metaDatum = {};
    
    // Instantiate two histogram objects, one for the spectrum, another for the background
    for (int i = 0; i < paths.size(); i++) {
        // Check provided path is valid (will return empty string if not valid)
        auto const [path, token] = check_path(paths[i]);
        
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
        
        // Attempt to parse detector times and channel numbers from the BSA-style blocks
        SpeMetaData metaData; // struct SpeMetaData metaData; // NOTE: Can do either in c++
        int const headerError = parse_headers(metaData);
        
        if (headerError) {
            std::cerr << "\nAborting: Header parsing error!\n";
            return 1;
        }
        
        // Cache the instantiated and filled meta data object
        metaDatum.push_back(metaData);
        
        // Attempt to instantiate histogram object
        std::optional<TH1*> histResult = create_hist((metaData.end - metaData.start + 1), metaData.start, (metaData.end + 1));
        
        if (!histResult.has_value()) {
            std::cerr << "\nAborting: Create hist error!\n";
            return 1;
        }
        
        // ...
        TH1* hist = histResult.value();
        
        // Attempt to populate histogram from ASCII file
        int const fillError = fill_hist_ascii(hist, metaData.start, metaData.end);
        
        if (fillError) {
            std::cerr << "\nAborting: Fill hist error!\n";
            return 1;
        }
        
        // Cache pointer to the instantiated and filled histogram
        histograms.push_back(hist);
    }
    
    // Vector indices for each respective spectra
    int const spectrumIdx = 0;
    int const backgroundIdx = 1;
    
    // Enable error tracking on the source histograms
    histograms[spectrumIdx]->Sumw2();
    histograms[backgroundIdx]->Sumw2();
    
    // Clone the original spectrum (this copy will hold the final result)
    TH1* subtracted = static_cast<TH1*>(histograms[spectrumIdx]->Clone("subtractedHpx"));
    
    // Convert values to double (else would have to multiply one of them by 1. when doing division)
    double const spectrumLiveTime = metaDatum[spectrumIdx].liveTime;
    double const backgroundLiveTime = metaDatum[backgroundIdx].liveTime;
    
    // Calculate the scaling factor
    double const bkgScaling = spectrumLiveTime / backgroundLiveTime;
    
    // Subtract the scaled background directly from the clone
    // subtracted->Add(histograms[spectrumIdx], histograms[backgroundIdx], 1, -bkgScaling);
    subtracted->Add(histograms[backgroundIdx], -bkgScaling);
    // NOTE: Can use either addition function overload above, but this way is likely
    // safer, as it avoids accidents with histogram matching
    
    // NOTE: DEBUG
    // for (int i = 0; i < subtracted->GetNbinsX(); i++) {
    //     double value = subtracted->GetBinContent(i);
    //     if (value < 0) {
    //         std::cout << "NEGATIVE VALUE: " << value << " AT IDX: " << i << "\n";
    //     }
    // }
    
    // Attempt to create canvas
    int const canvasError = create_canvas();
    
    if (canvasError) {
        std::cerr << "\nAborting: Create canvas error!\n";
        return 1;
    }
    
    // Attempt to draw the histogram to the canvas
    int const renderError = render_hist(subtracted);
    
    if (renderError) {
        std::cerr << "\nAborting: Render histogram error!\n";
        return 1;
    }
    
    // Cap the y-axis at zero (dont show negative y)
    subtracted->SetMinimum(0);
    gPad->Modified();
    canvas->Update();
    // NOTE: Since there is mostly only ~(0 to 4) negative counts in ~50 bins towards
    // the higher energy side of the spectrum NaI 2' 137Cs, visually this is fine,
    // and allows for more direct comparison to simulated (no background spectra)
    
    // paths.
    hpx = subtracted;
    
    // ...
    return 0;
}


/*
 * Prompts user for input, reads response, returns success/fail val based on response
 */
int prompt_user_char(std::string const &question = "Do you wish to overwrite existing file?") {
    // Prompt user for input
    std::cout << "\n" << question << "\n";
    std::cout << "[y/n]: ";
    
    // Store user input
    std::string userInput;
    
    // Enter user input loop
    while (true) {
        // Capture the line
        if (!std::getline(std::cin, userInput)) { // NOTE: std::cin >> userInput;
            std::cerr << "\nError: Failed to read from stdin.\n";
            return 1;
        } 
        
        // Handle yes/no reponse, or invalid input
        if (userInput == "y") {
            return 0;
        } 
        else if (userInput == "n") {
            return 1;
        }
        // If userInput.empty(), or invalid char, etc
        else {
            // Return to previous line, move to start of prev line, clear stdout
            std::cout << "\033[A" << "\r" << "\033[2K" << "[y/n]: " << std::flush;
            
            // NOTE: "\033[A" = move the cursor up one line
            // "\r" = move the cursor to the beginning of that line
            // "033[2K" = clear everything from the cursor down to the bottom the screen
            
            // NOTE: Flush ensures the reset sequence prints to the screen immediately
            // without waiting for a standard neline "\n" to force a bufffer flush
        }
        // NOTE: Else doesnt really need enclosure here
    }
}

/*
 * Check if file already exists, if it does, check if user wants to overwrite
 * 
 * TODO: Edge case? if (temp && !temp->IsOpen())
 * 
 * NOTE: Try/catch not needed here, open doesnt appear to throw, just prints error
 * to stdout and sets temp = nullptr
 * 
 * TODO: Probably worth doing a gDirectory check before TFile::Open to avoid the
 * error still though
 */
int check_file(char const* path) {
    // Attempt to open file with provided filename
    TFile const* temp = TFile::Open(path, "READ"); // NOTE: Read only mode
    
    // If file already exists and was opened
    // NOTE: Not sure if !IsZombie() is needed, as it just checks for corruption,
    if (temp && !temp->IsZombie() && temp->IsOpen()) {
        std::cerr << "\nError [check_file()]: File already exists!\n";
        // Close the readonly file
        delete temp;
        temp = nullptr;
                
        // Get user response
        int const abort = prompt_user_char(); // NOTE: 0 = overwrite, 1 = abort

        // ...
        if (!abort) std::cout << "\nOverwriting existing file...\n";
        
        return abort;
    }
    
    // If the file does not already exist (!temp & !temp->IsOpen()) 
    return 0;
}

/*
 * ....
 * 
 * NOTE: This only accepts a ".root" outfile extension
 */
int save_to(std::string const& path, TH1* hpx, std::string const& name) {    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (save()): Histogram not found!\n";
        return 1;
    }
    // NOTE: Do this first else subsequent path/file checking is redundant on null hpx
    
    // Check that the provided filename has a ROOT extension
    std::string const extension = get_extension(path);
    
    if (extension.empty() || (extension != ".root")) {
        std::cerr << "\nError {save()}: Invalid extension!\n";
        return 1;
    }
    
    // Check provided path is valid (will return empty string if not valid)
    std::string const validPath = expand_path(path);
    
    if (validPath.empty()) {
        std::cerr << "\nError {save()}: Invalid path!\n";
        return 1;
    }
    
    // Convert from: std::string, to: const char*
    char const* convertedPath = validPath.c_str();
    
    // Check if file already exists, and if so, whether to overwrite
    int const dontWrite = check_file(convertedPath);
    
    if (dontWrite) {
        std::cerr << "\nAborting: Please call save() with a new path.\n";
        return 1;
    }
    
    // Open outfile in recreate mode (creates ROOT file, replacing it if it already exists)
    TFile* outfile = TFile::Open(convertedPath, "RECREATE");
    
    // Ensure outfile is open and ready for write operation
    if (!outfile || !outfile->IsOpen()) {
        std::cerr << "\nError [save()]: Couldnt create/open outfile!\n";
        return 1;
    }
    
    // Write the histogram object to the root file
    outfile->WriteObject(hpx, name.c_str());
    
    // All done
    delete outfile;
    outfile = nullptr;
    
    // Confirmation status
    std::cerr << "\nFile has been saved.\n";
    
    // No errors, all good
    return 0; 
}

/*
 * Wrapper for save_to(), enabling convinient ROOT interactive terminal usage
 * 
 * TODO: Take optional force overwrite param (if omitted, will still ask on naming conflict)
 */
int save(std::string const path) {
    std::cout << "\nAttempting to retrieve histogram...\n";
    
    // Get the active histogram for this plotting session
    if (!hpx) {
        std::cerr << "Aborting: No plotted histogram to save, call plot() first.\n";
    }
    
    std::cout << "\nExtracting histogram name...\n";
    
    // Use the name assigned to the histogram
    std::string const name = hpx->GetName();
    // NOTE: Will be same as ntuple name, or "EnergySpectrum" for ASCII histos
    
    std::cout << "\nAttempting to save histogram \"" << name << "\" to \"" << path << "\"\n";
    
    // Attempt to save it in a root file at the given path location
    int const success = save_to(path, hpx, name);
    
    return success;
}
