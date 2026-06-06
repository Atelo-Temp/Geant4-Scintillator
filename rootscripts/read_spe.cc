// Load an ASCII (.Spe) file into memory, fill a histogram, and display it on a canvas,
// then zoom in on a peak, and fit it with a user defined function

// ROOT lib
// #include <TFile.h> // NOTE: This is unused w/ current format, but will be used for omni_fit.cc
#include <TH1.h>
#include <TCanvas.h>
// #include <TF1.h>
// #include <TROOT.h> // TODO: I think this is unused ?
#include <TStyle.h> // gStyle
// #include <TFitResult.h>
// #include <TPaveStats.h>
// #include <TLatex.h>
// #include <TRandom.h> // TODO: This is unused w/ current format, but will be used for omni_fit.cc

// C lib
#include <iostream> // std::cout, std::cerr
#include <fstream> // std::ifstream
#include <sstream> // std:: istringstream
// #include <optional>
// #include <string>
// #include <algorithm>
// #include <charconv> // std::from_chars

// Global root object variables
std::ifstream in;
TH1* hpx = nullptr;
TCanvas* canvas = nullptr;

// Metadata object type for ASCII (.Spe) files
struct SpeMetaData {
    // Will be assigned the live and real time of the detector
    unsigned int liveTime = 0;
    unsigned int realTime = 0;
    
