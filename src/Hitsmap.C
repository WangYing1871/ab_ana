#define Hitsmap_cxx
#include <TStyle.h>
#include <sstream>
#include <string.h>
#include <vector>
#include <iostream>
#include <iomanip>
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
#include "TSpectrum.h"
#include "Hitsmap.h"
#include "Langaus.h"
using namespace std;
using namespace RICHsystem;
class RootConvert;

void RootConvert::Loop2(TTree *signaltree, TTree *bkgtree, double threshold){}
void RootConvert::Loop3(char *RootFileName, double threshold){}

void RootConvert::Loop(char *RootFileName)
{
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //IO stream
  string namesuffix="_hitsmap";
  string outputfile=(string)RootFileName;
  outputfile.replace(strlen(RootFileName)-5,5,namesuffix);
  int Lname=sizeof(outputfile.c_str());
  string out;
  out.assign(outputfile,11,Lname-11);
  string EPSFile="./Hitsmap/"+out+".eps";
  string RFile="./Hitsmap/"+out+".root";
  
  cout<<"Output files:"<<endl;
  cout<<"Histogram pictures: "<<EPSFile<<endl;
  cout<<"Histogram root: "<<RFile<<"\n"<<endl;

  TFile *outFile=new TFile(RFile.c_str(),"RECREATE");
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //IMPORTANT CHANGE: in this test(Fe55,TPC mm,version2), 1 fec(no.31) is used, for this fec, 4 chips(no.0-4) are used, for each chip 68 channels are used(no.0-67,4 of them are noise channels)
  //define the TH1F for pedestal 
//  const int Nsp=512;
//  const int Nch=68;
//  const int Nfec=2;
//  const int Nchip=4; //per FEC 
  char pedestaldata[80] ="./Calibration/20240126153039492_baseline_25MHz_500ns_1pC_pedestal.txt";
  // char pedestaldata[80] ="./Calibration/simulation_baseline.txt";
  LoadPedestal(pedestaldata);                             //*****************

  Bool_t Is_small_range = true;     //flag if the range is small/big
  Bool_t MultiPeak = false;         //flag if use peak finding method
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //define the TH1F for hitsmap
  //TH1F *effhits_hist= new TH1F("effhits",";ADC;counts",4200,-100,4100);
  //TH1F *peakingtime= new TH1F("peakingtime",";SamplingPoint;counts",300,50,350);
  //TH2F *hitsmap;
  //TH2F *effhitsmap= new TH2F("effhitsmap",";X(5mm);Y(5mm)",31,0,31,31,0,31);
  ////TH2F *hitmap =new TH2F(histitle,";X(5mm);Y(5mm)",33,0,33,33,0,33);
  //TGraph *waveform= new TGraph();
    
  //IMPORTANT CHANGE: new graphs defined
    //TGraph* waveform = new TGraph();                //ADC wave graph. 1 graph/channel
    TGraph* energy_lost_x = new TGraph();               //energy lost graph on x channels. 1 graph/event
    TGraph* energy_lost_y = new TGraph();               //energy lost graph on y channels. 1 graph/event
    TGraph* peakingtime_x = new TGraph();               //peaking time graph on x channels. 1 graph/event
    TGraph* peakingtime_y = new TGraph();               //peaking time graph on y channels. 1 graph/event

    TH1F* spectrum = new TH1F("Energy_Spectrum", "Energy spectrum;ADC channel;counts", 500, 0, 100000);      //energy spectrum. 1 graph in total
    TH1F* spectrum_central = new TH1F("spectrum_central","Central region spectrum;ADC channel;counts",500,0,100000);    //energy spectrum in central region
    // TH1F* spectrum1 = new TH1F("Energy_Spectrum_1", ";summaxADC;counts", 1000, 0, 20000);       //ONLY used in charge distribution test, first version of spectrum
    // TH1F* spectrum2 = new TH1F("Energy_Spectrum_2", ";summaxADC;counts", 1000, 0, 20000);       //ONLY used in charge distribution test, second version of spectrum
    TH2F *energy_vs_hits = new TH2F("energy_vs_track_length","Energy v.s. Track length;Track length;energy",100,0,100,200,0,100000);
    TH1F *effhits_hist= new TH1F("total_hits","Total hits number;Total hits;counts",120,0,120);     //efficient hits per event
    TH1F *xhits_hist = new TH1F("xhits", "Hit number in X;xhits;counts",60,0,60);            //x hits per event
    TH1F *yhits_hist = new TH1F("yhits", "Hit number in Y;yhits;counts",60,0,60);            //y hits per event
    TH1F *peakingtime_diff = new TH1F("transit_time","Transit time;time point;counts",400,0,500);       //diff between max and min peaking time
    TH2F* effhitsmap = new TH2F("hitsmap","Track start position;X;Y",120,1.,121.,120,1.,121.);       //2D hits map
    TH1F *max_energy_lost_pos = new TH1F("max_energy_lost_pos","Max energy lost position;Max energy lost position;counts",41,0,1.025);
    TH1F* incident_angle = new TH1F("angle","Incident angle;Cot(theta);counts",50,0,20);
    TH2F* energy_vs_angle = new TH2F("energy_vs_angle","Energy v.s. Cot(theta);Cot(theta);energy",100,0,20,200,0,100000);

    // TH1F* x_slope_var = new TH1F("Slope varience in x",";slope var;counts",105,0,21);
    // TH1F* y_slope_var = new TH1F("Slope varience in y",";slope var;counts",105,0,21);
    TH1F* rho_x = new TH1F("rho in x",";rho;counts",100,-2,2);
    TH1F* rho_y = new TH1F("rho in y",";rho;counts",100,-2,2);
    TH1F* hits_diff = new TH1F("Difference_in_x_y_hits","Difference in x y hits;diff;counts",100,0,100);
    TH1F* energy_per_length = new TH1F("energy_per_length","dE/dx;dE/dx;counts",200,0,100000);
    TH2F* energy_vs_transittime = new TH2F("energy_vs_transittime","Energy v.s. transit time;transit time;ADC channel",200,0,500,400,0,100000);
    TH2F* min_time_xy = new TH2F("min_time_xy","minimum time;X;Y",256,0,512,256,0,512);
    TH2F* max_time_xy = new TH2F("max_time_xy","maximum time;X;Y",256,0,512,256,0,512);
    TH2F* transittime_xy = new TH2F("transit_time_xy","transit time;X;Y",200,0,500,200,0,500);

    TH1F* tracklength_x = new TH1F("track_length_x","track length x;mm;counts",250,0,500);
    TH1F* tracklength_y = new TH1F("track_length_y","track length y;mm;counts",250,0,500);

    TF1* gaus1 = new TF1("gaus1", "gaus(0)", 11000, 18000);
    TF1* gaus2 = new TF1("gaus2", "gaus(0)", 0, 121*1.33);
    TF1* antilangau = new TF1("antilangau",antilangaufun,5000,18000,4);

    //histograms drawn after selection, in order to contrast with the un-selected ones
    TH1F* spectrum_2 = new TH1F("Energy_Spectrum_selected", "Energy spectrum;ADC channel;counts", 500, 0, 100000);
    TH1F* effhitsmap_x_2 = new TH1F("hitsmap_x_selected","Track start position X;position(mm);counts",120,1.*1.33,121.*1.33);       //1D hits map on X dim
    TH1F* effhitsmap_y_2 = new TH1F("hitsmap_y_selected","Track start position Y;position(mm);counts",120,1.*1.33,121.*1.33);       //1D hits map on Y dim
    TH1F *effhits_hist_2 = new TH1F("total_hits_selected","Total hits number;Total hits;counts",120,0,120);     //efficient hits per event
    TH1F *xhits_hist_2 = new TH1F("xhits_selected", "Hit number in X;xhits;counts",60,0,60);            //x hits per event
    TH1F *yhits_hist_2 = new TH1F("yhits_selected", "Hit number in Y;yhits;counts",60,0,60);            //y hits per event
    TH1F *peakingtime_diff_2 = new TH1F("transit_time_selected","Transit time;time point;counts",400,0,500);
    TH2F* effhitsmap_2 = new TH2F("hitsmap_selected","Track start position;X;Y",120,1.,121.,120,1.,121.);
    TH1F *max_energy_lost_pos_2 = new TH1F("max_energy_lost_pos_selected","Max energy lost position;Max energy lost position;counts",41,0,1.025);
    TH1F* hits_diff_2 = new TH1F("Difference_in_x_y_hits_selected","Difference in x y hits;diff;counts",100,0,100);
    TH1F* energy_per_length_2 = new TH1F("energy_per_length_selected","dE/dx;dE/dx;counts",200,0,100000);
    TH2F *energy_vs_hits_2 = new TH2F("energy_vs_track_length_selected","Energy v.s. Track length;Track length;energy",100,0,100,200,0,100000);
    TH1F* incident_angle_2 = new TH1F("angle_selected","Incident angle;Cot(theta);counts",50,0,20);
    TH2F* energy_vs_angle_2 = new TH2F("energy_vs_angle_selected","Energy v.s. Cot(theta);Cot(theta);energy",100,0,20,200,0,100000);
    TH2F* energy_vs_transittime_2 = new TH2F("energy_vs_transittime_selected","Energy v.s. transit time;transit time;ADC channel",200,0,500,400,0,100000);
    TH2F* min_time_xy_2 = new TH2F("min_time_xy_selected","minimum time;X;Y",256,0,512,256,0,512);
    TH2F* max_time_xy_2 = new TH2F("max_time_xy_selected","maximum time;X;Y",256,0,512,256,0,512);
    TH2F* transittime_xy_2 = new TH2F("transit_time_xy_selected","transit time;X;Y",200,0,500,200,0,500);
    TH1F* tracklength_x_2 = new TH1F("track_length_x_selected","track length x;mm;counts",250,0,500);
    TH1F* tracklength_y_2 = new TH1F("track_length_y_selected","track length y;mm;counts",250,0,500);
    

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ////mapL2 fec to connector Number
  //std::map <Int_t,Int_t> fecNo;                             //***********
  //fecNo.insert(pair<Int_t,Int_t>(5,0));//fec 1: hirose 0,1 
  //fecNo.insert(pair<Int_t,Int_t>(2,1));//fec 5: hirose 6,7
  //fecNo.insert(pair<Int_t,Int_t>(7,2));//fec 2: hirose 2,3
  //fecNo.insert(pair<Int_t,Int_t>(10,3));//fec 4: hirose 4,5
          
  //new mapL2
  std::map <Int_t, Int_t> mapL2;
  mapL2.insert(pair<Int_t, Int_t>(29, 0));                    //fec 29--->tag 0
  mapL2.insert(pair<Int_t, Int_t>(15, 1));                    //fec 15--->tag 1
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //Loop start
  cout<<"Loop Root File..."<<endl;
  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"~~~~~~~~~~~~~~~"<<endl;
  cout<<"Nentries:"<<nentries<<endl;
  Long64_t ientry;
  
  int effhit=0; 
  Int_t N_eff=0;
  double pedS,pedM;
  char grtitle[100];
  int x_count, y_count;

  double sumA[2];             //sum(maxADC) of x, y
  double sumAP[2];            //sum(maxADC*channel position) of x, y
  double start_pos[2];              //x, y position reconstructed
  Float_t x_timerange[4], y_timerange[4];
  //x_timerange[0]:the x coordinate of min time point in x strip graph
  //x_timerange[1]:the y coordinate of min time point in x strip graph
  //x_timerange[2]:the x coordinate of max time point in x strip graph
  //x_timerange[3]:the y coordinate of max time point in x strip graph
  Float_t x_range[4], y_range[4];        
  //x_range[0]:the minimum hit channel of x strip graph
  //x_range[1]:the peaking time at the minimum hit channel of x strip graph
  //x_range[2]:the maximum hit channel of x strip graph
  //x_range[3]:the peaking time at the maximum hit channel of x strip graph
  Float_t x_Emax[2], y_Emax[2];               
  //x_Emax[0]:the x coordinate of max energy lost point in x strip graph
  //x_Emax[1]:the y coordinate of max energy lost point in x strip graph
  Float_t timerange[2]; 
  Float_t time_of_Emax_x, time_of_Emax_y; 
  Double_t sqrt_hit; 
  
  Float_t summaxADC1, summaxADC2;     //FOR charge distribution test

  Double_t source[Nsp];         //save the current waveform
  Double_t dest[Nsp];           //save the analysed output waveform
  Int_t nfound;                 //number of peaks found
  TSpectrum *s = new TSpectrum();
  Double_t xpeaks[100];          //peak x position 
  Float_t ypeaks[100];          //peak y position
  Float_t summax_new;             //define a new variable to calculate the total energy deposition


  Float_t Cot_theta = 0;            //A number proportional to cotangent of the inlet angle, used for energy correction
  Float_t dE_dx = 0;                //Energy/sqrt_hit
  
  //for beta background discrimination, calculate the varience of the slope,[0] for X, [1] for Y
  Float_t slope[2];
  Float_t Var_slope[2];
  Float_t slope_bar[2], slope_2_bar[2];
  Float_t E_xy[2];
  Float_t E_x[2];
  Float_t E_x2[2];
  Float_t E_y[2];
  Float_t E_y2[2];
  Float_t rho[2];

  Bool_t cut_saturated = false;       // become true if there is at least one saturated channel in this event 
  Bool_t cut_event_flag = false;      //set to true if this event is to be dropped
  Bool_t cut_event_flag2[2];      //the new boolean variable to select the coincidence events

  Int_t N_a = 0;       //count alpha beta numbers

  //the following two variables only used if want to show waveform, they are treated as parameters in the Waveform() function
  Int_t events=0;
  Int_t event_num[100];

  // this part is for track finding
  vector<Point> XZ_points_in;
  vector<Point> YZ_points_in;
  Float_t X_tracklength;
  Float_t Y_tracklength;

  //A single loop for hitsmap one event
    for (Int_t jentry=0; jentry<nentries;jentry++){
        ientry = LoadTree(jentry);
        if (ientry < 0) break;
        fChain->GetEntry(jentry);
        //loop entries
        if(jentry%((int)(nentries/10.))==0){
            // cout<<"Events:"<<jentry<<"("<<(int)(jentry*100./nentries)<<"%)"<<endl;
            cout<<"Events:"<<jentry<<endl;
        }
        ////reset;

        memset(sumA,0,sizeof(sumA));
        memset(sumAP,0,sizeof(sumAP));
        memset(start_pos,0,sizeof(start_pos));

        effhit=0;
        //memset(histitle,0,sizeof(histitle));
        //sprintf(histitle,"Event_%d_hitmap",event);
        //hitsmap =new TH2F(histitle,";X(5mm);Y(5mm)",33,0,33,33,0,33);      //*****
        ////  std::cout<<"nHits ="<<nHits<<std::endl;
        
        //reset the parameters of 4 graphs
        energy_lost_x->SetMarkerStyle(7);
        memset(grtitle, 0, sizeof(grtitle));
        sprintf(grtitle, "Event%d_Energy_Lost_XZ;strip;amplitude", jentry);
        energy_lost_x->SetTitle(grtitle);
        energy_lost_x->SetName(grtitle);

        energy_lost_y->SetMarkerStyle(7);
        memset(grtitle, 0, sizeof(grtitle));
        sprintf(grtitle, "Event%d_Energy_Lost_YZ;strip;amplitude", jentry);
        energy_lost_y->SetTitle(grtitle);
        energy_lost_y->SetName(grtitle);

        peakingtime_x->SetMarkerStyle(7);
        memset(grtitle, 0, sizeof(grtitle));
        sprintf(grtitle, "Event%d_Track_XZ;strip;arrival time/40ns", jentry);
        peakingtime_x->SetTitle(grtitle);
        peakingtime_x->SetName(grtitle);

        peakingtime_y->SetMarkerStyle(7);
        memset(grtitle, 0, sizeof(grtitle));
        sprintf(grtitle, "Event%d_Track_YZ;strip;arrival time/40ns", jentry);
        peakingtime_y->SetTitle(grtitle);
        peakingtime_y->SetName(grtitle);

        energy_lost_x->Set(0);
        energy_lost_y->Set(0);
        peakingtime_x->Set(0);
        peakingtime_y->Set(0);
        
        x_count = 0;            //to count the x hits number in an event
        y_count = 0;            //to count the y hits number in an event

        memset(x_timerange, 0, sizeof(x_timerange));
        memset(y_timerange, 0, sizeof(y_timerange));
        memset(x_range, 0, sizeof(x_range));
        memset(y_range, 0, sizeof(y_range));
        memset(x_Emax, 0, sizeof(x_Emax));
        memset(y_Emax, 0, sizeof(y_Emax));

        summaxADC1 = 0;
        summaxADC2 = 0;

        memset(source, 0, sizeof(source));
        memset(dest, 0, sizeof(dest));
        nfound = 0;
        memset(ypeaks, 0, sizeof(ypeaks));
        summax_new = 0;

        memset(Var_slope, 0, sizeof(Var_slope));
        memset(slope_bar, 0, sizeof(slope_bar));
        memset(slope_2_bar, 0, sizeof(slope_2_bar));
        memset(E_xy, 0, sizeof(E_xy));
        memset(E_x, 0, sizeof(E_x));
        memset(E_x2, 0, sizeof(E_x2));
        memset(E_y, 0, sizeof(E_y));
        memset(E_y2, 0, sizeof(E_y2));
        memset(rho, 0, sizeof(rho));
        cut_event_flag = false;
        cut_saturated = false;
        cut_event_flag2[0] = true;
        cut_event_flag2[1] = true;

        XZ_points_in.clear();
        YZ_points_in.clear();
        X_tracklength = 0;
        Y_tracklength = 0;

        //fill 
        for(int i=0; i<nHits;i++){                     //********

            if(Chn[i]>67) continue;               //drop some wrong events
            if(maxADC[i]==0) continue;            //added on 2023.10.28, to cut the empty waveforms from simulation

            pedM= pedestal[mapL2[Fec[i]]][Chip[i]][Chn[i]][0];                    //mean is the 4th column of pedestal file
            pedS= pedestal[mapL2[Fec[i]]][Chip[i]][Chn[i]][1];                    //siga is the 5th column of pedestal file

            sumADC[i]=sumADC[i]-pedM*Nsp;
            

            // //calculate pedM dynamically(for beta typically)
            // pedM = 0;
            // for(int k = 300; k < 500; k++){
            //   pedM += ADC[i][k];
            // }
            // pedM = pedM/200;

            maxADC[i]=maxADC[i]-pedM;
            if(maxADC[i] < 10*pedS)  continue;

            // if (Chn[i] == 0 || Chn[i] == 67) continue;              //wrong signal in these two channals
            // if (pixelX[i]==0 && pixelY[i]==0) continue;                 //not a signal channel
            // if(maxPoint[i]>140||maxPoint[i]<100) continue;          //peaking time

            if((Chip[i]==2||Chip[i]==0) && (Chn[i]==67/* ||Chn[i]==66 */))  continue;

            //using 20% peak time as signal arrival time, can choose to use or not
            for(int k = maxPoint[i]; k > maxPoint[i]-100; k--){
                if(ADC[i][k]<=maxADC[i]*0.2+pedM){
                  maxPoint[i] = k + (maxADC[i]*0.2+pedM-ADC[i][k])/(ADC[i][k+1]-ADC[i][k]);
                  break;
                }
            }

            //cut the events that has a maxPoint[i] too big
            // if(maxPoint[i]>350) cut_event_flag = true;

            //NEW! Used when doing anti coincidence, if the upper detector (connecting to fec 29) has efficient hits, then drop this event
            if(pixelX[i]!=0 && pixelY[i]!=0){ 
              cut_event_flag = true;
              continue;         //this means don't count the Fec_30 hits
            }

            // //NEW!To select coincidence events, if both the two detectors are hit, then record the event information of the TPC
            // if((pixelX[i]!=0 && pixelY[i]==0) || (pixelY[i]!=0 && pixelX[i]==0)){
            //   cut_event_flag2[0] = false;
            // }
            // else if(pixelX[i]!=0 && pixelY[i]!=0){
            //   cut_event_flag2[1] = false;
            // }

            // //cut the events that hits the edge strips
            // if(pixelX[i]!=0 && (pixelX[i]<15||pixelX[i]>105) && pixelY[i]==0){
            //   cut_event_flag = true;
            //   //break;            //this means skip this event
            // }
            // if(pixelY[i]!=0 && (pixelY[i]<15||(pixelY[i]>105)) && pixelX[i]==0){
            //   cut_event_flag = true;
            //   //break;            //this means skip this event
            // }

            // //For coincidence only, don't count the MM hits
            // if(pixelX[i]!=0 && pixelY[i]!=0){ 
            //   continue;         //this means don't count the Fec_30 hits
            // }


            if(MultiPeak == false){
              //Note: the below part convert the ADC paeks into  collected charge(in pC)
                if(Is_small_range == true){
                  summax_new += maxADC[i];       //if necessary, this can be converted to charge(as below)
                }
                else{
                  // summax_new += 0.009194557*maxADC[i];     //unit of charge is pC
                  if(maxADC[i]<105)  summax_new += 0.0206*maxADC[i];
                  else if(maxADC[i]<330) summax_new += 0.008*maxADC[i]+1.421;
                  else  summax_new += 0.0069*maxADC[i]+1.423;
                }
            }
            else{
              //for peak finding, save the current waveform into an array
              for(int k = 0; k < Nsp; k++){
                source[k] = ADC[i][k];
              }   

              nfound = s->SearchHighRes(source, dest, Nsp, 6, 30, kTRUE, 3, kTRUE, 3);
              Double_t *x_peaks = s->GetPositionX();         //get the position of the found peaks
            

            // if (Chn[i] == 0 || Chn[i] == 67) continue;              //wrong signal in these two channals
            // if (pixelX[i]==0 && pixelY[i]==0) continue;                 //not a signal channel
            // if(maxPoint[i]>140||maxPoint[i]<100) continue;          //peaking time
            
              for(int k = 0; k < nfound; k++){
                xpeaks[k] = *(x_peaks+k);
                ypeaks[k] = ADC[i][(int)xpeaks[k]] - pedM;      //get the y position of found peak
                //summax_new += ypeaks[k];

                //using 50% peak time as signal arrival time, can choose to use or not
                for(int m = xpeaks[k]; m > xpeaks[k]-40; m--){
                    if(ADC[i][m]<=ypeaks[k]*0.5+pedM){
                      xpeaks[k] = m;
                      break;
                    }
                }

                //don't fill those peaks with too low amplitude
                if(ypeaks[k]< 5*pedS) ypeaks[k]=0;

                //Note: the below part convert the ADC paeks into  collected charge(in pC)
                if(Is_small_range == true){
                  summax_new += ypeaks[k];       //if necessary, this can be converted to charge(as below)
                }
                else{
                  summax_new += 0.009194557*ypeaks[k];     //unit of charge is pC
                }
              } 
            }

            //fill the 4 graphs
            if (pixelX[i]!=0 && pixelY[i]==0) {                         //this is an x hit
                if(maxADC[i]+pedM>=4095) cut_saturated = true;
                if(MultiPeak == false){
                  energy_lost_x->SetPoint(x_count, pixelX[i], maxADC[i]);
                  // peakingtime_x->SetPoint(x_count, pixelX[i], maxPoint[i]);
                  
                  XZ_points_in.push_back(Point(pixelX[i],maxPoint[i]));

                  // if(x_count>0){
                  //   slope[0] = (maxPoint[i]-peakingtime_x->GetPointY(x_count-1))/(pixelX[i]-peakingtime_x->GetPointX(x_count-1));
                  //   slope_bar[0] += slope[0];
                  //   slope_2_bar[0] += slope[0]*slope[0];
                  // }

                  E_xy[0] += pixelX[i]*maxPoint[i];
                  E_x[0] +=pixelX[i];
                  E_x2[0] += pixelX[i]*pixelX[i];
                  E_y[0] += maxPoint[i];
                  E_y2[0] += maxPoint[i]*maxPoint[i];
                  x_count++;
                  effhit++;

                  //for alpha test
                  if(maxPoint[i]<x_timerange[1]||x_count==1){
                    x_timerange[0] = pixelX[i];
                    x_timerange[1] = maxPoint[i];}
                  if(maxPoint[i]>x_timerange[3]){
                    x_timerange[2] = pixelX[i];
                    x_timerange[3] = maxPoint[i];}
                  
                  if(maxADC[i]>x_Emax[1]){
                    x_Emax[0] = pixelX[i];
                    x_Emax[1] = maxADC[i];
                    time_of_Emax_x = maxPoint[i];
                  }
                  
                  //for start position reconstruction(beta particularly)
                  if(pixelX[i]<=x_range[0]||x_range[0]==0){
                    x_range[0] = pixelX[i];
                    x_range[1] = maxPoint[i];
                  }
                  if(pixelX[i]>=x_range[2]){
                    x_range[2] = pixelX[i];
                    x_range[3] = maxPoint[i];
                  }
                  
                }
                else{
                  for(int k = 0; k < nfound; k++){

                  if(ypeaks[k]<5*pedS)  continue;       //Possibly a noise "signal"
                  
                  energy_lost_x->SetPoint(x_count, pixelX[i], ypeaks[k]);
                  // peakingtime_x->SetPoint(x_count, pixelX[i], xpeaks[k]);

                  XZ_points_in.push_back(Point(pixelX[i],xpeaks[k]));
                  
                  E_xy[0] += pixelX[i]*xpeaks[k];
                  E_x[0] +=pixelX[i];
                  E_x2[0] += pixelX[i]*pixelX[i];
                  E_y[0] += xpeaks[k];
                  E_y2[0] += xpeaks[k]*xpeaks[k];
                  x_count++;
                  effhit++;

                  //for alpha test
                  if(xpeaks[k]<x_timerange[1]||x_count==1){
                    x_timerange[0] = pixelX[i];
                    x_timerange[1] = xpeaks[k];}
                  if(xpeaks[k]>x_timerange[3]){
                    x_timerange[2] = pixelX[i];
                    x_timerange[3] = xpeaks[k];}

                  if(ypeaks[k]>x_Emax[1]){
                    x_Emax[0] = pixelX[i];
                    x_Emax[1] = ypeaks[k];
                    time_of_Emax_x = xpeaks[k];
                  }
                  
                  //for start position reconstruction(beta particularly)
                  if(pixelX[i]<=x_range[0]||x_range[0]==0){
                    x_range[0] = pixelX[i];
                    if(x_range[1]<=xpeaks[k]) x_range[1]=xpeaks[k];
                  }
                  if(pixelX[i]>=x_range[2]){
                    x_range[2] = pixelX[i];
                    if(x_range[3]<=xpeaks[k]) x_range[3]=xpeaks[k];
                  }
                  }
                }  
            }
            else if (pixelY[i]!=0 && pixelX[i]==0) {                    //this is a y hit
                if(maxADC[i]+pedM>=4095) cut_saturated = true;
                if(MultiPeak == false){
                  energy_lost_y->SetPoint(y_count, pixelY[i], maxADC[i]);
                  // peakingtime_y->SetPoint(y_count, pixelY[i], maxPoint[i]);

                  YZ_points_in.push_back(Point(pixelY[i],maxPoint[i]));

                  // if(y_count>0){
                  //   slope[1] = (maxPoint[i]-peakingtime_y->GetPointY(y_count-1))/(pixelY[i]-peakingtime_y->GetPointX(y_count-1));
                  //   slope_bar[1] += slope[1];
                  //   slope_2_bar[1] += slope[1]*slope[1];
                  // }
                  
                  E_xy[1] += pixelY[i]*maxPoint[i];
                  E_x[1] += pixelY[i];
                  E_x2[1] += pixelY[i]*pixelY[i];
                  E_y[1] += maxPoint[i];
                  E_y2[1] += maxPoint[i]*maxPoint[i];
                  y_count++;
                  effhit++;

                  //for alpha test
                  if(maxPoint[i]<y_timerange[1]||y_count==1){
                    y_timerange[0] = pixelY[i];
                    y_timerange[1] = maxPoint[i];}
                  if(maxPoint[i]>y_timerange[3]){
                    y_timerange[2] = pixelY[i];
                    y_timerange[3] = maxPoint[i];}

                  if(maxADC[i]>y_Emax[1]){
                    y_Emax[0] = pixelY[i];
                    y_Emax[1] = maxADC[i];
                    time_of_Emax_y = maxPoint[i];
                  }

                  //for start position reconstruction(beta particularly)
                  if(pixelY[i]<=y_range[0]||y_range[0]==0){
                    y_range[0] = pixelY[i];
                    y_range[1] = maxPoint[i];
                  }
                  if(pixelY[i]>=y_range[2]){
                    y_range[2] = pixelY[i];
                    y_range[3] = maxPoint[i];
                  }
                }
                else{
                  for(int k = 0; k < nfound; k++){

                  if(ypeaks[k]<5*pedS)  continue;       //Possibly a noise "signal"

                  energy_lost_y->SetPoint(y_count, pixelY[i], ypeaks[k]);
                  // peakingtime_y->SetPoint(y_count, pixelY[i], xpeaks[k]);

                  YZ_points_in.push_back(Point(pixelY[i],xpeaks[k]));

                  E_xy[1] += pixelY[i]*xpeaks[k];
                  E_x[1] += pixelY[i];
                  E_x2[1] += pixelY[i]*pixelY[i];
                  E_y[1] += xpeaks[k];
                  E_y2[1] += xpeaks[k]*xpeaks[k];
                  y_count++;
                  effhit++;
                  
                  //for alpha test
                  if(xpeaks[k]<y_timerange[1]||y_count==1){
                    y_timerange[0] = pixelY[i];
                    y_timerange[1] = xpeaks[k];}
                  if(xpeaks[k]>y_timerange[3]){
                    y_timerange[2] = pixelY[i];
                    y_timerange[3] = xpeaks[k];}

                  if(ypeaks[k]>y_Emax[1]){
                    y_Emax[0] = pixelY[i];
                    y_Emax[1] = ypeaks[k];
                    time_of_Emax_y = xpeaks[k];
                  }
                  
                  //for start position reconstruction(beta particularly)
                  if(pixelY[i]<=y_range[0]||y_range[0]==0){
                    y_range[0] = pixelY[i];
                    if(y_range[1]<=xpeaks[k])  y_range[1]=xpeaks[k];
                  }
                  if(pixelY[i]>=y_range[2]){
                    y_range[2] = pixelY[i];
                    if(y_range[3]<=xpeaks[k]) y_range[3]=xpeaks[k];
                  }
                  }
                }
            }
            else{             //this signal is NOT from a detector channel, drop it
                continue;
            }

            //ONLY for charge distribution test, fill the two spectrums
            if(pixelX[i]!=0||pixelY[i]!=0){
              if(Chip[i]==0||Chip[i]==1){
                summaxADC1 = summaxADC1+maxADC[i];
              }
              else{
                summaxADC2 = summaxADC2+maxADC[i];
              }
            }

        }//single event 

        vector<Point> XZ_points_out = findNearestNeighbors(XZ_points_in, X_tracklength);
        vector<Point> YZ_points_out = findNearestNeighbors(YZ_points_in, Y_tracklength);

        if(cut_event_flag == true)  continue;

        // if(cut_event_flag2[0] == true || cut_event_flag2[1] == true)  continue;       //for coincidence only

        if(x_count==0 || y_count==0)  continue;

        // if(x_Emax[1]>3600||y_Emax[1]>3600)  continue;       //cut out the out-of-range events
        if(cut_saturated) continue;               //cut out the out-of-range events (revised version 2023.12.13)
        

        // if(x_timerange[3]>y_timerange[3]-40 && x_timerange[3]<y_timerange[3]+40)  continue;

        
        //if(x_count>40||TMath::Abs(x_count-y_count)>20) continue;      //cut out the abnormal alpha bkg

        

        int id=0;
    
        for (const Point& XZ_point : XZ_points_out) {
          peakingtime_x->SetPoint(id, XZ_point.x, XZ_point.y);
          if(id>0){
            slope[0] = (XZ_point.y-peakingtime_x->GetPointY(id-1))/(XZ_point.x-peakingtime_x->GetPointX(id-1));
            slope_bar[0] += slope[0];
            slope_2_bar[0] += slope[0]*slope[0];
          }

          // E_xy[0] += XZ_point.x*XZ_point.y;
          // E_x[0] +=XZ_point.x;
          // E_x2[0] += XZ_point.x*XZ_point.x;
          // E_y[0] += XZ_point.y;
          // E_y2[0] += XZ_point.y*XZ_point.y;

          id++;
        }

        id=0;

        for (const Point& YZ_point : YZ_points_out) {
          peakingtime_y->SetPoint(id, YZ_point.x, YZ_point.y);
          if(id>0){
            slope[1] = (YZ_point.y-peakingtime_y->GetPointY(id-1))/(YZ_point.x-peakingtime_y->GetPointX(id-1));
            slope_bar[1] += slope[1];
            slope_2_bar[1] += slope[1]*slope[1];
          }
          
          // E_xy[1] += YZ_point.x*YZ_point.y;
          // E_x[1] += YZ_point.x;
          // E_x2[1] += YZ_point.x*YZ_point.x;
          // E_y[1] += YZ_point.y;
          // E_y2[1] += YZ_point.y*YZ_point.y;
          
          id++;
        }

        if(x_count>1 && y_count>1){
            slope_bar[0] /= (x_count-1);
            slope_2_bar[0] /= (x_count-1);
            slope_bar[1] /= (y_count-1);
            slope_2_bar[1] /= (y_count-1);

            Var_slope[0] = slope_2_bar[0]- slope_bar[0]*slope_bar[0];
            Var_slope[1] = slope_2_bar[1]- slope_bar[1]*slope_bar[1];
        }
        if(Var_slope[0]>20) Var_slope[0] = 20;
        if(Var_slope[1]>20) Var_slope[1] = 20;

        if(x_count>0){
          E_xy[0] /= x_count;
          E_x[0] /= x_count;
          E_x2[0] /= x_count;
          E_y[0] /= x_count;
          E_y2[0] /= x_count;
        }
        if(y_count>0){
          E_xy[1] /= y_count;
          E_x[1] /= y_count;
          E_x2[1] /= y_count;
          E_y[1] /= y_count;
          E_y2[1] /= y_count;
        }

        if((E_x2[0]-E_x[0]*E_x[0])!=0 && (E_y2[0]-E_y[0]*E_y[0])!=0){
          rho[0] = (E_xy[0]-E_x[0]*E_y[0])/TMath::Sqrt((E_x2[0]-E_x[0]*E_x[0])*(E_y2[0]-E_y[0]*E_y[0]));
        }
        else rho[0] = 0;
        if((E_x2[1]-E_x[1]*E_x[1])!=0 && (E_y2[1]-E_y[1]*E_y[1])!=0){
          rho[1] = (E_xy[1]-E_x[1]*E_y[1])/TMath::Sqrt((E_x2[1]-E_x[1]*E_x[1])*(E_y2[1]-E_y[1]*E_y[1]));
        }
        else rho[1] = 0;

        sqrt_hit = TMath::Sqrt(x_count*x_count+y_count*y_count);

        if(x_count==0){
          timerange[0]=y_timerange[1];
          timerange[1]=y_timerange[3];
        }
        else if(y_count==0){
          timerange[0]=x_timerange[1];
          timerange[1]=x_timerange[3];
        }
        else{
          if(x_timerange[1]<y_timerange[1]) timerange[0]=x_timerange[1];
          else  timerange[0]=y_timerange[1];
          if(x_timerange[3]>y_timerange[3]) timerange[1]=x_timerange[3];
          else  timerange[1]=y_timerange[3];
        }

        // Cot_theta = (timerange[1]-timerange[0])*6.5/40/(sqrt_hit*0.133);       // calculate the "cotangent theta", with the length units concerned
        Cot_theta = (timerange[1]-timerange[0]) * 40 * 3.5e-3/(sqrt_hit*0.133);
        dE_dx = summax_new/sqrt_hit;

        // //Charge mass center method for position reconstruction, ONLY for Fe55 test
        // //fill the hitsmap and central region spectrum
        // if(sumA[0]*sumA[1]!=0){
        //   for(int d=0;d<2;d++){
        //     start_pos[d] = sumAP[d]/sumA[d];        //position reconstructionn
        //   }
        //   //std::cout<<"posX ="<<start_pos[0]<<"; posY ="<<start_pos[1]<<std::endl;
        // }
        // else continue;

        //start position reconstruction for alpha
        start_pos[0] = x_timerange[2];
        start_pos[1] = y_timerange[2];

        // //start position reconstruction for beta
        // if(x_range[1]>x_range[3]) start_pos[0] = x_range[0];
        // else  start_pos[0] = x_range[2];
        // if(y_range[1]>y_range[3]) start_pos[1] = y_range[0];
        // else  start_pos[1] = y_range[2];

        // if(timerange[1]-timerange[0]<80)  continue;

        N_eff++;
        if(x_Emax[1]>y_Emax[1]){        //means the max energy lost is on x strip
          max_energy_lost_pos->Fill((timerange[1]-time_of_Emax_x)/(timerange[1]-timerange[0]));
        }
        else{               //means the max energy lost is on y strip
          max_energy_lost_pos->Fill((timerange[1]-time_of_Emax_y)/(timerange[1]-timerange[0]));
        }

        effhits_hist->Fill(effhit);         //fill the efficient hit hist
        xhits_hist->Fill(x_count);
        yhits_hist->Fill(y_count);
        spectrum->Fill(summax_new);          //Fill the spectrum with this event
        
        // //ONLY for charge distribution test
        // if(summaxADC1!=0) {
        //   spectrum1->Fill(summaxADC1);
        //   spectrum2->Fill(summaxADC2);
        // }

        energy_vs_hits->Fill(sqrt_hit, summax_new);
        peakingtime_diff->Fill(timerange[1]-timerange[0]);


        effhitsmap->Fill(start_pos[0],start_pos[1]);
				if(start_pos[0]>40&&start_pos[0]<80&&start_pos[1]>40&&start_pos[1]<80){
          spectrum_central->Fill(summax_new);
        }
        energy_vs_angle->Fill(Cot_theta, summax_new);
        //x_slope_var->Fill(Var_slope[0]);
        //y_slope_var->Fill(Var_slope[1]);
        rho_x->Fill(rho[0]);
        rho_y->Fill(rho[1]);
        hits_diff->Fill(TMath::Abs(x_count-y_count));
        energy_per_length->Fill(dE_dx);
        incident_angle->Fill(Cot_theta);
        tracklength_x->Fill(X_tracklength);
        tracklength_y->Fill(Y_tracklength);
        energy_vs_transittime->Fill(timerange[1]-timerange[0],summax_new);
        min_time_xy->Fill(x_timerange[1],y_timerange[1]);
        max_time_xy->Fill(x_timerange[3],y_timerange[3]);
        transittime_xy->Fill(x_timerange[3]-x_timerange[1],y_timerange[3]-y_timerange[1]);

        //Write and reset the 4 graphs of this event
        // if(effhit>2 && jentry<100){
        //     //  if(event<100&&nHitsUp>3)
        //     energy_lost_x->Write();
        //     energy_lost_y->Write();
        //     peakingtime_x->Write();
        //     peakingtime_y->Write();
        // }

        //--------------------------Alpha Cuts--------------------
        
        
        
        //--------------------------------------------------------
        
        //--------------------------Beta Cuts---------------------

        //if(effhit>40) continue;
        // if(TMath::Abs(rho[0])<0.99||TMath::Abs(rho[1])<0.99)  continue;
        // if((timerange[1]-timerange[0])<450||(timerange[1]-timerange[0])>600) continue;
        // if(cut_event_flag == true)  continue;
        // if(cut_event_flag2[0] == true || cut_event_flag2[1] == true)  continue;       //for coincidence only
        // if(timerange[1]-timerange[0]<80)  continue;
        // if(start_pos[0]<15||start_pos[0]>105||start_pos[1]<15||start_pos[1]>105)  continue;
        double d_center = sqrt(pow(start_pos[0]-60.5,2)+pow(start_pos[1]-60.5,2))*1.33;
        if(d_center>35) continue;

        // cut for simulation events, in order to remove short tracks, mainly low energy electrons
        // if(X_tracklength<2 && Y_tracklength<2) continue;

        // if(start_pos[0]<15||start_pos[0]>81||start_pos[1]<15||start_pos[1]>82)  continue;
        // if(start_pos[0]>21&&start_pos[0]<76&&start_pos[1]>20&&start_pos[1]<78)  continue;

        // if(x_Emax[1]>y_Emax[1]){        //means the max energy lost is on x strip
        //   if((timerange[1]-time_of_Emax_x)/(timerange[1]-timerange[0])>0.05)  continue;
        // }
        // else{               //means the max energy lost is on y strip
        //   if((timerange[1]-time_of_Emax_y)/(timerange[1]-timerange[0])>0.05)  continue;
        // }
        

        //--------------------------------------------------------

        //Fill the histograms after selection
        spectrum_2->Fill(summax_new);
        effhits_hist_2->Fill(effhit);   
        effhitsmap_x_2->Fill(start_pos[0]*1.33);
        effhitsmap_y_2->Fill(start_pos[1]*1.33);      
        xhits_hist_2->Fill(x_count);
        yhits_hist_2->Fill(y_count);
        peakingtime_diff_2->Fill(timerange[1]-timerange[0]);
        effhitsmap_2->Fill(start_pos[0],start_pos[1]);

        if(x_Emax[1]>y_Emax[1]){        //means the max energy lost is on x strip
          max_energy_lost_pos_2->Fill((timerange[1]-time_of_Emax_x)/(timerange[1]-timerange[0]));
        }
        else{               //means the max energy lost is on y strip
          max_energy_lost_pos_2->Fill((timerange[1]-time_of_Emax_y)/(timerange[1]-timerange[0]));
        }
        hits_diff_2->Fill(TMath::Abs(x_count-y_count));
        energy_vs_hits_2->Fill(sqrt_hit, summax_new);
        energy_per_length_2->Fill(dE_dx);
        incident_angle_2->Fill(Cot_theta);
        tracklength_x_2->Fill(X_tracklength);
        tracklength_y_2->Fill(Y_tracklength);
        energy_vs_angle_2->Fill(Cot_theta, summax_new);
        energy_vs_transittime_2->Fill(timerange[1]-timerange[0],summax_new);
        min_time_xy_2->Fill(x_timerange[1],y_timerange[1]);
        max_time_xy_2->Fill(x_timerange[3],y_timerange[3]);
        transittime_xy_2->Fill(x_timerange[3]-x_timerange[1],y_timerange[3]-y_timerange[1]);

        if(N_a<100){
            energy_lost_x->Write();
            energy_lost_y->Write();
            peakingtime_x->Write();
            peakingtime_y->Write();
            // if(events<100) {
            event_num[events] = jentry;
            events++;
            // }
        }
        N_a++;
    }//loop end 

    //show the first 20 saved events' waveform
    Waveform(RootFileName, events, event_num);
    //write the hits histogram, spectrum histogram, hitsmap
    outFile->cd();
    effhits_hist->Write();
    xhits_hist->Write();
    yhits_hist->Write();
    //----------spectrum fit and write-----------------
    gaus1->SetRange(0.8*spectrum->GetBinCenter(spectrum->GetMaximumBin()),1.3*spectrum->GetBinCenter(spectrum->GetMaximumBin()));
    spectrum->Fit(gaus1,"R0");                  //R表示Use the Range specified in the function range，0表示Do not plot the result of the fit. 
    double par1[3];                             //pre-fit gaus parameters
    double par2[4];                             //fit antilangaus parameters
    gaus1->GetParameters(par1);
    gaus1->Write();

    par2[0]=par1[2]*0.2;                    //landau width parameter
    par2[1]=par1[1];                        //Maximam position of the landau
    par2[2]=spectrum->Integral("width");                    //area(scaling) parameter
    par2[3]=par1[2]*0.8;                                           //gaus sigma
    antilangau->SetParameters(par2);
    antilangau->SetRange(par1[1]-8*par1[2],par1[1]+4*par1[2]);
    antilangau->SetParLimits(0,par2[0]*0.2,par2[0]*1.5);              //set the antilangau fit range
    antilangau->SetParLimits(1,par2[1]*0.5,par2[1]*1.8);
    antilangau->SetParLimits(2,par2[2]*0.5,par2[2]*2);
    antilangau->SetParLimits(3,par2[3]*0.2,par2[3]*1.5);

    spectrum->Fit(antilangau,"R");
    antilangau->GetParameters(par2);
    cout<< "\nEnergy resolution from the Anti-langau fit: "
        << fixed << setprecision(2) << 2.354*par2[3]/antilangau->GetMaximumX()*100 << "%"<< "\n" << endl;
    spectrum->Write();
    //---------end spectrum writing-----------------

    peakingtime_diff->Write();
    energy_vs_hits->Write();
    max_energy_lost_pos->Write();

    spectrum_central->Fit(gaus1,"R");
    spectrum_central->Write();
    //spectrum1->Write();
    //spectrum2->Write();
    effhitsmap->Write();
    incident_angle->Write();
    tracklength_x->Write();
    tracklength_y->Write();
    energy_vs_angle->Write();
    //x_slope_var->Write();
    //y_slope_var->Write();
    rho_x->Write();
    rho_y->Write();
    hits_diff->Write();
    energy_per_length->Write();
    energy_vs_transittime->Write();
    min_time_xy->Write();
    max_time_xy->Write();
    transittime_xy->Write();

    spectrum_2->Write();
    effhits_hist_2->Write();         //fill the efficient hit hist
    effhitsmap_x_2->Fit(gaus2,"R");
    effhitsmap_x_2->Write();
    effhitsmap_y_2->Fit(gaus2,"R");
    effhitsmap_y_2->Write();  
    xhits_hist_2->Write();
    yhits_hist_2->Write();
    peakingtime_diff_2->Write();
    effhitsmap_2->Write();
    max_energy_lost_pos_2->Write();
    hits_diff_2->Write();
    energy_vs_hits_2->Write();
    energy_per_length_2->Write();
    incident_angle_2->Write();
    tracklength_x_2->Write();
    tracklength_y_2->Write();
    energy_vs_angle_2->Write();
    energy_vs_transittime_2->Write();
    min_time_xy_2->Write();
    max_time_xy_2->Write();
    transittime_xy_2->Write();


    //effhits_hist->Write();
    //peakingtime->Write();
    //effhitsmap->Write();
    outFile->Close();

    cout<<"Efficient events: "<< N_eff << " Left events after cuts: " << N_a << "\n" <<endl;
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

//define main()
string filename;
void helpinfo(){

  cout<<"Usage is ./Hitsmap.exe ./Raw2ROOT/<filename>\n";
  cout<<"default output file name is <inputfilename.eps>"<<endl;	
  return;
} 
void phrase_command(int argc,char *argv[]){
  if (argc<2){ helpinfo();
    exit(0);
  }else {filename=(string) argv[1]; cout<<"START\n~~~~~*****~~~~~\nInput File : "<<filename<<endl;}
}

int main(int argc,char *argv[])
{

  phrase_command(argc,argv);
  string Tt="richraw";
  const char *TtreeName=(char *)(Tt.data());
  const Text_t* RawRootFile=(Text_t*)(filename.data());
  char *RawRootFile_c=(char *)(filename.data());
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(RawRootFile);
  if (!f) 
    {
      f = new TFile(RawRootFile_c);
     }
  TTree *tree = (TTree*)gDirectory->Get(TtreeName);
 
  RootConvert xx;
  xx.RootConvert::Init(tree);
  xx.RootConvert::Loop(RawRootFile_c);
    cout<<"\n~~~~~*****~~~~~\nTHE END"<<endl;
  return 1;

} 

