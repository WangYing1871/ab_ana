//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Aug  9 16:20:39 2019 by ROOT version 6.12/06
// from TTree richraw/richraw
// found on file: Raw2ROOT/20190809114545249.root
//////////////////////////////////////////////////////////

#ifndef RootConvert_h
#define RootConvert_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include "Constant.h"
using namespace std;
using namespace RICHsystem;

// Header file for the classes stored in the TTree if any.

class RootConvert {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           event;
   //Int_t           mode;
   //Int_t           trigger;
   Int_t           nHits;
   Int_t           Fec[1024];   //[nHits]
   Int_t           Chip[1024];   //[nHits]
   Int_t           Chn[1024];   //[nHits]
   Int_t           ADC[1024][512];   //[nHits]
   Float_t         sumADC[1024];   //[nHits]
   Float_t         maxADC[1024];   //[nHits]
   Float_t         maxPoint[1024];   //[nHits]
   Float_t         time[1024];       //20% rising time, replace maxPoint[1024]
   Float_t         summaxADC;       //NEW BRANCH! The sum of all the max ADC of an event,which can reflect the energy deposition of the event.
   Int_t         pixelX[1024];   //[nHits]
   Int_t         pixelY[1024];   //[nHits]
   
   Float_t        startpos_x;
   Float_t        startpos_y;
   Float_t        startpos_z;
   Float_t        kinE_start;

   // List of branches
   TBranch        *b_event;   //!
   //TBranch        *b_mode;   //!
   //TBranch        *b_trigger;   //!
   TBranch        *b_nHits;   //!
   TBranch        *b_Fec;   //!
   TBranch        *b_Chip;   //!
   TBranch        *b_Chn;   //!
   TBranch        *b_ADC;   //!
   TBranch        *b_sumADC;   //!
   TBranch        *b_maxADC;   //!
   TBranch        *b_maxPoint;   //!
   TBranch        *b_summaxADC;
   TBranch        *b_pixelX;   //!
   TBranch        *b_pixelY;   //!

   TBranch        *b_startpos_x;   //!
   TBranch        *b_startpos_y;   //!
   TBranch        *b_startpos_z;   //!
   TBranch        *b_kinE_start;   //!

   RootConvert(TTree *tree=0);
   virtual ~RootConvert();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(char *RootFileName);
   virtual void     Loop2(TTree *signaltree, TTree *bkgtree, double threshold);       //new method added, for likelihood ratio discriminant
   virtual void     Loop3(char *RootFileName, double threshold);
   virtual void     Waveform(char *RootFileName, Int_t events, Int_t event_num[]);       //new method added, for showing waveforms
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void     LoadPedestal(char *pedestaldata);
   double pedestal[Nfec][Nchip][Nch][3];
   //cluster search

  // vector<int> hitNo;
  // vector<int> hitADC;
  // vector<int> hitX;
  // vector<int> hitY;
   
  // int nClusters;
  // vector<vector<int>> clusterADC;
  // vector<int> clusterCharge;
  // vector<vector<int>> clusterX;
  // vector<vector<int>> clusterY;

  //irtual void     ClusterSearch();  

};
//#endif

//#ifdef RootConvert_cxx
RootConvert::RootConvert(TTree *tree) : fChain(0) {}

RootConvert::~RootConvert()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}
/*
void RootConvert::ClusterSearch(){
  for (int i=0;i<nHits;i++){
    hitNo.push_back(i);
  }

  //loop for cluster
  int maxhitADC=0,maxhitNo;
  int seedX,seedY;
  
  for(;!hitNo.empty();){

    //set maxADC hit as seed
    for(int j=0;j<hitNo.size();j++){
      int h=hitNo[j];
      if(maxhitADC<maxADC[h]){maxhitADC=maxADC[h];maxhitNo=j;}
    }
    seedX=pixelX[hitNo[maxhitNo]];
    seedY=pixelY[hitNo[maxhitNo]];
    hitNo.erase(maxhitNo);

  
  }

}*/
Int_t RootConvert::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RootConvert::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}


//new parameter:rising_time_mode, determine whether to use the 20% rising time 
//or the maxpoint(as before)
void RootConvert::Init(TTree *tree)       
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("event", &event, &b_event);
   //fChain->SetBranchAddress("mode", &mode, &b_mode);
   //fChain->SetBranchAddress("trigger", &trigger, &b_trigger);
   fChain->SetBranchAddress("nHits", &nHits, &b_nHits);
   fChain->SetBranchAddress("Fec", Fec, &b_Fec);
   fChain->SetBranchAddress("Chip", Chip, &b_Chip);
   fChain->SetBranchAddress("Chn", Chn, &b_Chn);
   fChain->SetBranchAddress("ADC", ADC, &b_ADC);
   fChain->SetBranchAddress("sumADC", sumADC, &b_sumADC);
   fChain->SetBranchAddress("maxADC", maxADC, &b_maxADC);
   fChain->SetBranchAddress("maxPoint", maxPoint, &b_maxPoint);
   fChain->SetBranchAddress("summaxADC", &summaxADC, &b_summaxADC);
   fChain->SetBranchAddress("pixelX", pixelX, &b_pixelX);
   fChain->SetBranchAddress("pixelY", pixelY, &b_pixelY);
   fChain->SetBranchAddress("startpos_x", &startpos_x, &b_startpos_x);
   fChain->SetBranchAddress("startpos_y", &startpos_y, &b_startpos_y);
   fChain->SetBranchAddress("startpos_z", &startpos_z, &b_startpos_z);
   fChain->SetBranchAddress("kinE_start", &kinE_start, &b_kinE_start);
   Notify();
}

