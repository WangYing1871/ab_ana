#define ShowWaveform_cxx
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

//This function is added at 2022/7/12, provide it with a rawroot file name and the event numbers that you want to show, it will save the 
//waveforms of the hit detector channels of these events in the ./Waveform folder

void RootConvert::Loop(char *RootFileName){}
void RootConvert::Loop2(TTree *signaltree, TTree *bkgtree, double threshold){}
void RootConvert::Loop3(char *RootFileName, double threshold){}



//define main()
string filename;
const int N_max_event = 20;                     //Maximuim event number at one time
Int_t events;
Int_t event_num[N_max_event];

void helpinfo(){
  cout<<"Usage is ./ShowWaveform_exe ./Raw2ROOT/<filename> <event1> <event2> <event3> ...\n";
  cout<<"default output file name is ./Waveform/<filname>_waveform.root"<<endl;	
  return;
} 
void phrase_command(int argc,char *argv[]){             //input the raw root file?
    if (argc<3||argc>N_max_event+2){ helpinfo();
        exit(0);
    }
    else{
        filename=(string) argv[1]; 
        events = argc-2;
        memset(event_num,0,sizeof(event_num));
        cout<<"START\n~~~~~*****~~~~~\nInput File : "<<filename<<endl;
        cout<<"Numbers of events to show waveform: " << events <<endl;
        for(int i=0;i<events;i++){
            event_num[i] = atoi(argv[i+2]);
            //cout << event_num[i] << "\t" <<endl;
        }
    }
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
  xx.RootConvert::Waveform(RawRootFile_c,events,event_num);
    cout<<"\n~~~~~*****~~~~~\nTHE END"<<endl;
  return 1;

} 

