#include <iostream>
#include <numeric>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>

//#include <boost/timer/timer.hpp>
#include "util.hpp"
#include "to_entry.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1I.h"
#include "TPDF.h"
#include "TCanvas.h"


void to_entry::run(){
  //boost::timer::auto_cpu_timer progress;
  auto const& clear_entry = [](entry& v){
    v.m_hits = 0;
    v.m_channels.clear();
    v.m_adcs.clear();
    v.m_sum_adc.clear();
    v.m_max_adc.clear();
    v.m_max_point.clear();
    v.m_sum_max_adc = 0; };

  auto const& complete_entry = [](entry& v){
    v.m_max_adc.resize(v.m_hits);
    v.m_sum_adc.resize(v.m_hits);
    v.m_max_point.resize(v.m_hits);
    for (int i=0; i<v.m_hits; ++i){
      info_out(v.m_adcs.size());
      //auto iter_max = std::max_element(std::begin(v.m_adcs[i]),std::end(v.m_adcs[i]));
      //v.m_max_adc[i] = *iter_max;
      //v.m_max_point[i] = std::distance(std::begin(v.m_adcs[i]),iter_max);
      //uint32_t sum_adc = std::accumulate(std::begin(v.m_adcs[i])
      //    ,std::end(v.m_adcs[i]),(uint32_t)0);
      //v.m_sum_adc[i] = sum_adc;
      //v.m_sum_max_adc += *iter_max;
    }
  };

  waveform_pack* ptr = new waveform_pack();
  auto* root_fin = new TFile(m_in_file.c_str());
  auto* tree = static_cast<TTree*>(root_fin->Get("raw"));
  tree->SetBranchAddress("data",&ptr);

  auto* file_out = new TFile(m_out_file.c_str(),"recreate"); 
  auto* tree_out = new TTree("entry","entry");
  entry data;
  tree_out->Branch("data",&data);

  auto const& store = [&,this](){
    if (m_entry_map.empty()) return;
    for (auto&& [x,y] : m_entry_map){
      data = y;
      data.m_max_adc.resize(data.m_hits);
      data.m_sum_adc.resize(data.m_hits);
      data.m_max_point.resize(data.m_hits);
      for (int i=0; i<data.m_hits; ++i){
        auto iter_max = std::max_element(std::begin(y.m_adcs[i]),std::end(y.m_adcs[i]));
        data.m_max_adc[i] = *iter_max;
        data.m_max_point[i] = std::distance(std::begin(y.m_adcs[i]),iter_max);
        uint32_t sum_adc = std::accumulate(std::begin(y.m_adcs[i])
            ,std::end(y.m_adcs[i]),(uint32_t)0);
        data.m_sum_adc[i] = sum_adc;
        data.m_sum_max_adc += *iter_max;
      }
      tree_out->Fill();
    }
  };


  //std::atomic<bool> _is_finish_read = false;
  //std::thread monitor([&](){
  //    while(!_is_finish_read.load())
  //      if(m_entry_map.size()>=10000){
  //      }else
  //        std::this_thread::sleep_for(std::chrono::milliseconds(500));

  //    });

  //
  std::map<uint16_t,TH1I*> baseline_map;

//  tree->GetEntry(0); 
//  uint32_t prev_evt_id = ptr->evt_id;
////  info_out(prev_evt_id); return;
//  for (long long i=0; i<tree->GetEntries(); ++i){
//    tree->GetEntry(i);
//    info_out(ptr->evt_id);
//    //if (ptr->evt_id != prev_evt_id){
//    //  info_out(ptr->evt_id);
//    //  info_out(ptr->channel_idx);
//    //  //complete_entry(data);
//    //  tree_out->Fill();
//    //  clear_entry(data);
//    //  prev_evt_id = ptr->evt_id;
//    //}
//    data.m_hits++;
//    data.m_channels.emplace_back(ptr->channel_idx);
//    std::vector<uint16_t> tmp(1024);
//    std::copy(std::begin(ptr->adc),std::end(ptr->adc),std::begin(tmp));
//    data.m_adcs.emplace_back(tmp);
//  }


  
  long long entry_index;
  long long total_entries = tree->GetEntries();
  std::atomic<bool> is_loop_end(false);
  std::thread slider([&,this](){
      while(!is_loop_end.load()){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout<<"\r\tto_entry\t";
        float schedule = entry_index/(float)total_entries;
        for (int i=0; i<schedule*100; ++i) std::cout.put('=');
        for (int i=schedule*100; i<100; ++i) std::cout.put('-');

        std::cout<<"\t"<<
          std::setprecision(4)<<entry_index/(float)total_entries*100<<"%"<<std::flush;
      }
      std::cout<<std::endl;

      });
  slider.detach();
  for (entry_index=0; entry_index<tree->GetEntries(); ++entry_index){
    tree->GetEntry(entry_index);
    if (_is_print_pdf){
      if(baseline_map.find(ptr->channel_idx)==baseline_map.end()){
        using std::begin; using std::end;
        std::vector<uint16_t> baseline(25);
        std::copy(std::begin(ptr->adc),std::begin(ptr->adc)+25,std::begin(baseline));
        uint16_t mean = std::round(std::accumulate(begin(baseline),end(baseline),0)
            /baseline.size());
        uint16_t range = *std::max_element(begin(baseline),end(baseline))-
          *std::min_element(begin(baseline),end(baseline));
        std::stringstream sstr(""); sstr<<util::config::channels_map[ptr->channel_idx];
        baseline_map.emplace(ptr->channel_idx,new TH1I(sstr.str().c_str(),sstr.str().c_str(),
              2*range,mean-range,mean+range));
        for(auto&& x : baseline) baseline_map.at(ptr->channel_idx)->Fill(x);
      }else{
        for (int i=0; i<25; ++i) baseline_map.at(ptr->channel_idx)->Fill(ptr->adc[i]);

      }
    }

    m_entry_map[ptr->evt_id].m_hits++;
    m_entry_map[ptr->evt_id].m_channels.emplace_back(ptr->channel_idx);
    std::vector<uint16_t> tmp(1024);
    std::copy(std::begin(ptr->adc),std::end(ptr->adc),std::begin(tmp));
    m_entry_map[ptr->evt_id].m_adcs.emplace_back(tmp);

    if (entry_index%100000==0 && entry_index!=0){
      auto iter_last_evt = m_entry_map.find(ptr->evt_id);
      auto last = iter_last_evt->second;
      m_entry_map.erase(iter_last_evt);
      file_out->cd();
      store();
      m_entry_map.clear();
      m_entry_map[ptr->evt_id] = last;
    }
  }
  is_loop_end.store(true);
  if (m_entry_map.size()!=0){ store(); m_entry_map.clear(); }


  delete ptr;
  root_fin->Close(); delete root_fin;

  if (_is_print_pdf){
    if (m_out_pdf=="")
      m_out_pdf = m_in_file.substr(0,m_in_file.find_last_of("_"))+"_baseline.pdf";
    auto* canvas = new TCanvas("canvas","canvas",800,600);
    if(baseline_map.size()<=1){
    }else{
      std::stringstream sstr(m_out_pdf); sstr<<"(";
      baseline_map.begin()->second->Draw();
      //canvas->Print(sstr.str().c_str(),"Title:");
      canvas->Print("plots.pdf(","Title:");
      for(auto iter = std::next(baseline_map.begin());
          iter !=std::prev(baseline_map.end()) ; ++iter)
        //iter->second->Draw(), canvas->Print(m_out_pdf.c_str(),"");
        iter->second->Draw(), canvas->Print("plots.pdf","");

      std::prev(baseline_map.end(),1)->second->Draw();
      sstr.clear(); sstr<<m_out_pdf<<")";
      //canvas->Print(sstr.str().c_str(),"Title:");
      canvas->Print("plots.pdf)","Title:");
    }
  }
  tree_out->Write(); file_out->Write(); file_out->Close();
}
