//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: 2835516101@qq.com     
//^-^ Time: 2024-07-31 09:01:47   Posi: Hefei
//^-^ File: main.cpp
//--------------------------------------------------------------------
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "util.hpp"
#include "data_strcut_cint.h"
#include "unpack.h"
#include "to_entry.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TPDF.h"
#include "TCanvas.h"
#include "TF1.h"

//namespace util::math{
//double gaus_(double* x,double* p){
//  return p[0]*std::exp(-(x[0]-p[1])*(x[0]-p[1])/2/p[2]/p[2]);
//}
//}
template <class _iter_t>
float mean(_iter_t first, _iter_t last){
  return std::accumulate(first,last,0.)/std::distance(first,last); }

using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){

  /*
  auto* aa = new waveform();
  unpacking_tool unpacker;
  unpacker.m_parser = aa;


  std::string dat_fname = argv[1];
  std::string raw_root_fname = dat_fname.substr(0,dat_fname.find_last_of("."))+"_raw.root";
  unpacker.m_in_file = dat_fname;
  unpacker.m_out_file = raw_root_fname;
  unpacker.unpack();
  */

  /*
  {
    std::string channel_in_raw_file = argv[1];
    waveform_pack* ptr = new waveform_pack();
    auto* root_fin = new TFile(channel_in_raw_file.c_str());
    auto* tree = static_cast<TTree*>(root_fin->Get("raw"));
    tree->SetBranchAddress("data",&ptr);

    std::unordered_map<int,TH1F*> baselines;
    std::unordered_map<int,std::vector<float>> baselines_store;
    std::unordered_map<int,std::pair<float,float>> aa;
    info_out(tree->GetEntries());
    for (long long i=0; i<tree->GetEntries(); ++i){
      tree->GetEntry(i);
      auto& adcs = ptr->adc;
      baselines_store[ptr->channel_idx].emplace_back(
        std::min(mean(adcs,adcs+25),mean(adcs+999,adcs+1024)));
      //if (!baselines[ptr->channel_idx]){
      //  std::stringstream sstr; sstr<<util::config::channels_map[ptr->channel_idx];
      //  baselines[ptr->channel_idx] = new TH1I(sstr.str().c_str(),sstr.str().c_str(),
      //      1000,500,1500);
      //}

      //baselines[ptr->channel_idx]->Fill(std::min(
      //      mean(adcs,adcs+25),mean(adcs+999,adcs+1024)));
    }

    for (auto&& [x,y] : baselines_store){
      auto m_s = util::math::mean_and_standard_deviation(std::begin(y),std::end(y));
      std::stringstream sstr; sstr<<util::config::channels_map[x];
      auto* his = new TH1F(sstr.str().c_str(),sstr.str().c_str()
          ,(int)6*m_s.second,m_s.first-3*m_s.second,m_s.first+3*m_s.second);

      for (auto&& y0 : y) his->Fill(y0);
      baselines.emplace(x,his);
    }
    TCanvas* canvas = new TCanvas("canvas","canvas",800,600);
    std::string pdf_name = "out.pdf";
    bool is_pdf_open = false;

    auto* fout = new TFile("out.root","recreate");
    fout->cd();
    for (auto&& [x,y] :baselines){
      //auto* f_gaus = new TF1("f_gaus",util::math::gaus,y->GetMean()-3*y->GetRMS(),
      //    y->GetMean()+3*y->GetRMS());
      auto* f_gaus = new TF1("f_gaus","gaus",y->GetMean()-3*y->GetRMS(),y->GetMean()+3*y->GetRMS());
      y->Fit(f_gaus,"RQ");
      if (!is_pdf_open){
        is_pdf_open = true;
        std::stringstream sstr;
        sstr<<pdf_name<<"(";
        y->Draw();
        canvas->Print(sstr.str().c_str(),"Title:");
      }else{
        y->Draw();
        canvas->Print(pdf_name.c_str(),"Title:");
      }
      y->Write();
    }
    canvas->Print("out.pdf)","Title:");
    fout->Write();
    fout->Close(); 


  }
  */
 
  //std::string channel_in_raw_file = argv[1];
  /*
  std::string channel_in_raw_file = raw_root_fname;
  std::string entry_out_file = channel_in_raw_file.substr(
      0,channel_in_raw_file.find_last_of("_"))+"_entry.root";
  to_entry bb;
  bb.m_in_file = channel_in_raw_file;
  bb.m_out_file = entry_out_file;
  bb.is_print_pdf(true);
  bb.run();
  */
  
  std::string dat_name = argv[1];
  std::string entry_out_file = dat_name.substr(
      0,dat_name.find_last_of("."))+"_entry.root";
  
  TFile* fout = new TFile(entry_out_file.c_str(),"recreate");
  TTree* data_tree = new TTree("CollectionTree","CollectionTree");
  entry_new entry_buffer;
  data_tree->Branch("data",std::addressof(entry_buffer));
  
  std::ifstream fin(dat_name.c_str());
  fin.seekg(0,std::ios_base::end);
  size_t fsz = fin.tellg();
  fin.seekg(0,std::ios_base::beg);
  char* data = new char[fsz];
  char* iter_beg = data;
  fin.read(data,fsz);
  fin.close();
  waveform_by_entry wf;

  wf.set_store(entry_buffer);
  wf.set_tree(data_tree);

  wf.parse(iter_beg,iter_beg+fsz);
  fout->cd();
  data_tree->Write(); 
  fout->Write(); fout->Close(); 
  //wf.display();
  delete[] data;
  

  



  return 0;
}
