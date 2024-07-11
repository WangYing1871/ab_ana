#define Raw2ROOT_cxx
#include "vector"
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TTree.h"
#include "TPostScript.h"
#include "TH2F.h"
#include "TGraph.h"
#include <TFile.h>
#include <string.h>
#include "fstream"
#include <iostream>
#include <iomanip>
#include <bitset>
#include <map>
#include "Raw2ROOT.h"

//using std::bitset;
using namespace std;
using namespace RICHsystem;
string filename;
void helpinfo(){
        cout<<"Usage is ./Raw2ROOT_exe  <inputfilename.dat>\n";
        cout<<"default output file name is <inputfilename.root>"<<endl;	
        return;
}

void phrase_command(int argc,char *argv[]){
        if (argc<2){ helpinfo();
                exit(0);
        }else {filename=(string) argv[1]; cout<<"Input File is "<<filename<<endl;}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//main
int main(int argc,char *argv[])
{
        phrase_command(argc,argv);

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // denfine IO
        string namesuffix=".root";
        string OF=filename;

        vector<string> tmpName;                                 //creat a string container 
        char *tokenPtr;
        tokenPtr=strtok((char*)OF.c_str(),"/");                 //split the string using "/"
        while(tokenPtr!=NULL) 
        {
                tmpName.push_back((string)tokenPtr);            //push the splited parts into the container
                tokenPtr=strtok(NULL,"/");
        }
        if(tmpName.size()>0)
        {
                OF=tmpName.at(tmpName.size()-1);
                //	cout<<OF<<endl;
        }

        OF.replace(strlen(OF.c_str())-4,4,namesuffix);          //.dat-->.root
        string outputfile="./Raw2ROOT/"+OF;                     //the output file is in the ./Raw2ROOT directory
        // string outputfile="./Raw2ROOT/20231229215934617_bkg_BA4_1h_Vfc829_Vm861_Va1251_Vm_anti98_Va_anti488_25MHz_500ns_1pC.root";         //In order to combine multiple dat files, it should be given a same name

        cout<<"Output file name is: "<<outputfile<<endl;
        TFile *outFile=new TFile(outputfile.c_str(),"RECREATE");            //change "RECREATE" to "UPDATE" for combining files
        if (outFile==(TFile*) NULL)
        {
                cerr<<"Error:Could not Open Conversion ROOT File!"<<endl;
                exit(1);
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //Mm constants information
    /*    const int Nfec = 4;
        const int Nchip = 4;
        const int Nch =68;              //IMPORTANT CHANGE!
        const int Nsp =512; //sampling points
        const int Tchip = Nfec*Nchip;
        const int Tch = Tchip*Nch;
        const int Nconnector =8;
        const int Ncchn=128;
      */  //
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //map file
        //mapL0: MMegas readout pixels to connector pins
        //Connector number; pins; pixelX; pixelY;
        ifstream map;
        // map.open("./map/mapL0");
        // if(!map.good()) {std::cout<<"Can not open mapL0 file!";exit(1);}
        // int mapL0[Nconnector][Ncchn][2];
        // short nchn;
        // short ncon;
        // for(int f=0; f<Tch;f++){
        //        map>>ncon;ncon = ncon-1;
        //        map>>nchn;nchn = nchn-7;
        //        map>>mapL0[ncon][nchn][0]>>mapL0[ncon][nchn][1];
        // }
        // map.close();
        // //mapL1:connector pins to FEE channels
        // //chip No.; chip chn; hirose (connector) pin;
        // //chip 1-4 is corresponding to A-D
        // map.open("./map/mapL1");
        // if(!map.good()) {std::cout<<"Can not open mapL1 file!";exit(1);}
        // int mapL1[Nchip][Nch];
        // short ichip=0,ichn=0,ipin=0;
        // for(int f=0; f<Nchip*Nch;f++){
        //        map>>ichip>>ichn>>ipin;
        //        mapL1[ichip][ichn]=ipin-7;
        // }
        // map.close();


        //new map 
        //map0:MMegas readout strips(96 x and 96 y) to chips and channels;
        //chips:0~3     channels:0~67 
        map.open("./map/map0");
        if(!map.good()){std::cout<<"Can not open map0 file!";exit(1);}
        int map0[Nchip][Nch][2];
        memset(map0,0,sizeof(map0));
        short ichip=0,ichn=0;
        for(int f=0;f<256;f++){                         //120 strips each for x, y direction readout, and 16 channels for anti coincidence readout
            map>>ichip>>ichn;
            map>>map0[ichip][ichn][0]>>map0[ichip][ichn][1];            //x strip no. and y strip no.
        }
        map.close();

        // //mapL2 fec to connector Number
        // std::map <Int_t,Int_t> mapL2;
        // mapL2.insert(pair<Int_t,Int_t>(5,0));//fec 1: hirose 0,1 
        // mapL2.insert(pair<Int_t,Int_t>(2,6));//fec 5: hirose 6,7
        // mapL2.insert(pair<Int_t,Int_t>(7,2));//fec 2: hirose 2,3
        // mapL2.insert(pair<Int_t,Int_t>(10,4));//fec 4: hirose 4,5

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


        //  Int_t nSamplingPoint = Nsp;
        Int_t event=0;
        //Int_t trigger=-1;
        //Int_t mode=0;//0:external trigger 1:self-trigger
        Int_t nHits=0;
        Int_t Fec[Tch];                     //Tch is the total channel number
        Int_t Chip[Tch];
        Int_t Chn[Tch];
        Int_t Channel[Tch];
        

        //hit Inf
      //  Int_t MM[Tch];
        Int_t ADC[Tch][Nsp];
        Float_t sumADC[Tch];// normalized /Nsp
        Float_t maxADC[Tch];// maximum /Nsp
        Float_t maxPoint[Tch];// maximum ADC sampling point /Nsp
        Float_t summaxADC=0;              //NEW BRANCH! The sum of all the max ADC of an event,which can reflect the energy deposition of the event.
        Int_t pixelX[Tch];
        Int_t pixelY[Tch];

        //tree and branch
        TTree *agetTree= (TTree*)outFile->Get("richraw");   

        if(agetTree ==(TTree*)NULL){
            cout<<"---------RECREATE MODE--------"<<endl;

            agetTree = new TTree("richraw","richraw"); 
            agetTree->Branch("event",&event,"event/I");
            //agetTree->Branch("mode",&mode,"mode/I");
            //agetTree->Branch("trigger",&trigger,"trigger/I");
            agetTree->Branch("nHits",&nHits,"nHits/I");
            agetTree->Branch("Fec",Fec,"Fec[nHits]/I");
            agetTree->Branch("Chip",Chip,"Chip[nHits]/I");
            agetTree->Branch("Chn",Chn,"Chn[nHits]/I");
            agetTree->Branch("Channel",Channel,"Channel[nHits]/I");
            agetTree->Branch("ADC",ADC,"ADC[nHits][512]/I");
            agetTree->Branch("sumADC",sumADC,"sumADC[nHits]/F");
            agetTree->Branch("maxADC",maxADC,"maxADC[nHits]/F");
            agetTree->Branch("maxPoint",maxPoint,"maxPoint[nHits]/F");
            agetTree->Branch("summaxADC", &summaxADC, "summaxADC/F");
            agetTree->Branch("pixelX",pixelX,"pixelX[nHits]/I");
            agetTree->Branch("pixelY",pixelY,"pixelY[nHits]/I");
        }
        else{
            cout<< "---------UPDATE MODE--------"<<endl;
            
            agetTree->SetBranchAddress("event", &event);
            agetTree->SetBranchAddress("nHits", &nHits);
            agetTree->SetBranchAddress("Fec", Fec);
            agetTree->SetBranchAddress("Chip", Chip);
            agetTree->SetBranchAddress("Chn", Chn);
            agetTree->SetBranchAddress("Channel", Channel);
            agetTree->SetBranchAddress("ADC", ADC);
            agetTree->SetBranchAddress("sumADC", sumADC);
            agetTree->SetBranchAddress("maxADC", maxADC);
            agetTree->SetBranchAddress("maxPoint", maxPoint);
            agetTree->SetBranchAddress("summaxADC", &summaxADC);
            agetTree->SetBranchAddress("pixelX", pixelX);
            agetTree->SetBranchAddress("pixelY", pixelY);
            event = agetTree->GetEntries();
        }

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //data buffers for single packet(header/data/end) 
        //read data from RawData File
        cout<<"Loop Raw Data..."<<endl;

        //To count how many pads are triggered in a single event(just x or just y or both)
        //unsigned int pad_count=0;

        //judge if this is a data packet
        Bool_t isdata;

        //for protocol header
        unsigned int gheader[2];//global header

        //the next three parts for packet payload
        //for the event header packet payload
        unsigned int event_headerbuf;
        unsigned int timestampbuf[3];
        unsigned int triggeridbuf[2];
        unsigned int bithigh;
        unsigned int bitlow;
        unsigned int header_restbuf[8];

        //for the event data packet payload
        unsigned int datafirstlinebuf;              //first line of data packet payload, contains fec, chip and channel numbers,IMPORTANT
        unsigned int fecbuf;
        unsigned int chipbuf;
        unsigned int chipbuf1;
        unsigned int chipbuf2;
        unsigned int chnbuf;
        unsigned int ADC_and_PFX;                   //ADC and PREFIX. IMPORTANT
        unsigned int ADCbuf1;
        unsigned int ADCbuf2;
        unsigned int ADCbuf[Nsp];
        unsigned int sumADCbuf;
        unsigned int maxADCbuf;
        unsigned int maxPointbuf;
        unsigned int nullwordbuf;


        //for the event end packet
        unsigned int checkbuf;
        unsigned int reservedbuf;
        unsigned int eventsizebuf[2];

        // for protocal trailer
        unsigned int gtail[2];//global tail


        unsigned int triggerid[2];                  //save this packet's and the former packet's trigger id
        memset(triggerid, 0, sizeof(triggerid));

        unsigned int k=0;

        //histogram 
       // TGraph *graphbuf[Tch];
       //2F *hitmap;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //read binary data

        ifstream stream(filename.c_str(), ios_base::binary);
        for(; !stream.eof();){ //packet by packet loop

        //buffer reset
            isdata = false;
            memset(gheader,0,sizeof(gheader));
            event_headerbuf=0;
            memset(timestampbuf,0,sizeof(timestampbuf));
            memset(triggeridbuf, 0, sizeof(triggeridbuf));
            bithigh = 0;
            bitlow = 0;
            memset(header_restbuf,0,sizeof(header_restbuf));
            datafirstlinebuf=0;              //first line of data packet payload, contains fec, chip and channel numbers
            fecbuf=0;
            chipbuf=0;
            chipbuf1 = 0;
            chipbuf2 = 0;
            chnbuf=0;
            ADC_and_PFX=0;                   //ADC and ADC PREFIX
            ADCbuf1 = 0;
            ADCbuf2 = 0;
            memset(ADCbuf,0,sizeof(ADCbuf));
            sumADCbuf=0;
            maxADCbuf=0;
            maxPointbuf=0;
            nullwordbuf=0;
            checkbuf=0;
            reservedbuf=0;
            memset(eventsizebuf, 0, sizeof(eventsizebuf));
            memset(gtail, 0, sizeof(gtail));


                stream.read((char *)(&gheader[0]), 2);                  //read 2 bytes(16bits) a time, give it to gheader[0]
                if (gheader[0] == 0x0fac) {
                    //--------------READING A SINGLE PACKET-----------------------------
                    stream.read((char *)(&gheader[1]), 2);
                    if (gheader[1] == 0x1c40) {

                        //cout << "entering event header" << endl;
                        ////this means this packet is an event header
                        //pad_count++;            //one header packet means one pad triggered
                        //if (pad_count != 1 && pad_count != 2) {
                        //    cout << "Warning: The number of event header and event end do not consist!!Current event number:"<<event << endl;
                        //    break;
                        //}
                        if (nHits != 0) {
                            event++;                        //event count +1
                            if(event%1000==0){
                                cout << "Event: " << event <<endl;
                            }
                            summaxADC = 0;                          //NEW! Compute the summaxADC of this event
                            for (int i = 0; i < nHits; i++) {
                                summaxADC = summaxADC + maxADC[i];
                                if(Chn[i]>67)  cout<<"Chip no:"<<Chip[i]<<"  Channel no:"<<Chn[i]<<" Event no:"<<event<<endl;
                            }

                            agetTree->Fill();               //fill this event
                            nHits = 0;                      //reset nHits

                        }

                        stream.read((char*)(&event_headerbuf),2);                //get the first line of event header,contains source id,seems useless now
                        for (int i = 0; i < 3; i++) {
                            stream.read((char*)(&timestampbuf[i]), 2);          //get the time stamp of this event, seems useless now
                        }
                        stream.read((char*)(&triggeridbuf[0]), 2);
                        stream.read((char*)(&triggeridbuf[1]), 2);

                        bithigh = ((triggeridbuf[0] << 24) >> 24);
                        bitlow = (triggeridbuf[0] << 16) >> 24;                       //get the trigger id of this event, IMPORTANT!
                        triggeridbuf[0] = bithigh * 256 + bitlow;

                        bithigh = ((triggeridbuf[1] << 24) >> 24);
                        bitlow = (triggeridbuf[1] << 16) >> 24;
                        triggeridbuf[1] = bithigh * 256 + bitlow;

                        triggerid[1] = 4096 * triggeridbuf[1] + triggeridbuf[0];           //renew current trigger id
                        
                        for (int i = 0; i < 8; i++) {
                            stream.read((char*)(&header_restbuf[i]), 2);        //get rest parts of the header packet,useless now
                        }

                    }
                    else if (gheader[1] == 0x0404) {
                        //this means this packet is an event data packet
                        ////data packet must be inside a header-end pair
                        //if (pad_count != 2 && pad_count != 1) {
                        //    cout << "Warning: The number of event header and event end do not consist!!Current event number:" << event << endl;
                        //    break;
                        //}
                        isdata = true;
                        stream.read((char*)(&datafirstlinebuf), 2);
                        fecbuf = (datafirstlinebuf << 26) >> 27;
                        chipbuf1 = (datafirstlinebuf << 31) >> 31;
                        chipbuf2 = (datafirstlinebuf << 16) >> 31;
                        chipbuf = 2 * chipbuf1 + chipbuf2;
                        chnbuf = (datafirstlinebuf << 17) >> 25;

                        for (int i = 0; i < Nsp; i++) {
                            stream.read((char*)(&ADC_and_PFX), 2);
                            ADCbuf1 = (ADC_and_PFX << 28) >> 28;
                            ADCbuf2 = (ADC_and_PFX << 16) >> 24;
                            ADCbuf[i] = 256 * ADCbuf1 + ADCbuf2;
                            
                            //get the maximum point and ADC
                            if (ADCbuf[i] > maxADCbuf) {
                                maxADCbuf = ADCbuf[i];
                                maxPointbuf = i;
                            }
                            sumADCbuf += ADCbuf[i];
                        }
                        stream.read((char*)(&nullwordbuf), 2);          //null word of the payload, useless now
                    }
                    else if (gheader[1] == 0x0820) {
                        //this means this packet is an event tail
                        //pad_count--;                                            //end of a header-end pair
                        //if (pad_count != 0 && pad_count != 1) {
                        //    cout << "Warning: The number of event header and event end do not consist!!Current event number:"<<event << endl;
                        //    break;
                        //}
                        stream.read((char*)(&checkbuf), 2);                     //contains the source id for checking, useless now
                        stream.read((char*)(&reservedbuf), 2);                  //reserved for future use
                        stream.read((char*)(&eventsizebuf[0]), 2);
                        stream.read((char*)(&eventsizebuf[1]), 2);
                    }
                    else {
                        cout << "Warning: Something wrong with the header format!" << endl;
                        continue;
                    }

                    //read the Protocol Trailer of the packet, seems useless now
                    stream.read((char*)(&gtail[0]), 2);
                    stream.read((char*)(&gtail[1]), 2);

                    //--------------------END OF READING PACKET-----------------------



                    //hit data recorded
                    if (isdata) {
                        Fec[nHits] = fecbuf;
                        Chip[nHits] = chipbuf;
                        Chn[nHits] = chnbuf;
                        Channel[nHits] = chipbuf*Nch+chnbuf;


                        //regular test with 1 fec board used:
                        if(Chn[nHits]>=Nch) {           //Wrong chn number
                            pixelX[nHits]=0;
                            pixelY[nHits]=0;
                        }
                        else{
                            pixelX[nHits]=map0[Chip[nHits]][Chn[nHits]][0];         //if the hit is in x channels,pixelX!=0,pixelY=0
                            pixelY[nHits]=map0[Chip[nHits]][Chn[nHits]][1];         //if the hit is in y channels,pixelX=0,pixelY!=0
                        }

                        // if(Chip[nHits] == 0 && Chn[nHits] == 50){
                        //     cout << "pixelX[nHits] " << pixelX[nHits] << " pixelY[nHits] " << pixelY[nHits] << endl;
                        // }

                        // //charge distribution test with 2 boards used:
                        // if(Fec[nHits]==31){
                        //     if(Chip[nHits]==0||Chip[nHits]==2){
                        //         pixelX[nHits]=map0[0][Chn[nHits]][0];         //if the hit is in x channels,pixelX!=0,pixelY=0
                        //         pixelY[nHits]=map0[0][Chn[nHits]][1];         //if the hit is in y channels,pixelX=0,pixelY!=0
                        //     }
                        //     else{
                        //         pixelX[nHits]=map0[1][Chn[nHits]][0];
                        //         pixelY[nHits]=map0[1][Chn[nHits]][1];
                        //     }
                        // }
                        // else{
                        //     if(Chip[nHits]==0||Chip[nHits]==2){
                        //         pixelX[nHits]=map0[2][Chn[nHits]][0];
                        //         pixelY[nHits]=map0[2][Chn[nHits]][1];
                        //     }
                        //     else{
                        //         pixelX[nHits]=map0[3][Chn[nHits]][0];         
                        //         pixelY[nHits]=map0[3][Chn[nHits]][1]; 
                        //     }
                        // }

                        if(pixelX[nHits]==0&&pixelY[nHits]==0){
                            //cout<<"Warning: There maybe hits directing to wrong x, y position!\n";
                            k++;
                        }
                        sumADC[nHits] = sumADCbuf;
                        maxADC[nHits] = maxADCbuf;

                        maxPoint[nHits] = maxPointbuf;


                        //Int_t conNo = mapL2[Fec[nHits]] + (int)(chipbuf / 12);
                        //Int_t conCh = mapL1[chipbuf - 10][chnbuf];                      //4 cards*4 chips*64 channels-->32 xpixels*32 ypixels
                        //pixelX[nHits] = mapL0[conNo][conCh][0];
                        //pixelY[nHits] = mapL0[conNo][conCh][1];
                        for (int i = 0; i < Nsp; i++) {
                            ADC[nHits][i] = ADCbuf[i];
                        }
                        nHits++;
                    }

                    // if (triggerid[0]!=triggerid[1]) {               //this means a new trigger begins, i.e. a new event
                    //     if (nHits != 0) {
                    //         event++;                        //event count +1
                    //         summaxADC = 0;                          //NEW! Compute the summaxADC of this event
                    //         for (int i = 0; i < nHits; i++) {
                    //             summaxADC = summaxADC + maxADC[i];
                    //         }

                    //         agetTree->Fill();               //fill this event
                    //     }
                    //     // else {
                    //     //     cout << "Current event:" << event << endl;
                    //     //     cout << "last trigger:" << triggerid[0] << "this trigger:" << triggerid[1] << endl;
                    //     //     k++;
                    //     // }
                    //     triggerid[0] = triggerid[1];
                    //     nHits = 0;                      //reset hit number, prepare for the next event
                    // }

                    //cout << "looping" << endl;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

          } //single packact loop end
                else continue;
        }//Loop end

        //count and fill the final event
        if (nHits != 0) {
            event++;                        //event count +1
            summaxADC = 0;
            for(int i = 0;i<nHits;i++){
                summaxADC = summaxADC + maxADC[i];
                if(Chn[i]>67)  cout<<"Chip no:"<<Chip[i]<<"  Channel no:"<<Chn[i]<<" Event no:"<<event<<endl;
            }
            agetTree->Fill();               //fill this event
            nHits = 0;
        }
        //cout << "Total wrong event count:" << k << endl;
        cout << "Total hits with invalid x, y position:" << k << endl;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        outFile->cd();
        outFile->Write();
        outFile->Close();
        cout<<"Loop end!"<<endl;
        cout<<"Total Events:"<<dec<<event<<endl;
        return 0;
}
