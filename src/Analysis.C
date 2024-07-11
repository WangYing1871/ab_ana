#define Analysis_cxx
#include <TStyle.h>
#include <sstream>
#include <string.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <utility>
#include <stdio.h>
#include <TCanvas.h>
#include <TSpectrum.h>
#include "TFile.h"
#include <TF1.h>
#include "TInterpreter.h"
#include <TStyle.h>
#include "TTree.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TGraph.h"
#include "Analysis.h"
using namespace std;
using namespace RICHsystem;
class RootConvert;

void RootConvert::Loop2(TTree *signaltree, TTree *bkgtree, double threshold){}
void RootConvert::Loop(char *RootFileName){}

long double CalculatePDF(double x, TH1F* hist, int cut = 10){
  //this function takes a variable value x, its training histogram pointer hist as parameters,and a cut measuring the diviation of the variable
  //it returns a likelihood value of variable x
  double mean = hist->GetMean();
  double rms = hist->GetRMS();
  long double factor = 1./(hist->GetEntries()*hist->GetBinWidth(1));
  double var = TMath::Abs((x-mean)/rms);
  long double pdf = hist->GetBinContent(hist->FindBin(x));
  if(var < cut) {
    if(pdf!=0)  return pdf;
    else  return factor;
  }
  else return 1.e-10*factor;

}

