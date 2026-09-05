// ...

// ROOT lib
// #include <TDirectory.h> // NOTE: Unused
#include <RtypesCore.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TLeaf.h> // Incomplete type without explicit import
#include <TH1.h>
#include <TCanvas.h>
#include <TROOT.h> // gROOT
#include <TStyle.h> // gStyle
#include <TRandom.h> // gRandom
#include <TVirtualPad.h> // gPad
// Fitting
#include <TF1.h>
#include <TFitResult.h>
#include <TPaveStats.h>
#include <TLatex.h>

// C lib
#include <iostream> // cerr, cin, cout
#include <fstream> // ifstream
#include <sstream> // istringstream
#include <optional> // optional, nullopt
#include <unordered_set> // NOTE: Import not technically needed (something else must be importing it)
#include <unordered_map> // NOTE: ^ same
#include <variant>

// Object containing file path and file extension
struct PathResult {
    std::string const path;
    std::string const token;
};

// Accepted file types
enum class FileType {
    ROOT,
    ASCII,
    NULLFILE
};

// Forward declaration
class PlotSession;
// NOTE: Temporarily declaring this as i think its still a tad too early for header files etc

// Global session pointer
PlotSession* gSession = nullptr;

// TODO: Eventually move class definitions to headers

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int oop_analysis() {
    // Usage
    std::cout << "\n-----------------------------------------------------------------------\n";
    std::cout << "\nConvert ASCII & ROOT Ntuples to Root Histogram.\n";
    std::cout << "\nTo get started, call: plot(\"path.ext\"), passing path to ASCII (.Spe) or ROOT (.root) file as param.\n";
    std::cout << "\nTo refine the initial plot, call: replot(nbins, xmin, xmax, doPostProcessing, sigmaFactor, lowerDiscriminator, channelOffset).\n";
    std::cout << "\nTo save the plotted histogram, call save(\"dir/out.root\").\n";
    std::cout << "\nTo fit the plotted histogram, call fit({centroid1, centroid2, ...}, fwhm), passing curly brackets containing all centroids to fit as arg1 and an estimated fwhm integer as arg2.\n";
    std::cout << "\n-----------------------------------------------------------------------\n";
    
    // fin
    return 0;
}

/*
 * Describe plot(...) API parameters (available overloads), replot(...), add_axis_title(...), save(...)
 * 
 * TODO
 */