    // Will be assigned start and end channel numbers (i.e., 0 and 2047)
    unsigned int start = 0;
    unsigned int end = 0;
};

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int read_spe() {
    // Usage
    std::cout << "\n-----------------------------------------------------------------------\n";
    std::cout << "\nConvert ASCII to Root Histogram.\n";
    std::cout << "\nTo get started, call: plot(\"~/dir/filename.Spe\"), passing path to ASCII (.Spe) file as param.\n";
    std::cout << "\n-----------------------------------------------------------------------\n";
    
    // fin
    return 0;
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
 * @nbins // number of channels (bins)
 * @xmin // minimum channel
 * @xmax // maximum channel
 */
int create_hist(unsigned int const& nbins = 2048, unsigned int const& xmin = 0, unsigned int const& xmax = 2048) {
    // Histogram args
    // int const nbins = 2048; // 2048 channels (bins)
    // int const xmin = 0; // min channel
    // int const xmax = 2048; // max channel (3500 photons)
    
    // Create a histogram (TH1I = integer - channel/counts both ints)
    hpx = new TH1D(
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
 * 3) Read next 2048 lines, i.e.:
 * if (start > end) break;
 */
// int fill_hist_ascii() {
//     // Handle missing input file
//     if (!in.is_open()) {
//         std::cerr << "\nError: No infile to read!\n";
//         return 1;
//     }
//     
//     // Handle missing histogram
//     if (!hpx) {
//         std::cerr << "\nError (fill_hist()): Histogram not found!\n";
//         return 1;
//     }
// 
//     // Buffer to store lines from the infile
//     std::string buffer;
//     
//     // Will be assigned the live and real time of the detector
//     unsigned int liveTime = 0;
//     unsigned int realTime = 0;
//     
//     // Will be assigned start and end channel numbers (i.e., 0 and 2047)
//     unsigned int start = 0;
//     unsigned int end = 0;
//     // NOTE: Start will be incremented up to max channel number, i.e. 2047
//     
//     // State flags
//     bool readingData = false;
//     
//     // Get line reads a line from input stream into a string, until end of stream encountered
//     // NOTE: Stores characters from current line of infile in the buffer, until "\n" is encountered
//     // NOTE: Contents of buffer are erased at the start of next line before reading commences again
//     // NOTE: The "\n" at the end of the line is not stored in the buffer
//     while (std::getline(in, buffer)) {
//         // Print line number (NOTE: debug)
//         // std::cout << nlines << "\n";
//         
//         // Print line contents (NOTE: debug)
//         // std::cout << buffer << "\n";
//         
//         // Remove trailing carriage return (lines contain hidden carriage return: "$MEAS_TIM:\r")
//         if (!buffer.empty() && (buffer.back() == '\r')) buffer.pop_back();
//         // NOTE: Single quote for single character delimiter, double quotes for string of chars
//         
//         // if (lineContent == "$MEAS_TIM:") {
//         if (buffer == "$MEAS_TIM:") {
//             // Go to next line, read it into the buffer
//             if (!std::getline(in, buffer)) return 1;
//             // std::cout << buffer << "\n"; // NOTE: debug
//             
//             // Create a stream for the current line
//             std::istringstream stringStream(buffer);
//             // NOTE: Using string stream saves doing: lineContent.substr(whitespaceIdx); etc
//             // also, will convert from string to integer automatically
//             
//             // Try to pipe the line into the two line contents variables
//             if (!(stringStream >> liveTime >> realTime)) return 1;
//             // NOTE: Expecting: <livetime> <truetime>
//             
//             // NOTE: The ">>" operator attempts to read data from the stream and parse it into
//             // the variable. The expression "stream >> variable" returns a reference to the stream
//             // itself. When placed inside an if statement, the stream is automatically evaluated as 
//             // a boolean, returning true if read was successful or false if it failed.       
//             
//             // Debug
//             std::cout << "\nLive Time: " << liveTime << " Real Time: " << realTime << "\n";
//         }
//         
//         // This header marks the start of the data entries
//         // NOTE: The next line will contain lower/upper channel numbers,
//         // and then the line after that will be the channel 0 value
//         else if (buffer == "$DATA:") {
//             // Go to next line, read it into the buffer
//             if (!std::getline(in, buffer)) return 1;
//             // std::cout << buffer << "\n"; // NOTE: debug
//             
//             // Create a stream for the current line
//             std::istringstream stringStream(buffer);
//             
//             // Expecting: <lower channel number> <upper channel number>
//             if (!(stringStream >> start >> end)) return 1;
//             
//             // Debug
//             std::cout << "\nStart: " << start << " End: " << end << "\n";
//             
//             // Enable reading data flag
//             readingData = true;
//         }
//         
//         // Only parse data values (i.e. next 2048 lines for 2048 channels)
//         while (readingData && std::getline(in, buffer) && (start <= end)) {
//             // std::cout << buffer << "\n"; // NOTE: debug
//             
//             // Convert string to integer (NOTE: Strips leading whitespace)
//             unsigned int const lineValue = stoi(buffer);
//             
//             // Set current bin to the integer value on current line
//             hpx->SetBinContent(start, lineValue);
//             // NOTE: Dont use h->Fill(converted), Instead of filling bin 0 with line 0,
//             // its filling bin 0 every time 0 is encountered
//             
//             // Increment bin counter
//             start++;
//         }
// 
//         // Stop reading file once data has been parsed
//         if ((end != 0) && (start > end)) {
//             // std::cout << buffer << "\n"; // NOTE: Debug, should == "$ROI:"
//             break;
//         }
//     }
//     
//     // Detach histogram from input file, then close input file
//     hpx->SetDirectory(nullptr);
//     in.close();
//     in.clear();
//     
//     // No errors, all good
//     return 0;
// }

/*
 * Iterate through ASCII file, populating histogram with per-bin values
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
 * 3) Read next 2048 lines, i.e.:
 * if (start > end) break;
 */
// int fill_hist_ascii() {
//     // Handle missing input file
//     if (!in.is_open()) {
//         std::cerr << "\nError: No infile to read!\n";
//         return 1;
//     }
//     
//     // Handle missing histogram
//     if (!hpx) {
//         std::cerr << "\nError (fill_hist()): Histogram not found!\n";
//         return 1;
//     }
// 
//     // Buffer to store lines from the infile
//     std::string buffer;
//     
//     // Will be assigned the live and real time of the detector
//     unsigned int liveTime = 0;
//     unsigned int realTime = 0;
//     
//     // Will be assigned start and end channel numbers (i.e., 0 and 2047)
//     unsigned int start = 0;
//     unsigned int end = 0;
//     // NOTE: Start will be incremented up to max channel number, i.e. 2047
//     
//     // State flags
//     bool readingData = false;
//     
//     // Get line reads a line from input stream into a string, until end of stream encountered
//     // NOTE: Stores characters from current line of infile in the buffer, until "\n" is encountered
//     // NOTE: Contents of buffer are erased at the start of next line before reading commences again
//     // NOTE: The "\n" at the end of the line is not stored in the buffer
//     while (std::getline(in, buffer)) {
//         // Print line number (NOTE: debug)
//         // std::cout << nlines << "\n";
//         
//         // Print line contents (NOTE: debug)
//         // std::cout << buffer << "\n";
//         
//         // Remove trailing carriage return (lines contain hidden carriage return: "$MEAS_TIM:\r")
//         if (!buffer.empty() && (buffer.back() == '\r')) buffer.pop_back();
//         // NOTE: Single quote for single character delimiter, double quotes for string of chars
//         
//         // if (lineContent == "$MEAS_TIM:") {
//         if (buffer == "$MEAS_TIM:") {
//             // Go to next line, read it into the buffer
//             if (!std::getline(in, buffer)) return 1;
//             // std::cout << buffer << "\n"; // NOTE: debug
//             
//             // Create a stream for the current line
//             std::istringstream stringStream(buffer);
//             // NOTE: Using string stream saves doing: lineContent.substr(whitespaceIdx); etc
//             // also, will convert from string to integer automatically
//             
//             // Try to pipe the line into the two line contents variables
//             if (!(stringStream >> liveTime >> realTime)) return 1;
//             // NOTE: Expecting: <livetime> <truetime>
//             
//             // NOTE: The ">>" operator attempts to read data from the stream and parse it into
//             // the variable. The expression "stream >> variable" returns a reference to the stream
//             // itself. When placed inside an if statement, the stream is automatically evaluated as 
//             // a boolean, returning true if read was successful or false if it failed.       
//             
//             // Debug
//             std::cout << "\nLive Time: " << liveTime << " Real Time: " << realTime << "\n";
//         }
//         
//         // This header marks the start of the data entries
//         // NOTE: The next line will contain lower/upper channel numbers,
//         // and then the line after that will be the channel 0 value
//         else if (buffer == "$DATA:") {
//             // Go to next line, read it into the buffer
//             if (!std::getline(in, buffer)) return 1;
//             // std::cout << buffer << "\n"; // NOTE: debug
//             
//             // Create a stream for the current line
//             std::istringstream stringStream(buffer);
//             
//             // Expecting: <lower channel number> <upper channel number>
//             if (!(stringStream >> start >> end)) return 1;
//             
//             // Debug
//             std::cout << "\nStart: " << start << " End: " << end << "\n";
//             
//             // Enable reading data flag
//             readingData = true;
//         }
//         
//         // Only try to parse once $DATA header encountered
//         if (!readingData) continue;
//         
//         // ...
//         unsigned int dataValue;
// 
//         // Only parse data values (i.e. next 2048 lines for 2048 channels)
//         for (int i = start; i <= end; i++) {
//             // std::cout << buffer << "\n"; // NOTE: debug
//             
//             // ...
//             if (!std::getline(in, buffer)) {
//                 std::cerr << "\nError: Failed to read line\n";
//                 return 1;
//             }
//             
//             // Create a stream for the current line
//             std::istringstream dataStream(buffer);
//             
//             // Convert string to integer (NOTE: Strips leading whitespace)
//             if (!(dataStream >> dataValue)) {
//                 std::cerr << "\nError: Failed to pipe data\n";
//                 return 1;
//             }
//             
//             // Set current bin to the integer value on current line
//             hpx->SetBinContent(i, dataValue);
//             // NOTE: Dont use h->Fill(converted), Instead of filling bin 0 with line 0,
//             // its filling bin 0 every time 0 is encountered
//             
//             // Mark parsing as complete
//             if (i == end) readingData = false;
//         }
//         
//         // Stop reading file once data has been parsed (NOTE: Dont technically need bool check here)
//         if (!readingData) {
//             // if (!std::getline(in, buffer)) return 1; // NOTE: Line still = channel 2048 after loop
//             // std::cout << buffer << "\n"; // NOTE: Debug, should == "$ROI:"
//             break;
//         }
//     }
//     
//     // Detach histogram from input file, then close input file
//     hpx->SetDirectory(nullptr);
//     in.close();
//     in.clear();
//     
//     // No errors, all good
//     return 0;
// }


// /*
//  * Iterate through ASCII file, populating histogram with per-bin values
//  * 
//  * NOTE: If "$MEAS_TIM:" is encountered:
//  * 1) Read the next line for detector times
//  * 300 307
//  * 2) Set
//  * liveTime = 300;
//  * realTime = 307;
//  * 
//  * Parses the infile header for start, $DATA, then skip next line, then the following line is bin 0
//  * 
//  * NOTE: If "$DATA:" is encountered:
//  * 1) Read the next line for min/max channel numbers, i.e.:
//  * 0 2047
//  * 2) Parse the channel numbers and assign them to variables, i.e.:
//  * start = 0;
//  * end = 2047;
//  * 3) Read next 2048 lines, i.e.:
//  * if (start > end) break;
//  */
// int fill_hist_ascii() {
//     // Handle missing input file
//     if (!in.is_open()) {
//         std::cerr << "\nError: No infile to read!\n";
//         return 1;
//     }
//     
//     // Handle missing histogram
//     if (!hpx) {
//         std::cerr << "\nError (fill_hist()): Histogram not found!\n";
//         return 1;
//     }
// 
//     // Buffer to store lines from the infile
//     std::string buffer;
//     
//     // Will be assigned the live and real time of the detector
//     unsigned int liveTime = 0;
//     unsigned int realTime = 0;
//     
//     // Will be assigned start and end channel numbers (i.e., 0 and 2047)
//     unsigned int start = 0;
//     unsigned int end = 0;
//     // NOTE: Start will be incremented up to max channel number, i.e. 2047
//     
//     // State flags
//     bool readingData = false;
//     
//     // Get line reads a line from input stream into a string, until end of stream encountered
//     // NOTE: Stores characters from current line of infile in the buffer, until "\n" is encountered
//     // NOTE: Contents of buffer are erased at the start of next line before reading commences again
//     // NOTE: The "\n" at the end of the line is not stored in the buffer
//     while (std::getline(in, buffer)) {
//         // Print line number (NOTE: debug)
//         // std::cout << nlines << "\n";
//         
//         // Print line contents (NOTE: debug)
//         // std::cout << buffer << "\n";
//         
//         // Remove trailing carriage return (lines contain hidden carriage return: "$MEAS_TIM:\r")
//         if (!buffer.empty() && (buffer.back() == '\r')) buffer.pop_back();
//         // NOTE: Single quote for single character delimiter, double quotes for string of chars
//         
//         // if (lineContent == "$MEAS_TIM:") {
//         if (buffer == "$MEAS_TIM:") {
//             // Go to next line, read it into the buffer
//             if (!std::getline(in, buffer)) return 1;
//             // std::cout << buffer << "\n"; // NOTE: debug
//             
//             // Create a stream for the current line
//             std::istringstream stringStream(buffer);
//             // NOTE: Using string stream saves doing: lineContent.substr(whitespaceIdx); etc
//             // also, will convert from string to integer automatically
//             
//             // Try to pipe the line into the two line contents variables
//             if (!(stringStream >> liveTime >> realTime)) return 1;
//             // NOTE: Expecting: <livetime> <truetime>
//             
//             // NOTE: The ">>" operator attempts to read data from the stream and parse it into
//             // the variable. The expression "stream >> variable" returns a reference to the stream
//             // itself. When placed inside an if statement, the stream is automatically evaluated as 
//             // a boolean, returning true if read was successful or false if it failed.       
//             
//             // Debug
//             std::cout << "\nLive Time: " << liveTime << " Real Time: " << realTime << "\n";
//         }
//         
//         // This header marks the start of the data entries
//         // NOTE: The next line will contain lower/upper channel numbers,
//         // and then the line after that will be the channel 0 value
//         else if (buffer == "$DATA:") {
//             // Go to next line, try to read it into the buffer
//             if (!std::getline(in, buffer)) return 1;
//             // std::cout << buffer << "\n"; // NOTE: debug
//             
//             // Create a stream for the current line
//             std::istringstream stringStream(buffer);
//             
//             // Expecting: <lower channel number> <upper channel number>
//             if (!(stringStream >> start >> end)) return 1;
//             
//             // Debug
//             std::cout << "\nStart: " << start << " End: " << end << "\n";
//             
//             // Only try to parse once $DATA header encountered
//             unsigned int dataValue;
// 
//             // Only parse data values (i.e. next 2048 lines for 2048 channels)
//             for (int i = start; i <= end; i++) {
//                 // std::cout << buffer << "\n"; // NOTE: debug
//                 
//                 // Go to next line, try to read it into the buffer
//                 if (!std::getline(in, buffer)) {
//                     std::cerr << "\nError: Failed to read line\n";
//                     return 1;
//                 }
//                 
//                 // Create a stream for the current line
//                 std::istringstream dataStream(buffer);
//                 
//                 // Convert string to integer
//                 if (!(dataStream >> dataValue)) {
//                     std::cerr << "\nError: Failed to pipe data\n";
//                     return 1;
//                 }
//                 // NOTE: Strips leading whitespace, which is desirable as data entries have format:
//                 // "       0", "    8010", etc
//                 // NOTE: std::to_chars doesnt strip leading whitespace, otherwise it would be preferred
//                 
//                 // Set current bin to the integer value on current line
//                 hpx->SetBinContent(i + 1, dataValue);
//                 // NOTE: Dont use h->Fill(converted), Instead of filling bin 0 with line 0,
//                 // its filling bin 0 every time 0 is encountered
//                 
//                 // NOTE: When instantiating a ROOT histogram with nbins = 2048, there are actually
//                 // 2050 bins created, with bin 0 being underflow, and bin 2050 being overflow, so we
//                 // want to increment i by 1 when writing to bins
//             }
//         }
//         
//         // NOTE: Instead of checking for data reading being complete and exiting,
//         // could add $ROI, $PRESETS, etc checks down here
//     }
//     
//     // Detach histogram from input file, then close input file
//     hpx->SetDirectory(nullptr);
//     in.close();
//     in.clear();
//     
//     // No errors, all good
//     return 0;
// }

/*
 * Iterate through ASCII file, populating histogram with per-bin values
 * 
 * NOTE: Assumes parse_headers() has been called first, so that next call to getline()
 * returns channel 0 data
 * 
 * TODO: May want a separate close_ascii() method, and call it from the top level,
 * rather than calling it inside of here (separation of responsibilities)
 */
int fill_hist_ascii(unsigned int const& start, unsigned int const& end) {
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
    unsigned int dataValue;

    // Only parse data values (i.e. next 2048 lines for 2048 channels)
    // Increment from start up to max channel number, i.e. 2047
    for (unsigned int i = start; i <= end; i++) {
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
    
    // TEST ...
    // Attempt to parse detector times and channel numbers from the BSA-style blocks
    SpeMetaData metaData;
    int const headerError = parse_headers(metaData);
    
    if (headerError) {
        std::cerr << "\nAborting: Header parsing error!\n";
        return 1;
    }
    // TEST ...
    
    // Attempt to instantiate histogram object
    int const histError = create_hist((metaData.end - metaData.start + 1), metaData.start, (metaData.end + 1));
    
    if (histError) {
        std::cerr << "\nAborting: Create hist error!\n";
        return 1;
    }
    
    // Attempt to populate histogram from ASCII file
    int const fillError = fill_hist_ascii(metaData.start, metaData.end);
    
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
