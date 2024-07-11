###########################################################
# fragment for ROOT compilation

ROOTCONFIG    = $(ROOTSYS)/bin/root-config
CINT          = $(ROOTSYS)/bin/rootcint

ROOTLIBS      = $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS     = $(shell $(ROOTCONFIG) --glibs) -lSpectrum
ROOTCFLAGS    = $(shell $(ROOTCONFIG) --cflags) 

CXX           = g++
LD            = g++
FOR           = 
SOFLAGS       = -shared
GLIBS         = -L/usr/X11R6/lib -lXpm -lX11 $(ROOTGLIBS)
CXXFLAGS      =  -Wall $(ROOTCFLAGS) -Iinclude/ -g
LIBS          = $(ROOTLIBS) -lMinuit -lMLP -lMinuit -lTreePlayer -lTMVA -lTMVAGui -lXMLIO  -lMLP -lm

all:	Raw2ROOT_exe Pedestal_exe Hitsmap_exe ChargeDistribution_exe Train_exe ConstructTree_exe Analysis_exe Scaling_exe ShowWaveform_exe TMVATrain_exe TMVAAnalysis_exe
	
clean:
	rm -rf obj/* dict/* *.so;

obj/%.o : src/%.C include/%.h 
##include/%.h
	mkdir -p obj/; $(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.co: %.dict.cc
	mkdir -p obj/; $(CXX) $(CXXFLAGS) -c -o $@ $<; mv $< $*.dict.h dict/.

%.dict.cc: include/%.h 
	mkdir -p dict/; $(CINT) -f $@ -c -I$(ROOTSYS)/include $< 

%_exe: obj/%.o
	$(LD)    $(ROOTCFLAGS) $(LIBS) $< -o $@ $(ROOTGLIBS)
# Raw2ROOT_exe:  obj/Raw2ROOT.o  
# 	$(LD)    $(ROOTCFLAGS) $(LIBS) $(ROOTGLIBS) obj/Raw2ROOT.o  -o $@
# Pedestal_exe:  obj/Pedestal.o  
# 	$(LD)    $(ROOTCFLAGS) $(LIBS) $(ROOTGLIBS) obj/Pedestal.o  -o $@
# Hitsmap_exe:  obj/Hitsmap.o  
# 	$(LD)    $(ROOTCFLAGS) $(LIBS) $(ROOTGLIBS) obj/Hitsmap.o  -o $@
# ChargeDistribution_exe:  obj/ChargeDistribution.o  
# 	$(LD)    $(ROOTCFLAGS) $(LIBS) $(ROOTGLIBS) obj/ChargeDistribution.o  -o $@
# Train_exe:  obj/Train.o  
# 	$(LD)    $(ROOTCFLAGS) $(LIBS) $(ROOTGLIBS) obj/Train.o  -o $@
# ConstructTree_exe:  obj/ConstructTree.o  
# 	$(LD)    $(ROOTCFLAGS) $(LIBS) $(ROOTGLIBS) obj/ConstructTree.o  -o $@
# Analysis_exe:  obj/Analysis.o  
# 	$(LD)    $(ROOTCFLAGS) $(LIBS) $(ROOTGLIBS) obj/Analysis.o  -o $@
#electron.exe:  obj/ElecIDAna.o  obj/EMuHist.o obj/mainElectron.o obj/W_MT.o 
#	$(LD)    $(ROOTCFLAGS) $(LIBS) $(ROOTLIBS) obj/ElecIDAna.o  obj/EMuHist.o obj/mainElectron.o  obj/W_MT.o -o $@
