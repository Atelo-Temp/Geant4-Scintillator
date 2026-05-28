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

#include <TKey.h>

// C lib
#include <fstream>
#include <sstream>
// #include <stdexcept>
// #include <unordered_set>

// Global root object variables
std::ifstream inASCII;
TFile* inROOT = nullptr;
TTree* nTuple = nullptr;
TH1 *hpx = nullptr;
TCanvas *c = nullptr;

// Accepted file types
enum class FileType {
    ROOT,
    ASCII
};

// Active file type flag
FileType fileType;


// TODO: Remove undefined behaviour integers: int x; // Set them equal to default val
// NOTE: undefined string = "", so not as much of an issue

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int omni_plot() {
    // Usage
    std::cout << "\n-----------------------------------------------------------------------\n";
    std::cout << "\nConvert ASCII & ROOT Ntuples to Root Histogram.\n\nTo get started, call: plot(\"path.ext\"), passing path to ASCII (.Spe) or ROOT (.root) file as param.\n";
    std::cout << "\nTo save the plotted histogram, call save(\"dir/out.root\").\n";
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
 * TODO: Make arg[0]: std::string const path (so its immutable)
 */
std::string check_path(std::string path) { // std::string check_path(std::string const path) {
    // Print path to stdout
    std::cout << "\nUser provided path:\n";
    std::cout << path << "\n";
    
    // Check if string is empty (returns true if string is empty)
    if (path.empty()) {
        // Error message
        std::cout << "Error [check_path()]: Empty string.\n";
        
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
        std::cout << "Error: No file extension.\n";
        
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
    if (token != spe && token != root) {
        // Write to stdout
        std::cout << "Error: Invalid extension.\n";
        
        // Error value
        return "";
    }
    // If its an ASCII file extension, log it and set ASCII flag
    else if (token == spe) {
        // std::cout << "\nASCII file detected.\n\n";
        std::cout << "\nASCII file detected.\n";
        fileType = FileType::ASCII;
    }
    // If its a ROOT file extension, log it and set ROOT flag
    else if (token == root) {
        // std::cout << "\nRoot file detected.\n\n";
        std::cout << "\nRoot file detected.\n";
        fileType = FileType::ROOT;
    }

    // Replace tilde if passed
    std::string const tilde = "~"; // char tilde[2]

    // If reference to character at [0] is tilde character
    if (path[0] == tilde) {
        // Get the home path (~) from the environment variable
        char const* home = getenv("HOME");
        // std::cout << home << std::endl; // debug
        
        // Trim "~" from the start of the string (start at idx = 1, as "~" at 0)
        auto trimmedPath = path.substr(1, path.size());
        // std::cout << trimmedPath << std::endl; // debug
        
        // Update the path, replacing "~" with "/home/user" (NOTE: Not sure if this is "okay" to do), but is simple
        path = home + trimmedPath; 
        // TODO: Not sure about updating path var directly, maybe just return new string here
        
        // ...
        // std::string expandedPath = home + trimmedPath; // TODO
        
        // Tilde expansion was successful
        std::cout << "\nPath has been expanded:\n";
        std::cout << path << "\n";
        // NOTE: This is now an absolute path
        
        // return expandedPath; // TODO
    }
    
    // Confirmation status
    std::cout << "\nPath is valid.\n";
    
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
 * Prompts user for input, parses response, validates it, returns valid response as integer
 * 
 * TODO: "0001" will pass
 * also "    1" will pass
 * 
 * TODO: The stdout from this function is pretty messy, perhaps use other input function line clear methodology
 * 
 * TODO: This could be if (converted >= low && converted <= high) { return converted; }
 * then the std::cout could be taken outside of enclosure
 */
int prompt_user_int(int low, int high) {
    // Prompt user for input
    // std::cout << "[int]: ";
    
    // Store user input
    std::string userInput; // == ""
    
    // NOTE: Could bring converted out here, and break on passing checks, then return at end of function
    
    while (true) {
        // ...
        std::cout << "[int]: ";
        
        // Capture the line;
        std::getline(std::cin, userInput);
        
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
            
            // ...
            // converted = std::stoi(userInput); // NOTE: If this fails, converted = -1 still
            converted = std::stoi(userInput, &numChars);
            
            if (numChars != userInput.size()) throw std::invalid_argument("Trailing characters.");
        }
        // ..
        catch (const std::invalid_argument& e) {
            if (strcmp(e.what(), "stoi")) std::cerr << e.what() << std::endl; // print trailing chars message, but not "stoi" default error msg
            std::cerr << "Invalid argument!\n" << std::endl;
            continue;
        }
        // ..
        catch (const std::out_of_range& e) {
            std::cerr << "Out of range\n" << std::endl;
            continue;
        }
        
        // ...
        if ((converted > high) || (converted < low)) {
            std::cout << "Please select an integer between: " << low << " and " << high << std::endl;
        } else {
            // ...
            return converted;
        }
    }
}

/*
 * Open ROOT file containing ROOT objects, load it into local memory, check its not empty 
 * (i.e., contains at least one ROOT object)
 */
int load_root_file(std::string path) {
    // Convert: std::string, to: char const*
    char const *charPath = path.c_str();
    
    // Fetch and open root file
    inROOT = TFile::Open(charPath);
    
    // Handle incorrect path
    if (!inROOT) {
        printf("\nError: ROOT file not found!\n");
        return 1;
    }
    
    // Handle no ROOT objects being found (i.e. TTree, TH1D, etc)
    if (inROOT->GetNkeys() == 0) {
        std::cerr << "\nError: ROOT file is empty, closing file.\n";
        inROOT->Close();
        return 1;
    }
    
    // Success message
    std::cout << "\nROOT file has been loaded into memory.\n";
    
    // No errors, all good
    return 0;
}

/*
 * TTree (Ntuple), TH1D (1D Histogram)
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
 * TODO: May have to do another map, which stores:
 * key: TTree, TH1D, etc
 * val: std::vector<char const*> (containing: entry->GetName()) (EventData, TrackData, etc)
 * 
 * ^^^^^^^^ populate it during this first loop, rather than doing another loop
 * 
 * TODO: Will need to append GetName() with ";1" to match tree name
 * NOTE: Histos dont have branches
 */
int select_root_type() {
    // Handle unloaded root file (i.e., via calling this method directly, or some bug)
    if (!inROOT) {
        printf("\nError: File not found!\n");
        return 1;
    }
    
    // inROOT->ls();
    // std::cout << inROOT->FindKey("TTree") << std::endl;
    // auto y = inROOT->FindKey("TTree");
    
    // ...
    TList* entries = inROOT->GetListOfKeys();
    
    // ...
    if (!entries) {
        std::cerr << "\nError: Failed to get ROOT file keys.\n";
        return 1;
    }
    
    // std::cout << "LAST INDEX: " << entries->LastIndex() << std::endl; // 5
    // std::cout << "SIZE: " << entries->GetSize() << std::endl; // 6
    
    // inROOT->GetFileCounter();
    
    // std::cout << "\nPlease enter a number from the options below:\n";
    std::cout << "\nWhat object type would you like to access? (type a number from the options below):\n";
    
    // TList* entries = inROOT->GetList(); // NOTE: This doesnt work with loop below, pretty sure you have to do address thing like in branch iteration
    
    // ...
    // std::unordered_set<std::string> objectTypes = {}; // NOTE: will automatically create std::string copy of const char*
    std::unordered_set<std::string_view> objectTypes = {};
    
    // ...
    std::unordered_map<int, std::string> objectMap = {};
    
    // ...
    for (int i = 0; i < entries->GetSize(); i++) {
        // ...
        TObject* entry = entries->At(i);
        // std::cout << i << ": " << entry->GetName() << std::endl; // NOTE: Prints the name of each object (EventData, TrackData, etc)
        // TODO: STORE THESE NAMES IN A MAP UNDER KEY OF TTree, TH1D, etc
        
        // ..
        // auto x = entry->IsA(); // Returns TClass*
        // std::cout << "CLASSNAME: " << x->ClassName()  << std::endl; // NOTE: Prints "TClass"
        
        // entry->InheritsFrom(TTree::Class());
        
        // if (entry->IsA() == TTree::Class()) {
        // if (entry->InheritsFrom(TTree::Class())) {
//         if (entry->IsA()->InheritsFrom(TTree::Class())) {
//             std::cout << "IS TTREE\n";
//         }
//         else if (entry->IsA() == TH1::Class()) {
//             std::cout << "IS TH1\n";
//         }
//         
        // std::cout << "CLASSNAME: " << entry->IsA()->GetActualClass(entry)->GetName();
        
        // ...
        // auto key = dynamic_cast<TKey*>(entry);
        auto key = static_cast<TKey*>(entry); 
        // NOTE: Since "GetListOfKeys()" returns TList<TKey*>, we can safely static_cast here,
        // if there was any doubt about it being TKeys, static_cast would not be safe
        
        // std::cout << "CLASSNAME: " << key->GetClassName() << std::endl;
        
        // std::cout << "CLASSNAME: " << entry->IsA()->GetName() << std::endl; // "TKey"
        
        // ..
        // entry->get
        
        // std::cout << "CLASSNAME: " << inROOT->Get(entry->GetName())->ClassName() << std::endl; // NOTE: This works for getting names: TTree, TH1D, but is a little messy
        // inROOT->GetObject(entry->GetName(), pointerToAssignTo); // NOTE: Not good here, requires assignment
        
        // uniqueObjects.push_back(key->GetClassName());
        
        // if (objectTypes.find(key->GetClassName())) objectTypes.insert(key->GetClassName());
        // if (objectTypes.contains(key->GetClassName())) objectTypes.insert(key->GetClassName());
        
        auto name = key->GetClassName();
        // std::cout << "CLASSNAME: " << name << std::endl;
        
        // if (objectTypes.find(name) == objectTypes.end()) { // .find returns key if found, or .end() if not found
        // if (!objectTypes.count(name)) { // returns: not found = 0, found = 1
        //     objectTypes.insert(name);
        //     std::cout << "NEW KEY\n";
        // }
        // else std::cout << "DUPLICATE KEY\n";
        
        std::pair pair = objectTypes.insert(name); // NOTE: Automatically only adds on unique
        // NOTE: insert returns a pair of: iterator & success status
        
        // If success status = true, item was inserted, else status = false
        if (pair.second) {
            const int itemNo = objectTypes.size();
            // std::cout << "NEW KEY: " << name << std::endl;
            std::cout << itemNo << ") " << name << std::endl;
            
            // objectMap.insert(itemNo, name);
            objectMap[itemNo] = name; // NOTE: Inserts if missing, updates if present
        }
    }
    // NOTE: Either just get all names now, and determine what object type is later
    // i.e. print all TTrees and Hists, let user select from those
    
    // std::unique(uniqueObjects);
    
    std::cout << "SET SIZE: " << objectTypes.size() << std::endl;
    std::cout << "MAP SIZE: " << objectMap.size() << std::endl;
    
    int res = prompt_user_int(1, objectMap.size());
    
    std::cout << "USER SELECTED INT: " << res << std::endl;
    
    std::string objectType = objectMap.at(res); // Retrieve the string associate with the integer key
    // NOTE: Equivalent to .get() in typescript
    
    std::cout << "USER SELECTED OBJECT: " << objectType << std::endl;
    
    // Or print all object types, and show list of only those
    
    // TODO: May need to return vector containing object names matching object type
    // just return that and success/fail message, may need std::optional return type 
    // for this function and return std::nullopt on failed error checks above
    return 0;
}

/*
 * Prompt user to select the name of the object theyd like to access 
 * (with selection list filtered by the object type theyve chosen)
 * 
 * TODO: May need to return vector from select root type and pass it into this function from load_root()
 * 
 * NOTE: Can probably reuse prompt_user_int(), just passing in low = 1 and new high value
 */
int select_root_object() {
    return 1;
}

/*
 * TODO: Small function, literally just loading the object into memory
 */
int load_root_object() {
    return 1;
}

/*
 * Print valid branch names for selected TTree object
 * 
 * Have user choose desired branch name (similar to select root type)
 * 
 * NOTE: Can probably reuse prompt_user_int(), just passing in low = 1 and new high value
 */
int select_tree_branch() {
    return 1;
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
 * 
 * ^ could print out tree/branch names line by line with an index, i.e.:
 * 1) Tree A
 * 2) Tree B
 * etc ...
 * 
 * Then just get user to input the integer, rather than typing full name
 */
int load_tree() { // (char const* treeName)
    // Handle incorrect path
    if (!inROOT) {
        printf("Error: File not found!\n");
        return 1;
    }
    
    // TODO: Have this passed as param
    char const treeName[16] = "TrackData;1";
    
    // Get the TTree from the root file and assign it to the TTree pointer
    inROOT->GetObject(treeName, nTuple); // NOTE: Also works: TTree* nTuple = in->Get<TTree>(treeName);
    
    // Handle missing ntuple (incorrect tree name, etc)
    if (!nTuple) {
        printf("Error: Couldnt load TTree!\n");
        return 1;
    }
    
    // TODO: For dynamic hist upper limit setting
    // nbins = nTuple->GetMaximum("Distance");
    // NOTE: would have to store in global variable
    
    std::cout << "\nTree: \"" << treeName << "\" has been loaded into memory.\n";
    
    // No errors, all good
    return 0;
}

/*
 * 
 */
int load_root_hist() {
   return 1; 
}

/*
 * Handles full ROOT file pipeline:
 * 
 * 1) Open ROOT file, load it into local memory, check its not empty
 * 2) Get list of object types, and have user select object type of interest
 * 3) Have user select from list of objects matching said type
 * 4) load object 
 * 
 * TODO: load_tree() (see below) will be called from in here, insead of load_file(), if TTree selected
 * TODO: Load tree will also need to do a branch check, and have user select name similar to tree name
 * NOTE: Dont implement 3D shit just yet, can save that for a later date (will need to select multiple branch names, etc)
 * ^ this should just be flexible enough to handle any Ntuples i create from the sim for now
 * 
 * TODO: Will need helper function for histos too
 */
int load_root(std::string path) {
    int loadError = load_root_file(path);
    
    if (loadError) {
        std::cerr << "\nError: Failed to load root file into memory.\n";
    }
    
    select_root_type(); // choose TTree (Ntuple), TH1D (1D Hist), etc
    
    select_root_object(); // get tree/hist/etc name
    
    load_root_object(); // load root object with said name
    
    // No errors, all good
    return 0;
}

/*
 * Executes ASCII or ROOT file procedures based on file type flag
 * 
 * TODO: Assumes Ntuple in root file
 * 
 * TODO: Check what objects are in the root file 
 * ^ (if there is no histogram, go the Ntuple route)
 * ^ if there is a histogram
 */
int load_file(std::string path) {
    // Success status
    int status;
    
    // Attempt to load the ASCII file into memory
    if (fileType == FileType::ASCII) {
        status = load_ascii(path);
        // std::cout << "\nASCII file has been loaded into memory.\n\n";
        std::cout << "\nASCII file has been loaded into memory.\n";
    }
    // Attempt to load the ROOT file into memory
    else if (fileType == FileType::ROOT) {
        status = load_root(path);
        // std::cout << "\nROOT file has been loaded into memory.\n\n";
        // std::cout << "\nROOT file has been loaded into memory.\n";
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
 * 
 * TODO: Have user specify whether hist params should be automatically calculated,
 * via finding max value from dataset (+say 5-10% for xmax), and either calulating nbins, or leaving at 1024-4096 bins
 * or if theyd like a specific setup (i.e. 2048 channels)
 */
int create_hist() {
    // Histogram args
    // int const nbins = 2048; // 2048 channels (bins)
    // int const xmin = 0; // min channel
    // int const xmax = 2048; // max channel (3500 photons)
    
    // int const nbins = 21; // 2048 channels (bins) // NOTE: Too few
    // int const nbins = 200; // 2048 channels (bins) // NOTE: Too few 
    // int const nbins = 1024; // 2048 channels (bins) // NOTE: Good
    int const nbins = 2048; // 2048 channels (bins) // NOTE: Decent
    // int const nbins = 4096; // 2048 channels (bins) // NOTE: Almost too many (but less compressed than 2048 on log scale)
    int const xmin = 0; // min channel
    int const xmax = 3000; // max channel (3500 photons)
    
    // Create a histogram (TH1I = integer - channel/counts both ints)
    hpx = new TH1I(
        "hpx", // Legend title
        "distance travelled", // Histo title
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
    hpx->SetXTitle("Distance (mm)");
    
    // No errors, all good
    return 0;
}

/*
 * Iterate through tree branch, populating histogram with per-event values
 * 
 * TODO: Split out getting branch and checking its valid, from the actual reading
 * of the branch. I.e.: load_branch(), read_branch()
 * 
 * TODO: Make post-processing optional
 * 
 * TODO: Make it so branchname is actually passed in as param
 * 
 * TODO: Maybe query list of branchnames in tree
 */
int fill_hist_ntuple(char const branchName[16] = "Distance") {
    // char const branchName[16] = "NumPhotons";
    
    // Get the TBranch of interest from the TTree, and assign it to pointer
    TBranch* const eventData = nTuple->GetBranch(branchName);
    
    // Handle invalid branch name
    if (!eventData) {
        std::cerr << "Error: Branch not found, closing root file.\n";
        inROOT->Close();
        return 1;
    }
    
    
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^
    // TODO: SPLIT THIS FN HERE
    // \/\/\/\/\/\/\/\/\/\/\/\/\/
    
    
    // To read a tree, neeed to associate variables with the trees branches
    double entry;
    nTuple->SetBranchAddress(branchName, &entry);
    // NOTE: When loading a tree entry, the tree will set the variables to the branches value as read from the storage

    // Get the number of entries in the branch (i.e., length for iteration limit)
    long long const numEntries = eventData->GetEntries();
    
    // Read all entries in the branch
    for (long long i = 0; i < numEntries; i++) {
        // Load the data for the given tree entry
        eventData->GetEntry(i); // returns bytes read, not the actual val
        // NOTE: The "entry" variable will now be updated
        
        // Add a count to the appropriate bin for that value
        hpx->Fill(entry);
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
 * Router for histogram fill methodology, switches based on input file type
 * 
 * TODO: draw_hist_root() <- From ROOT histogram
 */
int draw_hist() {
    // If hist is not filled by one means or another defaults to error
    int status = 1;
    
    // Switch on file type, set status to 0 if hist was filled successfully
    if (fileType == FileType::ASCII) {
        status = fill_hist_ascii();
    }
    else if (fileType == FileType::ROOT) {
        status = fill_hist_ntuple();
    }
    
    // Success message
    if (status == 0) {
        std::cout << "\nHistogram has been populated.\n\n";
    }
    
    // No errors, all good
    return status;
}

/*
 * Instantiates a canvas object, populating the global variable, then renders a 
 * histogram on the canvas
 * 
 * TODO: Maybe separate out histogram rendering
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
    
    // TODO: \/\/\/\/\/\/\/\/\/ MAYBE EXTRACT THIS OUT TBH (render_hist())
    
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
 * Validate file path, load file into memory, instantiate histogram, fill histogram,
 * instantiate canvas, render histogram
 */
int plot(std::string fileName) {
    // Check provided path is valid (will return empty string if not valid)
    std::string path = check_path(fileName);
    
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
    
    // TODO: Maybe here, if filetype == root, do load Hist, or TTree logic
    // ^ or just do it in load_file
    
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
 * Automatically find exponential centroid, derive hist low/high, fit convolution of 
 * exponential rise and decay to the data, and display the fit
 * 
 * TODO: This didnt quite fit to my data, but it may come in handy at a later date
 * 
 * TODO: Define a top level: fit(), method, which directs you to exponential, double_exponential, etc, etc
 * 
 * TODO: This needs updating with some of the updated error handling from ascii_fit.cc
 */
int rise_and_decay() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
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
    double x_rise = hpx->GetBinCenter(riseBin); // grab center of rise bin
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
    double x_fall = hpx->GetBinCenter(fallBin);
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

    // ...
    auto result = hpx->Fit(pulseFit, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = improves the fit quality
    
    // Draw the fit line (ROOT internally stores the fit function with the histogram after fitting)
    hpx->GetFunction("fitFn")->Draw("SAME");
    // NOTE: The "HIST" option suppresses drawing associated functions (including fits),
    // hence why "hpx->Draw()" works here instead of drawing the fit fn (but we lose the histogram view),
    // and why "hpx->Draw("HIST")" doesnt work alone, so calling draw on the stored fn is the way,
    // it is also not enough to just call Modified() & Update().
    
    printf("Peak X: %f, Peak Y: %f", peakX, peakY);
    
    // TODO: Display relevant fit values in top right info box post-fit (chi^2, centroid, etc)
    
    return 0;
}

/*
 * Automatically find exponential centroid, derive hist low/high, fit convolution of 
 * exponential rise and decay to the data, and display the fit
 * 
 * TODO: This didnt quite fit to my data, but it may come in handy at a later date
 * 
 * TODO: Define a top level: fit(), method, which directs you to exponential, double_exponential, etc, etc
 * 
 * TODO: This needs updating with some of the updated error handling from ascii_fit.cc
 */
int double_exponential_fit() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
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
    double x_rise = hpx->GetBinCenter(riseBin); // grab center of rise bin
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
    double x_fall = hpx->GetBinCenter(fallBin);
    double estimated_tau_fall = x_fall - peakX; // TODO: Comments
    
    if (estimated_tau_fall <= 0) estimated_tau_fall = 10.0; // Fallback safety
    
    // Define and initialise double exponential fit (fast rise component & slow fall component)
    auto pulseFit = new TF1("fitFn", "[0]*(exp(-(x-[1])/[3]) - exp(-(x-[1])/[2]))", low, high);
    // A*(exp(-(x-[x_rise])/[tau_fall]) - exp(-(x-[x_rise])/[tau_rise]))
    // Where: A = amplitude, x_rise = delay time, tau_rise = rise time constant, tau_fall = decay time constant
    
    // ...
    pulseFit->SetParameters(peakY, x_rise, estimated_tau_rise, estimated_tau_fall);
    pulseFit->SetParNames("Amplitude", "t0", "tau_rise", "tau_fall");

    // Enforce limits so the fitter doesnt swap rise and fall constants
    pulseFit->SetParLimits(2, 0.01, estimated_tau_fall);
    pulseFit->SetParLimits(3, estimated_tau_fall * 0.1, estimated_tau_fall * 10.);

    // ...
    auto result = hpx->Fit(pulseFit, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = improves the fit quality
    
    // Draw the fit line (ROOT internally stores the fit function with the histogram after fitting)
    hpx->GetFunction("fitFn")->Draw("SAME");
    // NOTE: The "HIST" option suppresses drawing associated functions (including fits),
    // hence why "hpx->Draw()" works here instead of drawing the fit fn (but we lose the histogram view),
    // and why "hpx->Draw("HIST")" doesnt work alone, so calling draw on the stored fn is the way,
    // it is also not enough to just call Modified() & Update().
    
    printf("Peak X: %f, Peak Y: %f", peakX, peakY);
    
    // TODO: Display relevant fit values in top right info box post-fit (chi^2, centroid, etc)
    
    return 0;
}

/*
 * TODO: Needs faster rise time to fit to the photon distance travelled data
 * 
 * 1) Log normal
 * 2) Gamma Distribution
 * 3) ExGaussian
 */
int fit() {
    // Find the tallest point in the current histogram range
    int const maxBin = hpx->GetMaximumBin();
    double const peakX = hpx->GetXaxis()->GetBinCenter(maxBin); // get the x-axis location of max counts bin
    double const peakY = hpx->GetBinContent(maxBin); // get the y-axis number of counts for max bin, i.e. amplitude
    
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
    double x_rise = hpx->GetBinCenter(riseBin); // grab center of rise bin
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
    double x_fall = hpx->GetBinCenter(fallBin);
    double estimated_tau_fall = x_fall - peakX; // TODO: Comments
    
    if (estimated_tau_fall <= 0) estimated_tau_fall = 10.0; // Fallback safety
    
    
    // Define the fit function
    // auto fitFn = new TF1("fitFn", "expo", low, high);
    // auto fitFn = new TF1("fitFn", "[0]*exp(-x/[1])", low, high); // exponential decay
    // NOTE: "gaus" is built-in ROOT shorthand for [0]*exp(-x/[1])
    
    // ..
    // fitFn->SetParameters(peakY, 20.0);
    // fitFn->SetParNames("Amplitude", "Decay Constant");
    
    
    // Define and initialise double exponential fit (fast rise component & slow fall component)
    auto pulseFit = new TF1("fitFn", "[0]*(exp(-(x-[1])/[3]) - exp(-(x-[1])/[2]))", low, high);
    
    // ...
    pulseFit->SetParameters(peakY, x_rise, estimated_tau_rise, estimated_tau_fall);
    pulseFit->SetParNames("Amplitude", "t0", "tau_rise", "tau_fall");

    // Enforce limits so the fitter doesnt swap rise and fall constants
    pulseFit->SetParLimits(2, 0.01, estimated_tau_fall);
    pulseFit->SetParLimits(3, estimated_tau_fall * 0.1, estimated_tau_fall * 10.);

    // ...
    auto result = hpx->Fit(pulseFit, "RS");
    // "R" = use the range of the function
    // "S" = return a TFitResultPtr for further analysis
    // "M" = improves the fit quality
    
    // Draw the fit line (ROOT internally stores the fit function with the histogram after fitting)
    hpx->GetFunction("fitFn")->Draw("SAME");
    // NOTE: The "HIST" option suppresses drawing associated functions (including fits),
    // hence why "hpx->Draw()" works here instead of drawing the fit fn (but we lose the histogram view),
    // and why "hpx->Draw("HIST")" doesnt work alone, so calling draw on the stored fn is the way,
    // it is also not enough to just call Modified() & Update().
    
    printf("Peak X: %f, Peak Y: %f", peakX, peakY);
    
    // TODO: Display relevant fit values in top right info box post-fit (chi^2, centroid, etc)
    
    return 0;
}


/*
 * Prompts user for input, reads response, returns success/fail val based on response
 */
int prompt_user(std::string const &question = "Do you wish to overwrite existing file?") {
    // Prompt user for input
    std::cout << "\n" << question << "\n";
    std::cout << "[y/n]: ";
    
    // Store user input
    std::string userInput;
    
    // Enter user input loop
    while (true) {
        // Capture the line
        std::getline(std::cin, userInput);
        // NOTE: std::cin >> userInput;
        
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
 */
int check_file(char const* path) {
    // Attempt to open file with provided filename
    auto temp = TFile::Open(path, "READ"); // NOTE: Read only mode
    
    // If file already exists and was opened
    if (temp && temp->IsOpen()) { // if (temp && !temp->IsZombie()) {
        // printf("\nError [check_file()]: File already exists!\n");
        std::cerr << "\nError [check_file()]: File already exists!\n";
        
        // Get user response
        int abort = prompt_user(); // NOTE: 0 = overwrite, 1 = abort
        
        // Close the readonly file
        temp->Close();
        
        if (!abort) printf("\nOverwriting existing file...\n");
        
        return abort;
    }
    
    // If the file does not already exist (!temp & !temp->IsOpen()) 
    return 0;
}

/*
 * TODO: This would currently accept a .Spe outfile extension, wanna limit it to .root really
 * 
 * TODO: Maybe excessive but check if outfile closed after .Close()
 */
int save(std::string path) {
    // Check provided path is valid (will return empty string if not valid)
    std::string validPath = check_path(path);
    
    if (validPath.empty()) {
        printf("\nError {save()}: Invalid path error!\n");
        return 1;
    }
    
    // Convert from: std::string, to: const char*
    auto convertedPath = validPath.c_str();
    
    // Check if file already exists, and if so, whether to overwrite
    int invalidPath = check_file(convertedPath);
    
    if (invalidPath) {
        printf("\nAborting: Please call save() with a new path.\n");
        return 1;
    }
    
    // Open outfile in recreate mode (creates ROOT file, replacing it if it already exists)
    auto outfile = TFile::Open(convertedPath, "RECREATE");
    
    // Handle incorrect path
    if (!outfile->IsOpen()) {
        printf("\nError [save()]: Couldnt create/open outfile!\n");
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        printf("\nError (save()): Histogram not found!\n");
        return 1;
    }
    
    // Write the histogram object to the root file
    outfile->WriteObject(hpx, "Spectrum");
    
    // All done
    outfile->Close();
    
    // Confirmation status
    printf("\nFile has been saved.\n");
    
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