Bool_t RootConvert::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void RootConvert::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t RootConvert::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
void RootConvert::LoadPedestal(char *pedestaldata){
        ifstream ped;
        int jfec, jchip, jch;
        ped.open(pedestaldata);
        //cout<<ped.good()<<endl;
        for(int i=0; i<Nfec;i++){
          for(int j=0; j<Nchip;j++){
            for(int k=0;k<Nch;k++){
              ped>>jfec>>jchip>>jch;
              //cout<<jfec<<","<<jchip<<","<<jch<<endl;
              for(int l=0;l<3;l++){
                ped>>pedestal[i][j][k][l];
                //cout<<pedestal[i][j][k][l]<<endl;
              }
            }
          }
        }
        ped.close();
}

void RootConvert::Waveform(char *RootFileName, Int_t events, Int_t event_num[])
{
    //Input parameters:
    //RootFileName: the file name of the rawroot file that we want to show waveforms
    //events: number of events that we want to show waveforms
    //event num[]: the array of selected event no. maximum is 20 
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  string namesuffix="_waveform";
  string outputfile=(string)RootFileName;
  outputfile.replace(strlen(RootFileName)-5,5,namesuffix);
  int Lname=sizeof(outputfile.c_str());
  string out;
  out.assign(outputfile,11,Lname-11);                       //remove the first 11 chars in outputfile to get a name for out file
  string RFile="./Waveform/"+out+".root";                   //build a .root file in ./Waveform
    //   string TXTFile="./Calibration/"+out+".txt";               //build a .txt file in ./Calibration
  
  cout<<"Output  waveforms  in file: "<< RFile <<endl;
    //   cout<<"Pedestal Parameters in file: "<< TXTFile <<"\n"<<endl;

  TFile *outFile1=new TFile(RFile.c_str(),"RECREATE");


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  

  //Constants definition 
    // const int Nfec = 2;
    // const int Nchip = 4;
    // const int Nch = 68;


  TGraph* waveform[2][120][100];

  //waveform of anti coincident MM detector, it has 4*4 pads, each connecting to a channel
  TGraph* waveform_MM[4][4][100];
  //the first number to distinguish X/Y channel hits
  //the second number is the true X/Y channel number on the detector

  for (int f=0;f<2;f++){                         //create and name the histograms of each channel by its fec and channel
      for (int g=0;g<120;g++){
        for (int h=0;h<events;h++){
            waveform[f][g][h] = new TGraph();
            char cc[120];
            memset(cc,0,sizeof(cc));
            if(f==0) sprintf(cc,"Dimension_X_Chn_%02d_Event%02d_waveform;Channel;Amplitude(ADC channel)",g+1,event_num[h]);
            else sprintf(cc,"Dimension_Y_Chn_%02d_Event%02d_waveform;Channel;Amplitude(ADC channel)",g+1,event_num[h]);

            waveform[f][g][h]->SetMarkerStyle(7);
            waveform[f][g][h]->SetTitle(cc);
            waveform[f][g][h]->SetName(cc);
        }
      }
  }

  for (int x=0;x<4;x++){
    for (int y=0;y<4;y++){
      for (int h=0;h<events;h++){
            waveform_MM[x][y][h] = new TGraph();
            char cc[120];
            memset(cc,0,sizeof(cc));
            sprintf(cc,"AntiCoincidentMM_Chn_X_%02d_Y_%02d_Event%02d_waveform;Channel;Amplitude(ADC channel)",x+1,y+1,event_num[h]);
            
            waveform_MM[x][y][h]->SetMarkerStyle(7);
            waveform_MM[x][y][h]->SetTitle(cc);
            waveform_MM[x][y][h]->SetName(cc);
        }
    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //Loop start
  
  //cout<<"Loop Root File..."<<endl;
  if (fChain == 0) {
   cout<<"fChain == 0" <<endl;
   return;
   }
  //Long64_t nentries = fChain->GetEntriesFast();
  //cout<<"~~~~~~~~~~~~~~~"<<endl;
  //cout<<"Nentries:"<<nentries<<endl;
  Long64_t ientry;
  
    //IMPORTANT: one loop for one event
    for(int c=0;c<events;c++){  
        ientry = LoadTree(event_num[c]);
        if (ientry < 0) break;
        fChain->GetEntry(event_num[c]);
        //fill 
        for(int i=0; i<nHits;i++){
            if(pixelX[i]!=0 && pixelY[i]==0){
                for(int j=0;j<Nsp;j++){
                    //fill the waveforms
                    // cout << "pixel X: " << pixelX[i] << endl;
                    waveform[0][pixelX[i]-1][c]->SetPoint(j,j,ADC[i][j]);
                    // cout << "11111" << endl;
                }
                waveform[0][pixelX[i]-1][c]->Write();
            }
            else if(pixelY[i]!=0 && pixelX[i]==0){
                for(int j=0;j<Nsp;j++){
                    //fill the waveforms
                    // cout << "pixel Y: " << pixelY[i] << endl;
                    waveform[1][pixelY[i]-1][c]->SetPoint(j,j,ADC[i][j]);
                    // cout << "22222" << endl;
                }
                waveform[1][pixelY[i]-1][c]->Write();
            }
            else if(pixelX[i]!=0 && pixelY[i]!=0){
              for(int j=0;j<Nsp;j++){
                    //fill the waveforms
                    // cout << "pixel Y: " << pixelY[i] << endl;
                    waveform_MM[pixelX[i]-1][pixelY[i]-1][c]->SetPoint(j,j,ADC[i][j]);
                    // cout << "22222" << endl;
                }
                waveform_MM[pixelX[i]-1][pixelY[i]-1][c]->Write();
            }
        }
    }

  outFile1->Close();
  cout << "Waveform writing finished! " << endl;

}

#endif // #ifdef RootConvert_cxx
