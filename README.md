#make
  g++ -std=c++14 -pthread -Wall -fPIC -m64 -O3 -rdynamic ana.cpp -o main `root-config --libs --cflags` -lboost_timer

#run
  ./main <baseline_file-name> <signal_file_name>


#ana.cpp
  这是个”总操作“  ab_ana就是其分解


#ab_ana TODO
cd ab_ana
mkdir build
cd build
cmake ../ && make


# ab_ana 计划分4个部分
  依赖root开发工具; CMake构建工程

  1. 解包 目前只完成了波形模式的包
  2. 逐通道分析 得出基线分布 并按照事例重新排布 
     2.1 取波形前25个点（1024点 uint16_t) 填基线分布直方图
     2.2 存pdf文件，现在都叫"plots.pdf"
     2.3 存txt文件未完成
     2.4 通道的波形按照evt_id归纳，且有内存清除操作，对不间断采集，但是evt_id 重新计数情况未测试
  3. 特征提取 未做
  4. mva等    未做

# 使用
  考虑不同平台，未启用boost::program_options

  ./main your_alpha_bate.dat

  result: your_alpha_bate_raw.root 按通道分布
          your_alpha_bate_entry.root 按事例分布

  所有数据结构： data_strcut_cint.h
         

# 速度测试
   测试文件大小(.dat) 9.9G
   unpack 时间: 
    137.187116s wall, 130.680000s user + 3.990000s system = 134.670000s CPU (98.2%)
  to_entry 时间：
    190.046933s wall, 184.840000s user + 2.920000s system = 187.760000s CPU (98.8%)

  plots.pdf完整输出(基线值方图)

   存root文件花费较多时间
