// ...

// ROOT lib
#include <TDirectory.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TLeaf.h> // Incomplete type without explicit import
#include <TH1.h>
#include <TCanvas.h>
#include <TROOT.h> // TODO: I think this is unused ?
#include <TStyle.h> // gStyle
#include <TRandom.h> // gRandom
#include <TVirtualPad.h>

// C lib
#include <iostream> // cerr, cin, cout
#include <fstream> // ifstream
#include <sstream> // istringstream
#include <optional> // optional, nullopt
#include <unordered_set> // NOTE: Import not technically needed (something else must be importing it)
#include <unordered_map> // NOTE: ^ same

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

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int oop_plot() {
    // Usage
    std::cout << "\n-----------------------------------------------------------------------\n";
    std::cout << "\nConvert ASCII & ROOT Ntuples to Root Histogram.\n";
    std::cout << "\nTo get started, call: plot(\"path.ext\"), passing path to ASCII (.Spe) or ROOT (.root) file as param.\n";
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
    
    // Acceptable file extensions
    std::string const spe = ".Spe"; // const char*
    std::string const root = ".root"; // const char*
    
    // Check path ends with valid extension, reject invalid file type
    if ((token != spe) && (token != root)) {
        // Write to stdout
        std::cerr << "\nError: Invalid extension.\n";
        
        // Error value
        return PathResult {};
    }
    // If its an ASCII file extension, log it and set ASCII flag
    else if (token == spe) {
        std::cout << "\nASCII file detected.\n";
    }
    // If its a ROOT file extension, log it and set ROOT flag
    else if (token == root) {
        std::cout << "\nROOT file detected.\n";
    }

    // Check if tilde expansion needed, perform it if so
    std::string const returnPath = expand_path(path);
    
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
 * ...
 */
class ASCIIHandler {
    // ...
    private:
        // ...
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
    
    // ...
    public:
        /*
        * ...
        */
        ASCIIHandler() = default;
        
        /*
        * ...
        */
        ~ASCIIHandler() = default;
    
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
            
            // Handle ASCII (.Spe) file format
            
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
            
            // Handle ASCII files and ROOT TBranches containing integers
            
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
            
            TH1* hpx = histError.value();
            
            // Attempt to populate histogram from ASCII or ROOT file
            int const fillError = fill_hist_ascii(hpx, metaData.start, metaData.end);
            
            if (fillError) {
                std::cerr << "\nAborting: Fill hist error!\n";
                return std::nullopt;
            }
            
            return hpx;
        }
};


/*
 * ...
 */
class ROOTHandler {
    // ...
    private:
        // ...
        TFile* inROOT = nullptr;
        TTree* nTuple = nullptr;
        TBranch* branch = nullptr;
        TLeaf* leaf = nullptr;
        
        // ...
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
        * 
        */
        ROOTHandler() = default;
        
