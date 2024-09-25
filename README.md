# What
  JW alpha-beta data analysis code

# How to Make
  > CREN-ROOT needed
  > C++ Boost maybe needed in forward develop
  > Qt6  maybe needed in forward develop

  cd master && mkdir build && cd build && cmake ../

# Notice
  [cmake invoke rootcint] 
    If 'data_strcut_cint.h' has been changed
    please update 'LinkDef.h' and remake build/Makefile

# How to run
  cd build
  ./main /path/to/your-file
  [i.g ./main /home/wangying/Documents/xwechat_files/wxid_65uvd5q0l6ms22_ff84/msg/file/2024-09/20240924095111_baseline.dat]

# Result
  unpack root file will be genrated in /path/to/
  data struct: entry_new [Defined in 'data_strcut_cint.h']

# E-mail
  2835516101@qq.com
