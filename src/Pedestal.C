#define Pedestal_cxx
#include <TStyle.h>
#include <sstream>
#include <string.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <utility>
#include <stdio.h>
#include <map>
#include <TCanvas.h>
#include <TSpectrum.h>
#include "TFile.h"
#include <TF1.h>
#include "TInterpreter.h"
#include <TStyle.h>
#include "TTree.h"
#include "TMath.h"
#include "TH1F.h"
#include "TGraph.h"
#include "Pedestal.h"
#include "Langaus.h"
using namespace std;
using namespace RICHsystem;
class RootConvert;

void RootConvert::Loop2(TTree *signaltree, TTree *bkgtree, double threshold){}
void RootConvert::Loop3(char *RootFileName, double threshold){}

void RootConvert::Loop(char *RootFileName)
{
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  string namesuffix="_pedestal";
  string outputfile=(string)RootFileName;
  outputfile.replace(strlen(RootFileName)-5,5,namesuffix);
  int Lname=sizeof(outputfile.c_str());
  string out;
  out.assign(outputfile,11,Lname-11);                       //remove the first 11 chars in outputfile to get a name for out file
  string RFile="./Pedestal/"+out+".root";                   //build a .root file in ./Pedestal
  string TXTFile="./Calibration/"+out+".txt";               //build a .txt file in ./Calibration
  
  cout<<"Output files:"<<endl;
  cout<<"Parameters: "<<TXTFile<<"\n"<<endl;

  TFile *outFile=new TFile(RFile.c_str(),"RECREATE");


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  

  //Constants definition 
//  const int Nsp=1024;
//  const int Nch=96;
//  const int Nfec=2;


  TGraph* waveform[Nfec][Nchip][Nch];

  for (int f=0;f<Nfec;f++){                         //create and name the histograms of each channel by its fecand channel
    for(int c=0;c<Nchip;c++){
      for (int h=0;h<Nch;h++){
        waveform[f][c][h] = new TGraph();
        char cc[60];
        memset(cc,0,sizeof(cc));
        sprintf(cc,"FEC%02d_Chip%01d_Chn%02d_waveform;Channel;Amplitude(ADC channel)",f,c,h);
        waveform[f][c][h]->SetMarkerStyle(7);
        waveform[f][c][h]->SetTitle(cc);
        waveform[f][c][h]->SetName(cc);
      }
    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //Loop start
  
  cout<<"Loop Root File..."<<endl;
  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"~~~~~~~~~~~~~~~"<<endl;
  cout<<"Nentries:"<<nentries<<endl;
  Long64_t ientry;
  
        ////mapL2 fec to connector Number
        //std::map <Int_t,Int_t> mapL2;                                                       //*********
        //mapL2.insert(pair<Int_t,Int_t>(5,0));//fec 1: hirose 0,1 
        //mapL2.insert(pair<Int_t,Int_t>(2,1));//fec 5: hirose 6,7
        //mapL2.insert(pair<Int_t,Int_t>(7,2));//fec 2: hirose 2,3
        //mapL2.insert(pair<Int_t,Int_t>(10,3));//fec 4: hirose 4,5

        //new mapL2
  std::map <Int_t, Int_t> mapL2;
  mapL2.insert(pair<Int_t, Int_t>(29, 0));                    //fec 29--->tag 0
  mapL2.insert(pair<Int_t, Int_t>(15, 1));                    //fec 15--->tag 1
   

  Double_t mean = 0;
  Double_t sigma = 0;
  Double_t threshold = 0;
  
  //IMPORTANT: only use the first event to pedestal
  ientry = LoadTree(10);
  if (ientry < 0) return;
  fChain->GetEntry(10);
  //fill 
  for(int i=0; i<nHits;i++){
    for(int j=0;j<Nsp;j++){
        waveform[mapL2[Fec[i]]][Chip[i]][Chn[i]]->SetPoint(j,j,ADC[i][j]);             //draw the waveform
    }    
  }

  ofstream paras;
  paras.open(TXTFile.c_str());

  paras << "BoardID ChipID ChannelID Mean Sigma Threshold" << "\n";

  for (int f=0;f<1;f++){        //because only one fec is used in this test
  // for (int f=0;f<Nfec;f++){                         //
    for(int c=0;c<Nchip;c++){
      for (int h=0;h<Nch;h++){
        //get the baseline and noise rms
        if(waveform[f][c][h]->Integral() == 0 )  {
          cout << "Empty channel, please change the event number!" << endl;
          continue;}
        waveform[f][c][h]->Write();
        mean = waveform[f][c][h]->GetMean(2);
        sigma = waveform[f][c][h]->GetRMS(2);
        // if(f==0 && c==3){threshold = mean + 15*sigma;}
        // else{threshold = mean + 10*sigma;}
        threshold = mean + 10*sigma;
        paras<< f <<" " << c << " " << h << " " << mean << " "<< sigma << " " << threshold << "\n";
      }
    }
  }
  paras.close();
  outFile->Close();

}



//define main()
string filename;
void helpinfo(){
  cout<<"Usage is ./Pedestal_exe ./Raw2ROOT/<filename>\n";
  cout<<"default output file name is ./Pedestal/<filname>_pedestal.root and ./Caliberation/<filename>_pedestal.txt"<<endl;	
  return;
} 
void phrase_command(int argc,char *argv[]){             //input the raw root file?
  if (argc<2){ helpinfo();
    exit(0);
  }else {filename=(string) argv[1]; cout<<"START\n~~~~~*****~~~~~\nInput File : "<<filename<<endl;}
}

int main(int argc,char *argv[])
{

  phrase_command(argc,argv);
  string Tt="richraw";                                      //IMPORTANT!THIS IS THE TREE NAME IN OUR RAW ROOT FILE 
  const char *TtreeName=(char *)(Tt.data());
  const Text_t* RawRootFile=(Text_t*)(filename.data());
  char *RawRootFile_c=(char *)(filename.data());
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(RawRootFile);              //find the input raw root file in the system?
  if (!f) 
    {
      f = new TFile(RawRootFile_c);
    }
  TTree *tree = (TTree*)gDirectory->Get(TtreeName);                                 //Find the tree with name "richraw" in this directory, let the current tree point to it.
  


  RootConvert xx;
  xx.RootConvert::Init(tree);                                                       
  xx.RootConvert::Loop(RawRootFile_c);
    cout<<"\n~~~~~*****~~~~~\nTHE END"<<endl;
  return 1;

} 

