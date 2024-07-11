#define Train_cxx
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
#include "TGraph.h"
#include "Train.h"
using namespace std;
using namespace RICHsystem;
class RootConvert;
void RootConvert::Loop(char *RootFileName){}
void RootConvert::Loop3(char *RootFileName, double threshold){}

long double CalculatePDF(double x, TH1F* hist, int cut = 10){
  //this function takes a variable value x, its training histogram pointer hist as parameters,and a cut measuring the diviation of the variable
  //it returns a likelihood value of variable x
  double mean = hist->GetMean();
  double rms = hist->GetRMS();
  long double factor = 1./(hist->GetEntries()*hist->GetBinWidth(1));      //suppose there is only 1 entry in this bin, instead of 0
  double var = TMath::Abs((x-mean)/rms);
  long double pdf = hist->GetBinContent(hist->FindBin(x));
  if(var < cut) {
    if(pdf!=0)  return pdf;
    else  return factor;
  }
  else return 1.e-10*factor;

}

void RootConvert::Loop2(TTree *signaltree, TTree *bkgtree, double threshold)
{
  TFile* outFile = new TFile("./training_output.root","RECREATE");


  //useful histograms (p.d.f) for discriminiting signal/background, [0] for signal training, [1] for backgroud training
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
  TH1F* kinetic_Energy_start[2];

  //--- to draw the feature graphs of signal and bkg ----
  TCanvas* c1 = new TCanvas("c1","c1",800,800);
  TCanvas* c2 = new TCanvas("c2","c2",800,800);
  TCanvas* c3 = new TCanvas("c3","c3",800,800);

  c1->Divide(2,2);
  c2->Divide(2,2);
  c3->Divide(2,2);
  
  total_energy[0] = new TH1F("total_energy_signal","total energy",500,0,100000);
  hits[0] = new TH1F("hits_signal","hits",120,0,120);
  xhits[0] = new TH1F("xhits_signal","xhits",60,0,60);
  yhits[0] = new TH1F("yhits_signal","yhits",60,0,60);
  hitsdiff[0] = new TH1F("hits_diff_signal","hits diff",40,0,40);
  peakingtime_diff[0] = new TH1F("peakingtime_diff_signal","difference of max and min peakingtime",512,0,512);
  cot_theta[0] = new TH1F("cot_theta_signal","cotangent theta of incident direction",80,0,20);
  x_pos[0] = new TH1F("x_pos_signal","x position of the start point",120,1,121);
  y_pos[0] = new TH1F("y_pos_signal","y position of the start point",120,1,121);
  max_energy_time_ratio[0] = new TH1F("max_energy_time_ratio_signal","the relative  time position of the max energy deposition point in track",101,0,1.01);
  x_rho[0] = new TH1F("rho_in_x_signal","rho in x peakingtime graph",101,-2,2.04);
  y_rho[0] = new TH1F("rho_in_y_signal","rho in y peakingtime graph",101,-2,2.04);
  kinetic_Energy_start[0] = new TH1F("kinetic_energy_start_signal","start kinetic energy (MC)",200,0.,4.);

  total_energy[0]->SetLineColor(kRed);
  hits[0]->SetLineColor(kRed);
  xhits[0]->SetLineColor(kRed);
  yhits[0]->SetLineColor(kRed);
  hitsdiff[0]->SetLineColor(kRed);
  peakingtime_diff[0]->SetLineColor(kRed);
  cot_theta[0]->SetLineColor(kRed);
  x_pos[0]->SetLineColor(kRed);
  y_pos[0]->SetLineColor(kRed);
  max_energy_time_ratio[0]->SetLineColor(kRed);
  x_rho[0]->SetLineColor(kRed);
  y_rho[0]->SetLineColor(kRed);
  kinetic_Energy_start[0]->SetLineColor(kRed);
  
  total_energy[1] = new TH1F("total_energy_bkg","total energy",500,0,100000);
  hits[1] = new TH1F("hits_bkg","hits",120,0,120);
  xhits[1] = new TH1F("xhits_bkg","xhits",60,0,60);
  yhits[1] = new TH1F("yhits_bkg","yhits",60,0,60);
  hitsdiff[1] = new TH1F("hits_diff_bkg","hits diff",40,0,40);
  peakingtime_diff[1] = new TH1F("peakingtime_diff_bkg","difference of max and min peakingtime",512,0,512);
  cot_theta[1] = new TH1F("cot_theta_bkg","cotangent theta of incident direction",80,0,20);
  x_pos[1] = new TH1F("x_pos_bkg","x position of the start point",120,1,121);
  y_pos[1] = new TH1F("y_pos_bkg","y position of the start point",120,1,121);
  max_energy_time_ratio[1] = new TH1F("max_energy_time_ratio_bkg","the relative  time position of the max energy deposition point in track",101,0,1.01);
  x_rho[1] = new TH1F("rho_in_x_bkg","rho in x peakingtime graph",101,-2,2.04);
  y_rho[1] = new TH1F("rho_in_y_bkg","rho in y peakingtime graph",101,-2,2.04);
  kinetic_Energy_start[1] = new TH1F("kinetic_energy_start_bkg","start kinetic energy (MC)",200,0.,4.);

  total_energy[1]->SetLineColor(kBlue);;
  hits[1]->SetLineColor(kBlue);;
  xhits[1]->SetLineColor(kBlue);;
  yhits[1]->SetLineColor(kBlue);;
  hitsdiff[1]->SetLineColor(kBlue);;
  peakingtime_diff[1]->SetLineColor(kBlue);;
  cot_theta[1]->SetLineColor(kBlue);;
  x_pos[1]->SetLineColor(kBlue);;
  y_pos[1]->SetLineColor(kBlue);;
  max_energy_time_ratio[1]->SetLineColor(kBlue);;
  x_rho[1]->SetLineColor(kBlue);;
  y_rho[1]->SetLineColor(kBlue);;
  kinetic_Energy_start[1]->SetLineColor(kBlue);;

  TGraph* ROC_curve = new TGraph();
  ROC_curve->SetMarkerStyle(7);
  ROC_curve->SetTitle("ROC curve of training;signal reservation;bkg rejection");
  ROC_curve->SetName("ROC_curve_of_training");

  int x_count, y_count;
  int count_diff;           //difference of x_count and y_count
  Float_t count;
  Float_t summax_new;
  Float_t start_pos_x, start_pos_y;
  Float_t timediff;
  Float_t ratio;
  Float_t incident_angle;
  Float_t rho_x, rho_y;
  Float_t startpos_x_MC, startpos_y_MC, startpos_z_MC, kinE_start_MC;

  signaltree->SetBranchAddress("energy",&summax_new);
  signaltree->SetBranchAddress("hits",&count);
  signaltree->SetBranchAddress("x_hits",&x_count);
  signaltree->SetBranchAddress("y_hits",&y_count);
  signaltree->SetBranchAddress("hits_diff",&count_diff);
  signaltree->SetBranchAddress("timediff",&timediff);
  signaltree->SetBranchAddress("incident_angle",&incident_angle);
  signaltree->SetBranchAddress("start_pos_x",&start_pos_x);
  signaltree->SetBranchAddress("start_pos_y",&start_pos_y);
  signaltree->SetBranchAddress("ratio",&ratio);
  signaltree->SetBranchAddress("rho_x",&rho_x);
  signaltree->SetBranchAddress("rho_y",&rho_y);
  signaltree->SetBranchAddress("startpos_x_MC", &startpos_x_MC);
  signaltree->SetBranchAddress("startpos_y_MC", &startpos_y_MC);
  signaltree->SetBranchAddress("startpos_z_MC", &startpos_z_MC);
  signaltree->SetBranchAddress("kinE_start_MC", &kinE_start_MC);

  bkgtree->SetBranchAddress("energy",&summax_new);
  bkgtree->SetBranchAddress("hits",&count);
  bkgtree->SetBranchAddress("x_hits",&x_count);
  bkgtree->SetBranchAddress("y_hits",&y_count);
  bkgtree->SetBranchAddress("hits_diff",&count_diff);
  bkgtree->SetBranchAddress("timediff",&timediff);
  bkgtree->SetBranchAddress("incident_angle",&incident_angle);
  bkgtree->SetBranchAddress("start_pos_x",&start_pos_x);
  bkgtree->SetBranchAddress("start_pos_y",&start_pos_y);
  bkgtree->SetBranchAddress("ratio",&ratio);
  bkgtree->SetBranchAddress("rho_x",&rho_x);
  bkgtree->SetBranchAddress("rho_y",&rho_y);
  bkgtree->SetBranchAddress("startpos_x_MC", &startpos_x_MC);
  bkgtree->SetBranchAddress("startpos_y_MC", &startpos_y_MC);
  bkgtree->SetBranchAddress("startpos_z_MC", &startpos_z_MC);
  bkgtree->SetBranchAddress("kinE_start_MC", &kinE_start_MC);
  
  //Training begin---------------------------------------------------------
  cout<<"Training proscess begins!"<<endl;
  Long64_t entries;

  for(int k=0;k<2;k++){
    
    if(k == 0){
      //filling signal histograms
      cout<<"Filling signal histograms..."<<endl;
      entries = signaltree->GetEntriesFast();
      cout<<"Total signal event numbers: "<< entries << endl;

      for(int jentry=0; jentry < (float)entries/2;jentry++){
        //use the entries:0,2,4,...... for training
        signaltree->GetEntry(2*jentry);

        total_energy[0]->Fill(summax_new);
        hits[0]->Fill(count);
        xhits[0]->Fill(x_count);
        yhits[0]->Fill(y_count);
        hitsdiff[0]->Fill(count_diff);
        peakingtime_diff[0]->Fill(timediff);
        cot_theta[0]->Fill(incident_angle);
        x_pos[0]->Fill(start_pos_x);
        y_pos[0]->Fill(start_pos_y);
        max_energy_time_ratio[0]->Fill(ratio);
        x_rho[0]->Fill(rho_x);
        y_rho[0]->Fill(rho_y);
        kinetic_Energy_start[0]->Fill(kinE_start_MC);
      }
    }
    if(k == 1){
      //filling bkg histograms
      cout<<"Filling background histograms..."<<endl;
      entries = bkgtree->GetEntriesFast();
      cout<<"Total background event numbers: "<< entries << endl;

      for(int jentry=0; jentry < (float)entries/2;jentry++){
        //use the entries:0,2,4,...... for training
        bkgtree->GetEntry(2*jentry);

        total_energy[1]->Fill(summax_new);
        hits[1]->Fill(count);
        xhits[1]->Fill(x_count);
        yhits[1]->Fill(y_count);
        hitsdiff[1]->Fill(count_diff);
        peakingtime_diff[1]->Fill(timediff);
        cot_theta[1]->Fill(incident_angle);
        x_pos[1]->Fill(start_pos_x);
        y_pos[1]->Fill(start_pos_y);
        max_energy_time_ratio[1]->Fill(ratio);
        x_rho[1]->Fill(rho_x);
        y_rho[1]->Fill(rho_y);
        kinetic_Energy_start[1]->Fill(kinE_start_MC);
      }
    }
    //loop to train half of the samples


    total_energy[k]->Scale(1./total_energy[k]->Integral(),"WIDTH");
    hits[k]->Scale(1./hits[k]->Integral(),"WIDTH");
    xhits[k]->Scale(1./xhits[k]->Integral(),"WIDTH");
    yhits[k]->Scale(1./yhits[k]->Integral(),"WIDTH");
    hitsdiff[k]->Scale(1./hitsdiff[k]->Integral(),"WIDTH");
    peakingtime_diff[k]->Scale(1./peakingtime_diff[k]->Integral(),"WIDTH");
    cot_theta[k]->Scale(1./cot_theta[k]->Integral(),"WIDTH");
    x_pos[k]->Scale(1./x_pos[k]->Integral(),"WIDTH");
    y_pos[k]->Scale(1./y_pos[k]->Integral(),"WIDTH");
    max_energy_time_ratio[k]->Scale(1./max_energy_time_ratio[k]->Integral(),"WIDTH");
    x_rho[k]->Scale(1./x_rho[k]->Integral(),"WIDTH");
    y_rho[k]->Scale(1./y_rho[k]->Integral(),"WIDTH");
    kinetic_Energy_start[k]->Scale(1./kinetic_Energy_start[k]->Integral(),"WIDTH");

    if(k==0){
      c1->cd(1);
      total_energy[k]->Draw();
      c1->cd(2);
      hits[k]->Draw();
      c1->cd(3);
      xhits[k]->Draw();
      c1->cd(4);
      yhits[k]->Draw();
      c2->cd(1);
      hitsdiff[k]->Draw();
      c2->cd(2);
      peakingtime_diff[k]->Draw();
      c2->cd(3);
      cot_theta[k]->Draw();
      c2->cd(4);
      max_energy_time_ratio[k]->Draw();
      c3->cd(1);
      x_pos[k]->Draw();
      c3->cd(2);
      y_pos[k]->Draw();
      c3->cd(3);
      x_rho[k]->Draw();
      c3->cd(4);
      y_rho[k]->Draw();

    }
    else if(k==1){
      c1->cd(1);
      total_energy[k]->Draw("same");
      c1->cd(2);
      hits[k]->Draw("same");
      c1->cd(3);
      xhits[k]->Draw("same");
      c1->cd(4);
      yhits[k]->Draw("same");
      c2->cd(1);
      hitsdiff[k]->Draw("same");
      c2->cd(2);
      peakingtime_diff[k]->Draw("same");
      c2->cd(3);
      cot_theta[k]->Draw("same");
      c2->cd(4);
      max_energy_time_ratio[k]->Draw("same");
      c3->cd(1);
      x_pos[k]->Draw("same");
      c3->cd(2);
      y_pos[k]->Draw("same");
      c3->cd(3);
      x_rho[k]->Draw("same");
      c3->cd(4);
      y_rho[k]->Draw("same");

    }

    total_energy[k]->Write();
    hits[k]->Write();
    xhits[k]->Write();
    yhits[k]->Write();
    hitsdiff[k]->Write();
    peakingtime_diff[k]->Write();
    cot_theta[k]->Write();
    x_pos[k]->Write();
    y_pos[k]->Write();
    max_energy_time_ratio[k]->Write();
    x_rho[k]->Write();
    y_rho[k]->Write();
    kinetic_Energy_start[k]->Write();

    c1->Write();
    c2->Write();
    c3->Write();
  
  }
  
  cout<<"Training process ends!"<<endl;
  //Training end----------------------------------------------------------

  //Testing begins-------------------------------------------------------------
  cout<<"Testing process begins!"<<endl;

  double likelihood_ratio;
  TH1F* signal_test = new TH1F("signal_test","likelihood ratio of signal",500,0,1);
  TH1F* bkg_test = new TH1F("bkg_test","likelihood ratio of background",500,0,1);
  //these are variables to get the p.d.f of each histogram
  long double p_total_energy[2], p_hits[2], p_xhits[2], p_yhits[2], p_hitsdiff[2], p_peakingtime_diff[2], p_incident_angle[2], p_x_pos[2], p_y_pos[2], p_ratio[2], p_var_x[2], p_var_y[2];
  //the multiplied likelihood function
  long double p_signal, p_bkg;

  for(int k=0;k<2;k++)
  {
    if(k==0){
      cout<<"Testing the signal samples..."<<endl;
      entries = signaltree->GetEntriesFast();
    }
    else{
      cout<<"Testing the background samples..."<<endl;
      entries = bkgtree->GetEntriesFast();
    }
    
    for(int jentry=1; jentry < (float)entries/2;jentry++)
    {
      //use the entries:1,3,5,...... for testing
        if(k==0) signaltree->GetEntry(2*jentry-1);
        else bkgtree->GetEntry(2*jentry-1);

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
        // p_signal = p_total_energy[0]*p_peakingtime_diff[0]*p_ratio[0]*p_hits[0]*p_incident_angle[0]*p_var_x[0]*p_var_y[0];        //*p_var_x[0]*p_var_y[0]*p_xhits[0]*p_yhits[0]*p_incident_angle[0]
        // p_bkg =    p_total_energy[1]*p_peakingtime_diff[1]*p_ratio[1]*p_hits[1]*p_incident_angle[1]*p_var_x[1]*p_var_y[1];        //*p_var_x[1]*p_var_y[1]*p_xhits[1]*p_yhits[1]*p_incident_angle[1]
        p_signal = p_peakingtime_diff[0]*p_ratio[0]*p_xhits[0]*p_yhits[0]*p_incident_angle[0]*p_var_x[0]*p_var_y[0]*p_x_pos[0]*p_y_pos[0];        //*p_var_x[0]*p_var_y[0]*p_xhits[0]*p_yhits[0]*p_incident_angle[0]
        p_bkg =    p_peakingtime_diff[1]*p_ratio[1]*p_xhits[1]*p_yhits[1]*p_incident_angle[1]*p_var_x[1]*p_var_y[1]*p_x_pos[1]*p_y_pos[1];        //*p_var_x[1]*p_var_y[1]*p_xhits[1]*p_yhits[1]*p_incident_angle[1]
        likelihood_ratio = p_signal/(p_signal+p_bkg);

        //Apply some cuts to reject background
        if(timediff>peakingtime_diff[0]->GetMean()+4*peakingtime_diff[0]->GetRMS()) likelihood_ratio = 0;

        if(k==0){
          signal_test->Fill(likelihood_ratio);
        }
        else{
          bkg_test->Fill(likelihood_ratio);
          // if(likelihood_ratio>0.99){
          //   cout<<"This background event has a likelihood ratio larger than 0.99: "<<2*jentry-1<<endl;
          //   cout<<"Likelihood ratio: "<<likelihood_ratio<<endl;
          //   cout<<"Hits: "<<count<<endl;
          //   cout<<"total energy lost: "<<summax_new<<endl;
          //   cout<<"Transvering time: "<<timediff<<endl;
          //   cout<<"Max energy relative position: "<< ratio<<endl;
          // }
        }
    }
  }
  
  signal_test->Write();
  bkg_test->Write();
  cout<< "Number of signal events with likelihood ratio > " << threshold <<": "<<signal_test->Integral((int)500*threshold,501) <<"\n"
      << "Ratio of kept signal events: " << signal_test->Integral((int)500*threshold,501)/signal_test->GetEntries() << endl;
  cout<< "Number of background events with likelihood ratio > " << threshold <<": "<<bkg_test->Integral((int)500*threshold,501) << "\n"
      << "Ratio of kept background events: " << bkg_test->Integral((int)500*threshold,501)/bkg_test->GetEntries() << endl;


  for(int i=0;i<500;i++){
    ROC_curve->SetPoint(i,signal_test->Integral(i+1,501)/signal_test->GetEntries(),bkg_test->Integral(0,i)/bkg_test->GetEntries());
  }
  ROC_curve->Write();
  //Testing ends---------------------------------------------------------------
  outFile->Close();

}

