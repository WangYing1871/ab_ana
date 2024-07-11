#include <iostream>
#include <fstream>
#include "TF1.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TString.h"
#include "../include/Langaus.h"
using namespace std;

void landau_test(string inputfile){
    // TString ratio = inputfile;
    // cout << ratio << endl;
    // ratio.Remove(0, ratio.Last('/') + 1);
    // ratio.Remove(ratio.Last('.'));
    // cout << ratio << endl;
    // std::string name(ratio);
    // std::string histname = "Fe55_spectrum_" + name;
    // std::string canvasname = "spectrumFe55_" + name;
    // std::cout << histname << std::endl;
    // std::cout << canvasname << std::endl;

    TH1F *Spectrum = new TH1F("Spectrum", "Alpha spectrum;ADC;counts", 2048, 0, 2048);
    TF1 *mygaus1 = new TF1("mygaus1", "gaus", 0, 2048);
    // TF1 *mygaus2 = new TF1("mygaus2", "gaus", 0, 5000);
    // TF1 *mygaus3 = new TF1("mygaus3", "gaus", 0, 5000);
    TF1 *antilangau = new TF1("antilangau", antilangaufun, 0, 2048,4);

    //read raw data
    ifstream data;
    data.open(inputfile);
    //data.open("Top/M2/transparency/0V_750V_889V_Amp_2p5_10_mesh02.Spe");
    //data.open("480_605_2.5_40_Ar_93_CO2_7.Spe");
    //data.open("490_618_2.5_40_Ar_93_CO2_7.Spe");
    //data.open("500_631_2.5_40_Ar_93_CO2_7.Spe");
    //data.open("510_644_2.5_20_Ar_93_CO2_7.Spe");
    //data.open("520_656_2.5_20_Ar_93_CO2_7.Spe");
    //data.open("530_669_2.5_20_Ar_93_CO2_7.Spe");
    //data.open("535_675_2.5_20_Ar_93_CO2_7.Spe");
    //ta.open("540_681_2.5_10_Ar_93_CO2_7.Spe");

    if (!data.good())
    {
        cout << "Can not open data TXT File!!!" << endl;
        exit(0);
    }
    char data_header[12][80];
    for(int i=0;i<12;i++){
        data.getline(data_header[i],80);                //前12行没有数据，将它们读入一个二维字符数组，80表示第i行第80位填入结束符'\0'
    }
    //cout<<"~~~~~~~~~~~~~"<<endl;
    //cout<<data_header[11]<<endl;
    //cout<<"~~~~~~~~~~~~~"<<endl;
    int adc[2048];
    int max=0;
    int counts =0;
    for(int j=0;j<2048;j++){
        data>>adc[j];
        //cout<<adc[j]<<endl;
        Spectrum->SetBinContent(j+1,adc[j]);

        if(j>=80&&adc[j]>counts){
            counts=adc[j];                     //循环的目的是找到计数的最大值counts及其对应道数位置max
            max=j;
        }
    }
    data.close();



    //draw&fit
    gStyle->SetOptStat(000);
    gStyle->SetOptFit(1111);
    TCanvas *Spe=new TCanvas("c1","c1");
    Spectrum->Draw();



    mygaus1->SetRange(max-max*0.2,max+max*0.3);
    Spectrum->Fit(mygaus1,"R0");              //R表示Use the Range specified in the function range，0表示Do not plot the result of the fit. 
    double par1[3];                             //pre-fit gaus parameters
    double par2[4];                             //fit antilangaus parameters
    mygaus1->GetParameters(par1);
    //Spectrum->GetXaxis()->SetRange(0,par1[1]*1.8);
    //r[0]=;
    //r[3]=200;
    //r[6]=200;
    par2[0]=par1[2]*0.2;                    //landau width parameter
    par2[1]=par1[1];                        //Maximam position of the landau
    par2[2]=Spectrum->Integral(par1[1]-3*par1[2],par1[1]+3*par1[2]);                    //area(scaling) parameter
    par2[3]=par1[2]*0.8;                                                //gaus sigma
    antilangau->SetParameters(par2);
    antilangau->SetRange(par1[1]-4*par1[2],par1[1]+4*par1[2]);
    
    antilangau->SetParLimits(0,par2[0]*0.05,par2[0]*20);              //set the antilangau fit range
    antilangau->SetParLimits(1,par2[1]*0.5,par2[1]*1.8);
    antilangau->SetParLimits(2,par2[2]*0.05,par2[2]*20);
    antilangau->SetParLimits(3,par2[3]*0.2,par2[3]*1.5);

    //antilangau->SetRange(400,3500);
    antilangau->SetLineColor(kRed);
    antilangau->SetLineStyle(1);
    antilangau->SetLineWidth(2);
    Spectrum->Fit(antilangau,"R");
    /*   Int_t ci;      // for color index setting
       TColor *color; // for color definition with alpha
       ci = TColor::GetColor("#000099");
       Spectrum->SetLineColor(ci);
       Spectrum->SetMarkerStyle(20);
       Spectrum->Draw("E1");
    */
    antilangau->GetParameters(par2);
    //antilangau->SetParameters(par);
    //Spectrum->Fit(antilangau,"R");
    //mygaus1->SetRange(par[1]-2*par[2],par[1]+2*par[2]);
    mygaus1->SetLineStyle(1);
    mygaus1->SetLineWidth(2);
    mygaus1->SetLineColor(kCyan);
    mygaus1->Draw("same");                                //draw the pre-fit gauss in the same canvas




    TLegend *leg=new TLegend(0.13,0.65,0.49,0.87);
    leg->SetTextFont(72);
    leg->SetHeader("Energy Resolution(FWHM)");
    leg->SetTextSize(0.04);
    leg->SetFillColor(0);
    float er[2];
    char bb='%';
    char cc[2][40];
    er[0]=par1[2]*2.354/par1[1]*100;
    er[1]=par2[3]*2.354/par2[1]*100;
    sprintf(cc[0],"Gauss:%.1f%c",er[0],bb);
    sprintf(cc[1],"Anti-Lan-gauss:%.1f%c",er[1],bb);

    //ring x1=(string)cc[0];
    //string x2=(string)cc[1];
    //cout<<x1<<endl;
    //cout<<x2<<endl;
    //x1=x1+"%";
    //x2=x2+"%";
    //cout<<x1<<endl;
    //cout<<x2<<endl;
    leg->AddEntry(mygaus1,cc[0],"L");
    leg->AddEntry(antilangau,cc[1],"L");
    leg->Draw();
}