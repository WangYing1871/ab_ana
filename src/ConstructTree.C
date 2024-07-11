#define ConstructTree_cxx
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
#include "TSpectrum.h"
#include "ConstructTree.h"

using namespace std;
using namespace RICHsystem;
class RootConvert;
void RootConvert::Loop2(TTree *signaltree, TTree *bkgtree, double threshold) {}
void RootConvert::Loop3(char *RootFileName, double threshold){}

// This loop takes the events in the rawroot file and construct some useful variables for MVA classification
// then save the variables of each event in a tree
void RootConvert::Loop(char *RootFileName)
{
  string namesuffix = "_selection2_tree";
  string outputfile = (string)RootFileName;
  outputfile.replace(strlen(RootFileName) - 5, 5, namesuffix);
  int Lname = sizeof(outputfile.c_str());
  string out;
  out.assign(outputfile, 11, Lname - 11);
  string outfilename = "./VariableTree/" + out + ".root";

  cout << "Output file name: " << outfilename << "\n"
       << endl;
  // create a specific root file for this data set in the folder "./ConstructTree/"
  TFile *outfile = new TFile(outfilename.c_str(), "RECREATE");

  // load the pedestal file
  // char pedestaldata[80] ="./Calibration/20240126153039492_baseline_25MHz_500ns_1pC_pedestal.txt";
  char pedestaldata[80] = "./Calibration/simulation_baseline.txt";
  LoadPedestal(pedestaldata); //*****************

  Bool_t MultiPeak = false; // flag if use peak finding method

  double pedS, pedM;
  int x_count, y_count;
  int count_diff; // difference of x_count and y_count
  Float_t count;
  Double_t source[Nsp]; // save the current waveform
  Double_t dest[Nsp];   // save the analysed output waveform
  Int_t nfound;         // number of peaks found
  TSpectrum *s = new TSpectrum();
  Double_t xpeaks[100]; // peak x position
  Float_t ypeaks[100];  // peak y position
  Float_t summax_new;
  Float_t x_range[4], y_range[4];
  // x_range[0]:the minimum hit channel of x strip graph
  // x_range[1]:the peaking time at the minimum hit channel of x strip graph
  // x_range[2]:the maximum hit channel of x strip graph
  // x_range[3]:the peaking time at the maximum hit channel of x strip graph
  Float_t x_timerange[4], y_timerange[4];
  // x_timerange[0]:the x coordinate of min time point in x strip graph
  // x_timerange[1]:the y coordinate of min time oint in x strip graph
  // x_timerange[2]:the x coordinate of max time point in x strip graph
  // x_timerange[3]:the y coordinate of max time point in x strip graph
  Float_t start_pos_x, start_pos_y;
  Float_t timerange[2];
  Float_t x_Emax, y_Emax;
  Float_t time_of_Emax_x, time_of_Emax_y;
  Float_t timediff;
  Float_t E_xy[2];
  Float_t E_x[2];
  Float_t E_x2[2];
  Float_t E_y[2];
  Float_t E_y2[2];

  Float_t ratio;
  Float_t incident_angle; // new variable to represent the cotanget of incident angle (between the track and z axis)
  Float_t rho_x, rho_y;
  Float_t startpos_x_MC, startpos_y_MC, startpos_z_MC, kinE_start_MC;

  Bool_t cut_saturated = false;       // become true if there is at least one saturated channel in this event 
  Bool_t cut_event_flag = false;
  Bool_t cut_event_flag2[2]; // the new boolean variable to select the coincidence events

  int n1 = 0;       //count the events that passes first level selection

  // this part is for calculating track length
  vector<Point> XZ_points_in;
  vector<Point> YZ_points_in;
  Float_t X_tracklength;
  Float_t Y_tracklength;

  // new mapL2
  std::map<Int_t, Int_t> mapL2;
  mapL2.insert(pair<Int_t, Int_t>(29, 0)); // fec 29--->tag 0
  mapL2.insert(pair<Int_t, Int_t>(15, 1)); // fec 15--->tag 1

  TTree *tree0 = new TTree("variabletree", "variabletree");

  tree0->Branch("energy", &summax_new, "summax_new/F");
  tree0->Branch("hits", &count, "count/F");
  tree0->Branch("x_hits", &x_count, "x_count/I");
  tree0->Branch("y_hits", &y_count, "y_count/I");
  tree0->Branch("hits_diff", &count_diff, "count_diff/I");
  tree0->Branch("timediff", &timediff, "timediff/F");
  tree0->Branch("start_pos_x", &start_pos_x, "start_pos_x/F");
  tree0->Branch("start_pos_y", &start_pos_y, "start_pos_y/F");
  tree0->Branch("ratio", &ratio, "ratio/F");
  tree0->Branch("incident_angle", &incident_angle, "incident_angle/F");
  tree0->Branch("rho_x", &rho_x, "rho_x/F");
  tree0->Branch("rho_y", &rho_y, "rho_y/F");
  tree0->Branch("tracklength_x", &X_tracklength, "X_tracklength/F");
  tree0->Branch("tracklength_y", &Y_tracklength, "Y_tracklength/F");
  tree0->Branch("startpos_x_MC", &startpos_x_MC, "startpos_x_MC/F");
  tree0->Branch("startpos_y_MC", &startpos_y_MC, "startpos_y_MC/F");
  tree0->Branch("startpos_z_MC", &startpos_z_MC, "startpos_z_MC/F");
  tree0->Branch("kinE_start_MC", &kinE_start_MC, "kinE_start_MC/F");

  // Begin filling the tree
  Long64_t entries, ientry;
  cout << "Loop Root File..." << endl;
  if (fChain == 0)
    return;
  entries = fChain->GetEntriesFast();
  cout << "Total event numbers: " << entries << endl;

  for (int jentry = 0; jentry < entries; jentry++)
  {
    // loop to read the events in the rawroot file
    ientry = LoadTree(jentry);
    if (ientry < 0)
      break;
    fChain->GetEntry(jentry);

    // loop entries
    if (jentry % ((int)(entries / 10.)) == 0)
    {
      // cout<<"Events:"<<jentry<<"("<<(int)(jentry*100./nentries)<<"%)"<<endl;
      cout << "Events:" << jentry << endl;
    }

    // reset
    x_count = 0;
    y_count = 0;
    count = 0;
    summax_new = 0;
    memset(x_timerange, 0, sizeof(x_timerange));
    memset(y_timerange, 0, sizeof(y_timerange));
    memset(x_range, 0, sizeof(x_range));
    memset(y_range, 0, sizeof(y_range));
    x_Emax = 0;
    y_Emax = 0;
    time_of_Emax_x = 0;
    time_of_Emax_y = 0;

    memset(E_xy, 0, sizeof(E_xy));
    memset(E_x, 0, sizeof(E_x));
    memset(E_x2, 0, sizeof(E_x2));
    memset(E_y, 0, sizeof(E_y));
    memset(E_y2, 0, sizeof(E_y2));
    rho_x = 0;
    rho_y = 0;
    cut_event_flag = false;
    cut_saturated = false;
    cut_event_flag2[0] = true;
    cut_event_flag2[1] = true;

    XZ_points_in.clear();
    YZ_points_in.clear();
    X_tracklength = 0;
    Y_tracklength = 0;

    for (int i = 0; i < nHits; i++)
    {

      if (Chn[i] > 67)
        continue; // drop some wrong events
      if (maxADC[i] == 0)
        continue; // added on 2023.10.28, to cut the empty waveforms from simulation

      pedM = pedestal[mapL2[Fec[i]]][Chip[i]][Chn[i]][0]; // mean is the 4th column of pedestal file
      pedS = pedestal[mapL2[Fec[i]]][Chip[i]][Chn[i]][1];

      sumADC[i] = sumADC[i] - pedM * Nsp;

      // summaxADC = summaxADC - pedM;

      // //calculate pedM dynamically(for beta typically)
      // pedM = 0;
      // for(int k = 0; k < 200; k++){
      //   pedM += ADC[i][k];
      // }
      // pedM = pedM/200;

      maxADC[i] = maxADC[i] - pedM;
      if(maxADC[i] < 10*pedS)  continue;

      // wrong signal in these two channals
      if ((Chip[i] == 2 || Chip[i] == 0) && (Chn[i] == 67/*  || Chn[i] == 66 */))
        continue;

      // using 50% peak time as signal arrival time, can choose to use or not
      for (int k = maxPoint[i]; k > maxPoint[i] - 100; k--)
      {
        if (ADC[i][k] <= maxADC[i] * 0.2 + pedM)
        {
          maxPoint[i] = k + (maxADC[i] * 0.2 + pedM - ADC[i][k]) / (ADC[i][k + 1] - ADC[i][k]);
          break;
        }
      }

      // NEW! Used when doing anti coincidence, if the upper detector (connecting to fec 15) has efficient hits, then drop this event
      if (pixelX[i]!=0 && pixelY[i]!=0)
      {
        cut_event_flag = true;
        continue;
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

      if (MultiPeak == false)
      {
        summax_new += maxADC[i]; // sum to calculate the energy
      }
      else
      {
        // for peak finding, save the current waveform into an array
        for (int k = 0; k < Nsp; k++)
        {
          source[k] = ADC[i][k];
        }

        nfound = s->SearchHighRes(source, dest, Nsp, 6, 30, kTRUE, 3, kTRUE, 3);
        Double_t *x_peaks = s->GetPositionX(); // get the position of the found peaks

        for (int k = 0; k < nfound; k++)
        {
          xpeaks[k] = *(x_peaks + k);
          ypeaks[k] = ADC[i][(int)xpeaks[k]] - pedM; // get the y position of found peak
          // summax_new += ypeaks[k];

          // using 50% peak time as signal arrival time, can choose to use or not
          for (int m = xpeaks[k]; m > xpeaks[k] - 40; m--)
          {
            if (ADC[i][m] <= ypeaks[k] * 0.5 + pedM)
            {
              xpeaks[k] = m + (ypeaks[k]*0.5+pedM-ADC[i][m])/(ADC[i][m+1]-ADC[i][m]);
              break;
            }
          }

          // don't fill those peaks with too low amplitude
          if (ypeaks[k] < 5 * pedS)
            ypeaks[k] = 0;
          summax_new += ypeaks[k]; // if necessary, this can be converted to charge(as below)
        }
      }

      if (pixelX[i] != 0 && pixelY[i]==0)
      {
        if(maxADC[i]+pedM>=4095) cut_saturated = true;
        if (MultiPeak == false)
        {
          XZ_points_in.push_back(Point(pixelX[i],maxPoint[i]));
          E_xy[0] += pixelX[i] * maxPoint[i];
          E_x[0] += pixelX[i];
          E_x2[0] += pixelX[i] * pixelX[i];
          E_y[0] += maxPoint[i];
          E_y2[0] += maxPoint[i] * maxPoint[i];
          x_count++;
          count++;

          if (maxPoint[i] < x_timerange[1] || x_count == 1)
          {
            x_timerange[0] = pixelX[i];
            x_timerange[1] = maxPoint[i];
          }
          if (maxPoint[i] > x_timerange[3])
          {
            x_timerange[2] = pixelX[i];
            x_timerange[3] = maxPoint[i];
          }
          if (maxADC[i] > x_Emax)
          {
            x_Emax = maxADC[i];
            time_of_Emax_x = maxPoint[i];
          }

          // for start position reconstruction(beta particularly)
          if (pixelX[i] <= x_range[0] || x_range[0] == 0)
          {
            x_range[0] = pixelX[i];
            x_range[1] = maxPoint[i];
          }
          if (pixelX[i] >= x_range[2])
          {
            x_range[2] = pixelX[i];
            x_range[3] = maxPoint[i];
          }
        }
        else
        {
          for (int k = 0; k < nfound; k++)
          {

            if (ypeaks[k] < 5 * pedS)
              continue; // Possibly a noise "signal"

            XZ_points_in.push_back(Point(pixelX[i],xpeaks[k]));
            E_xy[0] += pixelX[i] * xpeaks[k];
            E_x[0] += pixelX[i];
            E_x2[0] += pixelX[i] * pixelX[i];
            E_y[0] += xpeaks[k];
            E_y2[0] += xpeaks[k] * xpeaks[k];
            x_count++;
            count++;

            // for alpha test
            if (xpeaks[k] < x_timerange[1] || x_count == 1)
            {
              x_timerange[0] = pixelX[i];
              x_timerange[1] = xpeaks[k];
            }
            if (xpeaks[k] > x_timerange[3])
            {
              x_timerange[2] = pixelX[i];
              x_timerange[3] = xpeaks[k];
            }

            if (ypeaks[k] > x_Emax)
            {
              x_Emax = ypeaks[k];
              time_of_Emax_x = xpeaks[k];
            }

            // for start position reconstruction(beta particularly)
            if (pixelX[i] <= x_range[0] || x_range[0] == 0)
            {
              x_range[0] = pixelX[i];
              if (x_range[1] <= xpeaks[k])
                x_range[1] = xpeaks[k];
            }
            if (pixelX[i] >= x_range[2])
            {
              x_range[2] = pixelX[i];
              if (x_range[3] <= xpeaks[k])
                x_range[3] = xpeaks[k];
            }
          }
        }
      }
      else if (pixelY[i] != 0 && pixelX[i]==0)
      {
        if(maxADC[i]+pedM>=4095) cut_saturated = true;
        if (MultiPeak == false)
        {
          YZ_points_in.push_back(Point(pixelY[i],maxPoint[i]));
          E_xy[1] += pixelY[i] * maxPoint[i];
          E_x[1] += pixelY[i];
          E_x2[1] += pixelY[i] * pixelY[i];
          E_y[1] += maxPoint[i];
          E_y2[1] += maxPoint[i] * maxPoint[i];
          y_count++;
          count++;

          if (maxPoint[i] < y_timerange[1] || y_count == 1)
          {
            y_timerange[0] = pixelY[i];
            y_timerange[1] = maxPoint[i];
          }
          if (maxPoint[i] > y_timerange[3])
          {
            y_timerange[2] = pixelY[i];
            y_timerange[3] = maxPoint[i];
          }
          if (maxADC[i] > y_Emax)
          {
            y_Emax = maxADC[i];
            time_of_Emax_y = maxPoint[i];
          }

          // for start position reconstruction(beta particularly)
          if (pixelY[i] <= y_range[0] || y_range[0] == 0)
          {
            y_range[0] = pixelY[i];
            y_range[1] = maxPoint[i];
          }
          if (pixelY[i] >= y_range[2])
          {
            y_range[2] = pixelY[i];
            y_range[3] = maxPoint[i];
          }
        }
        else
        {
          for (int k = 0; k < nfound; k++)
          {
            if (ypeaks[k] < 5 * pedS)
              continue; // Possibly a noise "signal"

            YZ_points_in.push_back(Point(pixelY[i],xpeaks[k]));
            E_xy[1] += pixelY[i] * xpeaks[k];
            E_x[1] += pixelY[i];
            E_x2[1] += pixelY[i] * pixelY[i];
            E_y[1] += xpeaks[k];
            E_y2[1] += xpeaks[k] * xpeaks[k];
            y_count++;
            count++;

            // for alpha test
            if (xpeaks[k] < y_timerange[1] || y_count == 1)
            {
              y_timerange[0] = pixelY[i];
              y_timerange[1] = xpeaks[k];
            }
            if (xpeaks[k] > y_timerange[3])
            {
              y_timerange[2] = pixelY[i];
              y_timerange[3] = xpeaks[k];
            }

            if (ypeaks[k] > y_Emax)
            {
              y_Emax = ypeaks[k];
              time_of_Emax_y = xpeaks[k];
            }

            // for start position reconstruction(beta particularly)
            if (pixelY[i] <= y_range[0] || y_range[0] == 0)
            {
              y_range[0] = pixelY[i];
              if (y_range[1] <= xpeaks[k])
                y_range[1] = xpeaks[k];
            }
            if (pixelY[i] >= y_range[2])
            {
              y_range[2] = pixelY[i];
              if (y_range[3] <= xpeaks[k])
                y_range[3] = xpeaks[k];
            }
          }
        }
      }
      else
      { // this signal is NOT from a detector channel, drop it
        continue;
      }
    }

    findNearestNeighbors(XZ_points_in, X_tracklength);
    findNearestNeighbors(YZ_points_in, Y_tracklength);

    if(cut_event_flag==true) continue;
    // if(cut_event_flag2[0] == true || cut_event_flag2[1] == true)  continue;       //for coincidence only
    if (x_count == 0 || y_count == 0) continue;
    // if(x_Emax>3600||y_Emax>3600)  continue;       //cut out the out-of-range events
    if(cut_saturated) continue;               //cut out the out-of-range events (revised version 2023.12.13)

    n1++;

    if (x_count == 0)
    {
      timerange[0] = y_timerange[1];
      timerange[1] = y_timerange[3];
    }
    else if (y_count == 0)
    {
      timerange[0] = x_timerange[1];
      timerange[1] = x_timerange[3];
    }
    else
    {
      if (x_timerange[1] < y_timerange[1])
        timerange[0] = x_timerange[1];
      else
        timerange[0] = y_timerange[1];
      if (x_timerange[3] > y_timerange[3])
        timerange[1] = x_timerange[3];
      else
        timerange[1] = y_timerange[3];
    }

    if (timerange[0] == timerange[1])
    {
      ratio = 0;
    }
    else if (x_Emax > y_Emax)
    { // means the max energy lost is on x strip
      ratio = (timerange[1] - time_of_Emax_x) / (timerange[1] - timerange[0]);
    }
    else
    { // means the max energy lost is on y strip
      ratio = (timerange[1] - time_of_Emax_y) / (timerange[1] - timerange[0]);
    }

    if (ratio > 1)
    {
      cout << "ratio: " << ratio << endl;
      cout << "timerange[1]: " << timerange[1] << " timerange[0]: " << timerange[0] << endl;
      cout << "x_Emax:  " << x_Emax << " y_Emax: " << y_Emax << endl;
      cout << "time_of_Emax_x: " << time_of_Emax_x << " time_of_Emax_y: " << time_of_Emax_y << endl;
    }

    timediff = timerange[1] - timerange[0];

    // start position reconstruction method 1, for alpha
    start_pos_x = x_timerange[2];
    start_pos_y = y_timerange[2];

    // //start position reconstruction method 2, for beta
    // if(x_range[1]>x_range[3]) start_pos[0] = x_range[0];
    // else  start_pos[0] = x_range[2];
    // if(y_range[1]>y_range[3]) start_pos[1] = y_range[0];
    // else  start_pos[1] = y_range[2];

    // new definition of count:sqrt(x_count^2+y_count^2), to be consistent with the cuts
    count = TMath::Sqrt(x_count * x_count + y_count * y_count);

    // calculate the "cotangent theta", with the length units concerned, "40" is the peak position of transit time
    // incident_angle = (timerange[1] - timerange[0]) * 6.5 / 40 / (count * 0.133);
    // calculate the "cotangent theta" in the condition of known drift velocity (3.5cm/us)
    incident_angle = (timerange[1] - timerange[0]) * 40 * 3.5e-3 / (count * 0.133);

    // calculate count _diff
    count_diff = TMath::Abs(x_count - y_count);

    E_xy[0] /= x_count;
    E_x[0] /= x_count;
    E_x2[0] /= x_count;
    E_y[0] /= x_count;
    E_y2[0] /= x_count;
    E_xy[1] /= y_count;
    E_x[1] /= y_count;
    E_x2[1] /= y_count;
    E_y[1] /= y_count;
    E_y2[1] /= y_count;

    if((E_x2[0] - E_x[0] * E_x[0])*(E_y2[0] - E_y[0] * E_y[0])>0){
      rho_x = (E_xy[0] - E_x[0] * E_y[0]) / TMath::Sqrt((E_x2[0] - E_x[0] * E_x[0]) * (E_y2[0] - E_y[0] * E_y[0]));
    }
    else rho_x=0;
    if((E_x2[1] - E_x[1] * E_x[1])*(E_y2[1] - E_y[1] * E_y[1])>0){
      rho_y = (E_xy[1] - E_x[1] * E_y[1]) / TMath::Sqrt((E_x2[1] - E_x[1] * E_x[1]) * (E_y2[1] - E_y[1] * E_y[1]));
    }
    else rho_y=0;

    startpos_x_MC = startpos_x;
    startpos_y_MC = startpos_y;
    startpos_z_MC = startpos_z;
    kinE_start_MC = kinE_start;

    // if (start_pos_x < 15 || start_pos_x > 105 || start_pos_y < 15 || start_pos_y > 105) continue;
    double d_center = sqrt(pow(start_pos_x-60.5,2)+pow(start_pos_y-60.5,2))*1.33;
    if(d_center>35) continue;

    // cut for simulation events, in order to remove short tracks, mainly low energy electrons
    if(X_tracklength<2 && Y_tracklength<2) continue;

    tree0->Fill();
  }

  tree0->Write();

  cout << "Events after 1st level selection: " << n1 << endl;
  cout << "Total event number: " << tree0->GetEntries() << endl;
  outfile->Close();
}

// define main()
string filename;
void helpinfo()
{

  cout << "Usage is ./ConstructTree_exe ./Raw2ROOT/<filename>.root\n";
  cout << "default output file name is \"./VariableTree/<filename>_tree.root\"" << endl;
  return;
}
void phrase_command(int argc, char *argv[])
{
  if (argc < 2)
  {
    helpinfo();
    exit(0);
  }
  else
  {
    filename = (string)argv[1];
    cout << "START\n~~~~~*****~~~~~\nInput File : " << filename << endl;
  }
}

int main(int argc, char *argv[])
{

  phrase_command(argc, argv);
  string Tt = "richraw";
  const char *TtreeName = (char *)(Tt.data());
  const Text_t *RawRootFile = (Text_t *)(filename.data());
  char *RawRootFile_c = (char *)(filename.data());
  TFile *f = (TFile *)gROOT->GetListOfFiles()->FindObject(RawRootFile);
  if (!f)
  {
    f = new TFile(RawRootFile_c);
  }
  TTree *tree = (TTree *)gDirectory->Get(TtreeName);

  RootConvert xx;
  xx.RootConvert::Init(tree);
  xx.RootConvert::Loop(RawRootFile_c);
  cout << "\n~~~~~*****~~~~~\nTHE END" << endl;
  return 1;
}