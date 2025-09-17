#include "TCanvas.h"
#include "TH1.h"

#include "Riostream.h" // ifstream

// TODO: Replaces existing TH1


// Load in get_path, and render_histo
int ascii_canvas() {
    // fin
    return 0;
}

// Validate file can be opened, create canvas, read input ASCII file, plot histogram
// TODO: Probably wanna validate is expected ASCII format too
int draw_histo(std::string path) {
    // Input file stream
    std::ifstream in;
    
    // Debug
    // in.open("/home/user/Maestro/NaI/NaI_3inch_1000s_readings/22Na_NaI_800v_100coarse_3cm.Spe");
    // in.open("~/Maestro/NaI/NaI_3inch_1000s_readings/22Na_NaI_800v_100coarse_3cm.Spe");
    // NOTE: Cant pass ~ to in.open(), must replace it in string
    
    // Open the ASCII file with validated .Spe extension
    in.open(path);
    
    // Ensure file was found, exit with error if its not
    // NOTE: No need to reprompt, user can just call the function again
    if (!in.is_open()) {
        // Error message
        std::cout << "Error: File not found." << std::endl;
        
        // Error
        return 1;
    };
    
    // Canvas args
    Int_t winX = 0; // Top left of screen
    Int_t winY = 0; // Top left of screen
    Int_t width = 1200;
    Int_t height = 800;
    
    // Create a canvas display
    auto c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    
    // Create a histogram (TH1I = integer, dont need float TH1F, or double TH1D - channel/counts both ints)
    auto h = new TH1I(
        "h", // Legend title
        "Spectrum", // Histo title
        2048, // num bins
        0, // x low
        2048 // x up
    );
    
    // X-axis title
    h->SetXTitle("Channels");
    
    // Line counter
    int nLines = 0;

    // ...
    std::string line;
    
    // Histo bin counter
    int nBins = 0;
    
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
        nLines++;
        
        // Print 
        std::istringstream iss(line);
        std::string a; // NOTE: int a; does not work seemingly, need to convert from string to int
        
        // ...
        if (!(iss >> a)) break;
        
        // only lines 13-2060
        if (nLines >= 13 && nLines <= 2060) {
            // std::cout << a << std::endl;
            // std::cout << stoi(a) << std::endl;
                            
            // NOTE: Instead of filling bin 0 with line 0
            // its filling bin 0 every time 0 is encountered
            // h->Fill(stoi(a));
            
            // Set current bin to the integer value on current line
            h->SetBinContent(nBins, stoi(a));
            
            // Increment bin counter
            nBins++;
        }
    }
    
    // Close input file
    in.close();   
    
    // Render the specified values
    h->Draw();
    
    // fin
    return 0;
}

// Takes path as arg, validates string is valid, updates it if needed, calls draw_histo
// TODO: Doesnt handle references to current dir, i.e. "../xyz.Spe", as delimiter is found at start
// NOTE: Just check if first char is ".", go until "/" is found, repeat until no "." at start, continue with delimiter
int get_path(std::string path) {
    // Print path to stdout
    std::cout << path << std::endl;
    
    // Check if string is empty (returns true if string is empty)
    if (path.empty()) {
        // Error message
        std::cout << "Error: Empty string." << std::endl;
        
        // Error value
        return 1;
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
        std::cout << "Error: No file extension." << std::endl;
        
        // Error value
        return 1;
    }
    
    // Get substring from index of delimiter to the end of the string
    auto token = path.substr(delimiterIdx, path.size());
    // NOTE: 0, delimiterIdx would get everything prior to delimiter
    
    // Debug
    // std::cout << token << std::endl;
    
    // Check file ends with ".Spe", reject invalid file type
    if (token != ".Spe") {
        // Write to stdout
        std::cout << "Error: Invalid extension." << std::endl;
        
        // Error value
        return 1;
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
    std::string tilde = "~";

    // If reference to character at [0] is tilde character
    if (path[0] == tilde) {
        // Get the home path (~) from the environment variable
        const char* home = getenv("HOME");
        // std::cout << home << std::endl; // debug
        
        // Trim "~" from the start of the string (start at idx = 1, as "~" at 0)
        auto trimmedPath = path.substr(1, path.size());
        // std::cout << trimmedPath << std::endl; // debug
        
        // Update the path, replacing "~" with "/home/user" (NOTE: Not sure if this is "okay" to do), but is simple
        path = home + trimmedPath;
        std::cout << path << std::endl;
    }
    
    // Pass path to canvas and histo renderer
    draw_histo(path);
    
    // fin
    return 0;
}