        /*
        * ...
        */
        ~ROOTHandler() = default;
    
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
        std::optional<SelectionReturnType> select_root_type(std::unordered_map<int, std::string>& objectMap) {
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
        std::string select_root_object(SelectionReturnType const& selectionParams, std::unordered_map<std::string, std::vector<std::string>>& categoryMap) {
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
            
            // Cache chosen root object name
            lastObjectName = selectedObjectName;
            
            return selectedObjectName;
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
            
            // Cache chosen branch name
            lastBranchName = chosenBranch;
            
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
        * 
        * TODO: @branchName arg
        * if (!branchName.empty()) do cli (select_branch())
        * else: skip select_branch()
        */
        int load_root_tree(char const* treeName) {
            // Cache TTree pointer
            int const loadTreeError = cache_tree(treeName);
            
            if (loadTreeError) {
                std::cerr << "\nError: Failed to cache TTree pointer.\n";
                return 1;
            }
            
            // Prompt user to select from available TBranches in TTree
            std::string const branchName = select_branch();
            
            if (branchName.empty()) {
                std::cerr << "\nError: Failed to get branch selection.\n";
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
        * Load histogram object (TH1D, TH1I, etc) from ROOT input file and cache pointer
        */
        int load_root_hist(char const* histName) {
            // Handle incorrect path
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
        int load_root_object(std::string const& objectName) {
            // Convert std::string to c string pointer
            char const* name = objectName.c_str();
            
            // Load object router
            if (rootObjectType == RootObjectType::TTree) {
                int const loadRootTreeError = load_root_tree(name);
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
        * TODO: @objectName arg
        * if (!objectName.empty()) do cli
        * else go straight to load_root_object(objectName)
        * 
        * TODO: root_explorer() method
        * ^ but is this unnecessary abstraction?
        */
        int load_root(std::string const& path) {
            // Attempt to open the ROOT file
            int const loadError = load_root_file(path);
            
            if (loadError) {
                std::cerr << "\nError: Failed to load ROOT file into memory.\n";
                return 1;
            }
            
            
            // TODO: root_explorer() method \/\/\/\/\/\/\/\/\/\/\/
            
            // Query the ROOT object to get a list of object types, and create key-value maps
            std::optional<QueryReturnType> querySuccess = get_root_types();
            
            if (!querySuccess.has_value()) {
                std::cerr << "\nError: Failed to query ROOT file.\n";
                return 1;
            }
            
            QueryReturnType query = querySuccess.value();
            
            // Prompt user for ROOT object type
            std::optional<SelectionReturnType> const selectionSuccess = select_root_type(query.objectMap); // choose TTree (Ntuple), TH1D (1D Hist), etc
            
            if (!selectionSuccess) {
                std::cerr << "\nError: Failed to get user selection for object type.\n";
                return 1;
            }
            
            SelectionReturnType const result = selectionSuccess.value();
            
            // Prompt user for ROOT object name
            std::string const objectName = select_root_object(result, query.categoryMap); // get tree/hist/etc name
            
            if (objectName.empty()) {
                std::cerr << "\nError: ROOT object name is empty.\n";
                return 1;
            }
            
            // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            
            
            // Cache pointers for chosen ROOT object
            int const loadObjectError = load_root_object(objectName); // load root object with said name
            
            if (loadObjectError) {
                std::cerr << "\nError: Failed to access ROOT object.\n";
                return 1;
            }
            
            std::cout << "\nROOT file and chosen object loaded.\n";
            
            // No errors, all good
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
        */
        int create_hist_root() {
            // Handle ROOT Ntuple
            
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
            
            // Set xmin to zero or branch minimum, whichever is lower, and xmax to max + 10%
            int const nbins = 2048; // TODO: Dynamic binning
            double const xmin = ((branchMin < 0.) ? branchMin : 0.); // should be zero, unless negative axis
            double const xmax = (branchMax) * 1.1; // +10% (NOTE: Using max is very succeptible to outliers)
            
            // NOTE: If they have been passed, use them
            
            // Get TTree data type by reading the leaves
            std::string_view const leafType = leaf->GetTypeName();
            // std::cout << "LEAF TYPE: " << leafType << "\n";
            
            std::cout << "\nHistogram args set to:\n";
            std::cout << ">>> Num Bins: " << nbins << " XMIN: " << xmin << " XMAX: " << xmax << "\n";
            
            // Reject invalid histogram args
            if (title.empty() || legendTitle.empty() || (xmin >= xmax)) {
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
        int fill_hist_ntuple(TH1* hpx) {
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
            
            std::cout << "\nSetting branch address for: " << branchName << "\n";
            
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
                
                // Add a count to the appropriate bin for that value, introduce smearing if requested
                if (dataType == intType) {
                    hpx->Fill(intEntry);
                }
                else if (dataType == doubleType) {
                    hpx->Fill(doubleEntry);
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
        * TODO: Have load_root() take objectName & branchName (but use default params = ""),
        * then if they arent empty, skip root explorer CLI methods
        * 
        * TODO: Maybe get the TH1 from load_root_hist
        * 
        * NOTE: ^ Returning hist from create_hist_root() is easy enough, but trying to get hist
        * from load_root_hist() requires an optional union return type from load_root_object()
        * and load_root(), which just seems like more hassle than its worth, and messy, compared
        * to just using a hpx class property ...
        */
        std::optional<TH1*> plot_root(std::string const& path) {
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
            
            // Attempt to load ROOT file into memory
            int const fileError = load_root(path);
            
            if (fileError) {
                std::cerr << "\nAborting: Load file error!\n";
                return std::nullopt;
            }
            
            // Only call: create_hist() & fill_hist(); if its ROOT Ntuple
            // NOTE: If its ROOT Histogram, hpx pointer will already be populated
            // TODO: This enlosure feels a bit dirty, likely a better way to do this
            if (rootObjectType == RootObjectType::TTree) {
            
                // Attempt to instantiate histogram object
                int const histError = create_hist_root();
                
                if (histError) {
                    std::cerr << "\nAborting: Create hist error!\n";
                    return std::nullopt;
                }
                
                // Attempt to populate histogram from ASCII or ROOT file
                int const fillError = fill_hist_ntuple(hpx);
                
                if (fillError) {
                    std::cerr << "\nAborting: Fill hist error!\n";
                    return std::nullopt;
                }
            }
            
            // Cache last used path
            lastPath = path;
            
            // ...
            return hpx;
        }
};

/*
 * Instantiates a canvas object, populating the global variable
 */
std::optional<TCanvas*> create_canvas() {
    std::cout << "\nCreating canvas...\n";
    
    // Canvas args
    Int_t const winX = 0; // Top left of screen
    Int_t const winY = 0; // Top left of screen
    Int_t const width = 1200;
    Int_t const height = 800;
    
    // Create a canvas display
    auto canvas = new TCanvas("canvas", "Histogram Viewer", winX, winY, width, height);
    
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
int render_hist(TH1* hpx, TCanvas* canvas, bool hideDefaultStats = true) {
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
    hpx->Draw(); 
    // NOTE: With histos filled from ASCII & ntuples, "HIST" no longer needed, and for TH1 created
    // by Geant4, the draw option "HIST" is set to the object when loading it from the root file
    
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
 */
int plot(std::string const userPath) {
    // Active file type flag
    FileType fileType = FileType::NULLFILE;
    
    // Check provided path is valid (will return empty strings if not valid)
    auto const [path, token] = check_path(userPath);
    
    if (path.empty()) {
        std::cerr << "\nAborting: Invalid path error!\n";
        return 1;
    }

    // If its an ASCII file extension, log it and set ASCII flag
    else if (token == ".Spe") {
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
    TH1* hpx = nullptr;
    
    // ...
    if (fileType == FileType::ASCII) {
        ASCIIHandler handler;
        result = handler.plot_ascii(path);
    }
    else if (fileType == FileType::ROOT) {
        ROOTHandler handler;
        result = handler.plot_root(path);
    }
    else {
        std::cerr << "\nError: Unsupported file type.\n";
    }
    // TODO: This logic could be achieved via previous if block, fileType is redundant currently
    
    if (!result.has_value()) {
        std::cerr << "\nAborting: Plotting error!\n";
        return 1;
    }
    
    hpx = result.value();
    
    // Attempt to create canvas
    std::optional<TCanvas*> canvasError = create_canvas();
    
    if (!canvasError.has_value()) {
        std::cerr << "\nAborting: Create canvas error!\n";
        return 1;
    }
    
    TCanvas* canvas = canvasError.value();
    
    // Attempt to draw the histogram to the canvas
    int const renderError = render_hist(hpx, canvas);
    
    if (renderError) {
        std::cerr << "\nAborting: Render histogram error!\n";
        return 1;
    }
    
    return 0;
}
