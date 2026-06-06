// Load an ASCII (.Spe) or ROOT (.root) file into memory, fill a histogram, and display it on a canvas

// ROOT lib
#include <TDirectory.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TLeaf.h> // Incomplete type without explicit import
#include <TH1.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TROOT.h> // TODO: I think this is unused ?
#include <TStyle.h> // gStyle
// #include <TFitResult.h> // NOTE: Fitting libs \/\/
// #include <TPaveStats.h>
// #include <TLatex.h> // ^^^^^^^
#include <TRandom.h> // gRandom

// C lib
#include <fstream>
#include <sstream>
#include <optional>
// #include <unordered_set>
// #include <unordered_map>
// #include <variant>

// Global root object variables
std::ifstream inASCII;
TFile* inROOT = nullptr;
TTree* nTuple = nullptr;
TBranch* branch = nullptr;
TLeaf* leaf = nullptr;
TH1 *hpx = nullptr;
TCanvas *canvas = nullptr;

// Accepted file types
enum class FileType {
    ROOT,
    ASCII,
    NULLFILE
};

// Active file type flag
FileType fileType = FileType::NULLFILE;

// Currently supported object types
enum class RootObjectType {
    TTree,
    TH1D,
    NULLOBJ
};

// Active object type flag
RootObjectType rootObjectType = RootObjectType::NULLOBJ;

/*
 * Load in plotting and fitting functions
 * 
 * NOTE: Executes automatically on script start (shares name with the macro file)
 * NOTE: Choose another function name if you wish to manually call it instead
 */
int omni_plot() {
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
    if (token != spe && token != root) {
        // Write to stdout
        std::cerr << "\nError: Invalid extension.\n";
        
        // Error value
        return "";
    }
    // If its an ASCII file extension, log it and set ASCII flag
    else if (token == spe) {
        std::cout << "\nASCII file detected.\n";
        fileType = FileType::ASCII;
    }
    // If its a ROOT file extension, log it and set ROOT flag
    else if (token == root) {
        std::cout << "\nROOT file detected.\n";
        fileType = FileType::ROOT;
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
 * To be called on error
 * 
 * NOTE: Since the TBranch is owned by the TTree, and the TTree is owned by the TFile
 * (assuming TTree->SetDirectory(nullptr) hasnt been called), it is not necessary to
 * manually call "delete" on the TBranch or TTree, 
 * 
 * NOTE: Calling TTree->SetDirector(nullptr) is unwise
 * 
 * NOTE: Closing the TFile
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
    
    // TEST: Segfault
    // delete branch;
    // branch = nullptr;
    // delete nTuple;
    // nTuple = nullptr;
    // delete inROOT;
    // inROOT = nullptr;
    
    // TEST: Segfault
    // delete nTuple;
    // nTuple = nullptr;
    // delete branch;
    // branch = nullptr;
    // delete inROOT;
    // inROOT = nullptr;
    
    // TEST: Segfault
    // delete inROOT;
    // inROOT = nullptr;
    // delete nTuple;
    // nTuple = nullptr;
    // delete branch;
    // branch = nullptr;
    
    // TEST: Segfault
    // delete inROOT;
    // inROOT = nullptr;
    // delete branch;
    // branch = nullptr;
    // delete nTuple;
    // nTuple = nullptr;
    
    // TEST: No segfault
    // delete branch;
    // branch = nullptr;
    
    // TEST: No segfault
    // delete nTuple;
    // nTuple = nullptr;
    
    // TEST: No segfault
    // delete inROOT;
    // inROOT = nullptr;
    
    // TEST: No segfault
    // branch = nullptr;
    // delete nTuple;
    // nTuple = nullptr;
    // delete inROOT;
    // inROOT = nullptr;
    
    // TEST: No segfault
    // branch = nullptr;
    // delete branch;
    // delete inROOT;
    // inROOT = nullptr;
    
    // TEST: No segfault
    // delete branch;
    // branch = nullptr;
    // delete inROOT;
    // inROOT = nullptr;
    
    // TEST: No segfault
    // delete inROOT;
    // inROOT = nullptr;
    // delete branch;
    // branch = nullptr;
    
    // TEST: No segfault
    // delete inROOT;
    // inROOT = nullptr;
    // delete nTuple;
    // nTuple = nullptr;
    
    // TEST: No segfault
    // delete nTuple;
    // nTuple = nullptr;
    // delete inROOT;
    // inROOT = nullptr;
    
    // TEST: Segfault
    // delete nTuple;
    // nTuple = nullptr;
    // delete branch;
    // branch = nullptr;
    
    // TEST: Segfault
    // delete branch;
    // branch = nullptr;
    // delete nTuple;
    // nTuple = nullptr;
}

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
 * Validate .Spe file can be opened/exists, load it into local memory if so
 * 
 * TODO: Probably wanna validate is expected ASCII format too:
 * line 1: $SPEC_ID:
 * 
 * NOTE: std::ifstream sets internal error flags immediately on failure,
 * so only need to check (!inASCII) really
 */
int load_ascii(std::string const& path) {
    // Open the ASCII file with validated .Spe extension
    inASCII.open(path);
    
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
 */
int prompt_user_int(int low, int high) {
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
            // if (userInput == "q") gApplication->Terminate(0); // TODO: exit also ends root session
            
            // ...
            // converted = std::stoi(userInput); // NOTE: If this fails, converted = -1 still
            converted = std::stoi(userInput, &numChars);
            
            // ....
            if (numChars != userInput.size()) throw std::invalid_argument("Trailing characters.");
        }
        // ..
        catch (const std::invalid_argument& e) {
            if (strcmp(e.what(), "stoi")) std::cerr << e.what() << std::endl; // print trailing chars message, but not "stoi" default error msg
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
            std::cout << "Please select an integer between: " << low << " and " << high << std::endl;
        } else {
            // ...
            return converted;
        }
    }
}

