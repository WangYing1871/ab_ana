#include "collection.h"


#include "TFile.h"
#include "TTree.h"

int collection::init(){

  return 0;
}

int collection::run(){
  auto* fin = new TFile(m_in_file.c_str());
  auto* tree_in = static_cast<TTree*>(fin->Get("entry"));
  entry* ptr = new entry;
  tree_in->SetBranchAddress("data",&ptr);

  auto* fout = new TFile(m_out_file.c_str(),"recreate");
  auto* tree_out = new TTree("collection","collection");
  tree_out->Branch("data",&m_data);

  for (long long i=0; i<tree_in->GetEntries(); ++i){
    tree_in->GetEntry(i);
    
  }


  return 0;



}

int collection::finalize(){
  return 0;
}
