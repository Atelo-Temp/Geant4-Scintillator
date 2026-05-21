// Load an Ntuple (TTree) into memory from a '.root' file, fill a histogram, and display it on a canvas

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>

// Executes automatically on script start (NOTE: Choose another function name if you wish to manually call it instead) 
int plot_ntuples() {
    // std::string fileName = "~/geant4/geant4-v11.3.2/project/data/19_6_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_3-5res_1000000event.root";
    char fileName[512] = "~/geant4/geant4-v11.3.2/project/data/19_6_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_3-5res_1000000event.root";
    char treeName[16] = "EventData;1";
    char branchName[16] = "NumPhotons";
    
    // Fetch and open root file
    TFile* in = TFile::Open(fileName);
    
    // Get the TTree from the root file and assign it to the TTree pointer
    TTree* nTuple = nullptr;
    in->GetObject(treeName, nTuple); // NOTE: Also works: TTree* nTuple = in->Get<TTree>(treeName);
    
    // Get the TBranch of interest from the TTree, and assign it to pointer
    TBranch* eventData = nTuple->GetBranch(branchName);
    
    // To read a tree, neeed to associate variables with the trees branches
    int entry;
    nTuple->SetBranchAddress(branchName, &entry);
    // NOTE: When loading a tree entry, the tree will set the variables to the branches value as read from the storage

    // Get the number of entries in the branch (i.e., length for iteration limit)
    long long const numEntries = eventData->GetEntries();
    
    // Create a histogram (TH1I = integer - channel/counts both ints)
    auto hpx = new TH1I(
        "hpx", // Legend title
        "optical photons", // Histo title
        2048, // num bins
        0, // x low
        3500 // x up
    );
    // NOTE: TH1I works while num photons is int, but may need long64 (TH1L) for gain applied num photons,
    // or TH1F (float - 4 bytes) / TH1D (double - 8 bytes) if using floating point values
    
    // Read all entries in the branch
    for (long long i = 0; i < numEntries; i++) {
        // printf("Entry No.: %lld\n", i); // NOTE: Debugging
        
        // Load the data fore the given tree entry
        eventData->GetEntry(i); // returns bytes read, not the actual val
        // NOTE: The "entry" variable will now be updated
        
        // printf("Photons for this event: %d\n", entry); // NOTE: Debugging
        
        // Fill histogram with current entry
        hpx->Fill(entry);
    }
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    in->Close();

    // Canvas args
    Int_t winX = 0; // Top left of screen
    Int_t winY = 0; // Top left of screen
    Int_t width = 1200;
    Int_t height = 800;
    
    // Create a canvas display
    auto c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    
    // Handle error creating canvas
    if (!c) {
        printf("Error: Couldnt create canvas!\n");
        return 1;
    }
    
    // Draw histogram with default option
    hpx->Draw();
    
    return 0;
}
