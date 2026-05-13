// Load a histogram into memory from a '.root' file

#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

// Executes automatically on script start (NOTE: Choose another function name if you wish to manually call it instead) 
int load_root () {
    // Fetch and open root file
    auto *in = TFile::Open("~/geant4/geant4-v11.3.2/project/gps/build/output0.root");
    
    // Get the histogram from the root file
    TH1 *hpx = nullptr;
    in->GetObject("Photons;2", hpx);
    
    // Canvas args
    Int_t winX = 0; // Top left of screen
    Int_t winY = 0; // Top left of screen
    Int_t width = 1200;
    Int_t height = 800;
    
    // Create a canvas display
    auto c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    
    // Draw histogram with default option
    hpx->DrawCopy("HIST");
    
    // Close input file
    in->Close();
    
    return 0;
}
