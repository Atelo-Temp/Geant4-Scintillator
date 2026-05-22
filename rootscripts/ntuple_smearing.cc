// Load an Ntuple (TTree) into memory from a '.root' file, fill a histogram, and display it on a canvas

#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TRandom.h>

// Executes automatically on script start (NOTE: Choose another function name if you wish to manually call it instead) 
int ntuple_smearing() {
    // std::string fileName = "~/geant4/geant4-v11.3.2/project/data/19_6_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_3-5res_1000000event.root";
    char const fileName[512] = "~/geant4/geant4-v11.3.2/project/data/19_6_final_Ntuple_NaI-Tl_gpsvolsrc_randomseed_EM4-PIXE-cut100um_source-casing_diffusebackpaint_0-96R_sigalpha0-1_rindexAir_pc-20nm-GND-R-QE_3cm_137cs_1024bin_3-5res_1000000event.root";
    char const treeName[16] = "EventData;1";
    char const branchName[16] = "NumPhotons";
    
    // Fetch and open root file
    TFile* const in = TFile::Open(fileName);
    
    // Get the TTree from the root file and assign it to the TTree pointer
    TTree* nTuple = nullptr;
    in->GetObject(treeName, nTuple); // NOTE: Also works: TTree* nTuple = in->Get<TTree>(treeName);
    
    // Get the TBranch of interest from the TTree, and assign it to pointer
    TBranch* const eventData = nTuple->GetBranch(branchName);
    
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
        // 3500 // x up
        2048 // x up
    );
    // NOTE: TH1I works while num photons is int, but may need long64 (TH1L) for gain applied num photons,
    // or TH1F (float - 4 bytes) / TH1D (double - 8 bytes) if using floating point values
    
    // PMT gain factor
    // int const gain = 1e6;
    
    // TODO: Maybe explore for reproducability
    // gRandom->SetSeed(...);
    
    // Read all entries in the branch
    for (long long i = 0; i < numEntries; i++) {
        // Load the data fore the given tree entry
        eventData->GetEntry(i); // returns bytes read, not the actual val
        // NOTE: The "entry" variable will now be updated
        
        // NOTE: With the higher resolution (2048 bins vs 1024 bins previously),
        // aliasing is seen when plotting the Ntuples data in a histogram,
        // to account for the higher resolution, we can apply a gaussian smearing
        // to reduce the jagged edges
        
        // In counting statistics: sigma = sqrt(N)
        double const sigma = std::sqrt(entry);
        // TODO: This assumes pure Poisson statistics, in a real detector system,
        // resolution is a combination of:
        // sigma_scintillator + sigma_transfer + sigma_PMT
        // in geant4 RESOLUTIONSCALE covers sigma_scintillator,
        // geometry and photocathode efficiency covers sigma_transfer,
        // and this sigma covers sigma_PMT (but doesnt neccesarily model is accurately)
        
        // Apply gaussian smearing to the photons detected in this event
        double const smeared = gRandom->Gaus(entry, sigma);
        
        // Conversion factor from num optical photons "detected" to 0-2048 channel number
        double const conversion = 2048. / 3500.;
        // NOTE: 3500 photons is arbitrary currently, in practice, this value should
        // reflect the upper window limit for the energy region of interest, i.e.:
        // 0 - 2 MeV
        
        // Convert entry to channel number
        // int const channel = conversion * smeared; // int channel = std::floor(conversion * entry);
        double const channel = conversion * smeared; // NOTE: Let H1 handle binning doubles
        
        // NOTE: Can also apply a gain factor (but would likely want to establish 
        // this value accurately from the physical detector rather than using estimate):
        // int const nTotal = entry * gain;
        // double const sigma = gain * std::sqrt(entry);
        // double const smeared = gRandom->Gaus(nTotal, sigma);
        // double const conversion = 2048. / (3500. * gain);
        // double const channel = conversion * smeared;
        
        // Add a count to the channel number associated with the current event
        hpx->Fill(channel);
    }
    
    // Detach histogram from input file, then close input file
    hpx->SetDirectory(nullptr);
    in->Close();

    // Canvas args
    Int_t const winX = 0; // Top left of screen
    Int_t const winY = 0; // Top left of screen
    Int_t const width = 1200;
    Int_t const height = 800;
    
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