void RootConvert::Loop3(char *RootFileName, double threshold)
{
  //Get the training output file
  string pdfFileName = "./training_output.root";
  TFile* pdfFile = TFile::Open(pdfFileName.c_str());
  if(!pdfFile){
      cout<<"Could NOT find the p.d.f file: "<<pdfFileName<<endl;
      exit(0);
  }


  //Get the p.d.fs(histograms) of signal and bkg from the training output file
  TH1F* total_energy[2];
  TH1F* hits[2];
  TH1F* xhits[2];
  TH1F* yhits[2];
  TH1F* hitsdiff[2];
  TH1F* peakingtime_diff[2];
  TH1F* cot_theta[2];
  TH1F* x_pos[2];
  TH1F* y_pos[2];
  TH1F* max_energy_time_ratio[2];
  TH1F* x_rho[2];
  TH1F* y_rho[2];
  
  total_energy[0] = (TH1F*)pdfFile->Get("total_energy_signal");
  hits[0] = (TH1F*)pdfFile->Get("hits_signal");
  xhits[0] = (TH1F*)pdfFile->Get("xhits_signal");
  yhits[0] = (TH1F*)pdfFile->Get("yhits_signal");
  hitsdiff[0] = (TH1F*)pdfFile->Get("hits_diff_signal");
  peakingtime_diff[0] = (TH1F*)pdfFile->Get("peakingtime_diff_signal");
  cot_theta[0] = (TH1F*)pdfFile->Get("cot_theta_signal");
  x_pos[0] = (TH1F*)pdfFile->Get("x_pos_signal");
  y_pos[0] = (TH1F*)pdfFile->Get("y_pos_signal");
  max_energy_time_ratio[0] = (TH1F*)pdfFile->Get("max_energy_time_ratio_signal");
  x_rho[0] = (TH1F*)pdfFile->Get("rho_in_x_signal");
  y_rho[0] = (TH1F*)pdfFile->Get("rho_in_y_signal");
  
  total_energy[1] = (TH1F*)pdfFile->Get("total_energy_bkg");
  hits[1] = (TH1F*)pdfFile->Get("hits_bkg");
  xhits[1] = (TH1F*)pdfFile->Get("xhits_bkg");
  yhits[1] = (TH1F*)pdfFile->Get("yhits_bkg");
  hitsdiff[1] = (TH1F*)pdfFile->Get("hits_diff_bkg");
  peakingtime_diff[1] = (TH1F*)pdfFile->Get("peakingtime_diff_bkg");
  cot_theta[1] = (TH1F*)pdfFile->Get("cot_theta_bkg");
  x_pos[1] = (TH1F*)pdfFile->Get("x_pos_bkg");
  y_pos[1] = (TH1F*)pdfFile->Get("y_pos_bkg");
  max_energy_time_ratio[1] = (TH1F*)pdfFile->Get("max_energy_time_ratio_bkg");
  x_rho[1] = (TH1F*)pdfFile->Get("rho_in_x_bkg");
  y_rho[1] = (TH1F*)pdfFile->Get("rho_in_y_bkg");

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //Create a new file named "./AnalysisResult/<filename>_lr.root"
  
  string namesuffix="_lr";
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
  Float_t startpos_x_MC, startpos_y_MC, startpos_z_MC, kinE_start_MC;

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
  outtree->Branch("startpos_x_MC", &startpos_x_MC, "startpos_x_MC/F");
  outtree->Branch("startpos_y_MC", &startpos_y_MC, "startpos_y_MC/F");
  outtree->Branch("startpos_z_MC", &startpos_z_MC, "startpos_z_MC/F");
  outtree->Branch("kinE_start_MC", &kinE_start_MC, "kinE_start_MC/F");

  //Analysing begins-------------------------------------------------------------
  cout<<"Analysing process begins!"<<endl;

  double likelihood_ratio;
  TH1F* test = new TH1F("test","likelihood ratio of signal",500,0,1);
  TH1F* kept_signal = new TH1F("kept_signal","kept_signal v.s cut",500,0,1);
  //these are variables to get the p.d.f of each histogram
  long double p_total_energy[2], p_hits[2], p_xhits[2], p_yhits[2], p_hitsdiff[2], p_peakingtime_diff[2], p_incident_angle[2], p_x_pos[2], p_y_pos[2], p_ratio[2], p_var_x[2], p_var_y[2];
  //the multiplied likelihood function
  long double p_signal, p_bkg;
  Long64_t entries;
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
        

        //Get the p.d.f for each training histogram
        for(int i=0;i<2;i++){
          
          p_total_energy[i] = CalculatePDF(summax_new, total_energy[i]);
          p_hits[i] = CalculatePDF(count, hits[i]);
          p_xhits[i] = CalculatePDF(x_count, xhits[i]);
          p_yhits[i] = CalculatePDF(y_count, yhits[i]);
          p_hitsdiff[i] = CalculatePDF(count_diff, hitsdiff[i]);
          p_peakingtime_diff[i] = CalculatePDF(timediff, peakingtime_diff[i]);
          p_incident_angle[i] = CalculatePDF(incident_angle, cot_theta[i]);
          p_x_pos[i] = CalculatePDF(start_pos_x, x_pos[i]);
          p_y_pos[i] = CalculatePDF(start_pos_y, y_pos[i]);
          p_ratio[i] = CalculatePDF(ratio, max_energy_time_ratio[i]);
          p_var_x[i] = CalculatePDF(rho_x, x_rho[i]);
          p_var_y[i] = CalculatePDF(rho_y, y_rho[i]);
        }

        //Compute the likelihood ratio
        // p_signal = p_total_energy[0]*p_peakingtime_diff[0]*p_ratio[0]*p_hits[0]*p_incident_angle[0]*p_var_x[0]*p_var_y[0];            //*p_var_x[0]*p_var_y[0]*p_xhits[0]*p_yhits[0]
        // p_bkg =    p_total_energy[1]*p_peakingtime_diff[1]*p_ratio[1]*p_hits[1]*p_incident_angle[1]*p_var_x[1]*p_var_y[1];            //*p_var_x[1]*p_var_y[1]*p_xhits[1]*p_yhits[1]
        p_signal = p_peakingtime_diff[0]*p_ratio[0]*p_xhits[0]*p_yhits[0]*p_incident_angle[0]*p_var_x[0]*p_var_y[0]*p_x_pos[0]*p_y_pos[0];            //*p_var_x[0]*p_var_y[0]*p_xhits[0]*p_yhits[0]
        p_bkg =    p_peakingtime_diff[1]*p_ratio[1]*p_xhits[1]*p_yhits[1]*p_incident_angle[1]*p_var_x[1]*p_var_y[1]*p_x_pos[1]*p_y_pos[1];            //*p_var_x[1]*p_var_y[1]*p_xhits[1]*p_yhits[1]
        likelihood_ratio = p_signal/(p_signal+p_bkg);

        //Apply some cuts to reject background
        if(timediff>peakingtime_diff[0]->GetMean()+4*peakingtime_diff[0]->GetRMS()) likelihood_ratio = 0;


        test->Fill(likelihood_ratio);

        //============================
        //show the variable distribution & the event no. of the rest events

        if(likelihood_ratio>=threshold){
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


  for(Int_t i=0;i<500;i++){
    kept_signal->AddBinContent(i,test->Integral(i,501));
  }
  kept_signal->Scale(1./test->Integral(),"WIDTH");

  outFile->cd();
  test->Write();
  kept_signal->Write();
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
  pdfFile->Close(); 
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

