#define TMVAAnalysis_cxx
#include <TStyle.h>
#include <sstream>
#include <string.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <utility>
#include <stdio.h>

#include "TFile.h"

#include "TInterpreter.h"
#include <TStyle.h>
#include "TTree.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TGraph.h"
#include "TString.h"
#include "TMVAAnalysis.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

using namespace std;
using namespace RICHsystem;
class RootConvert;

void RootConvert::Loop2(TTree *signaltree, TTree *bkgtree, double threshold){}
void RootConvert::Loop(char *RootFileName){}

void RootConvert::Loop3(char *RootFileName, double threshold)
{
  string selection = "selection2_2";
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //Create a new file named "./AnalysisResult/<filename>_lr.root"
  
  string namesuffix="_tmva";
  string outputfile=(string)RootFileName;
  outputfile.replace(strlen(RootFileName)-5,5,namesuffix);
  int Lname=sizeof(outputfile.c_str());
  string out;
  out.assign(outputfile,15,Lname-15);
  string RFile="./AnalysisResult/"+out+".root";
 
  cout<<"Output files:"<<endl;
  cout<<"Analysis result (likelihood ratio) in file: "<<RFile<<"\n"<<endl;

  TFile *outFile=new TFile(RFile.c_str(),"RECREATE");
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  TFile *inFile = (TFile*)gROOT->GetListOfFiles()->FindObject(RootFileName);
  if (!inFile) 
  {
    inFile = new TFile(RootFileName);
  }

  string Tt="variabletree";
  const char *TtreeName=(char *)(Tt.data());
  TTree *tree = (TTree*)inFile->Get(TtreeName);

  int x_count, y_count;
  int count_diff;           //difference of x_count and y_count
  Float_t count;
  Float_t summax_new;
  Float_t start_pos_x, start_pos_y;
  Float_t timediff;
  Float_t incident_angle;           //new variable to represent the cotanget of incident angle (between the track and z axis)
  Float_t ratio;
  Float_t rho_x, rho_y;
  Float_t tracklength_x, tracklength_y;
  Float_t startpos_x_MC, startpos_y_MC, startpos_z_MC, kinE_start_MC;

  // float version of the int variables
  Float_t x_count1, y_count1;
  Float_t count_diff1;           //difference of x_count and y_count

  tree->SetBranchAddress("energy",&summax_new);
  tree->SetBranchAddress("hits",&count);
  tree->SetBranchAddress("x_hits",&x_count);
  tree->SetBranchAddress("y_hits",&y_count);
  tree->SetBranchAddress("hits_diff",&count_diff);
  tree->SetBranchAddress("timediff",&timediff);
  tree->SetBranchAddress("incident_angle",&incident_angle);
  tree->SetBranchAddress("start_pos_x",&start_pos_x);
  tree->SetBranchAddress("start_pos_y",&start_pos_y);
  tree->SetBranchAddress("ratio",&ratio);
  tree->SetBranchAddress("rho_x",&rho_x);
  tree->SetBranchAddress("rho_y",&rho_y);
  tree->SetBranchAddress("tracklength_x",&tracklength_x);
  tree->SetBranchAddress("tracklength_y",&tracklength_y);
  tree->SetBranchAddress("startpos_x_MC", &startpos_x_MC);
  tree->SetBranchAddress("startpos_y_MC", &startpos_y_MC);
  tree->SetBranchAddress("startpos_z_MC", &startpos_z_MC);
  tree->SetBranchAddress("kinE_start_MC", &kinE_start_MC);

  // NEW! Create a root file (in ./VariableTree directory) to save the event information of the selected events
  string treefilename = (string)RootFileName;
  namesuffix = "_selected.root";
  treefilename.replace(strlen(RootFileName)-5,5,namesuffix);
  TFile* treefile = new TFile(treefilename.c_str(),"RECREATE");

  TTree* outtree = new TTree("variabletree","variabletree");

  outtree->Branch("energy",&summax_new,"summax_new/F");
  outtree->Branch("hits",&count,"count/F");
  outtree->Branch("x_hits",&x_count,"x_count/I");
  outtree->Branch("y_hits",&y_count,"y_count/I");
  outtree->Branch("hits_diff",&count_diff,"count_diff/I");
  outtree->Branch("timediff",&timediff,"timediff/F");
  outtree->Branch("start_pos_x",&start_pos_x,"start_pos_x/F");
  outtree->Branch("start_pos_y",&start_pos_y,"start_pos_y/F");
  outtree->Branch("ratio",&ratio,"ratio/F");
  outtree->Branch("incident_angle",&incident_angle,"incident_angle/F");
  outtree->Branch("rho_x",&rho_x,"rho_x/F");
  outtree->Branch("rho_y",&rho_y,"rho_y/F");
  outtree->Branch("tracklength_x",&tracklength_x,"tracklength_x/F");
  outtree->Branch("tracklength_y",&tracklength_y,"tracklength_y/F");
  outtree->Branch("startpos_x_MC", &startpos_x_MC, "startpos_x_MC/F");
  outtree->Branch("startpos_y_MC", &startpos_y_MC, "startpos_y_MC/F");
  outtree->Branch("startpos_z_MC", &startpos_z_MC, "startpos_z_MC/F");
  outtree->Branch("kinE_start_MC", &kinE_start_MC, "kinE_start_MC/F");

  //TMVA initialize
  TMVA::Tools::Instance();
  TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");

  // reader->AddVariable("energy",&summax_new);
  // reader->AddVariable("hits",&count);
  reader->AddVariable("x_hits",&x_count1);
  reader->AddVariable("y_hits",&x_count1);
  // reader->AddVariable("hits_diff",&count_diff1);
  reader->AddVariable("timediff",&timediff);
  reader->AddVariable("incident_angle",&incident_angle);
  reader->AddVariable("start_pos_x",&start_pos_x);
  reader->AddVariable("start_pos_y",&start_pos_y);
  reader->AddVariable("ratio",&ratio);
  reader->AddVariable("rho_x",&rho_x);
  reader->AddVariable("rho_y",&rho_y);
  reader->AddVariable("tracklength_x",&tracklength_x);
  reader->AddVariable("tracklength_y",&tracklength_y);
  // reader->AddVariable("startpos_x_MC", &startpos_x_MC);
  // reader->AddVariable("startpos_y_MC", &startpos_y_MC);
  // reader->AddVariable("startpos_z_MC", &startpos_z_MC);
  // reader->AddVariable("kinE_start_MC", &kinE_start_MC);

  //choices: TMVAdataset_selection1, TMVAdataset_selection2_1, TMVAdataset_selection2_2
  string path = "TMVAdataset_"+selection;
  string weightpath = "";
  fstream f;
  // f.open(weightpath.data());
  // if (!f)
  // {
  //     cout << "The weights file is not exist." << endl;
  //     return;
  // }
  // f.close();

  std::map<std::string,int> Use;
  Use["Cuts"]            = 0;
  Use["LikelihoodPCA"]   = 0;
  Use["PDERS"]           = 0;
  Use["LD"]              = 0;
  Use["MLPBNN"]          = 0;
  Use["BDT"]             = 1;
  Use["RuleFit"]         = 0;
#ifdef R__HAS_TMVAGPU
   Use["DNN_GPU"]         = 0; // CUDA-accelerated DNN training.
#else
   Use["DNN_GPU"]         = 0;
#endif
#ifdef R__HAS_TMVACPU
   Use["DNN_CPU"]         = 0; // Multi-core accelerated DNN.
#else
   Use["DNN_CPU"]         = 0;
#endif

  if (Use["Cuts"]) {
    string weightpath = path + "/weights/TMVAClassification_Cuts.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("Cuts", weightpath.data());
    cout << "Cuts method used!" << endl;
  }
  else if (Use["LikelihoodPCA"]) {
    string weightpath = path + "/weights/TMVAClassification_LikelihoodPCA.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("Likelihood", weightpath.data());
    cout << "LikelihoodPCA method used!" << endl;
  }
  else if (Use["PDERS"]) {
    string weightpath = path + "/weights/TMVAClassification_PDERS.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("PDERS", weightpath.data());
    cout << "PDERS method used!" << endl;
  }
  else if (Use["LD"]) {
    string weightpath = path + "/weights/TMVAClassification_LD.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("LD", weightpath.data());
    cout << "LD method used!" << endl;
  }
  else if (Use["MLPBNN"]) {
    string weightpath = path + "/weights/TMVAClassification_MLPBNN.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("MLPBNN", weightpath.data());
    cout << "MLPBNN method used!" << endl;
  }
  else if (Use["BDT"]) {
    string weightpath = path + "/weights/TMVAClassification_BDT.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("BDT", weightpath.data());
    cout << "BDT method used!" << endl;
  }
  else if (Use["RuleFit"]) {
    string weightpath = path + "/weights/TMVAClassification_RuleFit.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("RuleFit", weightpath.data());
    cout << "RuleFit method used!" << endl;
  }
  else if (Use["DNN_GPU"]) {
    string weightpath = path + "/weights/TMVAClassification_DNN_GPU.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("DNN_GPU", weightpath.data());
    cout << "DNN_GPU method used!" << endl;
  }
  else if (Use["DNN_CPU"]) {
    string weightpath = path + "/weights/TMVAClassification_DNN_CPU.weights.xml";
    f.open(weightpath.data());
    if (!f)
    {
        cout << "The weights file does not exist." << endl;
        return;
    }
    f.close();
    reader->BookMVA("DNN_CPU", weightpath.data());
    cout << "DNN_CPU method used!" << endl;
  }
  else {
    cout << "No method used!" << endl;
    return;
  }

  string TMVAfileName = "./TMVA/TMVATrainResult_"+selection+".root";
  TFile *TMVAFile = new TFile(TMVAfileName.data());

  //Analysing begins-------------------------------------------------------------
  cout<<"Analysing process begins!"<<endl;


  //these are variables to get the p.d.f of each histogram
  //the multiplied likelihood function
  Long64_t entries;
  double classifier = 0;
  Int_t N_counts = 0;
  Int_t N1=0, N2=0, N3=0;

  //================
  TH2F* effhitsmap = new TH2F("hitsmap","Track start position;X;Y",120,1.,121.,120,1.,121.);       //2D hits map
  // the following histograms are only for simulation, since they will use the MC truth information
  TH2F* MC_hitsmap = new TH2F("MC_hitsmap_selected","Track start position of MC truth;X(mm);Y(mm)",150,-75.,75.,150.,-75.,75.);       //2D hits map of MC truth
  TH3F* MC_hitsmap_3d = new TH3F("MC_hitsmap_3d_selected","Track start position of MC truth(3D);X(mm);Y(mm)",150,-75.,75.,150.,-75.,75.,100,-90.,10.);       //3D hits map of MC truth
  TH1F* kinetic_Energy_start = new TH1F("kinE_start_selected","Start kinetic energy of the track;Energy(MeV);counts",200,0,4.0);

  //================
  entries = tree->GetEntriesFast();
  cout<<"Total event numbers: "<< entries << endl;
    for(int jentry=0; jentry < (float)entries;jentry++)
    {
      //use the entries:1,3,5,...... for testing
        tree->GetEntry(jentry);

        x_count1 = (float)x_count;
        y_count1 = (float)y_count;
        count_diff1 = (float)count_diff;
        
        if (Use["Cuts"]) {
          classifier = reader->EvaluateMVA("Cuts");
        }
        else if (Use["LikelihoodPCA"]) {
          classifier = reader->EvaluateMVA("Likelihood");
        }
        else if (Use["PDERS"]) {
          classifier = reader->EvaluateMVA("PDERS");
        }
        else if (Use["LD"]) {
          classifier = reader->EvaluateMVA("LD");
        }
        else if (Use["MLPBNN"]) {
          classifier = reader->EvaluateMVA("MLPBNN");
        }
        else if (Use["BDT"]) {
          classifier = reader->EvaluateMVA("BDT");
        }
        else if (Use["RuleFit"]) {
          classifier = reader->EvaluateMVA("RuleFit");
        }
        else if (Use["DNN_GPU"]) {
          classifier = reader->EvaluateMVA("DNN_GPU");
        }
        else if (Use["DNN_CPU"]) {
          classifier = reader->EvaluateMVA("DNN_CPU");
        }

        //============================
        //show the variable distribution & the event no. of the rest events

        if(classifier>=threshold){
          // cout << "Rest event no: " << jentry+1 << " X start position: " << start_pos_x << " Y start position: " << start_pos_y <<endl;
          effhitsmap->Fill(start_pos_x, start_pos_y);
          MC_hitsmap->Fill(startpos_x_MC,startpos_y_MC);
          MC_hitsmap_3d->Fill(startpos_x_MC,startpos_y_MC,startpos_z_MC);
          kinetic_Energy_start->Fill(kinE_start_MC);
          N_counts++;

          if(startpos_z_MC>=-0.1) N1++;
          else if(startpos_z_MC>-53) N2++;
          else N3++;

          outtree->Fill();
        }
        //============================

    }


  outFile->cd();
  effhitsmap->Write();
  MC_hitsmap->Write();
  MC_hitsmap_3d->Write();
  kinetic_Energy_start->Write();

  treefile->cd();
  outtree->Write();

  cout<< "Number of \"signal-like events\" (with likelihood ratio>" << threshold << "): "<< N_counts << "\n"
      << "Ratio of \"signal-like events\" (with likelihood ratio>" << threshold << "): "<< (float)N_counts/entries << endl;

  cout << "\nNumber of events from above TPC part: " << N1 << "\n"
      << "Number of events from inside TPC: " << N2 << "\n"
      << "Number of events from below TPC part: " << N3 << endl;


  //Testing ends---------------------------------------------------------------
  
  outFile->Close();
  treefile->Close();
}

//define main()
string filename;
double th;
void helpinfo(){

  cout<<"Usage is ./Analysis.exe ./VariableTree/<filename>.root <threshold>\n";
  cout<<"default output file name is ./AnalysisResult/<filename>_lr.root"<<endl;	
  return;
} 
void phrase_command(int argc,char *argv[]){
  if (argc<3){ helpinfo();
    exit(0);
  }
  else {
    filename=(string) argv[1];
    th =  strtod(argv[2], NULL);
    cout<<"START\n~~~~~*****~~~~~\nInput File : " << " " <<filename<<endl;}
}

int main(int argc,char *argv[])
{

  phrase_command(argc,argv);

  char *inputFile_c=(char *)(filename.data());

  RootConvert xx;
  xx.RootConvert::Loop3(inputFile_c, th);
    cout<<"\n~~~~~*****~~~~~\nTHE END"<<endl;
  return 1;

} 

