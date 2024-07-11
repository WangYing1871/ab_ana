#define TMVATrain_cxx
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
#include "TFile.h"
#include <TF1.h>
#include "TInterpreter.h"
#include <TStyle.h>
#include "TTree.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TString.h"
#include "TMVATrain.h"

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"


using namespace std;
using namespace RICHsystem;
class RootConvert;
void RootConvert::Loop(char *RootFileName){}
void RootConvert::Loop3(char *RootFileName, double threshold){}


void RootConvert::Loop2(TTree *signaltree, TTree *bkgtree, double threshold)
{
  string selection = "selection2_2";
  string TMVAfileName = "./TMVA/TMVATrainResult_"+selection+".root";
  TFile* TMVAFile = TFile::Open( TMVAfileName.data(), "RECREATE" );

  

  double likelihood_ratio;
  TH1F* signal_test = new TH1F("signal_test","likelihood ratio of signal",500,0,1);
  TH1F* bkg_test = new TH1F("bkg_test","likelihood ratio of background",500,0,1);

  TGraph* ROC_curve = new TGraph();
  ROC_curve->SetMarkerStyle(7);
  ROC_curve->SetTitle("ROC curve of training;signal reservation;bkg rejection");
  ROC_curve->SetName("ROC_curve_of_training");

  TCanvas* c1 = new TCanvas("c1","c1",800,800);

  //--------Define TMVA methods------------------

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

  //----------------------------- 

  int x_count, y_count;
  int count_diff;           //difference of x_count and y_count
  Float_t count;
  Float_t summax_new;
  Float_t start_pos_x, start_pos_y;
  Float_t timediff;
  Float_t ratio;
  Float_t incident_angle;
  Float_t rho_x, rho_y;
  Float_t tracklength_x, tracklength_y;
  Float_t startpos_x_MC, startpos_y_MC, startpos_z_MC, kinE_start_MC;
  
  //Training begin---------------------------------------------------------
  cout<<"Training proscess begins!"<<endl;

  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", TMVAFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );

  //choices: TMVAdataset_selection1, TMVAdataset_selection2_1, TMVAdataset_selection2_2
  string path = "TMVAdataset_"+selection;
  TMVA::DataLoader *dataloader=new TMVA::DataLoader(path.data());

  // dataloader->AddVariable("energy", "energy", "", 'F' );
  // dataloader->AddVariable("hits", "hits", " ", 'F' );
  dataloader->AddVariable("x_hits", "x_hits", " ", 'I' );
  dataloader->AddVariable("y_hits", "y_hits", " ", 'I' );
  // dataloader->AddVariable("hits_diff", "hits_diff", " ", 'I' );
  dataloader->AddVariable("timediff", "timediff", "40ns", 'F' );
  dataloader->AddVariable("incident_angle", "incident_angle", "", 'F' );
  dataloader->AddVariable("start_pos_x", "start_pos_x", "channel", 'F' );
  dataloader->AddVariable("start_pos_y", "start_pos_y", "channel", 'F' );
  dataloader->AddVariable("ratio", "ratio", " ", 'F' );
  dataloader->AddVariable("rho_x", "rho_x", " ", 'F' );
  dataloader->AddVariable("rho_y", "rho_y", " ", 'F' );
  dataloader->AddVariable("tracklength_x", "tracklength_x", " ", 'F' );
  dataloader->AddVariable("tracklength_y", "tracklength_y", " ", 'F' );
  // ---- These are MC truth information, should not be used for classification -----
  // dataloader->AddVariable("startpos_x_MC", "startpos_x_MC", " ", 'F' );
  // dataloader->AddVariable("startpos_y_MC", "startpos_y_MC", " ", 'F' );
  // dataloader->AddVariable("startpos_z_MC", "startpos_z_MC", " ", 'F' );
  // dataloader->AddVariable("kinE_start_MC", "kinE_start_MC", " ", 'F' );
  // ----------------------------------------------

  // You can add an arbitrary number of signal or background trees
  dataloader->AddSignalTree    ( signaltree, 1.0 );
  dataloader->AddBackgroundTree( bkgtree, 1.0 );

  TCut mycuts = ""; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
  TCut mycutb = ""; // for example: TCut mycutb = "abs(var1)<0.5";

  dataloader->PrepareTrainingAndTestTree( mycuts, mycutb,
                                        "nTrain_Signal=1000000:nTest_Signal=50000:nTrain_Background=1000000:nTest_Background=50000:SplitMode=Random:NormMode=NumEvents:!V" );
  
  if (Use["Cuts"])
      factory->BookMethod( dataloader, TMVA::Types::kCuts, "Cuts",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

  if (Use["LikelihoodPCA"])
      factory->BookMethod( dataloader, TMVA::Types::kLikelihood, "LikelihoodPCA",
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmoothSig[0]=20:NSmoothBkg[0]=20:NSmooth=5:NAvEvtPerBin=50:VarTransform=PCA" );

  if (Use["PDERS"])
      factory->BookMethod( dataloader, TMVA::Types::kPDERS, "PDERS",
                           "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );
  
  if (Use["LD"])
      factory->BookMethod( dataloader, TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

  if (Use["MLPBNN"])
      factory->BookMethod( dataloader, TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=60:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" );

  if (Use["BDT"])  // Adaptive Boost
      factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT",
                           "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
  
  if (Use["RuleFit"])
      factory->BookMethod( dataloader, TMVA::Types::kRuleFit, "RuleFit",
                           "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );

  if (Use["DNN_CPU"] or Use["DNN_GPU"]) {
      // General layout.
      TString layoutString ("Layout=TANH|128,TANH|128,TANH|128,LINEAR");

      // Training strategies.
      TString training0("LearningRate=1e-2,Momentum=0.9,Repetitions=1,"
                        "ConvergenceSteps=30,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=None,"
                        "DropConfig=0.0+0.5+0.5+0.5, Multithreading=True");
      TString training1("LearningRate=1e-2,Momentum=0.9,Repetitions=1,"
                        "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=L2,"
                        "DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
      TString training2("LearningRate=1e-3,Momentum=0.0,Repetitions=1,"
                        "ConvergenceSteps=20,BatchSize=256,TestRepetitions=10,"
                        "WeightDecay=1e-4,Regularization=L2,"
                        "DropConfig=0.0+0.0+0.0+0.0, Multithreading=True");
      TString trainingStrategyString ("TrainingStrategy=");
      trainingStrategyString += training0 + "|" + training1 + "|" + training2;

      // General Options.
      TString dnnOptions ("!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:"
                          "WeightInitialization=XAVIERUNIFORM");
      dnnOptions.Append (":"); dnnOptions.Append (layoutString);
      dnnOptions.Append (":"); dnnOptions.Append (trainingStrategyString);

      // Cuda implementation.
      if (Use["DNN_GPU"]) {
         TString gpuOptions = dnnOptions + ":Architecture=GPU";
         factory->BookMethod(dataloader, TMVA::Types::kDL, "DNN_GPU", gpuOptions);
      }
      // Multi-core CPU implementation.
      if (Use["DNN_CPU"]) {
         TString cpuOptions = dnnOptions + ":Architecture=CPU";
         factory->BookMethod(dataloader, TMVA::Types::kDL, "DNN_CPU", cpuOptions);
      }
   }

  // Train MVAs using the set of training events
  factory->TrainAllMethods();

  // Evaluate all MVAs using the set of test events
  factory->TestAllMethods();

  // Evaluate and compare performance of all configured MVAs
  factory->EvaluateAllMethods();

  TMVAFile->Close();

  delete factory;
  delete dataloader;

  cout<<"Training process ends!"<<endl;
  //Training end----------------------------------------------------------

  //Testing begins-------------------------------------------------------------
  cout<<"Generating output results!"<<endl;

  TFile *testfile = new TFile(TMVAfileName.data());
  TFile* outFile = new TFile("./TMVAtraining_output.root","RECREATE");

  TLegend *leg = new TLegend(0.75, 0.8, 0.95, 0.95);

  TH1D *sighist, *bkghist, *ROChist;
  int color_code = 1;     //begin with red
  if(testfile->GetDirectory(path.data())){
    if (Use["Cuts"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_Cuts")
                    ->GetDirectory("Cuts")
                    ->Get("MVA_Cuts_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_Cuts")
                    ->GetDirectory("Cuts")
                    ->Get("MVA_Cuts_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_Cuts")
                    ->GetDirectory("Cuts")
                    ->Get("MVA_Cuts_trainingRejBvsS");
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_Cuts");
    }
    
    if (Use["LikelihoodPCA"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_Likelihood")
                    ->GetDirectory("LikelihoodPCA")
                    ->Get("MVA_LikelihoodPCA_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_Likelihood")
                    ->GetDirectory("LikelihoodPCA")
                    ->Get("MVA_LikelihoodPCA_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_Likelihood")
                    ->GetDirectory("LikelihoodPCA")
                    ->Get("MVA_LikelihoodPCA_trainingRejBvsS");
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_LikelihoodPCA");
    }

    if (Use["PDERS"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_PDERS")
                    ->GetDirectory("PDERS")
                    ->Get("MVA_PDERS_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_PDERS")
                    ->GetDirectory("PDERS")
                    ->Get("MVA_PDERS_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_PDERS")
                    ->GetDirectory("PDERS")
                    ->Get("MVA_PDERS_trainingRejBvsS");
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_PDERS");
    }

    if (Use["LD"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_LD")
                    ->GetDirectory("LD")
                    ->Get("MVA_LD_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_LD")
                    ->GetDirectory("LD")
                    ->Get("MVA_LD_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_LD")
                    ->GetDirectory("LD")
                    ->Get("MVA_LD_trainingRejBvsS");
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_LD");
    }

    if (Use["MLPBNN"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_MLP")
                    ->GetDirectory("MLPBNN")
                    ->Get("MVA_MLPBNN_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_MLP")
                    ->GetDirectory("MLPBNN")
                    ->Get("MVA_MLPBNN_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_MLP")
                    ->GetDirectory("MLPBNN")
                    ->Get("MVA_MLPBNN_trainingRejBvsS");
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_MLPBNN");
    }

    if (Use["BDT"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_BDT")
                    ->GetDirectory("BDT")
                    ->Get("MVA_BDT_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_BDT")
                    ->GetDirectory("BDT")
                    ->Get("MVA_BDT_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_BDT")
                    ->GetDirectory("BDT")
                    ->Get("MVA_BDT_trainingRejBvsS");
      
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_BDT");

      cout << "----->BDT:Integral of ROC curve: " << ROChist->Integral("width") << endl;
    }

    if (Use["RuleFit"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_RuleFit")
                    ->GetDirectory("RuleFit")
                    ->Get("MVA_RuleFit_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_RuleFit")
                    ->GetDirectory("RuleFit")
                    ->Get("MVA_RuleFit_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_RuleFit")
                    ->GetDirectory("RuleFit")
                    ->Get("MVA_RuleFit_trainingRejBvsS");
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_RuleFit");
    }

    if (Use["DNN_GPU"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_DL")
                    ->GetDirectory("DNN_GPU")
                    ->Get("MVA_DNN_GPU_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_DL")
                    ->GetDirectory("DNN_GPU")
                    ->Get("MVA_DNN_GPU_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_DL")
                    ->GetDirectory("DNN_GPU")
                    ->Get("MVA_DNN_GPU_trainingRejBvsS");
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_DNN_GPU");
    }

    if (Use["DNN_CPU"]){
      sighist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_DL")
                    ->GetDirectory("DNN_CPU")
                    ->Get("MVA_DNN_CPU_effS");
      bkghist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_DL")
                    ->GetDirectory("DNN_CPU")
                    ->Get("MVA_DNN_CPU_effB");
      ROChist = (TH1D *)testfile->GetDirectory(path.data())
                    ->GetDirectory("Method_DL")
                    ->GetDirectory("DNN_CPU")
                    ->Get("MVA_DNN_CPU_trainingRejBvsS");
      outFile->cd();
      sighist->Write();
      bkghist->Write();
      ROChist->Write();

      c1->cd();
      ROChist->SetLineColor(++color_code);
      ROChist->SetStats(0);
      ROChist->SetTitle("ROC curve;signal reservation;bkg rejection");
      ROChist->SetLineWidth(2);
      ROChist->Draw("same");
      leg->AddEntry(ROChist, "MVA_DNN_CPU");
    }
  }
  c1->cd();
  leg->Draw();
  outFile->cd();
  c1->Write();

  cout<<"Testing process ends!"<<endl;

  // for(int i=0;i<500;i++){
  //   ROC_curve->SetPoint(i,signal_test->Integral(i+1,501)/signal_test->GetEntries(),bkg_test->Integral(0,i)/bkg_test->GetEntries());
  // }
  // ROC_curve->Write();
  //Testing ends---------------------------------------------------------------
  testfile->Close();
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