//define main()
string signalFileName, bkgFileName;
double th;
void helpinfo(){

  cout<<"Usage is ./Train.exe ./VariableTree/<signal file name> ./VariableTree/<background file name> <threshold>\n";
  cout<<"default output file name is ./training_output.root"<<endl;	
  return;
} 
void phrase_command(int argc,char *argv[]){
  if (argc<4){ helpinfo();
    exit(0);
  }
  else {
    signalFileName=(string) argv[1];
    bkgFileName = (string) argv[2];
    th =  strtod(argv[3],NULL);
    cout<<"START\n~~~~~*****~~~~~\nSignal File : "<<signalFileName<<"\nBackground File:"<<bkgFileName<<endl;}
}

int main(int argc,char *argv[])
{

  phrase_command(argc,argv);
  string Tt="variabletree";
  const char *TtreeName=(char *)(Tt.data());

  //get the pointer to the signal tree
  const Text_t* signalFile=(Text_t*)(signalFileName.data());
  char *signalFile_c=(char *)(signalFileName.data());
  TFile *f1 = (TFile*)gROOT->GetListOfFiles()->FindObject(signalFile);
  if (!f1) 
    {
      f1 = new TFile(signalFile_c);
    }
  TTree *signal_tree = (TTree*)f1->Get(TtreeName);

  //get the pointer to the background tree 
  const Text_t* bkgFile=(Text_t*)(bkgFileName.data());
  char *bkgFile_c=(char *)(bkgFileName.data());
  TFile *f2 = (TFile*)gROOT->GetListOfFiles()->FindObject(bkgFile);
  if (!f2) 
    {
      f2 = new TFile(bkgFile_c);
    }
  TTree *bkg_tree = (TTree*)f2->Get(TtreeName);
 
  RootConvert xx;
  //xx.RootConvert::Init(tree);
  xx.RootConvert::Loop2(signal_tree, bkg_tree, th);
    cout<<"\n~~~~~*****~~~~~\nTHE END"<<endl;
  return 1;

} 

