// Load a histogram into memory from a '.root' file, and zoom in on a peak

#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>

// Global histo/canvas variables
TH1 *hpx = nullptr;
TCanvas *c = nullptr;

// Executes automatically on script start (NOTE: Choose another function name if you wish to manually call it instead) 
int hist_zoom () {
    // Fetch and open root file
    auto *in = TFile::Open("~/geant4/geant4-v11.3.2/project/gps/build/output0.root");
    
    // Get the histogram from the root file and assign it to the TH1 pointer
    // TH1 *hpx = nullptr;
    in->GetObject("Photons;2", hpx);
    // hpx = in->Get("Photons;2");
    
    // Handle missing histogram
    if (!hpx) {
        printf("Error: Histogram not found!\n");
        return 1;
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
    // auto c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    c = new TCanvas("c", "Spectrum", winX, winY, width, height);
    
    // Handle error creating canvas
    if (!c) {
        printf("Error: Couldnt create canvas!\n");
        return 1;
    }
    
    // Draw histogram with default option
    // hpx->DrawCopy("HIST");
    // copy = hpx->DrawCopy("HIST"); // NOTE: Could store copy in global variable and ref in range()
    hpx->Draw("HIST");
    
    return 0;
}

// Zoom in on a specific range
int range (double start, double end) {
    // Handle missing histogram/canvas
    if (!hpx || !c) {
        printf("Error: Histogram or Canvas not found!\n");
        return 1;
    }
    
    hpx->GetXaxis()->SetRangeUser(start, end);
    
    // hpx->DrawCopy("HIST"); // NOTE: If working with copy, need to call draw copy again
    
    // Notify canvas of update
    c->Modified();
    c->Update();
    
    printf("Zoomed to %.2f - %.2f\n", start, end);
    
    return 0;
}

// Zoom back out to the full view histogram (NOTE: Could also call range(0,0))
int reset () {
    // Handle missing histogram/canvas
    if (!hpx || !c) {
        printf("Error: Histogram or Canvas not found!\n");
        return 1;
    }
    
    hpx->GetXaxis()->UnZoom();
    
    c->Modified();
    c->Update();
    
    return 0;
}
