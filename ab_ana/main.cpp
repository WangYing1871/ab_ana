//--------------------------------Stamp-------------------------------
//^-^ Author: Zhi Heng            Email: 2835516101@qq.com     
//^-^ Time: 2024-07-31 09:01:47   Posi: Hefei
//^-^ File: main.cpp
//--------------------------------------------------------------------
#define info_out(X) std::cout<<"==> "<<__LINE__<<" "<<#X<<" |"<<(X)<<"|\n"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
 
#include "unpack.h"
using std::cout; using std::endl; using std::string; using std::vector;
int main(int argc, char* argv[]){

  auto* aa = new waveform();
  unpacking_tool unpacker;
  unpacker.m_parser = aa;

  //std::string dat_fname = "20240726125539_baseline.dat";
  std::string dat_fname = "/home/wangying/desktop/ab_soft/data/jw_test/jw_20240726_test/data_accumulation/backGroundData/waveform/20240726180153_signal.dat";
  std::string raw_root_fname = dat_fname.substr(0,dat_fname.find_last_of("."))+"_raw.root";
  unpacker.m_in_file = dat_fname;
  unpacker.m_out_file = raw_root_fname;
  unpacker.unpack();
  //unpacker.unpack_fast();

  //std::ifstream fin("20240726125539_baseline.dat",std::ios_base::binary);
  //fin.seekg(0,std::ios_base::end);
  //size_t fsz = fin.tellg();
  //char* file_buf = new char[fsz];
  //fin.seekg(0,std::ios_base::beg);

  //fin.read(file_buf,fsz);
  //fin.close();
  //for (auto iter = file_buf; (size_t)std::distance(file_buf,iter)<fsz; ){
  //  if (unpack.m_parser->parse(iter)){
  //    info_out("www");
  //    
  //  }
  //}



  return 0;
}
