#include "util.hpp"
#include "data_strcut_cint.h"
ClassImp(entry_new)

struct lcs_t{
  int layer_id=0;
  int channel_id=0;
  float compres=3;
};

lcs_t parse_lcs(std::string str){
  lcs_t rt;
  int index=0;
  while(str.find_first_of('>')!=std::string::npos){
    std::string v_str = str.substr(str.find_first_of('<')+1,str.find_first_of('>')-1);
    if (index==0) rt.layer_id=std::stoi(v_str.c_str());
    else if (index==1) rt.channel_id= std::stoi(v_str.c_str());
    else if (index==2) rt.compres=std::stof(v_str.c_str());
    str = str.substr(str.find_first_of('>')+1);
    index++; }
  return rt;
}
std::unordered_map<int,float> compres_table;
typedef std::unordered_map<int,std::pair<float,float>> mean_and_rms_t;
void generate_configs(
  std::string const& path
  ,mean_and_rms_t const& maps
  ,int compres
  ){
//  namespace fs = std::filesystem;
  using namespace util;
//if (!fs::exists(path.c_str())) fs::create_directory(path.c_str());
//  fs::path dir(path.c_str());
  char* frame_buf = new char[9];
  std::vector<char> frame(0);
  for (auto [x,y] : maps){
    uint16_t gid = x;
    if (compres_table.find(gid) != compres_table.end()) compres=compres_table.at(gid);
    uint8_t fec_id= uint8_t(x>>8);
    uint8_t channel_id = uint8_t(x&0xFF);
    std::stringstream sstr("");
    sstr<<"L"<<(int)fec_id<<"CompressThreshold_"<<(int)channel_id<<".dat";
    std::string fname=path+"/"+sstr.str();
    std::ofstream fout(fname.c_str(),std::ios::out|std::ios::binary);

    generate_frame(e_command_address::k_FEID,frame_buf,fec_id);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    generate_frame(e_command_address::k_ChannelID,frame_buf,channel_id);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    generate_frame(e_command_address::k_Threshold_Tag,frame_buf);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    for (int i=0; i<9; ++i) frame_buf[i]=0;
    generate_frame(e_command_address::k_Threshold_Value,frame_buf
        ,(uint16_t)std::floor(y.first)
        ,(uint16_t)std::floor(y.second)
        ,compres);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    for (int i=0; i<9; ++i) frame_buf[i]=0;
    generate_frame(e_command_address::k_Config,frame_buf);
    for (int i=0; i<9; ++i) frame.push_back(frame_buf[i]);
    fout.write(frame.data(),frame.size());
    frame.clear();
    fout.close();
  }
  delete[] frame_buf;
}
int generate_config(){
  auto argv_map = util::read_argv("config");
  compres_table.clear();
  for (auto&& x : argv_map){
    if(x.first[0]=='L' && x.second=="T"){
      auto ps = parse_lcs(x.first);
      int global_ids=((uint16_t)ps.layer_id<<8)+(uint16_t)ps.channel_id;
      compres_table[global_ids]=ps.compres;
    }
  }
  std::string entry_out_file=argv_map["filename"];
  TFile* rfin = new TFile(entry_out_file.c_str());
  auto* fout = new TFile("generate_config.root","recreate");
  auto* data_tree  = static_cast<TTree*>(rfin->Get("CollectionTree"));
  entry_new* entry_buffer_ptr = new entry_new;
  data_tree->SetBranchAddress("data",std::addressof(entry_buffer_ptr));
  std::unordered_map<int,TH1I*> baseline_map;
  for (long long i=0; i<data_tree->GetEntries(); ++i){
    data_tree->GetEntry(i);

    for (int j=0; j<entry_buffer_ptr->global_ids.size(); ++j){
      int x = entry_buffer_ptr->global_ids.at(j);

      if (baseline_map.find(x)==baseline_map.end()){
        std::stringstream sstr(""); sstr<<"baseline-"<<x;
        auto const& ref = entry_buffer_ptr->adcs[j];
        uint16_t min = *util::min_element(std::begin(ref),std::end(ref));
        uint16_t max = *util::max_element(std::begin(ref),std::end(ref));
        min -= 10; max += 10;
        //std::cout<<min<<" "<<max<<std::endl;
        baseline_map[x] = new TH1I(sstr.str().c_str(),sstr.str().c_str()
            ,max-min,min,max);
      }
      for (auto&& adc : entry_buffer_ptr->adcs[j]) baseline_map[x]->Fill(adc);
      //j++;

    }
    ////for (std::size_t j=0; auto&& x : entry_buffer_ptr->adcs){
    ////  auto* gr = new TGraph;
    ////  std::stringstream sstr; sstr<<i<<"-"<<j++;
    ////  gr->SetName(sstr.str().c_str());
    ////  for (int k=0; k<1024; ++k) gr->SetPoint(k,k,x[k]);
    ////  folder->Add(gr);
    ////}
    //fout->cd();
    //folder->Write();
  }

    fout->cd();
  std::unordered_map<int,std::pair<float,float>> mean_and_rms;
 // auto* folder_bl = new TFolder("baseline","baseline");
  for (auto iter = baseline_map.begin(); iter != baseline_map.end(); ++iter) {
     //TF1 f_gaus("f_gaus",util::math::gaus,y->GetMean()-3*y->GetRMS(),
    //TF1 f_gaus("f_gaus","gaus",y->GetMean()-3*y->GetRMS(),
    //y->GetMean()+3*y->GetRMS());
    //y->Fit(&f_gaus,"RQ");
    //folder_bl->Add(y);
    //mean_and_rms[x].first=f_gaus.GetParameter(1);
    //mean_and_rms[x].second=f_gaus.GetParameter(2);

    mean_and_rms[iter->first].first=iter->second->GetMean();
    mean_and_rms[iter->first].second=iter->second->GetRMS();
  }

    generate_configs(argv_map["path"],mean_and_rms,std::stoi(argv_map["sigma-compres"]));
  TH1I mean_dis("mean","mean",512,0,512);
  TH1F rms_dis("rms","rms",512,0,512);
  for (auto [x,y] : mean_and_rms){
    mean_dis.SetBinContent(x,y.first);
    rms_dis.SetBinContent(x,y.second);
  }
  mean_dis.Write();
  rms_dis.Write();
  rfin->Close();
  fout->Write(); fout->Close();
  delete entry_buffer_ptr;
  return 0;


}
