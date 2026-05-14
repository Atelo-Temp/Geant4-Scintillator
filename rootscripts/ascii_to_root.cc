// Read data from an acii file and create a root file with an histogram and an ntuple
// root.cern/doc/v636/tree101__basic_8C.html

#include "Riostream.h"

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TNtuple.h"

int main() {
    // Read file $ROOTSYS/tutrials/tree/basic.dat
    // this file has 3 columns of float data
    TString dir = gROOT->GetTutorialDir();
    
    dir.Append("/tree/");
    dir.ReplaceAll("/./", "/");
    
    std::ifstream in;
    in.open(Form("%sbasic.dat", dir.Data()));
    
    Float_t x, y, z;
    Int_t nlines = 0;
    auto f = TFile::Open("basic.root", "RECREATE");
    TH1F h1("h1", "x distribution", 100, -4, 4);
    TNtuple ntuple("ntuple", "data from ascii file", "x:y:z");
    
    while (1) {
        in >> x >> y >> z;
        
        if (!in.good()) break;
        
        if (nlines < 5) printf("x=%8f, y=%8f, z=%8f\n", x, y, z);
        
        h1.Fill(x);
        
        ntuple.Fill(x, y, z);
        
        nlines++;
    }
    
    printf(" found %d points\n", nlines);
    
    in.close();
    
    f->Write();
    
    return 0;
}


// #include "TLorentzVector.h"

// int read_ascii() {
//     TLorentzVector v(1, 2, 3, 4);
//     v.Print();
//     return 0;
// }
