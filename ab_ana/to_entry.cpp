#include <iostream>
#include <numeric>
#include "to_entry.h"

#include "TTree.h"
#include "TFile.h"

void to_entry::run(){
  //auto const& clear_entry = [](entry& v){
  //  v.m_hits = 0;
  //  v.m_channels.clear();
  //  v.adcs.clear();
  //  v.m_sum_adc.clear();
  //  v.m_max_adc.clear();
  //  v.m_max_point.clear();
  //  v.m_sum_max_adc = 0;
  //};

  waveform_pack* ptr = new waveform_pack();
  auto* root_fin = new TFile(m_in_file.c_str());
  auto* tree = static_cast<TTree*>(root_fin->Get("raw"));
  tree->SetBranchAddress("data",&ptr);

  auto* file_out = new TFile(m_out_file.c_str(),"recreate"); 
  auto* tree_out = new TTree("entry","entry");
  entry data;
  tree_out->Branch("data",&data);

  std::unordered_map<uint16_t,entry> entry_map;
  
  for (long long i=0; i<tree->GetEntries(); ++i){
    tree->GetEntry(i);
    entry_map[ptr->evt_id].m_hits++;
    entry_map[ptr->evt_id].m_channels.emplace_back(ptr->channel_idx);
    for (auto&& x : ptr->adc) entry_map[ptr->evt_id].adcs.emplace_back(x);
  }
  delete ptr;
  root_fin->Close();


  file_out->cd();
  for (auto&& [x,y] : entry_map){
    data = y;
    for (auto iter=std::begin(data.adcs); iter != std::end(data.adcs); iter += 1024){
      data.m_sum_adc.emplace_back(std::accumulate(iter,iter+1024,0));
      auto iter_max = std::max_element(iter,iter+1024);
      data.m_sum_max_adc += *iter_max;
      data.m_max_adc.emplace_back(*iter_max);
      data.m_max_point.emplace_back(std::distance(iter,iter_max));
    }
    tree_out->Fill();
  }
  tree->Write();
  file_out->Write(); file_out->Close();
  
  


  
}
