#make
  g++ -std=c++14 -pthread -Wall -fPIC -m64 -O3 -rdynamic ana.cpp -o main `root-config --libs --cflags` -lboost_timer -Draw_to_raw

#run
  ./main <baseline_file-name> <signal_file_name>