int help() {
    std::cout << "\n-----------------------------------------------------------------------\n";
    std::cout << "\n ... \n";
    std::cout << "\n-----------------------------------------------------------------------\n";
    
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
 * Prompts user for integer input in specified range, parses response, validates it, 
 * returns valid response as integer
 * 
 * TODO: "0001" will pass
 * also "    1" will pass
 * 
 * TODO: The stdout from this function is pretty messy, perhaps use other input function line clear methodology
 * 
 * TODO: This could be if (converted >= low && converted <= high) { return converted; }
 * then the std::cout could be taken outside of enclosure
 * 
 * TODO: Could just read number of chars equal to high, instead of user having to press enter
 * ^ but this wouldnt work if low was 1 digit, and high was 2 digits
 * 
 * TODO: std::from_chars (returns error codes instead of throwing)
 * 
 * TODO: if (!std::getline(...))
 */
int prompt_user_int(int const low, int const high) {
    // Prompt user for input
    // std::cout << "[int]: ";
    
    // Store user input
    std::string userInput; // == ""
    
    // NOTE: Could bring converted out here, and break on passing checks, then return at end of function
    
    // Start the read stdin loop
    while (true) {
        // ...
        std::cout << "[int]: ";
        
        // Capture the line;
        if (!std::getline(std::cin, userInput)) {
            std::cerr << "\nError: Failed to read from stdin.\n";
            return 1;
        }
        
        // ..
        int converted = -1;
        size_t numChars; // ensure only one char (prevent full stops etc)
        
        // NOTE: stoi already doing some heavy lifting so kinda dont need to store size,
        // stoi will floor decimals, remove prefix plus sign, etc
        // also, if a negative value is passed, it will fail the range check
        // if just chars are passed, string will be empty
        
        // NOTE: actually, is useful for trailing chars tho, i.e.: 2.35, 2assaf, 2#?!
        
        // ...
        try {
            // ...
            if (userInput == "q") exit(0); // NOTE: Provide an exit without keyboard interrupt
            // if (userInput == "q") gApplication->Terminate(0); // TODO: exit also ends root session
            
            // ...
            // converted = std::stoi(userInput); // NOTE: If this fails, converted = -1 still
            converted = std::stoi(userInput, &numChars);
            
            // ....
            if (numChars != userInput.size()) throw std::invalid_argument("Trailing characters.");
        }
        // ..
        catch (const std::invalid_argument& e) {
            if (strcmp(e.what(), "stoi")) std::cerr << e.what() << "\n"; // print trailing chars message, but not "stoi" default error msg
            std::cerr << "Invalid argument!\n";
            continue; // skip logic check below, and try take input again
        }
        // ..
        catch (const std::out_of_range& e) {
            std::cerr << "Out of range\n";
            continue; // skip
        }
        
        // ...
        if ((converted > high) || (converted < low)) {
            std::cout << "Please select an integer between: " << low << " and " << high << "\n";
        } else {
            // ...
            return converted;
        }
    }
}

/*
 * Apply gaussian smearing to raw value to produce a signal
 * 
 * NOTE: With the higher resolution (2048 bins vs 1024 bins previously),
 * aliasing is seen when plotting the Ntuples data in a histogram,
 * to account for the higher resolution, we can apply a gaussian smearing
 * to reduce the jagged edges
 * 
 * NOTE: This acts as a proxy for PMT smearing, which occurs BEFORE the value
 * is converted into a channel
 */
double smear_signal(int const entry, double const sigmaFactor = 1.) {
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
    
    // ...
    double const smear = sigma * sigmaFactor;
    
    // Apply gaussian smearing to the photons detected in this event
    double const smeared = gRandom->Gaus(entry, smear);
    // TODO: potentially reduce gaussian smearing, reducing from:
    // sigma = sqrt(n)
    // to:
    // sigma = 0.5 * sqrt(n)
    // reduces FWHM from ~105.35 FWHM (at 1.8 res scale), to 91.16 (still at 1.8 res scale)
    // NOTE: Impacts FWHM more than i initially thought
    
    return smeared;
}

/*
 * Converts a raw or smeared signal to a channel number
 * 
 * NOTE: In real detector physics this occurs after multiplication and smearing of primary electrons
 * 
 * TODO: Make smearing optional, potentially via boolean param (or just sigmaFactor = 0.)
 *
 * TODO: I dont think these should be optional params
 */
double signal_to_channel(double const signal, int const nbins, double const xmax) {
    // Conversion factor from num optical photons "detected" to 0-2048 channel number
    double const conversion = nbins / xmax;
    // double const conversion = 1024. / 3500.;
    // NOTE: 3500 photons is arbitrary currently, in practice, this value should
    // reflect the upper window limit for the energy region of interest, i.e.:
    // 0 - 2 MeV
    
    // TODO: ^^ potentially make nbins & xmax global variables, set prior to fit,
    // or pass them in as params to this fn
    
    // Convert entry to channel number
    // int const channel = conversion * smeared; // int channel = std::floor(conversion * entry);
    double const channel = conversion * signal;
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
 * Handle ASCII (.Spe) file format
 * 
 * Loads file -> Parses data -> Spits out a histogram
 * 
 * NOTE: Avoid adding responsibilities beyond this
 * 
 * TODO: Maybe just fully lean into class property usage, instead of args/return values
 * ^ but on the other hand, these methods are quite portable as is, dunno if its worth
 * overcommitting to this design too much
 */
class ASCIIHandler {
    // ...
    private:
        // ASCII infile stream
        std::ifstream inASCII;
        
        // Metadata object type for ASCII (.Spe) files
        struct SpeMetaData {
            // Will be assigned the live and real time of the detector
            int liveTime = 0;
            int realTime = 0;
            
            // Will be assigned start and end channel numbers (i.e., 0 and 2047)
            int start = 0;
            int end = 0;
        } metaData;
        
        // Instantiated histogram pointer (will be on the heap)
        TH1* hpx = nullptr;
    
    // ...
    public:
        /*
        * Constructor
        */
        ASCIIHandler() = default;
        
        /*
        * Destructor
        * 
        * NOTE: PlotSession handles deletion of TH1 from the heap, hence trying to delete here
        * will cause a segfault when using PlotSession::set_hpx()
        * 
        * DONT:
        * 
        * delete hpx;
        * hpx = nullptr;
        * 
        * NOTE: Not sure ascii_cleanup() even necessary, as all paths should have infile etc clear
        * by the time destructor gets called
        */
        ~ASCIIHandler() {
            ascii_cleanup();
        };
    
    // ...
    private:
        /*
        * Handle closing ASCII input file after reading complete
        * 
        * NOTE: Dont need: if (!inASCII.is_open()) check before clear here, will always
        * return false, std::ifstream close delegates task to lower level file buffer,
        * and if os error occurs, stream is marked broken by setting failbit, and even
        * if failbit is triggered, connection between ifstream object and the file is
        * completely severed
        * 
        * NOTE: Also, calling .clear() on clean stream does no harm
        */
        void ascii_cleanup() {
            inASCII.close(); // Sever connection
            inASCII.clear(); // NOTE: Resets the flags for the next file
        }

        /*
        * Validate .Spe file can be opened/exists, loading it into local memory in readonly mode if so
        * 
        * TODO: Probably wanna validate is expected ASCII format too:
        * line 1: $SPEC_ID:
        * 
        * NOTE: std::ifstream sets internal error flags immediately on failure,
        * so only need to check (!inASCII.is_open) really
        */
        int load_ascii_file(std::string const& path) {
            // Open the ASCII file with validated .Spe extension
            inASCII.open(path, std::ios_base::in);
            // NOTE: Read-only mode (std::ios_base::in)
            
            // Ensure file was found, exit with error if its not
            // NOTE: No need to reprompt, user can just call the function again
            if (!inASCII || !inASCII.is_open()) {
                // Error message
                std::cerr << "Error: File not found.\n";
                // NOTE: Triggers if file is missing, corrupted, or locked
                
                // Error
                return 1;
            };
            
            // Success message
            std::cout << "\nASCII file has been loaded into memory.\n";
            
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
            if (!inASCII.is_open()) {
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
                if (!std::getline(inASCII, buffer)) {
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
                    if (!std::getline(inASCII, buffer)) {
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
                    if (!std::getline(inASCII, buffer)) {
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
        * Handles full ASCII file pipeline:
        * 
        * 1) Open ASCII file, load it into local memory, check its not empty (will error out on missing headers)
        * 2) Read through the headers, cache detector live/real times, and channel numbers
        */
        int load_ascii(std::string const& path) {
            // Attempt to load the ASCII into memory
            int const fileError = load_ascii_file(path);
            
            if (fileError) {
                std::cerr << "\nAborting: Load file error!\n";
                return 1;
            }
            
            // Attempt to parse detector times and channel numbers from the BSA-style blocks
            // SpeMetaData metaData;
            int const headerError = parse_headers(metaData);
            
            if (headerError) {
                std::cerr << "\nAborting: Header parsing error!\n";
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
        std::optional<TH1*> create_hist_ascii(int nbins = -1, double xmin = -1., double xmax = -1.) {
            // Histogram args
            std::string title;
            std::string legendTitle;
            
            // For now, is safe assumption that this is an energy spectrum exported from Maestro
            title = "EnergySpectrum";
            legendTitle = "Energy Spectrum";
            
            // Use the meta data parsed from the ASCII file to define channels and low/high
            nbins = metaData.end - metaData.start + 1;
            xmin = metaData.start;
            xmax = metaData.end + 1;
            // TODO: These should be passed as params, instead of global object access
            
            std::cout << "\nHistogram args set to:\n";
            std::cout << ">>> Num Bins: " << nbins << " XMIN: " << xmin << " XMAX: " << xmax << "\n";
            
            // Reject invalid histogram args
            if (title.empty() || legendTitle.empty() || nbins == -1 || xmin == -1 || xmax == -1) {
                std::cerr << "\nError: Failed to define histogram args.\n";
                ascii_cleanup();
                return std::nullopt;
            }
            
            std::cout << "\n>>> Creating TH1I...\n";
            
            // Create a histogram (TH1I = integer - channel/counts both ints)
            auto hpx = new TH1I(
                title.c_str(), // "hpx", // Legend title
                legendTitle.c_str(), // "distance travelled", // Histo title
                nbins, // num bins
                xmin, // x low
                xmax // x up
            );
            // NOTE: TH1I works while num photons is int, but may need long64 (TH1L) for gain applied num photons,
            // or TH1F (float - 4 bytes) / TH1D (double - 8 bytes) if using floating point values
        
            // Handle missing histogram (failed instantiation for any reason)
            if (!hpx) {
                std::cerr << "\nError (create_hist()): Histogram not found!\n";
                // Need to close file
                if (inASCII.is_open()) ascii_cleanup(); // NOTE: if (inASCII) always returns true
                return std::nullopt;
            }
            
            // X-axis title
            // hpx->SetXTitle("Distance (mm)");
            
            // Detach the histogram from the current open ROOT TFile
            // hpx->SetDirectory(nullptr);
            
            std::cout << "\nHistogram instantiated.\n";
            
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
            if (!inASCII.is_open()) {
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
                if (!std::getline(inASCII, buffer)) {
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
            inASCII.close();
            inASCII.clear();
            
            std::cout << "\nHistogram filled.\n";
            
            // No errors, all good
            return 0;
        }
    
    public:
        /*
        * Validate file path, load file into memory, instantiate histogram, fill histogram
        * 
        * TODO: Low key may want to make hpx a class property like ROOT handler, this is
        * pretty clean as is, but either this class, and/or the hpx, will need to be a global
        * object most likely, rather than making both global, this class can store the hpx,
        * and maybe have a getter() to request hpx pointer post-instantiation/fill, i.e., for
        * save() methodology
        * 
        * TODO: Is load_ascii(...) and unnecessary abstraction layer?
        * ^ Would it be best to just call here:
        * load_ascii_file(...)
        * parse_headers(...)
        * ??
        */
        std::optional<TH1*> plot_ascii(std::string const& path) {
            // Error handlers to catch bad program state (these should all have been cleared)
            if (inASCII.is_open()) { // NOTE: if (inASCII) always returns true (even after .close() & .clear())
                std::cout << "\nFound existing ASCII file, clearing...\n";
                return std::nullopt;
            }
            // NOTE: The inASCII & inROOT cases should never really flag true now (closed after loading 
            // TH1D or Ntuple, respectively, or on error trying to load them)
            
            // Attempt to load ASCII or ROOT file into memory
            int const fileError = load_ascii(path);
            
            if (fileError) {
                std::cerr << "\nAborting: Load file error!\n";
                return std::nullopt;
            }
            
            // Attempt to instantiate histogram object (TODO: maybe just use class property and return int here)
            std::optional<TH1*> histError = create_hist_ascii();
            
            if (!histError.has_value()) {
                std::cerr << "\nAborting: Create hist error!\n";
                return std::nullopt;
            }
            
            hpx = histError.value();
            
            // Attempt to populate histogram from ASCII or ROOT file
            int const fillError = fill_hist_ascii(hpx, metaData.start, metaData.end);
            
            if (fillError) {
                std::cerr << "\nAborting: Fill hist error!\n";
                return std::nullopt;
            }
            
            return hpx;
        }
        
        /*
         * Getter function for histogram pointer
         */
        TH1* get_hpx() {
            // ...
            if (!hpx) {
                std::cerr << "\nError: Histogram null.\n";
                return nullptr;
            }
            // ...
            return hpx;
        }
};


/*
 * Handle ROOT (.root) file format
 * 
 * Loads file -> Parses data -> Spits out a histogram
 * 
 * NOTE: Avoid adding responsibilities beyond this
 * 
 * TODO: Maybe just fully lean into class property usage, instead of args/return values
 * ^ but on the other hand, these methods are quite portable as is, dunno if its worth
 * overcommitting to this design too much
 * 
 * TODO: Add TH1I support
 * ^ maybe just do "TH1" for root object type
 * ^ and potentially another histogram type flag
 */
class ROOTHandler {
    // ...
    private:
        // ROOT infile and infile owned object pointers
        TFile* inROOT = nullptr;
        TTree* nTuple = nullptr;
        TBranch* branch = nullptr;
        TLeaf* leaf = nullptr;
        
        // Histogram pointer
        TH1* hpx = nullptr;

        // Store last accessed path, object name, and optionally branch name (for ttrees), for replot functionality
        // NOTE: For replot, TH1 should never be considered anyways, so it should be treeName & branchName ...
        // std::string const* lastPath = nullptr;
        std::string lastPath;
        std::string lastObjectName;
        std::string lastBranchName;
        
        // Maps formed when reading ROOT file object list
        struct QueryReturnType {
            std::unordered_map<int, std::string> objectMap;
            std::unordered_map<std::string, std::vector<std::string>> categoryMap;
        };

        // Integer key to access maps, and type of object associated with that key
        struct SelectionReturnType {
            int selectedTypeIdx;
            std::string selectedObjectType;
        };

        // Data type identifiers (returned by TLeaf->GetTypeName())
        std::string const intType = "Int_t";
        std::string const doubleType = "Double_t";
        
        // Currently supported object types
        enum class RootObjectType {
            TTree,
            TH1D,
            NULLOBJ
        };

        // Active object type flag
        RootObjectType rootObjectType = RootObjectType::NULLOBJ;
    
    // ...
    public:
        /*
        * Constructor
        */
        ROOTHandler() = default;
        
        /*
        * Destructor
        * 
        * NOTE: PlotSession handles deletion of TH1 from the heap, hence trying to delete here
        * will cause a segfault when using PlotSession::set_hpx()
        * 
        * DONT:
        * 
        * delete hpx;
        * hpx = nullptr;
        * 
        * NOTE: Not sure root_cleanup() even necessary, as all paths should have infile etc clear
        * by the time destructor gets called
        */
        ~ROOTHandler() {
            root_cleanup();
        };
    
    // ...
    private:
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
        * Checks what objects are in the root file, creates key-value pairings, renders list
        * 
        * TTree (Ntuple), TH1D (1D Histogram), etc
        * 
        * TODO: Not sure whether to define some of these maps in load_root, and pass in pointers
        * to this function, or leave as is, will revisit this design choice later
        */
        std::optional<QueryReturnType> get_root_types() {
            // Handle unloaded root file (i.e., via calling this method directly, or some bug)
            if (!inROOT) {
                std::cerr << "\nError: File not found!\n";
                return std::nullopt;
            }
            
            // Get list containing keys for each object in the ROOT file
            TList const* entries = inROOT->GetListOfKeys();
            
            // ...
            if (!entries) {
                std::cerr << "\nError: Failed to get ROOT file keys.\n";
                return std::nullopt;
            }
            
            // ...
            std::cout << "\nShowing ROOT object types available in file: " << inROOT->GetName() << "\n";
            
            // A set containing the unique object types found in the ROOT file
            // std::unordered_set<std::string> objectTypes = {}; // NOTE: will automatically create std::string copy of const char*
            std::unordered_set<std::string_view> objectTypes = {}; // NOTE: ...
            // NOTE: Value: "TTree", "TH1D", etc
            
            // Key-value map of access indices assigned to each object type
            std::unordered_map<int, std::string> objectMap = {}; // NOTE: ...
            // NOTE: Key: 1, 2, 3, 4, etc
            // Value: "TTree", "TH1D", etc
            
            // Key-value map of object types, and the available objects matching that description
            std::unordered_map<std::string, std::vector<std::string>> categoryMap = {}; // NOTE: ...
            // NOTE: Key: "TTree", "TH1D", etc
            // Value: {"EventData", "TrackData", etc}, {"PhotonsSpectrum"}
            
            // Iterate over all objects in the ROOT file
            for (int i = 0; i < entries->GetSize(); i++) {
                // Get the TKey* at index i
                TObject* entry = entries->At(i); // TKey*
                // NOTE: Since we called GetListOfKeys(), this is actually a TKey*, TList* iteration just defaults to TObject*
                
                // TKeys are different to TObjects because ...
                
                // Get the name that was assigned to the ROOT object (Ntuple, TH1D, etc) at creation (i.e., EventData, TrackData, etc)
                char const* objectName = entry->GetName();
                // std::cout << i << ": " << objectName << "\n"; // NOTE: Prints the name of each object (EventData, TrackData, etc)
        
                // Cast TObject* to TKey*
                auto key = static_cast<TKey*>(entry);
                // NOTE: Since "GetListOfKeys()" returns TList<TKey*>, we can safely static_cast here,
                // if there was any doubt about it being TKeys, static_cast would not be safe
                // TODO: Could do a safety check here, check GetName() returns TKey BEFORE static_cast ...?
                
                // Returns the type of object this key is associated with (i.e., TTree, TH1D, etc)
                char const* objectType = key->GetClassName();
                // std::cout << "CLASSNAME: " << objectType << "\n";
                
                // If they object type key already exists in the object, pull the existing entry,
                // add the new name to the vector, update the entry in the map
                if (categoryMap.count(objectType)) { // NOTE: returns: not found = 0, found = 1 (number of elements with key = 0 || 1)
                    std::vector<std::string> existing = categoryMap.at(objectType); // Get entry for provided key
                    existing.push_back(objectName); // append new entry to the vector
                    categoryMap[objectType] = existing; // update map entry associated with this key
                }
                // Otherwise, make the first entry under that object type
                else {
                    std::vector<std::string> entry = { objectName };
                    categoryMap[objectType] = entry;
                }
                
                // Add unique object types to the set
                std::pair pair = objectTypes.insert(objectType); // NOTE: Automatically only adds on unique
                // NOTE: Insert returns a pair of: iterator & success status
                
                // If success status = true, item was inserted, else status = false
                if (pair.second) {
                    // Derive indices for object types based on set size
                    int const itemNo = objectTypes.size();
                    
                    // Print each unique object type discovered to stdout, with linked index
                    std::cout << itemNo << ") " << objectType << "\n";
                    // NOTE: I.e., 1) TH1D\n 2) TTree\n
                    
                    // Assign the object type as value for the integer key
                    objectMap[itemNo] = objectType; // NOTE: Inserts if missing, updates if present
                    // NOTE: objectMap.insert(itemNo, objectType); requires iterator as arg[0],
                    // i.e. if you want to re-enter a recently removed const iterator, not add
                    // a new item
                }
            }
            // NOTE: Either just get all names now, and determine what object type is later
            // i.e. print all TTrees and Hists, let user select from those
            
            return QueryReturnType { objectMap, categoryMap };
        }

        /*
        * ...
        * 
        * NOTE: If there is only one object, that name is selected by default, skipping user prompt
        */
        std::optional<SelectionReturnType> select_root_type(std::unordered_map<int, std::string> const& objectMap) {
            // ...
            int selectedTypeIdx = -1;
            
            // Switch on number of unique ROOT objects found
            if (objectMap.size() == 0) {
                std::cerr << "\nError: ROOT object map is empty.\n";
                return std::nullopt;
            }
            // If only one object type found, auto select that type
            else if (objectMap.size() == 1) {
                std::cout << "\nOnly one ROOT object type found, selecting...\n";
                selectedTypeIdx = 1;
            }
            // Otherwise prompt user for object type selection
            else {
                // ...
                std::cout << "\nWhat ROOT object type would you like to access? (type a number from the options above, or enter q to exit):\n";
                
                // Have the user select from one of the available options
                selectedTypeIdx = prompt_user_int(1, objectMap.size()); // each integer is mapped to an object type
                // NOTE: Non-zero based indexing feels more appropriate for this, with options starting
                // at one, and going up to the size of the map
            }
            
            // Retrieve the string associate with the integer key
            std::string const selectedObjectType = objectMap.at(selectedTypeIdx);
            // NOTE: Equivalent to .get() in typescript
            
            // Enable flag for chosen object type
            if (selectedObjectType == "TTree") {
                rootObjectType = RootObjectType::TTree;
            }
            else if (selectedObjectType == "TH1D") {
                rootObjectType = RootObjectType::TH1D;
            }
            else {
                std::cerr << "\nError: Unsupported ROOT object type.\n";
            }
            
            // ...
            struct SelectionReturnType const result { selectedTypeIdx, selectedObjectType };
            // NOTE: "struct" keyword here is optional in c++, but mandatory in c
            
            // ...
            return result;
            
            // NOTE: Not sure if accessing categoryMap here and returning "filteredObjects" is 
            // bit beyond scope of this function, or appropriate, will decide later
            
            // TODO: May need to return vector containing object names matching object type
            // just return that and success/fail message, may need std::optional return type 
            // for this function and return std::nullopt on failed error checks above
        }

        /*
        * Prompt user to select the name of the object theyd like to access 
        * (with selection list filtered by the object type theyve chosen)
        * 
        * NOTE: If there is only one object, that name is selected by default, skipping user prompt
        * 
        * NOTE: Pass by const reference (SelectionReturnType const&), instead of the object itself, 
        * or a pointer (as long as the argument cannot be null).
        * 
        * This eliminates the need for manual pointer dereferencing and null checks
        * 
        * Destructuring the params using: "auto const&" ensures the compiler creates reference
        * bindings directly to the memory address insside the struct, guaranteeing a zero-copy operation
        * 
        * NOTE: For: (SelectionReturnType const* selectionParams):
        * if (!params) ..
        * const auto& [selectedTypeIdx, selectedObjectType, categoryMap] = *params; 
        * dereference pointer, bind by reference to avoid copies
        */
        std::string select_root_object(SelectionReturnType const& selectionParams, std::unordered_map<std::string, std::vector<std::string>> const& categoryMap) {
            // Destructure the param object
            auto const& [selectedTypeIdx, selectedObjectType] = selectionParams;
            // NOTE: Zero copies, no pointer syntax
            
            // ...
            std::cout << "\nShowing options for object type: " << selectedTypeIdx << " - " << selectedObjectType << "\n";
            
            // Get a list of all objects matching the chosen type
            std::vector<std::string> const filteredObjects = categoryMap.at(selectedObjectType);
            
            // Iterate over list of objects matching the chosen object type
            for (int i = 0; i < filteredObjects.size(); i++) {
                // Display key-value list of objects
                std::cout << (i + 1) << ") " << filteredObjects[i] << "\n";
                // I.e., 1) StepDataDetection\n, 2) StepDataAbsorption\n, etc
            }
            
            // If there is only one object matching that type, select it by default
            if (filteredObjects.size() == 1) {
                std::cout << "\nOnly one object matching requested type found, selecting...\n";
                return filteredObjects[0];
            }
            
            // ...
            std::cout << "\nWhat object would you like to access? (type a number from the options above, or enter q to exit):\n";
            
            // Prompt user to select an object from the displayed list, via integer key
            int const selectedObjectIdx = prompt_user_int(1, filteredObjects.size());
            
            // Use the integer key to retrieve the name of the object selected
            std::string const selectedObjectName = filteredObjects.at(selectedObjectIdx - 1); // NOTE: Make selection zero-indexed again (minus 1)
            
            return selectedObjectName;
        }
        
        /*
         * ...
         * 
         * 1) Query the ROOT object to get a list of object types & display available types
         * 
         * 2) Prompt user to select a listed ROOT object type, then display filtered list of
         * objects matching said type
         * 
         * 3) Prompt user to select ROOT object name from filtered list
         */
        std::string object_explorer() {
            // Query the ROOT object to get a list of object types, and create key-value maps
            std::optional<QueryReturnType> const querySuccess = get_root_types();
            
            if (!querySuccess.has_value()) {
                std::cerr << "\nError: Failed to query ROOT file.\n";
                return "";
            }
            
            QueryReturnType const query = querySuccess.value();
            
            // Prompt user for ROOT object type
            std::optional<SelectionReturnType> const selectionSuccess = select_root_type(query.objectMap); // choose TTree (Ntuple), TH1D (1D Hist), etc
            
            if (!selectionSuccess) {
                std::cerr << "\nError: Failed to get user selection for object type.\n";
                return "";
            }
            
            SelectionReturnType const result = selectionSuccess.value();
            
            // Prompt user for ROOT object name
            std::string const selectedName = select_root_object(result, query.categoryMap); // get tree/hist/etc name
            
            if (selectedName.empty()) {
                std::cerr << "\nError: ROOT object name is empty.\n";
                return "";
            }
            
            return selectedName;
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
        int cache_tree(std::string const& selectedTree) {
            // Handle incorrect path
            if (!inROOT) {
                std::cerr << "\nError: File not found!\n";
                return 1;
            }
            
            // ...
            char const* treeName = selectedTree.c_str();
            
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
        * Select branch name from those available in chosen TTree
        * 
        * 1) Prints valid branch names for selected TTree object
        * 
        * 2) Has user choose desired branch name (similar to select root type)
        * 
        * NOTE: If there is only one branch, will select that name by default and skip prompt
        * 
        * 3) Returns chosen tree name
        */
        std::string select_branch() {
            // Handle missing ntuple
            if (!nTuple) {
                std::cerr << "\nError: Couldnt find TTree!\n";
                root_cleanup();
                return "";
            }
            
            // ...
            std::cout << "\nFetching branch list...\n";
            
            // Get iterable list of branches in this TTree
            TObjArray const* branches = nTuple->GetListOfBranches(); 

            if (!branches) {
                std::cerr << "\nError: Couldnt get TTree branches array!\n";
                root_cleanup();
                return "";
            }
            
            // ...
            std::cout << "\nBranch list has been loaded.\n";
            
            // Get the number of branches in the TTree
            int const numEntries = branches->GetEntries();
            
            if (numEntries == 0) {
                std::cerr << "\nError: Selected TTree contains no branches!\n";
                root_cleanup();
                return "";
            }
            
            // Store indices associated with branches in the chosen TTree
            std::unordered_map<int, std::string> branchMap = {};
            // NOTE: Key: 1, 2, 3, 4, etc ...
            // Value: "DetectionDistance", "TimeOfFlight", etc
            
            std::cout << "\nShowing branches in TTree - " << nTuple->GetName() << ":\n";
            
            // Iterate through the branches of the TTree
            for (int i = 0; i < numEntries; i++) {
                TObject const* entry = branches->At(i); // NOTE: Is TBranch*
                char const* branchName = entry->GetName();
                std::cout << (i + 1) << ") " << branchName << "\n";
                branchMap[i + 1] = branchName;
            }
            
            // ...
            std::string chosenBranch;
            
            // If there is only one branch available, select it
            if (numEntries == 1) {
                std::cout << "\nOnly one branch found in TTree, selecting...\n";
                chosenBranch = branchMap.at(1);
            }
            // Otherwise prompt user for selection
            else {
                std::cout << "\nWhat branch would you like to access? (type a number from the options above, or enter q to exit):\n";
                int const res = prompt_user_int(1, numEntries);
                chosenBranch = branchMap.at(res);
            }
            
            // ...
            std::cout << "\nBranch: \"" << chosenBranch << "\" selected.\n";
            
            return chosenBranch;
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
                std::cerr << "\nError: TTree branch \"" << branchName << "\" not found. Closing root file and deconstructing Ntuple.\n";
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
            
            // Cache chosen branch name (for replotting)
            lastBranchName = branchName;
            
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
        * 
        * NOTE: if @branchName arg is passed -> skip select branch CLI
        * else -> use CLI to select
        */
        int load_root_tree(std::string const& treeName, std::string const& branchName = "") {
            // Cache TTree pointer
            int const loadTreeError = cache_tree(treeName);
            
            if (loadTreeError) {
                std::cerr << "\nError: Failed to cache TTree pointer.\n";
                return 1;
            }
            
            std::string workingBranchName = branchName;
            
            // If no branch name provided, bring up branch selection CLI
            if (workingBranchName.empty()) {
                // Prompt user to select from available TBranches in TTree
                std::string const branchName = select_branch();
                
                if (branchName.empty()) {
                    std::cerr << "\nError: Failed to get branch selection.\n";
                    return 1;
                }
                
                workingBranchName = branchName;
            }
            
            // Cache TBranch pointer
            int const loadBranchError = cache_branch(workingBranchName);
            
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
        * Load histogram object (TH1D, TH1I, etc) from ROOT input file and cache pointer
        */
        int load_root_hist(char const* histName) {
            // Handle missing infile
            if (!inROOT) {
                std::cerr << "\nError: File not found!\n";
                return 1;
            }
            
            // Load the histogram object with the specified name into memory
            inROOT->GetObject<TH1>(histName, hpx);
            
            // Handle missing histogram
            if (!hpx) {
                std::cerr << "\nError (create_hist()): Histogram not found!\n";
                root_cleanup();
                return 1;
            }
            
            // Assign drawing option, so that calling "Draw()" later doesnt require "HIST" arg
            hpx->SetOption("HIST");
            
            // Detach histogram from input file, then close input file
            hpx->SetDirectory(nullptr);
            delete inROOT;
            inROOT = nullptr;
            // NOTE: Histogram entire payload typically already in memory, detaching is fairly
            // safe here, and once detached, we no longer need the ROOT file
            
            // Success message
            std::cout << "\nROOT histogram loaded.\n";
            
            // ...
            // return hpx;
            return 0;
        }

        /*
        * Attempts to load the chosen ROOT object into memory, routing to dedicated helper
        * functions for the various object types
        */
        int load_root_object(std::string const& objectName, std::string const& branchName = "") {
            // Convert std::string to c string pointer
            char const* name = objectName.c_str();
            
            // Load object router
            if (rootObjectType == RootObjectType::TTree) {
                int const loadRootTreeError = load_root_tree(objectName, branchName);
                if (loadRootTreeError) return 1;
            }
            else if (rootObjectType == RootObjectType::TH1D) {
                // Get pointer to TH1 object in ROOT file and cache it
                int const loadRootHistError = load_root_hist(name);
                if (loadRootHistError) return 1;
            }
            else {
                std::cerr << "\nError: Unsupported ROOT object type.\n";
                return 1;
                // NOTE: No need to call "root_cleanup()" here, each of these functions will
                // clean up after themselves if an error arises in their logic
            }
            
            // Success
            std::cout << "\nROOT object loaded.\n";
            
            // ...
            return 0;
        }

        /*
        * Handles full ROOT file pipeline:
        * 
        * 1) Open ROOT file, load it into local memory, check its not empty
        * 2) Get list of object types
        * 3) Have user select object type of interest
        * 4) Have user select from list of objects matching said type
        * 5) Load the chosen object (cache pointers)
        * 
        * NOTE: Not implementing 2D/3D hist shit just yet, can save that for a later date 
        * (will need to select multiple branch names, etc)
        * ^ this should just be flexible enough to handle any Ntuples i create from the sim for now
        * 
        * 
        * NOTE: OPTION #1: Print all object names,
        * have user select desired object name,
        * determine what the object type is,
        * route to the appropriate handler for said object type
        * i.e. print all TTrees and Hists, let user select from those
        * 
        * NOTE: OPTION #2: Print all unique object types
        * let user select appropriate object type
        * show list of only those
        * 
        * NOTE: #2 seems more scalable
        * 
        * NOTE: if @objectName arg is not passed -> bring up root explorer CLI
        * else -> go straight to load_root_object(), skipping CLI
        * 
        * TODO: root_explorer() method
        * ^ but is this unnecessary abstraction?
        */
        int load_root(std::string const& objectName = "", std::string const& branchName = "") {
            // Mutable copy of provided object name (empty or not)
            std::string workingName = objectName;
            
            // If no object name is passed
            if (workingName.empty()) {
                // Bring up the ROOT object explorer CLI
                std::string const selectedName = object_explorer();
                
                if (selectedName.empty()) {
                    std::cerr << "\nError: Failed to select ROOT object.\n";
                    return 1;
                }
                
                // Assign object selection if checks have passed
                workingName = selectedName;
            }
            
            // Cache pointers for chosen ROOT object
            int const loadObjectError = load_root_object(workingName, branchName); // load root object with said name
            
            if (loadObjectError) {
                std::cerr << "\nError: Failed to access ROOT object.\n";
                return 1;
            }
            
            // Cache chosen root object name (for replotting)
            lastObjectName = workingName;
            // TODO: Maybe set this before load_root_object, bit pedantic, but lastBranchName gets
            // assigned first currently, not really a big deal but yeah
            
            std::cout << "\nROOT file and chosen object loaded.\n";
            
            // No errors, all good
            return 0;
        }
        
        /*
         * Query the class name of an object (if the object exists in the file)
         */
        int query_object_type(std::string const& objectName) {
            // Handle missing infile
            if (!inROOT) {
                std::cerr << "\nError: File not found!\n";
                return 1;
            }
            
            // Temporary pointer to the object
            TObject* test = inROOT->Get(objectName.c_str());
            
            if (!test) {
                std::cerr << "\nError: Object not found.\n";
                return 1;
            }
            
            // Query found object for its type
            std::string const foundType = test->ClassName();                
            // std::cout << "CLASS NAME: " << foundType << "\n";
            
            // Prompt user to select branch (since we only have tree name)
            if (foundType == "TTree") {
                rootObjectType = RootObjectType::TTree;
            }
            // Attempt to load the histogram
            else if (foundType == "TH1D") {
                rootObjectType = RootObjectType::TH1D;
            }
            // Reject bad usage
            else {
                std::cerr << "\nError: Unsuported object type.\n";
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
        * 
        * TODO: Dont need forward declarations of some of these variables now that ASCII/ROOT
        * logic is separated
        */
        int create_hist_root(int nbins = -1, double xmin = -1., double xmax = -1.) {
            // Get the TTree and TBranch names
            std::string const nTupleName = nTuple->GetName();
            char const* branchName = branch->GetName();
            
            // Name the histogram after the ROOT object
            std::string const title = nTupleName + "Hpx"; // NOTE: Using the TTree name itself causes ROOT to think the histogram already exists
            std::string const legendTitle = branchName;
            
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
            std::string_view const leafType = leaf->GetTypeName(); // std::string leafType;
            // std::cout << "LEAF TYPE: " << leafType << "\n";
            
            std::cout << "\nHistogram args set to:\n";
            std::cout << ">>> Num Bins: " << nbins << " XMIN: " << xmin << " XMAX: " << xmax << "\n";
            
            // Reject invalid histogram args
            if (title.empty() || legendTitle.empty() || nbins == -1 || xmin == -1 || xmax == -1) {
                std::cerr << "\nError: Failed to define histogram args.\n";
                root_cleanup();
                return 1;
            }
            
            // Handle ROOT TBranches containing integers
            if (leafType == intType) {
                std::cout << "\n>>> Creating TH1I...\n";
                
                // Create a histogram (TH1I = integer - channel/counts both ints)
                hpx = new TH1I(
                    title.c_str(), // "hpx", // Legend title
                    legendTitle.c_str(), // "distance travelled", // Histo title
                    nbins, // num bins
                    xmin, // x low
                    xmax // x up
                );
                // NOTE: TH1I works while num photons is int, but may need long64 (TH1L) for gain applied num photons,
                // or TH1F (float - 4 bytes) / TH1D (double - 8 bytes) if using floating point values
            }
            // Handle ROOT TBranches containing doubles
            else if (leafType == doubleType) {
                std::cout << "\n>>> Creating TH1D...\n";
                
                // Create a histogram (TH1D = double)
                hpx = new TH1D(
                    title.c_str(), // "hpx", // Legend title
                    legendTitle.c_str(), // "distance travelled", // Histo title
                    nbins, // num bins
                    xmin, // x low
                    xmax // x up
                );
            }
            
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
        int fill_hist_ntuple(
            TH1* hpx,
            bool const doPostProcessing,
            int const nbins,
            double const xmax,
            double const sigmaFactor,
            double const lowerDiscriminator,
            double const channelOffset
        ) {
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
            
            // Query the cached TLeaf for TTree data type
            std::string_view const dataType = leaf->GetTypeName();
            std::cout << "\n>>> Data type: " << dataType << "\n";
            
            // Define branch entry variables
            int intEntry;
            double doubleEntry;
            // NOTE: Variable used changes based on Ntuple type (int, double, etc)
            
            std::cout << "\nSetting branch address for: \"" << branchName << "\"\n";
            
            // Switch on int or double branch type
            if (dataType == intType) {
                // To read a tree, neeed to associate variables with the trees branches
                nTuple->SetBranchAddress(branchName, &intEntry);
                // NOTE: When loading a tree entry, the tree will set the variables to the 
                // branches value as read from the storage
            } 
            else if (dataType == doubleType) {
                nTuple->SetBranchAddress(branchName, &doubleEntry);
            }
            // NOTE: Multiple variables can be set to different branches here, and TTree->GetEntry(i) 
            // will update all variables to the current index
            
            // ...
            std::cout << "\nBranch address set to: \"" << branchName << "\"\n";

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
            
            // 
            double lowerDiscriminatorChannel;
            
            if (lowerDiscriminator != -1.) {
                lowerDiscriminatorChannel = signal_to_channel(lowerDiscriminator, nbins, xmax);
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
                
                // Add a count to the appropriate bin for that value, introduce smearing if requested
                if (dataType == intType) {
                    if (doPostProcessing) {
                        // Apply smearing to the raw value to produce a signal
                        double const signal = smear_signal(intEntry, sigmaFactor);
                        
                        // Convert smeared signal to channel number
                        double channel = signal_to_channel(signal, nbins, xmax);
                        
                        // ...
                        channel += channelOffset;
                        
                        // If channel number is greater than the discriminator, add count to that channel
                        if (channel > lowerDiscriminatorChannel) hpx->Fill(channel);
                    }
                    else {
                        // Not a channel, fill bin with raw value
                        hpx->Fill(intEntry);
                    }
                }
                else if (dataType == doubleType) {
                    if (doPostProcessing) {
                        // Apply smearing to the raw value to produce a signal
                        double const signal = smear_signal(doubleEntry, sigmaFactor);
                        
                        // Convert smeared signal to channel number
                        double channel = signal_to_channel(signal, nbins, xmax);
                        
                        // ...
                        channel += channelOffset;
                        
                        // If channel number is greater than the discriminator, add count to that channel
                        if (channel > lowerDiscriminatorChannel) hpx->Fill(channel);
                    }
                    else {
                        // Not a channel, fill bin with raw value
                        hpx->Fill(doubleEntry);
                    }
                }
                // NOTE: Switching on int/double data type
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
    
    // ...
    public:
        /*
        * ...
        * 
        * TODO: Is it worth calling both load_root_file() and load_root_object() from this
        * top level method? I.e., omit the load_root() method ?
        * 
        * TODO: Maybe get the TH1 from load_root_hist
        * 
        * NOTE: ^ Returning hist from create_hist_root() is easy enough, but trying to get hist
        * from load_root_hist() requires an optional union return type from load_root_object()
        * and load_root(), which just seems like more hassle than its worth, and messy, compared
        * to just using a hpx class property ...
        * 
        * TODO: If just objectName is passed, it could be a TH1, or it could be a TTree,
        * need to check which -> if its TH1, open it, if its TTree, open branch select CLI
        * 
        * TODO: When passing objectName / branchName, maybe a "did you mean ..." prompt if slighly wrong?
        * 
        * TODO: THIS ABBREVIATED TREE LOADING PIPELINE IS USED BELOW TOO FOR USE CACHED
        * MAYBE MAKE SEPARATE METHODS FOR LOAD TREE w/ BRANCH SELECT CLI & LOAD TREE
        * BY PASSED NAME (or just rely on if statement around branch select CLI in load_root_tree)
        * 
        * TODO: ONE THING EACH OF THESE THREE APPROACHES HAVE IN COMMON IS THAT load_root_file() IS CALLED FIRST
        * MAYBE EXTRACT THAT OUT TO THE TOP LEVEL HERE, SEPARATE FILE LOADING FROM OBJECT LOADING
        * you always need to load the file, so dont abstract it away / couple it with other methods
        * ^ only thing is clauses above and below uses path while else clause uses lastPath
        * 
        * TODO: load_root_tree has logic check to see if branch name passed now, could just use that method...
        * but need to consider whether to create separate method (cache tree -> cache branch -> cache leaf),
        * i.e. just omits branch select CLI, or not
        */
        std::optional<TH1*> plot_root(
            std::string const& path,
            std::string const& objectName = "",
            std::string const& branchName = "",
            int const nbins = -1,
            double const xmin = -1.,
            double const xmax = -1.,
            bool const doPostProcessing = false,
            double const sigmaFactor = 1.,
            double const lowerDiscriminator = -1.,
            double const channelOffset = 0.
        ) {
            // Error handlers to catch bad program state (these should all have been cleared)
            if (leaf) {
                std::cout << "\nFound existing Ntuple branch leaf, clearing...\n";
                return std::nullopt;
            }
            if (branch) {
                std::cout << "\nFound existing Ntuple branch, clearing...\n";
                return std::nullopt;
            }
            if (nTuple) {
                std::cout << "\nFound existing Ntuple, clearing...\n";
                return std::nullopt;
            }
            if (inROOT) {
                std::cout << "\nFound existing ROOT file, clearing...\n";
                return std::nullopt;
            }
            // NOTE: These cases should never really flag true now (closed after loading 
            // TH1D or Ntuple, respectively, or on error while trying to load them)
            
            // Incase plot will be called multiple times in succession, ensure histo cleared each time
            if (hpx) {
                std::cout << "\nFound existing histogram, clearing...\n";
                delete hpx; // <<<<<<<<<<<<<<<<<
                hpx = nullptr;
            }
            
            // ^^^^^^^ TODO: Probably best to make a dedicated "clean()" method,
            // which checks other pointers are clear etc
            
            // std::cout << "OBJECT NAME: " << objectName << " BRANCH NAME: " << branchName << "\n";
            
            // 1) Open ROOT file
            
            // Attempt to open the ROOT file
            int const loadFileError = load_root_file(path);
            
            if (loadFileError) {
                std::cerr << "\nError: Failed to load ROOT file into memory.\n";
                return std::nullopt;
            }
            
            // 2) Cache ROOT object(s)
            
            // Default state (both "objectName" and "branchName" are omitted, so open selection CLI)
            if (objectName.empty() && branchName.empty()) {
                // Attempt to run ROOT explorer to load ROOT objects into memory
                int const loadObjectError = load_root();
                // NOTE: No object name and branch name specified, so will run selection CLI
                
                if (loadObjectError) {
                    std::cerr << "\nAborting: Load file error!\n";
                    return std::nullopt;
                }
            }

            // If only object name is passed (could be TTree or TH1 name)
            // need to check which -> if its TH1, open it, if its TTree, open branch select CLI
            else if (!objectName.empty() && branchName.empty()) {
                // Attempt to set the rootObjectType flag to match object type
                int const checkObjectError = query_object_type(objectName);
                
                if (checkObjectError) {
                    std::cerr << "\nError: Failed to check ROOT object type.\n";
                    return std::nullopt;
                }
                
                // Cache pointers for chosen ROOT object
                int const loadObjectError = load_root(objectName);
                // NOTE: Passing object name skips initial object selection CLI, 
                // but runs branch selection CLI if object is a TTree
                
                if (loadObjectError) {
                    std::cerr << "\nError: Failed to access ROOT object.\n";
                    return std::nullopt;
                }
            }
            
            // If both "objectName" and "branchName" are passed, we assume its a TTree,
            // if its not, it will error out due to not found / incorrect usage anyways
            // NOTE: This will also be used when replotting via cached TTree name and TBranch name
            else if (!objectName.empty() && !branchName.empty()) {
                // Manually set flag, and assume its TTree
                rootObjectType = RootObjectType::TTree;
                
                // Attempt to load ROOT file and object into memory
                int const loadObjectError = load_root(objectName, branchName);
                // NOTE: Passing both args skips the selection CLIs
                
                if (loadObjectError) {
                    std::cerr << "\nAborting: Load file error!\n";
                    return std::nullopt;
                }
            }
            
            // 3) Create and fill histogram
            
            // NOTE: Only call: create_hist() & fill_hist(); if its ROOT Ntuple
            // NOTE: If its ROOT Histogram, hpx pointer will already be populated with a filled hist
            if (rootObjectType == RootObjectType::TH1D) return hpx;
            
            // Attempt to instantiate histogram object
            int const histError = create_hist_root(nbins, xmin, doPostProcessing ? nbins : xmax);
            
            if (histError) {
                std::cerr << "\nAborting: Create hist error!\n";
                return std::nullopt;
            }
            
            // Attempt to populate histogram from ASCII or ROOT file
            int const fillError = fill_hist_ntuple(
                hpx,
                doPostProcessing,
                nbins,
                xmax,
                sigmaFactor,
                lowerDiscriminator,
                channelOffset
            );
            
            if (fillError) {
                std::cerr << "\nAborting: Fill hist error!\n";
                return std::nullopt;
            }
        
            // TODO: ^^ This enlosure feels a bit dirty, likely a better way to do this ^^
            // could just: if (... == TH1D) return hpx;
            
            // Cache last used path
            lastPath = path;
            
            std::cout << "\nROOT plotting complete.\n";
            
            // ...
            return hpx;
        }
        
        /*
        * ...
        * 
        * 1) User plots object of choice via interactive CLI
        * 2) User sees plot and knows how theyd like to rebin / rescale x-axis
        * 3) User calls replot with those values, without having to re-enter file name or going through CLI again,
        * due to cached "lastPath" and "lastObjectName"
        * 
        * TODO: Post-processing
        */
        std::optional<TH1*> replot(
            int const nbins,
            double const xmin,
            double const xmax,
            bool const doPostProcessing = false,
            double const sigmaFactor = 1.,
            double const lowerDiscriminator = -1.,
            double const channelOffset = 0.
        ) {
            // ...
            if (rootObjectType != RootObjectType::TTree) {
                std::cerr << "Error: Replot only available for ROOT Ntuples.\n";
                return std::nullopt;
            }
            
            // Handle bad args
            if (nbins <= 0) {
                std::cerr << "Error: Please select a positive number of bins.\n";
                return std::nullopt;
            }
            if (xmax <= xmin) {
                std::cerr << "Error: Please ensure xmax is greater than xmin.\n";
                return std::nullopt;
            }
            
            // ...
            if (lastPath.empty()) {
                std::cerr << "Error: Path not cached.\n";
                return std::nullopt;
            }
            if (lastObjectName.empty()) {
                std::cerr << "Error: Ntuple name not cached.\n";
                return std::nullopt;
            }
            if (lastBranchName.empty()) {
                std::cerr << "Error: Branch name not cached.\n";
                return std::nullopt;
            }
            
            // ...
            std::cout << "\nLAST PATH: " << lastPath << "\n";
            std::cout << "LAST TTREE: " << lastObjectName << "\n";
            std::cout << "LAST BRANCH: " << lastBranchName << "\n";
            
            // Pass 
            std::optional<TH1*> const success = plot_root(
                lastPath,
                lastObjectName,
                lastBranchName,
                nbins,
                xmin,
                xmax,
                doPostProcessing,
                sigmaFactor,
                lowerDiscriminator,
                channelOffset
            );
            // NOTE: Passing in the cached path, and both the cached TTree and TBranch names
            
            if (!success.has_value()) {
                std::cerr << "Error: Replot failed.\n";
                return std::nullopt;
            }
            
            return success.value();
        }
        
        /*
         * Getter function for histogram pointer
         */
        TH1* get_hpx() {
            // ...
            if (!hpx) {
                std::cerr << "\nError: Histogram null.\n";
                return nullptr;
            }
            // ...
            return hpx;
        }
};


/*
 * ...
 * 
 * Handles hpx and canvas lifetimes for the current plot
 */
class PlotSession {
    // ...
    private:
        // ...
        std::variant<std::monostate, ROOTHandler*, ASCIIHandler*> m_handler = std::monostate{};
        // NOTE: Initialises itself with first type listed in template args (monostate)
        
        // ...
        TH1* m_hpx = nullptr; // microsoft style class member (m_name)
        // TH1* hpx_ = nullptr; // google style class member (name_)
        
        // ...
        TCanvas* m_canvas = nullptr;
    
    // ...
    public:
        /*
         * Constructor
         */
        PlotSession() = default;
        /*
         * Destructor
         */
        ~PlotSession() {
            // ...
            clear_handler();
            
            delete m_hpx;
            m_hpx = nullptr;
            
            delete m_canvas;
            m_canvas = nullptr;
        };
        /*
         * Check which handler is active, and remove it from the heap
         */
        int clear_handler() {
            // Check which handler is active, and clear it
            if (auto** activeHandler = std::get_if<ASCIIHandler*>(&m_handler)) {
                std::cout << "\nFound existing ASCII handler, clearing...\n";
                delete *activeHandler;
            }
            else if (auto** activeHandler = std::get_if<ROOTHandler*>(&m_handler)) {
                std::cout << "\nFound existing ROOT handler, clearing...\n";
                delete *activeHandler;
            }
            
            return 0;
        }
        /*
         * ...
         * 
         * NOTE: Using std::holds_alternative for type checking here means when accessing 
         * the variant via std::get, another check is performed by get, whereas using
         * get_if to achieve both the check and variable assignment causes less overhead
         */
        int set_handler(std::variant<std::monostate, ROOTHandler*, ASCIIHandler*> handler) {
            // Attempt to clear the active handler before updating
            clear_handler();
            
            // ...
            m_handler = handler;
            
            return 0;
        }
        /*
         * ...
         */
        std::variant<std::monostate, ROOTHandler*, ASCIIHandler*> get_handler() {
            // ...
            return m_handler;
        }
        /*
         * ...
         * 
         * NOTE: Need to be careful when calling this after hpx has already been deleted elsewhere,
         * i.e., in the destructor of the handler class or such, as it will cause a segfault, when
         * calling delete on freed memory
         */
        int clear_hpx() {
            // ...
            if (m_hpx) {
                std::cout << "\nFound existing histogram, clearing...\n";
                
                delete m_hpx;
                m_hpx = nullptr;
                
                std::cout << "\nHistogram cleared.\n";
            }
            
            return 0;
        }
        /*
         * ...
         */
        int set_hpx(TH1* hpx) {
            // ...
            // std::cout <<"\nUpdating histogram pointer...\n";
            m_hpx = hpx;
            // std::cout <<"\nUpdated histogram pointer.\n";
            
            return 0;
        }
        /*
         * Getter function for active histogram pointer
         */
        TH1* get_hpx() {
            // ...
            if (!m_hpx) {
                std::cerr << "\nError: Histogram null.\n";
                return nullptr;
            }
            
            // ...
            return m_hpx;
        }
        /*
         * ...
         * 
         * NOTE: Need to be careful when calling this after canvas has already been deleted elsewhere,
         * i.e., in the plot() function, as it will cause a segfault, when calling delete on freed memory
         */
        int clear_canvas() {
            // ...
            if (m_canvas) {
                std::cout << "\nFound existing canvas, closing...\n";
                
                m_canvas->Close();
                delete m_canvas;
                m_canvas = nullptr;
                
                std::cout << "\nCanvas cleared.\n";
            }
            
            return 0;
        }
        /*
         * Set the active canvas for this plotting session
         * 
         * NOTE: If an error occurs after creating canvas and before canvas is set here,
         * there will be a canvas left floating, 
         */
        int set_canvas(TCanvas* canvas) {
            // ...
            m_canvas = canvas;
            
            // ...
            return 0;
        }
        /*
         * Getter function for active canvas pointer
         */
        TCanvas* get_canvas() {
            // ...
            return m_canvas;
        }
};


// // Global session pointer
// PlotSession* gSession = nullptr;
// // NOTE: Temporarily declaring this down here as i think its still a tad too early for header files etc


/*
 * Check if canvas already exists, close it if so
 * 
 * NOTE: While PlotSession manages its active canvas, if an error occurs before set_canvas()
 * is called, the canvas will remain floating in the global list of canvases, causing a
 * warning message when attempting to call create_canvas():
 * 
 * "Warning in <TCanvas::Constructor>: Deleting canvas with same name: canvas"
 * 
 * TODO: Likely want to implement some sort of check for both, but gotta be careful with
 * calling delete on an already deleted canvas, which will cause a segfault
 * 
 * TODO: Could also consider a clear all canvases in list approach
 * 
 * TODO: Currently unused
 */
int clear_canvas(std::string const& name = "canvas") {
    // ...
    TSeqCollection* canvasList = gROOT->GetListOfCanvases();
    
    // ...
    if (canvasList->IsEmpty()) {
        std::cout << "\nNo active canvases.\n";
        return 0;
    }
    
    // ...
    TObject* oldCanvas = canvasList->FindObject(name.c_str());
    
    // ...
    if (oldCanvas) {
        std::cout << "\nFound existing canvas, closing...\n";
        
        auto existingCanvas = static_cast<TCanvas*>(oldCanvas);    
        existingCanvas->Close();
        delete existingCanvas;
        
        std::cout << "\nCanvas closed.\n";
    }
    
    return 0;
}

/*
 * Instantiates a canvas object, populating the global variable
 * 
 * NOTE: WinX and WinY denote the screen coordinates of the top left of the created canvas
 */
std::optional<TCanvas*> create_canvas(std::string const& name = "canvas") {
    std::cout << "\nCreating canvas...\n";
    
    // Canvas args
    Int_t const winX = 0; // Top left of screen
    Int_t const winY = 0; // Top left of screen
    Int_t const width = 1200;
    Int_t const height = 800;
    
    // Create a canvas display
    auto canvas = new TCanvas(name.c_str(), "Histogram Viewer", winX, winY, width, height);
    
    // Handle error creating canvas
    if (!canvas) {
        std::cerr << "\nError (create_canvas()): Couldnt create canvas!\n";
        return std::nullopt;
    }
    
    std::cout << "\nCanvas created.\n";
    
    // No errors, all good
    return canvas;
}

/*
 * Renders the populated histogram object on to the instantiated canvas
 * 
 * NOTE: May not always want to setOptStat(0), is useful for energy spectra,
 * but for exponentials etc, having a way to leave it enabled is useful
 */
int render_hist(TH1* hpx, TCanvas* canvas, bool const hideDefaultStats = true) {
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
    // hpx->Draw();
    // NOTE: With histos filled from ASCII & ntuples, "HIST" no longer needed, and for TH1 created
    // by Geant4, the draw option "HIST" is set to the object when loading it from the root file
    
    // ...
    // canvas->Update(); // NOTE: Afaik, this is not needed
    
    // gPad->Update(); // Make sure the statistics box is created
    // NOTE: Without gPad update, FindObject("stats") may sometimes return null pointer (leading to undefined behaviour)
    
    // Clean the default histogram statistics box
    if (hideDefaultStats) {
        gStyle->SetOptStat(0); // default = 1111 (NOTE: 000001111 with zeros removed)
        // 0 = hides the statistics box entirely (leaving only fit box when fitted)
        // 10 = only number of entries
        // 110 = entries and mean
        // NOTE: Prefix zeros must be removed, as "01" is treated as octal number
        
        // hpx->SetStats(kFALSE); // TEST
    }
    
    // canvas->Update(); // TEST
    
    // Draw histogram to the canvas with default option
    hpx->Draw();
    // NOTE: With histos filled from ASCII & ntuples, "HIST" no longer needed, and for TH1 created
    // by Geant4, the draw option "HIST" is set to the object when loading it from the root file
    // NOTE: Calling this after SetOptStat means no need to update canvas
    
    // ...
    std::cout << "\nHistogram rendered to canvas.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Validate file path, load file into memory, instantiate histogram, fill histogram,
 * instantiate canvas, render histogram
 * 
 * TODO: Even though this moves away from global objects, when it comes to replot()
 * and save(), im still going to need to assign ASCIIHandler and ROOTHandler to
 * global pointers for macro to work smoothly in interactive ROOT terminal ...
 * 
 * else it will have to be "[hpx, canvas, handler] = plot_x" type shit, or even worse
 * considering id have to make this optional return type
 * 
 * with global vars i can literally call individually:
 * plot(...)
 * replot(...)
 * save(...)
 * 
 * NOTE: ^ no assignment, optional types, etc
 * 
 * NOTE: But also, eventually this may be compiled, so will want a methodology that can
 * work in both ROOT terminal and via executable ...
 * 
 * TODO: Consider whether to:
 * - close old canvas if it exists and reopen new one
 * - reuse old canvas if it exists
 */
int plot_x(
    std::string const& userPath,
    std::string const& objectName = "",
    std::string const& branchName = "",
    int const nbins = -1,
    double const xmin = -1.,
    double const xmax = -1.,
    bool const doPostProcessing = false,
    double const sigmaFactor = 1.
) {
    // Instantiate only once in a given plotting session
    if (!gSession) gSession = new PlotSession();
    
    // ...
    // std::cout << "\nHIST PARAMS:\nX Min: " << xmin << ", X Max: " << xmax << ", Num Bins: " << nbins << "\n";
    
    // Active file type flag
    FileType fileType = FileType::NULLFILE;
    
    // Check provided path is valid (will return empty strings if not valid)
    auto const [path, token] = check_path(userPath);
    
    if (path.empty()) {
        std::cerr << "\nAborting: Invalid path error!\n";
        return 1;
    }

    // If its an ASCII file extension, log it and set ASCII flag
    if (token == ".Spe") {
        fileType = FileType::ASCII;
    }
    // If its a ROOT file extension, log it and set ROOT flag
    else if (token == ".root") {
        fileType = FileType::ROOT;
    }
    // TODO: Maybe make ".Spe" and ".root" global variables, rather than defining here,
    // and in check_path() ...

    // ...
    std::optional<TH1*> result;
    
    // Instantiate appropriate file type handler on the heap, call plot method, store handler in plot session object
    if (fileType == FileType::ASCII) {
        auto handler = new ASCIIHandler();
        result = handler->plot_ascii(path);
        gSession->set_handler(handler);
    }
    else if (fileType == FileType::ROOT) {
        auto handler = new ROOTHandler();
        result = handler->plot_root(path, objectName, branchName, nbins, xmin, xmax, doPostProcessing, sigmaFactor);
        gSession->set_handler(handler);
    }
    else {
        std::cerr << "\nAborting: Unsupported file type.\n";
        return 1;
    }
    // TODO: Filetype flag feeling kinda redundant, could just do this logic in prior enclosure
    // TODO: Doing ASCII then ROOT then ASCII has both handlers active etc
    
    if (!result.has_value()) {
        std::cerr << "\nAborting: Plotting error!\n";
        return 1;
    }
    
    TH1* hpx = result.value();
    
    // TODO: if (!hpx) ...
    
    // Set the histogram for the current plotting session
    gSession->set_hpx(hpx);
    
    // Check if canvas already exists, close it if so
    gSession->clear_canvas();
    // TODO: clear_canvas() function (not class method) is arguably more robust,
    // ensuring canvas is closed even if prior call to plot() errored out before
    // canvas was set to PlotSession... however its potentially less scalable
    
    // Attempt to create canvas
    std::optional<TCanvas*> canvasError = create_canvas();
    
    if (!canvasError.has_value()) {
        std::cerr << "\nAborting: Create canvas error!\n";
        return 1;
    }
    
    TCanvas* canvas = canvasError.value();
    
    // TODO: if (!canvas) ....
    
    // TODO: Consider reusing existing canvas on subsequent calls to plot(), instead of
    // creating a new one each time (maybe not though)
    
    // Set the canvas for the current plotting session
    gSession->set_canvas(canvas);
    
    // Attempt to draw the histogram to the canvas
    int const renderError = render_hist(hpx, canvas);
    
    if (renderError) {
        std::cerr << "\nAborting: Render histogram error!\n";
        return 1;
    }
    
    return 0;
}

/*
 * Overload 1) ROOT object selection CLI and automated histogramming
 */
int plot(std::string const userPath) {
    int const success = plot_x(userPath);
    return success;
};

/*
 * Overload 2) Manual ROOT object name specification and automated histogramming (TH1)
 * 
 * TODO: If only object name passed, need to determine if its TH1 or TTree
 * if its TH1, plot it
 * if its TTree, ask user for branch name if there is more than one branch available
 */
int plot(std::string const userPath, std::string const objectName) {
    int const success = plot_x(userPath, objectName);
    return success;
};

/*
 * Overload 3) Manual ROOT object name specification and automated histogramming (TTree)
 */
int plot(std::string const userPath, std::string const objectName, std::string const branchName) {
    int const success = plot_x(userPath, objectName, branchName);
    return success;
};

/*
 * Overload 4) ROOT object selection CLI and pre-defined histogramming parameters
 * 
 * NOTE: Only available for TTree, as TH1 object in ROOT file already binned.
 */
int plot(std::string const userPath, int const nbins, double const xmin, double const xmax) {
    int const success = plot_x(userPath, "", "", nbins, xmin, xmax);
    return success;
};

/*
 * Overload 5) Manual ROOT TTree name specification, and pre-defined histogramming parameters
 * 
 * NOTE: Only available for TTree, as TH1 object in ROOT file already binned.
 */
int plot(std::string const userPath, std::string const objectName, std::string const branchName, int const nbins, double const xmin, double const xmax) {
    int const success = plot_x(userPath, objectName, branchName, nbins, xmin, xmax);
    return success;
}

// TODO: doPostProcessing & sigmaFactor overloads

/*
 * ...
 * 
 * 1) User plots object of choice via interactive CLI
 * 2) User sees plot and knows how theyd like to rebin / rescale x-axis
 * 3) User calls replot with those values, without having to re-enter file name or going through CLI again,
 * due to cached "lastPath" and "lastObjectName"
 * 
 * NOTE: Reuses existing canvas as is, but may want new canvas
 * 
 * NOTE: See below regarding hpx lifetime (ROOTHandler->replot(...) && PlotSession->set_hpx(...))
 */
int replot(
    int const nbins,
    double const xmin,
    double const xmax,
    bool const doPostProcessing = false,
    double const sigmaFactor = 1.,
    double const lowerDiscriminator = -1.,
    double const channelOffset = 0.
) {
    // Ensure session handler is instantiated
    if (!gSession) {
        std::cerr << "Aborting: Please call plot() before replotting.\n";
        return 1;
    }
    
    // Get the active handler
    auto activeHandler = gSession->get_handler();
    
    // Check and fetch pointer
    auto** isActive = std::get_if<ROOTHandler*>(&activeHandler);
    
    // If root handler is not the active handler, abort
    if (!isActive) {
        std::cerr << "\nAborting: Replot only available for ROOT objects.\n";
        return 1;
    }
    
    // ...
    ROOTHandler* gROOTHandler = *isActive;
    
    // Handle bad args
    if (nbins <= 0) {
        std::cerr << "Please select a positive number of bins.\n";
        return 1;
    }
    if (xmax <= xmin) {
        std::cerr << "Please ensure xmax is greater than xmin.\n";
        return 1;
    }
    
    // Replot the last accessed TBranch with the specified args
    std::optional<TH1*> result = gROOTHandler->replot(
        nbins,
        xmin,
        xmax,
        doPostProcessing,
        sigmaFactor,
        lowerDiscriminator,
        channelOffset
    );
    
    if (!result.has_value()) {
        std::cerr << "\nAborting: Plotting error!\n";
        return 1;
    }
    
    TH1* hpx = result.value();
    
    // TODO: if (!hpx) ...
    
    // Update the PlotSession with the new histogram
    gSession->set_hpx(hpx);
    // NOTE: Since ROOTHandler->replot deletes the hpx, without updating the hpx stored in the
    // plot session, add_axis_title() will cause a segfault when trying to write to that memory
    // location via SetXTitle(...), also, attempting to call gSession->clear_hpx() after replot
    // will do the same, or gSession->clear_hpx() before replot will cause segfault when 
    // ROOTHandler attempts to delete... need to be careful with this partly shared ownership
    
    // TODO: Maybe dont delete the hpx in handler->replot, but also that would be kinda shitty
    // incase an error occurs before plot session can delete it
    
//     // Attempt to reuse existing canvas
//     TObject* oldCanvas = gROOT->GetListOfCanvases()->FindObject("canvas");
//     
//     if (!oldCanvas) {
//         std::cerr << "\nAborting: Failed to find canvas.\n";
//         return 1;
//     }
//     
//     auto canvas = static_cast<TCanvas*>(oldCanvas);
    
    // Attempt to reuse existing canvas
    TCanvas* canvas = gSession->get_canvas();
    
    if (!canvas) {
        std::cerr << "\nAborting: Failed to find canvas.\n";
        return 1;
    }
    
    // Attempt to draw the histogram to the canvas
    int const renderError = render_hist(hpx, canvas);
    
    if (renderError) {
        std::cerr << "\nAborting: Render histogram error!\n";
        return 1;
    }
    
    return 0;
}

/*
 * Post-processing utility for making plots presentation ready
 * 
 * @title // x-axis title (i.e. "Distance (mm)")
 * 
 * TODO: Maybe store reference to both canvas & hpx on classes, and add getters
 */
int add_axis_title(std::string const& axis, std::string const& title, TH1* hpx, TCanvas* canvas) {
    // Handle error creating canvas
    if (!canvas) {
        std::cerr << "\nError (add_axis_title()): Couldnt find canvas!\n";
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (add_axis_title()): Histogram not found!\n";
        // NOTE: All files and objects should already be closed/deconstructed at this point
        return 1;
    }
    
    std::cout << "\nAdding " << axis << "-axis title \"" << title << "\"\n";
    
    // Assgn title to axis
    if (axis == "x") {
        // X-axis title
        hpx->SetXTitle(title.c_str());
    }
    else if (axis == "y") {
        // X-axis title
        hpx->SetYTitle(title.c_str());
    }
    else {
        std::cerr << "\nError: Invalid axis\n";
        return 1;
    }
    
    std::cout << "\nRefreshing canvas...\n";
    
    // Notify canvas of change to pad, and refresh
    gPad->Modified();
    canvas->Update();
    
    std::cout << "\nAxis title has been updated.\n";
    
    return 0;
}

/*
 * Wrapper for add_axis_title(), enabling convinient ROOT interactive terminal usage
 */
int x_title(std::string const title) {
    std::cout << "\nAttempting to retrieve histogram...\n";
    
    TH1* hpx = gSession->get_hpx();
    
    // NOTE: Debug
    // if (hpx->IsOnHeap()) {
    //     std::cout << "\nHPX IS ALIVE\n";
    // }
    // if (hpx->IsZombie()) {
    //     std::cout << "\nHPX IS UNDEAD\n";
    // }
    // if (hpx->IsDestructed()) {
    //     std::cout << "\nHPX IS DEAD\n";
    // }
    
    std::cout << "\nAttempting to retrieve canvas...\n";
    
    TCanvas* canvas = gSession->get_canvas();
    
    std::cout << "\nAttempting to set axis title...\n";
    
    int success = add_axis_title("x", title, hpx, canvas);
    
    return success;
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
    TH1* hpx = gSession->get_hpx();
    
    std::cout << "\nExtracting histogram name...\n";
    
    // Use the name assigned to the histogram
    std::string const name = hpx->GetName();
    // NOTE: Will be same as ntuple name, or "EnergySpectrum" for ASCII histos
    
    std::cout << "\nAttempting to save histogram \"" << name << "\" to \"" << path << "\"\n";
    
    // Attempt to save it in a root file at the given path location
    int const success = save_to(path, hpx, name);
    
    return success;
}


/*
 * Zoom in on a specific range
 */
int range (double start, double end) {
    // Grab the active histogram and canvas
    if (!gSession) {
        std::cerr << "Error: No active session.\n";
        return 1;
    }
    
    TH1* hpx = gSession->get_hpx();
    TCanvas* canvas = gSession->get_canvas();
    
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
    // Grab the active histogram and canvas
    if (!gSession) {
        std::cerr << "Error: No active session.\n";
        return 1;
    }
    
    TH1* hpx = gSession->get_hpx();
    TCanvas* canvas = gSession->get_canvas();
    
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
 * 
 * TODO: Need to introduce a means of checking whether the initial maxBin search
 * wanders off towards another peak, i.e., across the range provided, considering
 * a source with closely merged shoulders like 133Ba:
 * - when a positive gradient is encountered, and it tops out, then begins to fall,
 * mark the rough mean (dy/dx = 0) as a potential centroid, then continue the search,
 * marking any other centroids encountered. 
 * - After the search is complete, find the centroid which is closest to the rough
 * centroid provided
 * - Or, use peakNum, i.e., if search encounters 2 peaks, but peakNum = 1, go for
 * the first peak, else if peakNum = 2, go for the second peak
 */
std::optional<TFitResultPtr> fit_individual(TH1* hpx, int const& roughCentroid, int const& roughFWHM, int const& peakNum) {
    // Log input params to stdout
    std::cout << "Rough Centroid Arg: " << roughCentroid << " Rough FWHM: " << roughFWHM << "\n";
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (fit_individual()): Histogram not found!\n";
        return std::nullopt;
    }
    
    // TEST
    double const searchLow = roughCentroid - (1.5 * roughFWHM);
    double const searchHigh = roughCentroid + (1.5 * roughFWHM);
    int const searchLowBin = hpx->FindFixBin(searchLow);
    int const searchHighBin = hpx->FindFixBin(searchHigh);
    
    // int maxBin = -1;
    int maxBin = roughCentroid;
    
    for (int i = searchLowBin; i < searchHighBin; i++) {
        int const currBin = hpx->GetBinContent(i);
        if (currBin > hpx->GetBinContent(maxBin)) maxBin = i;
    }
    std::cout << "MAX BIN: " << maxBin << "\n";
    // TEST
    
    // Get the centre of the centroid channel, and number of counts in centroid bin
    // double const roughMean = hpx->GetXaxis()->GetBinCenter(roughCentroid); // get the x-axis location of max counts bin
    // double const roughAmplitude = hpx->GetBinContent(roughCentroid); // get the y-axis number of counts for max bin, i.e. amplitude
    double const roughMean = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const roughAmplitude = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
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
    // double const prefitAmplitudeLimitLow = roughAmplitude * 0.9;
    double const prefitAmplitudeLimitLow = roughAmplitude * 0.85;
    double const prefitAmplitudeLimitHigh = roughAmplitude * 1.1;
    prefitFn->SetParLimits(0, prefitAmplitudeLimitLow, prefitAmplitudeLimitHigh); // amplitude is arg[0]
    double const prefitCentroidLimitLow = roughMean - (1.5 * roughFWHM);
    double const prefitCentroidLimitHigh = roughMean + (1.5 * roughFWHM);
    prefitFn->SetParLimits(1, prefitCentroidLimitLow, prefitCentroidLimitHigh); // mean is arg[1]
    double const prefitSigmaLimitLow = roughSigma * 0.25;
    double const prefitSigmaLimitHigh = roughSigma * 2;
    prefitFn->SetParLimits(2, prefitSigmaLimitLow, prefitSigmaLimitHigh); // sigma is arg[2]
    // NOTE: May want to consder limits for amplitude and sigma, but not sure its as
    // relevant for those (maybe wrong on that though)
    std::cout << ">>> Amplitude Low: " << prefitAmplitudeLimitLow << " Rough Amplitude: " << roughAmplitude << " Amplitude High: " << prefitAmplitudeLimitHigh << "\n";
    std::cout << ">>> Mean Lower Window: " << roughLow << " Mean Low: " << prefitCentroidLimitLow << " Rough Mean: " << roughMean << " Mean High: " << prefitCentroidLimitHigh << " Mean Upper Window: " << roughHigh << "\n";
    std::cout << ">>> Sigma Low: " << prefitSigmaLimitLow << " Rough Sigma: " << roughSigma << " Sigma High: " << prefitSigmaLimitHigh << "\n";
    // TEST
    
    // Call the histograms fit method, passing the fit function and histogram fitting options string
    // TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS");
    // TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS+0");
    // TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS+0L");
    // TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS+0B");
    TFitResultPtr const initialResult = hpx->Fit(prefitFn, "RS+0BL");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = attempts to improve the fit quality
    // "L" = use log likelihood method (default chi-square), for use with counts histograms
    // "+" = adds this new fitted func to list of fitted funcs (default is delete previous keep last)
    // "0" = does not draw fitted function after fitting
    // "B" = use this to fix or set parameter limits with predefined funcs (i.e., "gaus"),
    // else default initial values and limits may be used
    // NOTE: Using log likelihood here seems to do ok
    
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
    double const refitLow = prefitCentroid - (2 * prefitFWHM);
    double const refitHigh = prefitCentroid + (2 * prefitFWHM);
    
    std::cout << "Refined Lower Bound: " << refitLow << " Refined Upper Bound: " << refitHigh << "\n";
    
    // TEST
    std::string const refitFuncName = "refitFn" + std::to_string(peakNum);
    // NOTE: Again this may only be needed for non-overwriting functions
    // TEST
    
    // Define the fit function which will take refined parameters
    // auto refitFn = new TF1("refitFn", "gaus", refitLow, refitHigh);
    auto refitFn = new TF1(refitFuncName.c_str(), "gaus", refitLow, refitHigh);
    
    // Perform a refit using the fitted params
    refitFn->SetParameters(prefitAmplitude, prefitCentroid, prefitSigma);
    
    // refitFn->SetParNames("Amplitude", "Centroid", "Sigma");
    refitFn->SetParNames(arg0Name.c_str(), arg1Name.c_str(), arg2Name.c_str()); // TEST - see prefit name comment above
    // Define names for each parameter
    
    // TEST - Set parameter limits for finding centroid
    double const refitAmplitudeLimitLow = prefitAmplitude * 0.9;
    double const refitAmplitudeLimitHigh = prefitAmplitude * 1.1;
    refitFn->SetParLimits(0, refitAmplitudeLimitLow, refitAmplitudeLimitHigh); // sigma is arg[2]
    double const refitCentroidLimitLow = prefitCentroid - (1 * prefitFWHM); // TODO: Maybe tighter here
    double const refitCentroidLimitHigh = prefitCentroid + (1 * prefitFWHM); // TODO: Maybe tighter here
    refitFn->SetParLimits(1, refitCentroidLimitLow, refitCentroidLimitHigh); // mean is arg[1]
    double const refitSigmaLimitLow = prefitSigma * 0.75; // TODO: Maybe tighter here
    double const refitSigmaLimitHigh = prefitSigma * 1.25; // TODO: Maybe tighter here
    refitFn->SetParLimits(2, refitSigmaLimitLow, refitSigmaLimitHigh); // sigma is arg[2]
    // NOTE: May want to consder limits for amplitude and sigma, but not sure its as
    // relevant for those (maybe wrong on that though)
    // TEST
    
    // Calculate the results of the refit
    // TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS"); // overwrite function list
    // TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS+0");
    // TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS+0L");
    TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS+0B");
    // TFitResultPtr const refitResult = hpx->Fit(refitFn, "RS+0BL");
    // NOTE: Append to function list ("+"), disable auto draw ("0"), respect limits/initial param val ("B")
    // NOTE: Implementing log likelihood method on the refit causes weird behaviour
    
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
std::vector<double> sideband_avg(TH1* hpx, TAxis const* xAxis, double const& xStart, double const& xEnd) {
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
std::optional<std::vector<double>> get_counts(TH1* hpx, TF1* fitFn, TFitResultPtr const result) {
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
// int get_stats_lines(TFitResultPtr const &result, std::vector<double> const &counts, TList* listOfLines) {    
//     // Retrieve the fit chi squared & n.d.f
//     double const chi2 = result->Chi2();
//     double const ndf = result->Ndf();
//     
//     // Calculate the goodness of fit
//     double const goodFit = chi2 / ndf;
//     
//     std::cout << "Goodness of Fit: " << goodFit << "\n";
//     
//     // Extract the fitted photopeak centroid and error on the result
//     double const fittedCentroid = result->Parameter(1);
//     double const fittedCentroidError = result->Error(1);
//     
//     // Calculate the updated FWHM, based on fitted sigma
//     double const fittedSigma = result->Parameter(2);
//     double const fittedFWHM = fittedSigma * 2.355;
//     
//     std::cout << "POST-FIT SIGMA: " << fittedSigma << "\n";
//     std::cout << "POST-FIT FWHM: " << fittedFWHM << "\n";
//     
//     // Calculate the error on the fitted FWHM, based on fitted sigma error
//     double const fittedSigmaError = result->Error(2);
//     double const fittedFWHMError = fittedSigmaError * 2.355;
//     
//     // Get integrated photopeak counts and error on counts from input vector
//     double const countsVal = counts[0];
//     double const countsErr = counts[1];
//     
//     // Add centroid (+/- error) to the stats box
//     char const* text1 = Form("Centroid = %.2f #pm %.2f", fittedCentroid, fittedCentroidError); // Format the entry (#pm generates +/-)
//     auto newLine1 = new TLatex(0, 0, text1); // <- may have to do Form() for string
//     newLine1->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
//     newLine1->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
//     listOfLines->Add(newLine1); // append the fwhm value & error to the fit stats
// 
//     // Add FWHM (+/- error) to the stats box
//     char const* text2 = Form("FWHM = %.2f #pm %.2f", fittedFWHM, fittedFWHMError); // Format the entry (#pm generates +/-)
//     auto newLine2 = new TLatex(0, 0, text2); // <- may have to do Form() for string
//     newLine2->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
//     newLine2->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
//     listOfLines->Add(newLine2); // append the fwhm value & error to the fit stats
//     
//     // Add counts (+/- error) to the stats box
//     char const* text3 = Form("Counts = %.2f #pm %.2f", countsVal, countsErr);
//     auto newLine3 = new TLatex(0, 0, text3);
//     newLine3->SetTextFont(gStyle->GetStatFont());
//     newLine3->SetTextSize(gStyle->GetStatFontSize());
//     listOfLines->Add(newLine3);
//     
//     // No errors, all good
//     return 0;
// }

/*
 * Extract fit statistics for individual peaks, create and populate a new line for
 * each chosen statistic, then add them to the full list of lines
 * 
 * NOTE: Adds peak number prefix:
 * 1-Centroid, 1-FWHM, 1-Counts
 * 2-Centroid, 2-FWHM, 2-Counts
 * 
 * TODO: If num peaks = 1, dont prefix fitted params, i.e.:
 * Centroid, FWHM, Counts
 * W/ no integer prefix
 * if only one peak fitted, no integer prefix
 * if >1 peak fitted, add peak number prefix
 * 
 * TODO: TFitResultPtr is already lightweight, reference maybe not needed
 * 
 * TODO: Take peak number as param or no
 */
int get_stats_lines(TFitResultPtr const &result, std::vector<std::vector<double>> const &counts, TList* listOfLines) {
    // ...
    int const numPeaks = counts.size();
    // NOTE: This kinda feels dirty, even though its a valid approach,
    // may pass as param instead, not sure
    
    // ...
    for (int i = 0; i < numPeaks; i++) {
        // Retrieve the fit chi squared & n.d.f
        double const chi2 = result->Chi2();
        double const ndf = result->Ndf();
        
        // Calculate the goodness of fit
        double const goodFit = chi2 / ndf;
        
        std::cout << "Goodness of Fit: " << goodFit << "\n";
        
        // Define indices
        int const arg0IDX = i * 3; // amplitude = [0], [3], [6]
        int const arg1IDX = arg0IDX + 1; // mean = [1], [4], [7]
        int const arg2IDX = arg1IDX + 1; // sigma = [2], [5], [8]
        
        // Extract the fitted photopeak centroid and error on the result
        double const fittedCentroid = result->Parameter(arg1IDX);
        double const fittedCentroidError = result->Error(arg1IDX);
        
        // Calculate the updated FWHM, based on fitted sigma
        double const fittedSigma = result->Parameter(arg2IDX);
        double const fittedFWHM = fittedSigma * 2.355;
        
        std::cout << "POST-FIT SIGMA: " << fittedSigma << "\n";
        std::cout << "POST-FIT FWHM: " << fittedFWHM << "\n";
        
        // Calculate the error on the fitted FWHM, based on fitted sigma error
        double const fittedSigmaError = result->Error(arg2IDX);
        double const fittedFWHMError = fittedSigmaError * 2.355;
        
        // Get integrated photopeak counts and error on counts from input vector
        double const countsVal = counts[i][0];
        double const countsErr = counts[i][1];
        
        // Add centroid (+/- error) to the stats box
        char const* text1 = Form("%i-Centroid = %.2f #pm %.2f", i, fittedCentroid, fittedCentroidError); // Format the entry (#pm generates +/-)
        auto newLine1 = new TLatex(0, 0, text1); // <- may have to do Form() for string
        newLine1->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
        newLine1->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
        listOfLines->Add(newLine1); // append the fwhm value & error to the fit stats

        // Add FWHM (+/- error) to the stats box
        char const* text2 = Form("%i-FWHM = %.2f #pm %.2f", i, fittedFWHM, fittedFWHMError); // Format the entry (#pm generates +/-)
        auto newLine2 = new TLatex(0, 0, text2); // <- may have to do Form() for string
        newLine2->SetTextFont(gStyle->GetStatFont()); // match font to existing stat box font
        newLine2->SetTextSize(gStyle->GetStatFontSize()); // match font size to existing stat box font size
        listOfLines->Add(newLine2); // append the fwhm value & error to the fit stats
        
        // Add counts (+/- error) to the stats box
        char const* text3 = Form("%i-Counts = %.2f #pm %.2f", i, countsVal, countsErr);
        auto newLine3 = new TLatex(0, 0, text3);
        newLine3->SetTextFont(gStyle->GetStatFont());
        newLine3->SetTextSize(gStyle->GetStatFontSize());
        listOfLines->Add(newLine3);
    }
    
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
int draw_fit_stats(TH1* hpx, TList* listOfLines) {
    // Format the output SetOptFit(pcev)
    // gStyle->SetOptFit(111); // NOTE: param is a bit-mask (4-digit integer)
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
        std::cerr << "\nError (draw_fit_stats()): Histogram not found!\n";
        return 1;
    }
    
    // gPad->Update(); // TEST
    
    // Get a pointer to the stats box object
    // auto ps = (TPaveStats*)(hpx->FindObject("stats"));
    // auto ps = reinterpret_cast<TPaveStats*>(hpx->FindObject("stats"));
    // auto ps = dynamic_cast<TPaveStats*>(hpx->FindObject("stats")); // NOTE: Works w/ combo
    // auto ps = dynamic_cast<TPaveStats*>(hpx->GetListOfFunctions()->FindObject("stats")); // NOTE: Works w/ combo
    // auto ps = dynamic_cast<TPaveStats*>(c->GetPrimitive("stats")); // NOTE: Works w/ combo
    // NOTE: Casting the ambiguous return type of find object (TObject*), to TPaveStats object
    
    // ...
    double const bottomLeftX = 0.65;
    double const bottomLeftY = 0.7;
    double const topRightX = 0.88;
    double const topRightY = 0.88;
    
    // ...
    // auto ps = new TPaveStats(); // TEST
    // auto ps = new TPaveStats(0.65, 0.70, 0.88, 0.88, "brNDC");
    // auto ps = new TPaveStats(0.65, 0.70, 0.88, 0.88, "blNDC");
    // auto ps = new TPaveStats(0.65, 0.70, 0.88, 0.88, "NDC");
    auto ps = new TPaveStats(bottomLeftX, bottomLeftY, topRightX, topRightY, "NDC");
    // NOTE: Args = X1, Y1, X2, Y2, option
    // these represent normalised device coordinates (NDC), where NDC maps 0.0 (bottom left) to 1.0 (top right)
    // (X1, Y1) defines bottom left corner of the box
    // (X2, Y2) defines top right corner of the box
    // "br" = Border-relative shadow (draws a drop-shadow cast under bottom right corner) (omittable)
    // "bl" = Border-left shadow (drop-shadow cast under bottom left corner ) (omittable)
    // "NDC" ensures stats box scales with window size and stays in position
    
    
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
    // ps->Clear();
    
    // TEST
    // ps->SetBorderSize(1);
    // ps->SetFillColor(0);
    // ps->SetTextFont(42);
    // ps->SetTextSize(0.035);
    // TEST
    
    // Get existing statistics box content
    TList* existingLines = ps->GetListOfLines();
    // auto existingLines = new TList();
    
    // ...
    // existingLines->Clear();
    
    // Append the custom stats to the TPaveStats list
    // for (int i = 0; i < listOfLines->GetSize(); i++) {
    //     existingLines->Add(listOfLines->At(i));
    // }
    existingLines->AddAll(listOfLines); // NOTE: Does same as above
    
    // Display the custom statistics box
    // hpx->SetStats(0); // Disable auto future stats regeneration
    // ps->Draw(); // Redraw custom box
    // c->Update(); // this can actually be omitted too
    
    // Attach custom stats box to list of primitives so it draws with the plot
    gPad->GetListOfPrimitives()->Add(ps);
    
    // Display the custom statistics box
    gPad->Modified();
    gPad->Update();
    
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
 * 
 * TODO: Consider parameter limits on full fit (but maybe omit)
 * ^ have to be careful, fwhm on 60Co individual peak fits is much larger
 * than the fwhm result from the full fit, using that larger fwhm with too
 * tight of param limits will mess up the full fit
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
    
    // 3) Grab the active histogram
    if (!gSession) {
        std::cerr << "Error: No active session.\n";
        return 1;
    }
    
    TH1* hpx = gSession->get_hpx();
    
    // 4) Perform fits for each individual peak selected, cache the result pointer in a vector
    std::vector<TFitResultPtr> fitResults = {};
    
    for (int i = 0; i < centroidVec.size(); i++) {
        std::optional<TFitResultPtr> fitResult = fit_individual(hpx, centroidVec[i], roughFWHM, i);
        
        if (!fitResult.has_value()) {
            std::cerr << "\nError: Failed to fit peak " << i << "\n";
            return 1;
        }
        
        fitResults.push_back(fitResult.value());
    }
    
    
    // return 1; // debug
    
    
    
    
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
    std::vector<double> const left = sideband_avg(hpx, xAxis, leftLow, leftHigh);
    
    std::cout << "\nRight sideband\n\n";
    std::vector<double> const right = sideband_avg(hpx, xAxis, rightLow, rightHigh);
    
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
        std::optional<std::vector<double>> countsResult = get_counts(hpx, peakFunctions[i], fitResults[i]);
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
    
//     for (int i = 0; i < numPeaks; i++) {
//         // Write custom statistics to list for each fitted peak
//         int statsLinesError = get_stats_lines(fitResults[i], countsResults[i], listOfLines);
//         // TODO: Individual peak fits are giving FWHM much larger than the full fit,
//         // yet im displaying initial fit values, maybe change this:
//         // int statsLinesError = get_stats_lines(fullFitResult, countsResults[i], listOfLines);
//         
//         // Handle statistics writing
//         if (statsLinesError) {
//             std::cerr << "\nError: Failed get fit statistics!\n";
//             return 1;
//         }
//     }
    
    // Write custom statistics to list for each fitted peak    
    int statsLinesError = get_stats_lines(fullFitResult, countsResults, listOfLines);
        
    // Handle statistics writing
    if (statsLinesError) {
        std::cerr << "\nError: Failed get fit statistics!\n";
        return 1;
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
    int statsDrawError = draw_fit_stats(hpx, listOfLines);
    
    // Handle statistics drawing error
    if (statsDrawError) {
        std::cerr << "\nFailed draw fit statistics!\n";
        return 1;
    }
    
    return 0;
}