// ...
struct SelectionReturnType {
    int selectedTypeIdx;
    std:: string selectedObjectType;
    std::unordered_map<std::string, std::vector<std::string>> categoryMap;
};

/*
 * TTree (Ntuple), TH1D (1D Histogram)
 * 
 * Checks what objects are in the root file
 * If there is only one object type, select that by default, skip prompt
 * ^ (if there is no histogram, go the Ntuple route)
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
 * TODO: Not sure whether to define some of these maps in load_root, and pass in pointers
 * to this function, or leave as is, will revisit this design choice later
 * 
 */
std::optional<SelectionReturnType> select_root_type() {
    // Handle unloaded root file (i.e., via calling this method directly, or some bug)
    if (!inROOT) {
        std::cerr << "\nError: File not found!\n";
        return std::nullopt;
    }
    
    // inROOT->ls();
    // std::cout << inROOT->FindKey("TTree") << std::endl;
    // auto y = inROOT->FindKey("TTree");
    
    // ...
    TList* entries = inROOT->GetListOfKeys();
    
    // ...
    if (!entries) {
        std::cerr << "\nError: Failed to get ROOT file keys.\n";
        return std::nullopt;
    }
    
    // ...
    // std::cout << "LAST INDEX: " << entries->LastIndex() << std::endl; // 5
    // std::cout << "SIZE: " << entries->GetSize() << std::endl; // 6
    
    // ...
    // inROOT->GetFileCounter();
    
    // ...
    std::cout << "\nShowing ROOT object types available in file: " << inROOT->GetName() << "\n";
    
    // ...
    // TList* entries = inROOT->GetList(); // NOTE: This doesnt work with loop below, pretty sure you have to do address thing, like in branch iteration
    
    // ...
    // std::unordered_set<std::string> objectTypes = {}; // NOTE: will automatically create std::string copy of const char*
    std::unordered_set<std::string_view> objectTypes = {}; // NOTE: ...
    // NOTE: Key: 
    // Value
    
    // Key-value map of access indices assigned to each object type
    std::unordered_map<int, std::string> objectMap = {}; // NOTE: ...
    // NOTE: Key:
    // Value:
    
    // Key-value
    std::unordered_map<std::string, std::vector<std::string>> categoryMap = {}; // NOTE: ...
    // NOTE: Key: "TTree", "TH1D", etc
    // Value: {"EventData", "TrackData", etc}, {"PhotonsSpectrum"}
    
    // ...
    for (int i = 0; i < entries->GetSize(); i++) {
        // Get the TKey* at index i
        TObject* entry = entries->At(i); // TKey*
        // NOTE: Since we called GetListOfKeys(), this is actually a TKey*, TList* iteration just defaults to TObject*
        
        // TKeys are different to TObjects because ...
        
        // Get the name that was assigned to the ROOT object (Ntuple, TH1D, etc) at creation (i.e., EventData, TrackData, etc)
        char const* objectName = entry->GetName();
        // std::cout << i << ": " << objectName << std::endl; // NOTE: Prints the name of each object (EventData, TrackData, etc)
        // TODO: STORE THESE NAMES IN A MAP UNDER KEY OF TTree, TH1D, etc
        
        // std::cout << "FILE TYPE A: " << inROOT->GetType(objectName) << std::endl; // TEST: Prints: 0
        // NOTE: I think this is more for checking 
        
        // ..
        // auto x = entry->IsA(); // Returns TClass*
        // std::cout << "CLASSNAME: " << x->ClassName()  << std::endl; // NOTE: Prints "TClass"
        
        // entry->InheritsFrom(TTree::Class());
        
        // if (entry->IsA() == TTree::Class()) { // NOTE: Doesnt work
        // if (entry->InheritsFrom(TTree::Class())) { // NOTE: Doesnt work
//         if (entry->IsA()->InheritsFrom(TTree::Class())) { // NOTE: Doesnt work
//             std::cout << "IS TTREE\n";
//         }
//         else if (entry->IsA() == TH1::Class()) { // NOTE: Doesnt work
//             std::cout << "IS TH1\n";
//         }
//         
        // std::cout << "CLASSNAME: " << entry->IsA()->GetActualClass(entry)->GetName() << std::endl; // TEST: Prints: TKey
        // TODO: Could do a safety check here, check GetName() returns TKey BEFORE static_cast ...?
        
        // std::cout << "OBJECT NAME:" << TClass::GetClass(objectName) << std::endl; // TEST: Prints: 0
        
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
        
        // Returns the type of object this key is associated with (i.e., TTree, TH1D, etc)
        char const* objectType = key->GetClassName();
        // std::cout << "CLASSNAME: " << objectType << std::endl;
        
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
        
        // ...
        // if (objectTypes.find(objectType) == objectTypes.end()) { // .find returns key if found, or .end() if not found
        // if (!objectTypes.count(objectType)) { // returns: not found = 0, found = 1
        //     objectTypes.insert(objectType);
        //     std::cout << "NEW KEY\n";
        // }
        // else std::cout << "DUPLICATE KEY\n";
        
        // Add unique object types to the set
        std::pair pair = objectTypes.insert(objectType); // NOTE: Automatically only adds on unique
        // NOTE: insert returns a pair of: iterator & success status
        
        // If success status = true, item was inserted, else status = false
        if (pair.second) {
            // ...
            int const itemNo = objectTypes.size();
            
            // ..
            // std::cout << "NEW KEY: " << objectType << std::endl;
            std::cout << itemNo << ") " << objectType << std::endl;
            
            // ...
            objectMap[itemNo] = objectType; // NOTE: Inserts if missing, updates if present
            // NOTE: objectMap.insert(itemNo, objectType); requires iterator as arg[0],
            // i.e. if you want to re-enter a recently removed const iterator, not add
            // a new item
        }
    }
    // NOTE: Either just get all names now, and determine what object type is later
    // i.e. print all TTrees and Hists, let user select from those
    
    // std::cout << "SET SIZE: " << objectTypes.size() << std::endl;
    // std::cout << "MAP SIZE: " << objectMap.size() << std::endl;
    
    // ...
    int selectedTypeIdx = -1;
    
    // Switch on number of unique ROOT objects found
    if (objectMap.size() == 0) {
        std::cerr << "\nError: ROOT object map is empty.\n";
        return std::nullopt;
    }
    // ...
    else if (objectMap.size() == 1) {
        std::cout << "\nOnly one ROOT object type found, selecting...\n";
        selectedTypeIdx = 1;
    } 
    else {
        // ...
        std::cout << "\nWhat ROOT object type would you like to access? (type a number from the options above, or enter q to exit):\n";
        
        // Have the user select from one of the available options
        selectedTypeIdx = prompt_user_int(1, objectMap.size()); // each integer is mapped to an object type
        // NOTE: Non-zero based indexing feels more appropriate for this, with options starting
        // at one, and going up to the size of the map
        
        // std::cout << "USER SELECTED INT: " << selectedTypeIdx << std::endl;
    }
    
    // ...
    std::string selectedObjectType = objectMap.at(selectedTypeIdx); // Retrieve the string associate with the integer key
    // NOTE: Equivalent to .get() in typescript
    
    // std::cout << "USER SELECTED OBJECT: " << selectedObjectType << std::endl;
    // std::cout << "FILE TYPE B: " << inROOT->GetType(objectType.c_str()) << std::endl; // TEST: Prints: 0
    
    // Or print all object types, and show list of only those
    
    // ...
    if (selectedObjectType == "TTree") {
        rootObjectType = RootObjectType::TTree;
    }
    else if (selectedObjectType == "TH1D") {
        rootObjectType = RootObjectType::TH1D;
    }
    
    // ...
    struct SelectionReturnType result { selectedTypeIdx, selectedObjectType, categoryMap };
    
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
std::string select_root_object(SelectionReturnType const& selectionParams) {
    // Destructure the param object
    auto const& [selectedTypeIdx, selectedObjectType, categoryMap] = selectionParams;
    // NOTE: Zero copies, no pointer syntax
    
    // ...
    std::cout << "\nShowing options for object type: " << selectedTypeIdx << " - " << selectedObjectType << std::endl;
    // std::cout << "\nWhat object would you like to access? (type a number from the options below, or enter q to exit):\n";
    
    // Get a list of all objects matching the chosen type
    std::vector<std::string>filteredObjects = categoryMap.at(selectedObjectType);
    
    // ...
    for (int i = 0; i < filteredObjects.size(); i++) {
        // ...
        std::cout << (i + 1) << ") " << filteredObjects[i] << std::endl;
    }
    
    // If there is only one object matching that type, select it by default
    if (filteredObjects.size() == 1) {
        std::cout << "\nOnly one object matching requested type found, selecting...\n";
        return filteredObjects[0];
    }
    
    // ...
    std::cout << "\nWhat object would you like to access? (type a number from the options above, or enter q to exit):\n";
    
    // ...
    int const selectedObjectIdx = prompt_user_int(1, filteredObjects.size());
    // std::cout << "USER SELECTED INT: " << selectedObjectIdx << std::endl;
    
    // ...
    std::string selectedObjectName = filteredObjects.at(selectedObjectIdx - 1); // NOTE: Make selection zero-indexed again (minus 1)
    // std::cout << "USER SELECTED OBJECT: " << selectedObjectName << std::endl;
    
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
 * ...
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
    TObjArray* branches = nTuple->GetListOfBranches(); 

    if (!branches) {
        std::cerr << "\nError: Couldnt get TTree branches array!\n";
        root_cleanup();
        return "";
    }
    
    // ...
    std::cout << "\nBranch list has been loaded.\n";
    
    // Get the number of branches in the TTree
    int numEntries = branches->GetEntries();
    
    if (numEntries == 0) {
        std::cerr << "\nError: Selected TTree contains no branches!\n";
        root_cleanup();
        return "";
    }
    
    // ...
    std::unordered_map<int, std::string> branchMap = {};
    // NOTE: Key: ...
    // Value: ...
    
    std::cout << "\nShowing branches in TTree - " << nTuple->GetName() << ":\n";
    
    // Iterate through the branches of the TTree
    for (int i = 0; i < numEntries; i++) {
        TObject* entry = branches->At(i); // NOTE: Is TBranch*
        char const* branchName = entry->GetName();
        std::cout << (i + 1) << ") " << branchName << "\n";
        branchMap[i + 1] = branchName;
    }
    
    // ...
    std::string chosenBranch;
    
    // ...
    if (numEntries == 1) {
        std::cout << "\nOnly one branch found in TTree, selecting...\n";
        chosenBranch = branchMap.at(1);
    }
    // ...
    else {
        std::cout << "\nWhat branch would you like to access? (type a number from the options above, or enter q to exit):\n";
        int res = prompt_user_int(1, numEntries);
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
 * NOTE: Saves making multiple leaf requests later for branch data type querying
 */
int cache_leaf() {
    // Handle invalid branch name
    if (!branch) {
        std::cerr << "\nError: TBranch pointer not found. Closing root file...\n";
        root_cleanup();
        return 1;
    }
    
    // ...
    char const* branchName = branch->GetName();
    
    // Cache a pointer to a leaf (for data type access later)
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
 * Load histogram object (TH1D, TH1I, etc) from ROOT input file and attach pointer
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
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    delete inROOT;
    inROOT = nullptr;
    // NOTE: Histogram entire payload typically already in memory, detaching is fairly
    // safe here, and once detached, we no longer need the ROOT file
    
    // Success message
    std::cout << "\nROOT histogram loaded.\n";
    
    // ...
    return 0;
}

/*
 * Attempts to load the chosen ROOT object into memory, routing to dedicated helper
 * functions for the various object types
 */
int load_root_object(std::string const& objectName) {
    // Convert std::string to c string pointer
    char const* name = objectName.c_str();
    
    // ...
    // int status = 1; 
    // NOTE: Defaults to failure (1), can only be set to success (0) if object type is selected
    
    // Load object router
    if (rootObjectType == RootObjectType::TTree) {
        // ...
        int const loadTreeError = cache_tree(name);
        // if (loadTreeError) return status; // TODO: Not sure about another layer of nesting here
        if (loadTreeError) return 1; // TODO: Not sure about another layer of nesting here
        
        // ...
        std::string const branchName = select_branch(); // TODO: setting status twice here feels redundant
        // if (branchName.empty()) return status;
        if (branchName.empty()) return 1;
        
        // ...
        // status = load_branch(branchName);
        int const loadBranchError = cache_branch(branchName);
        if (loadBranchError) return 1;
        
        // ...
        int const cacheLeafError = cache_leaf();
        if (cacheLeafError) return 1;
    }
    // ...
    else if (rootObjectType == RootObjectType::TH1D) {
        // ...
        // status = load_root_hist(name);
        int const loadRootHistError = load_root_hist(name);
        if (loadRootHistError) return 1;
    } 
    else {
        std::cerr << "\nError: Unsupported ROOT object type.\n";
        return 1;
        // NOTE: No need to call "root_cleanup()" here, each of these functions will
        // clean up after themselves if an error arises in their logic
    }
    
    // ...
    // if (!status) std::cout << "\nROOT object loaded.\n";
    std::cout << "\nROOT object loaded.\n";
    
    // ...
    // return status;
    return 0;
}

/*
 * Handles full ROOT file pipeline:
 * 
 * 1) Open ROOT file, load it into local memory, check its not empty
 * 2) Get list of object types, and have user select object type of interest
 * 3) Have user select from list of objects matching said type
 * 4) load the chosen object 
 * 
 * NOTE: Not implementing 2D/3D hist shit just yet, can save that for a later date 
 * (will need to select multiple branch names, etc)
 * ^ this should just be flexible enough to handle any Ntuples i create from the sim for now
 */
int load_root(std::string const& path) {
    // ....
    int loadError = load_root_file(path);
    
    if (loadError) {
        std::cerr << "\nError: Failed to load ROOT file into memory.\n";
        return 1;
    }
    
    // ....
    std::optional<SelectionReturnType> success = select_root_type(); // choose TTree (Ntuple), TH1D (1D Hist), etc
    
    if (!success) {
        std::cerr << "\nError: Failed to get user selection for object type.\n";
        return 1;
    }
    
    // ...
    SelectionReturnType result = success.value();
    
    // ...
    std::string objectName = select_root_object(result); // get tree/hist/etc name
    
    if (objectName.empty()) {
        std::cerr << "\nError: ROOT object name is empty.\n";
        return 1;
    }
    
    // ....
    int loadObjectError = load_root_object(objectName); // load root object with said name
    
    if (loadObjectError) {
        std::cerr << "\nError: Failed to access ROOT object.\n";
        return 1;
    }
    
    std::cout << "\nROOT file and chosen object loaded.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Executes ASCII or ROOT file procedures based on file type flag
 */
int load_file(std::string const& path) {
    // Success status
    int status = 1;
    // NOTE: Defaults to failure (1), and will only be set to success (0) on valid file load
    
    // Attempt to load the ASCII file into memory
    if (fileType == FileType::ASCII) {
        status = load_ascii(path);
    }
    // Attempt to load the ROOT file into memory
    else if (fileType == FileType::ROOT) {
        status = load_root(path);
    }
    // Reject invalid usage
    else {
        std::cerr << "\nError (load_file()): File type not set.\n";
    }
    
    return status;
}

/*
 * Instantiate a ROOT histogram object
 * 
 * TODO: Have user specify whether hist params should be automatically calculated,
 * via finding max value from dataset (+say 5-10% for xmax), and either calulating nbins, or leaving at 1024-4096 bins
 * or if theyd like a specific setup (i.e. 2048 channels)
 * 
 * TODO: Set title based on input type
 * i.e., ASCII = "Energy Spectrum"
 * ROOT Ntuple = name of ntuple
 * (this function wont be called with ROOT hist, so dont need to handle that)
 * 
 * TODO: Need to choose TH1 type based on input type too
 * i.e., ASCII = TH1I
 * ROOT Ntuple = TH1I, OR, TH1D
 * ^ maybe see if there is a way to check whether its an int or double ntuple
 * 
 * TODO: Im calling this in fill_hist_ntuple & fill_hist_ascii:
 * hpx->SetDirectory(nullptr);
 * Is it not better to just call it at the end of this function?
 */
int create_hist() {
    // Histogram args
    // int const nbins = 2048; // 2048 channels (bins)
    // int const xmin = 0; // min channel
    // int const xmax = 2048; // max channel (3500 photons)
    
    // int const nbins = 21; // 2048 channels (bins) // NOTE: Too few
    // int const nbins = 200; // 2048 channels (bins) // NOTE: Too few 
    // int const nbins = 1024; // 2048 channels (bins) // NOTE: Good
    // int const nbins = 2048; // 2048 channels (bins) // NOTE: Decent
    // // int const nbins = 4096; // 2048 channels (bins) // NOTE: Almost too many (but less compressed than 2048 on log scale)
    // int const xmin = 0; // min channel
    // int const xmax = 3000; // max channel (3500 photons)
    
    // Histogram args
    std::string title;
    std::string legendTitle;
    int nbins = -1; // 2048 channels (bins) // NOTE: Decent
    // int xmin = -1; // min channel value (i.e., usually 0, but maybe non-zero, or negative)
    // int xmax = -1; // max channel value (i.e., 3500 photons, 2000 mm, etc)
    double xmin = -1.; // min channel value (i.e., usually 0, but maybe non-zero, or negative)
    double xmax = -1.; // max channel value (i.e., 3500 photons, 2000 mm, etc)
    
    // std::string leafType;
    std::string_view leafType;
    
    // Lab spectra are already 2048 channels and appropriately binned
    if (fileType == FileType::ASCII) {
        title = "EnergySpectrum";
        legendTitle = "Energy Spectrum";
        nbins = 2048;
        // xmin = 0;
        // xmax = 2048;
        xmin = 0.;
        xmax = 2048.;
        // xTitle = "Channels";
    }
    // ...
    else if ((fileType == FileType::ROOT) && (rootObjectType == RootObjectType::TTree)) {
        // title = strcpy(nTuple->GetName());
        // strcpy(nTuple->GetName(), title);
        // title = (char*)(nTuple->GetName());
        // legendTitle =(char*)(branch->GetName());
        
        // char const* nTupleName = nTuple->GetName();
        // char const* branchName = branch->GetName();
        
        std::string const nTupleName = nTuple->GetName();
        char const* branchName = branch->GetName();
        
        double branchMin = nTuple->GetMinimum(branchName);
        
        title = nTupleName + "Hpx"; // NOTE: Using the TTree name itself causes ROOT to think the histogram already exists
        legendTitle = branchName;
        nbins = 2048; // TODO: Not sure on best approach for dynamic binning currently
        // xmin = nTuple->GetMinimum(branchName); // should be zero
        xmin = ((branchMin < 0.) ? branchMin : 0.); // should be zero
        // xmin = 0;
        // xmin = 0.;
        xmax = (nTuple->GetMaximum(branchName)) * 1.1; // +10%
        // xTitle = branchName;
        
        std::cout << "\nXMIN: " << xmin << " XMAX: " << xmax << "\n";
        
        // ...
        // std::cout << "BRANCH TYPE: " << branch->GetClassName() << "\n"; // NOTE: Prints ""
        
        // Get TTree data type by reading the leaves
        
        // char const* leafType = branch->GetLeaf(branchName)->GetTypeName(); // NOTE: Gives: "Double_t"
        // leafType = branch->GetLeaf(branchName)->GetTypeName(); // NOTE: Gives: "Double_t"
        
        // TODO: This leaf grabbig logic may be better suited to the load pipeline
        // can make global leaf variable, and just call GetTypeName() here
//         TLeaf* leaf = branch->GetLeaf(branchName);
//         
//         // NOTE: Fallback incase branch name and name required by GetLeaf() differ
//         if (!leaf) {
//             leaf = static_cast<TLeaf*>(branch->GetListOfLeaves()->At(0)); 
//             // Static cast is safe as we know list of leaves is not empty from 
//             // load pipeline, and were calling get leaves
//         }
        
        // ...
        leafType = leaf->GetTypeName();
        
        // ...
        // std::cout << "LEAF TYPE: " << leafType << "\n";
    }
    
    // ...
    if (title.empty() || legendTitle.empty() || nbins == -1 || xmin == -1 || xmax == -1) {
        std::cerr << "\nError: Failed to define histogram args.\n";
        
        // TODO: Clean objects (ASCII OR ROOT logic)
        
        return 1;
    }
    
    // TEST: Debug
    // if (hpx) {
    //     std::cerr << "\nHISTOGRAM IS NOT NULL\n";
    // }
    
    // TEST: Debug
    // TH1::AddDirectory(false);
    // NOTE: Resolves:
    // Warning in <TFile::Append>: Replacing existing TH1: TrackData (Potential memory leak).
    // But why?
    // That warning shouldnt be coming up in the first place?
    // NOTE: Because histogram names were colliding with ntuple names
    
    // TEST: Debug
    // gDirectory->ls();
    
    // ...
    if ((fileType == FileType::ASCII) || ((fileType == FileType::ROOT) && (leafType == "Int_t"))) {
        // ...
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
        
        // hpx->SetDirectory(nullptr);
    }
    // ...
    else if ((fileType == FileType::ROOT) && (leafType == "Double_t")) {
        // ...
        std::cout << "\n>>> Creating TH1D...\n";
        
        // Create a histogram (TH1D = double)
        hpx = new TH1D(
            title.c_str(), // "hpx", // Legend title
            legendTitle.c_str(), // "distance travelled", // Histo title
            nbins, // num bins
            xmin, // x low
            xmax // x up
        );
        // NOTE: TH1I works while num photons is int, but may need long64 (TH1L) for gain applied num photons,
        // or TH1F (float - 4 bytes) / TH1D (double - 8 bytes) if using floating point values
        
        // hpx->SetDirectory(nullptr);
    }
    
    // Handle missing histogram (failed instantiation for any reason)
    if (!hpx) {
        std::cerr << "\nError (create_hist()): Histogram not found!\n";
        // Need to close file (could be ascii or root)
        if (inROOT) root_cleanup();
        else if (inASCII.is_open()) ascii_cleanup(); // NOTE: if (inASCII) always returns true
        return 1;
    }
    
    // X-axis title
    // hpx->SetXTitle("Distance (mm)");
    
    std::cout << "\nHistogram instantiated.\n";
    
    // No errors, all good
    return 0;
}

/*
 * NOTE: With the higher resolution (2048 bins vs 1024 bins previously),
 * aliasing is seen when plotting the Ntuples data in a histogram,
 * to account for the higher resolution, we can apply a gaussian smearing
 * to reduce the jagged edges
 * 
 * TODO: Make smearing optional, potentially via boolean param
 * 
 * TODO: When calling plot(), may need to 
 */
double post_processing(int entry) {
// int post_processing(int entry) {
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
 * TODO: Split out getting branch and checking its valid, from the actual reading
 * of the branch. I.e.: load_branch(), read_branch()
 * 
 * TODO: Make post-processing optional
 * 
 * TODO: Make it so branchname is actually passed in as param
 * 
 * TODO: Maybe query list of branchnames in tree
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
    
    // ...
    char const* branchName = branch->GetName();
    
    // std::cout << "\nSetting branch address for: " << branchName << "\n";
    
    // std::cout << "\nDisabling non-essential branches...\n";
    
    // ...
    // std::string const dataType = branch->GetLeaf(branchName)->GetTypeName();
    // std::string_view const dataType = branch->GetLeaf(branchName)->GetTypeName();
    
    // Disable all branches from being read by TTree->GetEntry()
    nTuple->SetBranchStatus("*", false);
    // NOTE: It is recommended to only read the branches actually needed
    
    // Enable only the branch we need
    nTuple->SetBranchStatus(branchName, true);
    // NOTE: For iteration over multiple branches, we would just enable those too
    
    // ...
    // std::cout << "\nDisabling non-essential branches...\n";
    std::cout << "\nNon-essential branches have been disabled.\n";
    
    // ...
    if (!leaf) {
        std::cerr << "\nError [fill_hist_ntuple()]: Pointer to leaf in current TTree branch not found!\n";
        root_cleanup();
        return 1;
    }
    
    // ...
    std::string_view const dataType = leaf->GetTypeName();
    
    std::cout << "\n>>> Data type: " << dataType << "\n";
    
    // To read a tree, neeed to associate variables with the trees branches
    // double entry;
    // int entry; // TODO: This needs to change based on Ntuple type (int, double, etc)
    // std::variant<int, double> entry;
    // nTuple->SetBranchAddress(branchName, &entry);
    // NOTE: When loading a tree entry, the tree will set the variables to the branches value as read from the storage
    
    // ...
    int intEntry;
    double doubleEntry;
    
    std::cout << "\nSetting branch address for: " << branchName << "\n";
    
    //...
    if (dataType == "Int_t") {
        // std::cout << "\n>>> INT TYPE\n";
        nTuple->SetBranchAddress(branchName, &intEntry);
    } 
    else if (dataType == "Double_t") {
        // std::cout << "\n>>> DOUBLE TYPE\n";
        nTuple->SetBranchAddress(branchName, &doubleEntry);
    }
    // NOTE: Multiple variables can be set to different branches here, and TTree->GetEntry(i) will update all variables to the current index
    
    // ...
    std::cout << "\nBranch address set to: " << branchName << "\n";

    // Get the number of entries in the branch (i.e., length for iteration limit)
    long long const numEntries = branch->GetEntries();
    // long long const numEntries = 0; // TEST: For debugging (triggering the following clause)
    
    // ...
    std::cout << "\nGot num entries: " << numEntries << "\n";
    
    // ...
    if (numEntries == 0) {
        std::cerr << "\nError: Selected TTree branch contains no entries! Closing root file, and deconstructing Ntuple/branch.\n";
        root_cleanup();
        return 1;
    }
    
    // ...
    std::cout << "\nFilling histogram from TTree branch...\n";
    
    
    // TEST
    // double mean;
    // double min;
    // double max;
    // ...
    // TEST
    
    
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
        // hpx->Fill(entry);
        
        if (dataType == "Int_t") hpx->Fill(intEntry);
        else if (dataType == "Double_t") hpx->Fill(doubleEntry);
    }
    
    // ...
    std::cout << "\nBranch iteration complete.\n";
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    root_cleanup(); 
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
 * Iterate through ASCII file, populating histogram with per-bin values
 * 
 * TODO: Dont start from arbitrary line 13 and go until line 2060 
 * (parse the infile header for start, $DATA, then skip next line, then the following line is bin 0)
 * (when you read $ROI, break)
 * 
 * TODO: Parsing the entire file, yet only reading lines 13 -> 2060
 * 
 * TODO: if (!in.good()) check inside of while loop
 * 
 * TODO: !inASCII will always return true, do !inASCII.is_open()
 */
int fill_hist_ascii() {
    // Handle missing input file
    if (!inASCII) {
        std::cerr << "\nError (draw_hist_ascii()): No ASCII infile to read!\n";
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (draw_hist_ascii()): Histogram not found!\n";
        ascii_cleanup();
        return 1;
    }
    
    // ...
    std::cout << "\nFilling histogram from ASCII file...\n";
    
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
    ascii_cleanup();
    
    // ...
    std::cout << "\nHistogram filled from ASCII file. ASCII file closed.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Router for histogram fill methodology, switches based on input file type
 */
int fill_hist() {
    // ...
    std::cout << "\nRouting to: " << (fileType == FileType::ASCII ? "ASCII" : "ROOT") << " handler.\n";
    
    // If hist is not filled by one means or another defaults to error
    int status = 1;
    
    // Switch on file type, set status to 0 if hist was filled successfully
    if (fileType == FileType::ASCII) {
        status = fill_hist_ascii();
    }
    else if (fileType == FileType::ROOT) {
        status = fill_hist_ntuple(); // TODO: Getting branchname here is actually a bit of a shitter, may have to do global branch object, or global branch name
    }
    
    // Success message
    if (status == 0) {
        std::cout << "\nHistogram has been populated.\n";
    }
    
    // No errors, all good
    return status;
}

/*
 * Filter outliers in exponential/power/log normal plots:
 * 
 * - 99.5% quantile * 1.05
 * - 99.9% quantile
 */
int rebin_hist() {
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (rebin_hist()): Histogram not found!\n";
        // NOTE: All files and objects should already be closed/deconstructed at this point
        return 1;
    }
    
    // int mergeBins = 2; // number of bins to merge
    // hpx->Rebin();
    // hpx->RebinAxis(, );
    // hpx->RebinX();
    // hpx->SetAxisRange(xmin, xmax);
    
    std::cout << "Skewness: " << hpx->GetSkewness() << "\n";
    // NOTE: Vals < ~-2 or > ~+2: imply exponential, power, or log normal distribution
    // NOTE: Energy spectrum outputs ~-0.2, so can be disambiguated
    
    int const numQuartiles = 4;
    double out[numQuartiles] = {};
    double quartiles[numQuartiles] = {0.75, 0.99, 0.995, 0.999};
    
    hpx->GetQuantiles(numQuartiles, out, quartiles);
    
    for (int i = 0; i < numQuartiles; i++) {
        std::cout << ">>> " << quartiles[i] << "th Quartile: " << out[i] << "\n";
    }
    
    return 0;
}

/*
 * Instantiates a canvas object, populating the global variable
 */
int create_canvas() {
    // ...
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
    
    // ...
    std::cout << "\nCanvas created.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Renders the populated histogram object on to the instantiated canvas
 * 
 * TODO: May not always want to setOptStat(0)
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
        // NOTE: All files and objects should already be closed/deconstructed at this point
        return 1;
    }
    
    // ...
    std::cout << "\nRendering histogram to canvas...\n";
    
    // Switch render method based on input
    if (rootObjectType == RootObjectType::TH1D) {
        hpx->Draw("HIST"); // NOTE: For TH1 created by Geant4, need to specify hist flag
    }
    else {
        // Draw histogram to the canvas with default option
        hpx->Draw(); // NOTE: With ntuples, "HIST" no longer needed
    }
    
    // ...
    canvas->Update(); // NOTE: Afaik, this is not needed
    
    // Clean the default histogram statistics box (498.4, 291.1)
    // gStyle->SetOptStat(0); // default = 1111 (NOTE: 000001111 with zeros removed)
    // 0 = hides the statistics box entirely (leaving only fit box when fitted)
    // 10 = only number of entries
    // 110 = entries and mean
    // NOTE: Prefix zeros must be removed, as "01" is treated as octal number
    
    
    // TEST - This could also go at the end of fill_hist_ntuple()
    // if ((fileType == FileType::ROOT) && (rootObjectType == RootObjectType::TTree)) {
        // NOTE: If file type == ROOT & object type == TH1, the ROOT file will already 
        // be closed by this point
        // root_cleanup();
        // NOTE: Now that histogram has been populated via nTuple data, the ROOT infile
        // can safely be closed, doing so beforehand may cause undefined behaviour
    // }
    // TEST
    
    
    // ...
    std::cout << "\nHistogram rendered to canvas.\n";
    
    // No errors, all good
    return 0;
}

/*
 * Validate file path, load file into memory, instantiate histogram, fill histogram,
 * instantiate canvas, render histogram
 */
int plot(std::string const userPath, const double xmin = 0, const double xmax = 0, const int nbins = 0) {
    // ...
    // std::cout << "\nHIST PARAMS:\nX Min: " << xmin << " X Max: " << xmax << " Num Bins: " << nbins << "\n";
    
    // NOTE: Must delete TBranch -> then delete TTree -> then delete TFile,
    // trying to delete the TBranch AFTER already deleting the TTree will result in a segfault
    // likewise for trying to delete TTree after deleting TFile (assuming TTree->SetDirectory() wasnt called)
    // NOTE: This is incorrect ^^^ Just delete TFile
    if (leaf) {
        std::cout << "\nFound existing Ntuple branch leaf, clearing...\n";
        return 1;
    }
    if (branch) {
        std::cout << "\nFound existing Ntuple branch, clearing...\n";
        // delete branch; // NOTE: CALLING DELETE ON BRANCH AFTER DELETING NTUPLE CAUSES A SEGFAULT
        // branch = nullptr; // NOTE: CALLING DELETE ON BRANCH AFTER FILE IS CLOSED ALSO CAUSES A SEGFAULT SEEMINGLY
        return 1;
    }
    if (nTuple) {
        std::cout << "\nFound existing Ntuple, clearing...\n";
        // delete nTuple;
        // nTuple = nullptr;
        return 1;
    }
    if (inASCII.is_open()) { // NOTE: if (inASCII) always returns true (even after .close() & .clear())
        std::cout << "\nFound existing ASCII file, clearing...\n";
        return 1;
    }
    if (inROOT) {
        std::cout << "\nFound existing ROOT file, clearing...\n";
        return 1;
    }
    // NOTE: The inASCII & inROOT cases should never really flag true now (closed after loading 
    // TH1D or Ntuple, respectively, or on error trying to load them), but ntuple and branch can 
    // still flag true, if the error occurs in create_hist() or fill_hist()
    // (and since ASCII/ROOT Ntuples share some code execution, not sure deleting them on error 
    // in is very clean)
    
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
    
    // NOTE: Adding checks for all global objects for the moment, im pretty sure theyre
    // all being cleaned up appropriately, so some of these should be redundant, but will
    // provide feedback incase ive missed something
    
    // NOTE: Temporarily exiting program if an object is found to be present on recall,
    // except for the hpx, as that will always be present on recall, but the others
    // shouldnt be, so want to see the status message clearly if it happens
    
    // ^^^^^^^ TODO: Probably best to make a dedicated "clean()" method,
    // which checks other pointers are clear etc
    
    // Check provided path is valid (will return empty string if not valid)
    std::string const path = check_path(userPath);
    
    if (path.empty()) {
        std::cerr << "\nAborting: Invalid path error!\n";
        return 1;
    }
    
    // Attempt to load ASCII or ROOT file into memory
    int const fileError = load_file(path);
    
    if (fileError) {
        std::cerr << "\nAborting: Load file error!\n";
        return 1;
    }
    
    // TODO: is it worth having both load_file and load_object?
    // load_object would only run for non-ascii files
    
    // Only call: create_hist() & fill_hist(); if its ROOT Ntuple, or ASCII
    // ^ if its ROOT Histogram, hpx pointer will already be populated
    
    // TODO: This enlosure feels a bit dirty, likely a better way to do this
    // if ((fileType != FileType::ROOT) && (rootObjectType != RootObjectType::TH1D)) { // TEST: Uncomment to get Ntuple still loaded error
    if (rootObjectType != RootObjectType::TH1D) {
    
        // Attempt to instantiate histogram object
        int const histError = create_hist();
        
        if (histError) {
            std::cerr << "\nAborting: Create hist error!\n";
            return 1;
        }
        
        // Attempt to populate histogram from ASCII or ROOT file
        int const fillError = fill_hist();
        
        if (fillError) {
            std::cerr << "\nAborting: Fill hist error!\n";
            return 1;
        }
    }
    
    // Attempt to create canvas and paint the histogram
    int const canvasError = create_canvas();
    
    if (canvasError) {
        std::cerr << "\nAborting: Create canvas error!\n";
        return 1;
    }
    
    // Enclose create canvas, so that it doesnt get deleted when calling plot() multiple times in a row
    // NOTE: This doesnt work as intended unless calling canvas.clear(), otherwise render_hist bugs out
//     if (!canvas) {
//         // Attempt to create canvas and paint the histogram
//         int const canvasError = create_canvas();
//         
//         if (canvasError) {
//             std::cerr << "\nAborting: Create canvas error!\n";
//             return 1;
//         }
//     }
    
    // ...
    int const renderError = render_hist();
    
    if (renderError) {
        std::cerr << "\nAborting: Render histogram error!\n";
        return 1;
    }
    
    // Ensure flags are reset for consecutive calls (i.e., the if == TH1D logic above will run when loading a ROOT TH1D, THEN, an ASCII file (as ASCII logic doesnt touch the RootObjectType flag))
    rootObjectType = RootObjectType::NULLOBJ;
    fileType = FileType::NULLFILE;
    
    // No errors, all good
    return 0;
}

/*
 * TODO ...
 */
int add_axis_title(std::string const& title) {
    return 1;
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
        std::getline(std::cin, userInput); // NOTE: std::cin >> userInput;
        
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
 * TODO: This would currently accept a .Spe outfile extension, wanna limit it to .root really
 * 
 * TODO: Calling check_path here could change the fileType flag
 * ^ i dont think its necessarily an issue, as calling "plot()" again will flip it to appropriate type anyways
 * but check_path does a lot, maybe separate bits out soon
 */
int save(std::string const path) {
    // Check provided path is valid (will return empty string if not valid)
    std::string const validPath = check_path(path);
    
    if (validPath.empty()) {
        std::cerr << "\nError {save()}: Invalid path error!\n";
        return 1;
    }
    
    // Convert from: std::string, to: const char*
    char const* convertedPath = validPath.c_str();
    
    // Check if file already exists, and if so, whether to overwrite
    int const invalidPath = check_file(convertedPath);
    
    if (invalidPath) {
        std::cerr << "\nAborting: Please call save() with a new path.\n";
        return 1;
    }
    
    // Open outfile in recreate mode (creates ROOT file, replacing it if it already exists)
    TFile* outfile = TFile::Open(convertedPath, "RECREATE");
    
    // Handle incorrect path
    if (!outfile || !outfile->IsOpen()) {
        std::cerr << "\nError [save()]: Couldnt create/open outfile!\n";
        return 1;
    }
    
    // Handle missing histogram
    if (!hpx) {
        std::cerr << "\nError (save()): Histogram not found!\n";
        return 1;
    }
    
    // Write the histogram object to the root file
    outfile->WriteObject(hpx, "Spectrum");
    
    // All done
    delete outfile;
    outfile = nullptr;
    
    // Confirmation status
    std::cerr << "\nFile has been saved.\n";
    
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
