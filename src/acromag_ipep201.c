// old directions: run with  > ./digitalIO /dev/sis1100_00remote 0x5500ffff 
#include "acromag_ipep201.h"
//______________________________________________________________________________
void InitFPGAStruct(struct fpga *myFPGA){

   myFPGA->fCarrierAddr    = 0; 
   myFPGA->fIOSpaceAddr    = 0; 
   myFPGA->fIDSpaceAddr    = 0; 
   myFPGA->fNSignals       = 0;
   myFPGA->fBitPatternFlag = 0; 
   myFPGA->fClockFrequency = gFPGAClockFreq; // TODO: Is there a better way to do this?   
   
   int i=0;
   for(i=0;i<FPGAArraySize;i++){
      myFPGA->fFlag[i]              = 0; 
      myFPGA->fSignalAddrStartLo[i] = 0; 
      myFPGA->fSignalAddrStartHi[i] = 0; 
      myFPGA->fSignalAddrEndLo[i]   = 0; 
      myFPGA->fSignalAddrEndHi[i]   = 0; 
      myFPGA->fSignalStartTimeLo[i] = 0; 
      myFPGA->fSignalStartTimeHi[i] = 0; 
      myFPGA->fSignalEndTimeLo[i]   = 0; 
      myFPGA->fSignalEndTimeHi[i]   = 0; 
      myFPGA->fSignalPulseTimeLo[i] = 0; 
      myFPGA->fSignalPulseTimeHi[i] = 0; 
   }

}
//______________________________________________________________________________
void InitFPGAGlobalVariables(void){

   // FLASH programming; turn this on 
   // if the VHDL code was programmed to FLASH memory. 
   gIsFLASH = 1; 

   gFPGAClockFreq = 8E+6; // 8 MHz clock 

   gModBase    = 0x00000000;      
   // I/O Space
   gIPAIOSpace = 0x00000000; 
   gIPBIOSpace = 0x00000100; 
   gIPCIOSpace = 0x00000200; 
   gIPDIOSpace = 0x00000300; 

   gIPAIDSpace = gIPAIOSpace + 0x80;  
   gIPBIDSpace = gIPBIOSpace + 0x80;  
   gIPCIDSpace = gIPCIOSpace + 0x80;  
   gIPDIDSpace = gIPDIOSpace + 0x80;  

   RECEIVE_GATE_COUNTS    = 0; 
   RECEIVE_GATE_TIME_SEC  = 0; 
   
   const int cSIZE = 2000;  
   RECEIVE_GATE_INPUT_TIME_UNITS = (char *)malloc( sizeof(char)*cSIZE );

}
//______________________________________________________________________________
void InitFPGAAddresses(){

   // Set the (virtual) FPGA addresses (actually, they're SRAM addresses) 
   // for the various signals we need.  These correspond to addresses to pass to the 
   // memory address register on the FPGA.  These are needed
   // to access the SRAM from the FPGA. 

   // Map for signals coming out on 50-pin header 
   // Pin       Description    SRAM Address
   // -----------------------------------------------------
   // 1         mech_sw_1      0x0002
   // 2         mech_sw_2      0x000a
   // 3         mech_sw_3      0x0012
   // 4         mech_sw_4      0x001a
   // 5         rf_sw_1        0x0022
   // 6         rf_sw_2        0x002a
   // 7         rf_sw_3        0x0032
   // 8         rf_clear       0x003a
   // 9         rf_pulse       0x0042
   // 10        rf_gate        0x004a
   // 50        ground         ---  

   gMasterList[0]  = "global_on_off";  
   gMasterList[1]  = "mech_sw_1";  // second-to-least significant bit
   gMasterList[2]  = "mech_sw_2"; 
   gMasterList[3]  = "mech_sw_3";
   gMasterList[4]  = "mech_sw_4";
   gMasterList[5]  = "rf_sw_1";
   gMasterList[6]  = "rf_sw_2"; 
   gMasterList[7]  = "rf_sw_3";
   gMasterList[8]  = "rf_clear";
   gMasterList[9]  = "rf_pulse";  
   gMasterList[10] = "rf_gate";   // most significant bit

   // gMechSwitchAddr[0] = gOffset + 0x0002; 
   // gMechSwitchAddr[1] = gOffset + 0x000a; 
   // gMechSwitchAddr[2] = gOffset + 0x0012; 
   // gMechSwitchAddr[3] = gOffset + 0x001a; 
   // gRFSwitchAddr[0]   = gOffset + 0x0022; 
   // gRFSwitchAddr[1]   = gOffset + 0x002a; 
   // gRFSwitchAddr[2]   = gOffset + 0x0032; 
   // gRFSwitchAddr[3]   = gOffset + 0x003a; 
   // gRFPulseAddr       = gOffset + 0x0042; 
   // gRFGateAddr        = gOffset + 0x004a;
   // // // assuming there is no A(0) in the FPGA code
   // // gDigitizerAddr     = 0x0010;           // NOTE: this is NOT in the same space as the above (which looks at SRAM)   
   // // gDigitizerAddr2    = 0x0011;           // NOTE: this is NOT in the same space as the above (which looks at SRAM)  
   // // The FPGA code uses the labels starting from 0, going up through F.  However, 
   // // it's actually offset by 16, so we start with 0x20 (decimal 32) instead of 0x10 (decimal 16).   
   // gDigitizerAddr     = 0x0020;           // NOTE: this is NOT in the same space as the above (which looks at SRAM)   
   // gDigitizerAddr2    = 0x0024;           // NOTE: this is NOT in the same space as the above (which looks at SRAM)   

   gMechSwitchAddr[0] = gOffset + MECHANICAL_SWITCH_1_ADDR; 
   gMechSwitchAddr[1] = gOffset + MECHANICAL_SWITCH_2_ADDR; 
   gMechSwitchAddr[2] = gOffset + MECHANICAL_SWITCH_3_ADDR; 
   gMechSwitchAddr[3] = gOffset + MECHANICAL_SWITCH_4_ADDR; 
   gRFSwitchAddr[0]   = gOffset + RF_SWITCH_1_ADDR; 
   gRFSwitchAddr[1]   = gOffset + RF_SWITCH_2_ADDR; 
   gRFSwitchAddr[2]   = gOffset + RF_SWITCH_3_ADDR; 
   gRFSwitchAddr[3]   = gOffset + RF_CLEAR_ADDR; 
   gRFPulseAddr       = gOffset + RF_PULSE_ADDR; 
   gRFGateAddr        = gOffset + RF_GATE_ADDR;
   // The FPGA code uses the labels starting from 0, going up through F.  However, 
   // it's actually offset by 16, so we start with 0x20 (decimal 32) instead of 0x10 (decimal 16).   
   gDigitizerAddr     = DIGITIZER_ADDR_1;           // NOTE: this is NOT in the same space as the above (which looks at SRAM)   
   gDigitizerAddr2    = DIGITIZER_ADDR_2;           // NOTE: this is NOT in the same space as the above (which looks at SRAM)   

   int i=0;
   gMasterAddrList[0] = 0x0000; 
   for(i=0;i<4;i++) gMasterAddrList[i+1] = gMechSwitchAddr[i]; 
   for(i=0;i<4;i++) gMasterAddrList[i+5] = gRFSwitchAddr[i]; 
   gMasterAddrList[9]  = gRFPulseAddr;   
   gMasterAddrList[10] = gRFGateAddr;   

}
//______________________________________________________________________________
int InitFPGA(int p,struct fpga *myFPGA){

   InitFPGAGlobalVariables();                          // this must be called first!  
   InitFPGAAddresses();
   InitFPGAStruct(myFPGA);                             // no need for dereference, this function takes a pointer 

   int carrier_addr = gModBase; 
   int fpga_io_sp   = gIPBIOSpace;          
   int fpga_id_sp   = fpga_io_sp + 0x80;  

   // set the name of the FPGA 
   myFPGA->fName        = "Acromag IP-EP201";
   // set the carrier, IO space and ID space addresses 
   myFPGA->fCarrierAddr = carrier_addr; 
   myFPGA->fIOSpaceAddr = fpga_io_sp; 
   myFPGA->fIDSpaceAddr = fpga_id_sp; 

   // import the signals 
   char *pulse_fn = "./input/pulse-data.dat";
   ImportPulseData(pulse_fn,myFPGA);                   // must pass by reference so we keep the data in the struct!  InitFPGA takes myFPGA as a pointer originally  

   int ret_code = 0;
   // ret_code = TimingCheck(*myFPGA);
   printf("[AcromagFPGA]: WARNING: No timing check!\n"); 

   if(ret_code!=0) return ret_code; 

   // get recieve gate time 
   // int ReceiveGateCounts = 0;

   // char *rec_gate    = "rf_sw_3";
   // const int cSIZE   = 2000;
   // char *fpgaName    = (char *)malloc( sizeof(char)*cSIZE );
   // char *signalUnits = (char *)malloc( sizeof(char)*cSIZE );

   // double fpgaClkFreq = (double)myFPGA->fClockFrequency;

   // int NF = myFPGA->fNSignals;
   // int i=0;
   // for(i=0;i<NF;i++){
   //    fpgaName = myFPGA->fSignalName[i];
   //    if( AreEquivStrings(fpgaName,rec_gate) ){
   //       ReceiveGateCounts = myFPGA->fSignalPulseTimeLo[i] + pow(2,16)*myFPGA->fSignalPulseTimeHi[i];
   //       strcpy(signalUnits,myFPGA->fSignalUnits[i]);
   //    }
   // }

   // double ReceiveGateTime = GetTimeInUnits(ReceiveGateCounts,fpgaClkFreq,second);

   // RECEIVE_GATE_COUNTS    = ReceiveGateCounts; 
   // RECEIVE_GATE_TIME_SEC  = ReceiveGateTime; 
   // strcpy(RECEIVE_GATE_INPUT_TIME_UNITS,signalUnits);

   if(gIsDebug && gVerbosity >=4) PrintFPGA(*myFPGA);  // PrintFPGA takes the struct by value, so we dereference the pointer  

   // initialize the FPGA pins for output 

   int mode = -1; 
   if(gIsFLASH){
      // reset the board since we're in flash mode
      // (puts board in user mode) 
      mode = CheckMode(p,carrier_addr,fpga_id_sp);
      if(mode==0){ // if in configuration mode, reset the board
         BoardReset(p,carrier_addr,fpga_io_sp);  
         mode = CheckMode(p,carrier_addr,fpga_id_sp);
      }
   }else{
      mode = CheckMode(p,carrier_addr,fpga_id_sp);
   }

   // always issue a software reset at the start, according to the manual 
   SoftwareReset(p,carrier_addr,fpga_io_sp); 
   sleep(1);  // wait 1 second so we can see on the scope that things are being reset 
   // set the clock speed on the FPGA 
   // SetClockSpeed(p,carrier_addr,fpga_io_sp,choice);  
   // set control register ID bits -- tells the FPGA what kind of board we have
   int id_bit_pattern = 0x700; // IP EP-201 
   SetCtrlRegIDBits(p,carrier_addr,fpga_io_sp,id_bit_pattern);    
   // set direction control register; logic low => input; logic high => output 
   // bit 6  => pins 0--7
   // bit 7  => pins 8--15 
   // bit 8  => pins 16--23 
   // bit 9  => pins 24--31 
   // bit 10 => pins 32--39 
   // bit 11 => pins 40--47   
   int dir_ctrl_bit_pattern = 0x0fc0; // sets all pins to outputs  
   SetDirCtrlReg(p,carrier_addr,fpga_io_sp,dir_ctrl_bit_pattern);     
   // set a group of I/O bits  
   // bit_pattern = 0x2;    // set pin 1 high
   // bit_pattern = 0x3;    // set pin 0 and 1 high
   // bit_pattern = 0x4;    // set pin 2 high
   // bit_pattern = 0x5;    // set pin 0 and 2 high
   // bit_pattern = 0x7;    // set pin 0, 1 and 2 high
   int io_bit_pattern = 0xff; // set all pins high 
   SetIOBitsAlt(p,carrier_addr,fpga_io_sp,0x2,io_bit_pattern); // pins 0--7               
   io_bit_pattern = 0xffff; // set all pins high 
   SetIOBitsAlt(p,carrier_addr,fpga_io_sp,0x2,io_bit_pattern); // pins 8--15               
   io_bit_pattern = 0xff; // set all pins high 
   SetIOBitsAlt(p,carrier_addr,fpga_io_sp,0x4,io_bit_pattern); // pins 16--23              
   io_bit_pattern = 0xffff; // set all pins high 
   SetIOBitsAlt(p,carrier_addr,fpga_io_sp,0x4,io_bit_pattern); // pins 23--31              
   io_bit_pattern = 0xff; // set all pins high 
   SetIOBitsAlt(p,carrier_addr,fpga_io_sp,0x6,io_bit_pattern); // pins 32--39              
   io_bit_pattern = 0xffff; // set all pins high 
   SetIOBitsAlt(p,carrier_addr,fpga_io_sp,0x6,io_bit_pattern); // pins 40--47              
   
   return 0;
}
//______________________________________________________________________________
void BlankFPGA(int p,struct fpga *myFPGA){

   // set global on/off switch to off
   myFPGA->fFlag[0] = 0;   
    
   // reconstruct bit pattern flag 
   const int N             = myFPGA->fNSignals;  
   int bit_pattern_flag    = GetBitPattern(N,myFPGA->fSignalName,myFPGA->fFlag); 
   myFPGA->fBitPatternFlag = bit_pattern_flag; 

   // dereference the pointer, program new bit pattern to FPGA
   ProgramSignalsToFPGA(p,*myFPGA);    

}
//______________________________________________________________________________
int ProgramSignalsToFPGA(int p,const struct fpga myFPGA){

   int i=0;
   const int N          = myFPGA.fNSignals;  
   int carrier_addr     = myFPGA.fCarrierAddr; 
   int fpga_io_sp       = myFPGA.fIOSpaceAddr;
   int bit_pattern_flag = myFPGA.fBitPatternFlag;  

   int fpga_timing_flag = 0; 

   // blank the signals as we're going to set the timing. 
   // writing 0x0 to 0x0054 will turn off output, as this 
   // flips the value of the timing flag on the FPGA.  
   // WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,0x0054,0x0); 

   fpga_timing_flag = IsFPGATimingSet(p,carrier_addr,fpga_io_sp); 

   // Set flags on FPGA 
   if(gIsDebug && gVerbosity>=0)  printf("[AcromagFPGA]: Setting flags... \n"); 
   if(!gIsDebug) printf("[AcromagFPGA]: Setting flags... "); 
   WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,0x0000,bit_pattern_flag);    
   if(!gIsDebug) printf(" Done. \n"); 

   // blank the signals as we're going to set the timing. 
   // writing 0x0 to 0x0054 will turn off output, as this 
   // flips the value of the timing flag on the FPGA.  
   WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,0x0054,0x0); 

   if(gIsDebug && gVerbosity>=0)  printf("[AcromagFPGA]: Programming signals... \n"); 
   if(!gIsDebug) printf("[AcromagFPGA]: Programming signals... "); 
   // Write signals onto the SRAM for the FPGA to use   
   // Note: i = 0 is the global on/off, so we skip the 0th entry for StartTime and EndTime
   for(i=1;i<N;i++){
      // since we are encoding start and end times of signals, 
      // we need an extra address for a given probe.
      if(gIsDebug && gVerbosity>=0) printf("%s addr = 0x%04x start_lo = %d (0x%04x) start_hi = %d (0x%04x) end_lo = %d (0x%04x) end_hi = %d (0x%04x) \n",
            myFPGA.fSignalName[i],
            myFPGA.fSignalAddrStartLo[i],
            myFPGA.fSignalStartTimeLo[i],
            myFPGA.fSignalStartTimeLo[i],
            myFPGA.fSignalStartTimeHi[i],
            myFPGA.fSignalStartTimeHi[i],
            myFPGA.fSignalEndTimeLo[i],
            myFPGA.fSignalEndTimeLo[i],
            myFPGA.fSignalEndTimeHi[i],
            myFPGA.fSignalEndTimeHi[i]); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,myFPGA.fSignalAddrStartLo[i],myFPGA.fSignalStartTimeLo[i]);   
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,myFPGA.fSignalAddrStartHi[i],myFPGA.fSignalStartTimeHi[i]);   
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,myFPGA.fSignalAddrEndLo[i]  ,myFPGA.fSignalEndTimeLo[i]);   
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,myFPGA.fSignalAddrEndHi[i]  ,myFPGA.fSignalEndTimeHi[i]);   
   }

   // update signal timestamps
   WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,0x0054,0x1);  

   if(!gIsDebug) printf(" Done. \n"); 

   // read back what we wrote to the SRAM as a double-check 
   int data=0,dummy=0;  // dummy is used to remove warnings in compilation concerning data  
   data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,0x0000);
   dummy *= data;

   for(i=1;i<N;i++){
      if(gIsDebug && gVerbosity>=0) printf("%s addr = 0x%04x start_lo = %d (0x%04x) start_hi = %d (0x%04x) end_lo = %d (0x%04x) end_hi = %d (0x%04x) \n",
            myFPGA.fSignalName[i],
            myFPGA.fSignalAddrStartLo[i],
            myFPGA.fSignalStartTimeLo[i],
            myFPGA.fSignalStartTimeLo[i],
            myFPGA.fSignalStartTimeHi[i],
            myFPGA.fSignalStartTimeHi[i],
            myFPGA.fSignalEndTimeLo[i],
            myFPGA.fSignalEndTimeLo[i],
            myFPGA.fSignalEndTimeHi[i],
            myFPGA.fSignalEndTimeHi[i]); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,myFPGA.fSignalAddrStartLo[i]); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,myFPGA.fSignalAddrStartHi[i]);
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,myFPGA.fSignalAddrEndLo[i]);
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,myFPGA.fSignalAddrEndHi[i]);
   } 

   // read back value of updated signal timestamps
   data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,0x0054); 

   // print processed signals to screen 
   PrintSummary(myFPGA); 

   fpga_timing_flag = IsFPGATimingSet(p,carrier_addr,fpga_io_sp); 

   return fpga_timing_flag; 
}
//______________________________________________________________________________
void PrintFPGA(const struct fpga myFPGA){

   int N = myFPGA.fNSignals;
 
   if(N==0){
      printf("No data found in myFPGA!  Exiting... \n");
      exit(1);
   }else if(N>10){
      printf("Invalid number of entries in myFPGA!  N = %d.  Exiting... \n",N); 
      exit(1);
   }

   printf("FPGA Board Characteristics: \n");
   printf("name: %s carrier addr: 0x%04x io space addr: 0x%04x ID space addr: 0x%04x \n",myFPGA.fName,myFPGA.fCarrierAddr,myFPGA.fIOSpaceAddr,myFPGA.fIDSpaceAddr);

   printf("------------------------------- \n");  

   int i=0;
   for(i=0;i<N;i++){
      printf("name           = %-15s  \n",myFPGA.fSignalName[i]); 
      printf("units          = %-2s   \n",myFPGA.fSignalUnits[i]);
      printf("flag           = %-1d   \n",myFPGA.fFlag[i]); 
      printf("start addr (l) = 0x%04x \n",myFPGA.fSignalAddrStartLo[i]); 
      printf("start addr (h) = 0x%04x \n",myFPGA.fSignalAddrStartHi[i]); 
      printf("end addr (l)   = 0x%04x \n",myFPGA.fSignalAddrEndLo[i]); 
      printf("end addr (h)   = 0x%04x \n",myFPGA.fSignalAddrEndHi[i]); 
      printf("start t (l)    = %-6d   \n",myFPGA.fSignalStartTimeLo[i]);  
      printf("start t (h)    = %-6d   \n",myFPGA.fSignalStartTimeHi[i]);  
      printf("end t (l)      = %-6d   \n",myFPGA.fSignalEndTimeLo[i]);  
      printf("end t (h)      = %-6d   \n",myFPGA.fSignalEndTimeHi[i]);  
      printf("pulse t (l)    = %-6d   \n",myFPGA.fSignalPulseTimeLo[i]);  
      printf("pulse t (h)    = %-6d   \n",myFPGA.fSignalPulseTimeHi[i]); 
      printf("------------------------------- \n");  
   }

   u_int16_t hex_flag = (u_int16_t)myFPGA.fBitPatternFlag; 
   printf("FPGA flag bit pattern: 0x%04x (hex) %d (decimal) \n",hex_flag,myFPGA.fBitPatternFlag); 

}
//______________________________________________________________________________
void ImportPulseData(char *filename,struct fpga *myFPGA){

   // start_count = start of pulse
   // end_count   = end of pulse
   // count       = total duration of pulse 

   int ipulse_cnt,istart_cnt,iend_cnt;  
   double istart_time,ipulse_time; 
   int j=0,k=0,N=0; 
   const int MAX = 2000; 
   const int uMAX=2;
   const int fMAX=3;
   const int tMAX=20;  
   const int mMAX=20;  
   const int cMAX=2; 
   int istart_cnt_v[cMAX],iend_cnt_v[cMAX],ipulse_cnt_v[cMAX]; 
   char buf[MAX],itag[tMAX],iunit[uMAX],iflag[fMAX]; 
   char *mode = "r"; 

   char **module            ,**unit;
   int addr[mMAX]           ,flag[mMAX];
   int start_count_low[mMAX],start_count_high[mMAX];
   int end_count_low[mMAX]  ,end_count_high[mMAX];
   int pulse_count_low[mMAX],pulse_count_high[mMAX];

   module = (char**)malloc( sizeof(char*)*mMAX );
   unit   = (char**)malloc( sizeof(char*)*mMAX );

   int ii=0;
   for(ii=0;ii<mMAX;ii++){
      addr[ii]             = 0; 
      flag[ii]             = 0;
      start_count_low[ii]  = 0;  
      pulse_count_low[ii]  = 0;  
      end_count_low[ii]    = 0;  
      start_count_high[ii] = 0;  
      pulse_count_high[ii] = 0;  
      end_count_high[ii]   = 0;  
   }

   double ClockFreq = myFPGA->fClockFrequency; 

   FILE *infile; 
   infile = fopen(filename,mode);

   if(infile==NULL){
      printf("[AcromagFPGA::ImportPulseData]: Cannot open the file: %s.  Exiting... \n",filename);
      exit(1);  
   }else{
      if(gIsDebug) printf("[AcromagFPGA::ImportPulseData]: Opening the file: %s... \n",filename);
      while( !feof(infile) ){
         if(k==0){
            fgets(buf,MAX,infile);
         }else{
            fscanf(infile,"%s %s %lf %lf %s",itag,iflag,&istart_time,&ipulse_time,iunit);
            if( !AreEquivStrings(itag,eof_tag) ){ 
               // convert times to clock counts 
               istart_cnt          = GetClockCounts(istart_time,ClockFreq,iunit); 
               ipulse_cnt          = GetClockCounts(ipulse_time,ClockFreq,iunit); 
               iend_cnt            = istart_cnt + ipulse_cnt; 
               // divide numbers into low- and high-order bytes 
               ComputeLowAndHighBytes(istart_cnt,istart_cnt_v); 
               ComputeLowAndHighBytes(ipulse_cnt,ipulse_cnt_v); 
               ComputeLowAndHighBytes(iend_cnt  ,iend_cnt_v); 
               // fill arrays (low bytes) 
               start_count_low[j]  = istart_cnt_v[0]; 
               pulse_count_low[j]  = ipulse_cnt_v[0]; 
               end_count_low[j]    = iend_cnt_v[0]; 
               // fill arrays (high bytes) 
               start_count_high[j] = istart_cnt_v[1]; 
               pulse_count_high[j] = ipulse_cnt_v[1]; 
               end_count_high[j]   = iend_cnt_v[1]; 
               // get the module name  
               module[j]           = (char*)malloc( sizeof(char)*(mMAX+1) );  
               strcpy(module[j],itag);   
               // get the time units 
               unit[j]             = (char*)malloc( sizeof(char)*(mMAX+1) );  
               strcpy(unit[j],iunit);  
               // get address of module
               addr[j]             = GetAddress(module[j]);  
               // set on/off flag 
               if( AreEquivStrings(iflag,on) || AreEquivStrings(iflag,ON) ){  
                  flag[j] = 1; 
               }else if( AreEquivStrings(iflag,off) || AreEquivStrings(iflag,OFF) ){
                  flag[j] = 0;
               }
               // move on to next entry 
               if(gIsDebug && gVerbosity>=1) printf("Adding: index = %d \t",j); 
               if(gIsDebug && gVerbosity>=1) printf("ID = %s \t"           ,module[j]);
               if(gIsDebug && gVerbosity>=1) printf("addr = 0x%04x \t"     ,addr[j]);
               if(gIsDebug && gVerbosity>=1) printf("flag = %i \t"         ,flag[j]);
               if(gIsDebug && gVerbosity>=1) printf("start (l) = %d \t"    ,start_count_low[j]); 
               if(gIsDebug && gVerbosity>=1) printf("start (h) = %d \t"    ,start_count_high[j]);
               if(gIsDebug && gVerbosity>=1) printf("pulse (l) = %d \t"    ,pulse_count_low[j]); 
               if(gIsDebug && gVerbosity>=1) printf("pulse (h) = %d \t"    ,pulse_count_high[j]);
               if(gIsDebug && gVerbosity>=1) printf("end (l) = %d \t"      ,end_count_low[j]);
               if(gIsDebug && gVerbosity>=1) printf("end (h) = %d \n"      ,end_count_high[j]); 
               if(gIsDebug && gVerbosity>=1) printf("(pulse time = %lf %s) \n",ipulse_time,iunit);
               j++; 
            }else{
               break;
            }
         }
         k++; 
      }
      N = j; 
      fclose(infile); 
      if(N==0){
         printf("[AcromagFPGA::ImportPulseData]: No data!  Exiting... \n");
         exit(1);  
      }
   }

   // now add to myFPGA 
   myFPGA->fNSignals = N; 

   myFPGA->fSignalName  = (char**)malloc( sizeof(char*)*mMAX );
   myFPGA->fSignalUnits = (char**)malloc( sizeof(char*)*mMAX );

   int i=0;
   for(i=0;i<N;i++){
      myFPGA->fSignalName[i]        = (char*)malloc( sizeof(char)*(mMAX+1) );  
      strcpy(myFPGA->fSignalName[i],module[i]); 
      myFPGA->fSignalUnits[i]       = (char*)malloc( sizeof(char)*(mMAX+1) );  
      strcpy(myFPGA->fSignalUnits[i],unit[i]);
      myFPGA->fFlag[i]              = flag[i]; 
      myFPGA->fSignalAddrStartLo[i] = addr[i]; 
      myFPGA->fSignalStartTimeLo[i] = start_count_low[i];  
      myFPGA->fSignalStartTimeHi[i] = start_count_high[i];  
      myFPGA->fSignalEndTimeLo[i]   = end_count_low[i];  
      myFPGA->fSignalEndTimeHi[i]   = end_count_high[i];  
      myFPGA->fSignalPulseTimeLo[i] = pulse_count_low[i];  
      myFPGA->fSignalPulseTimeHi[i] = pulse_count_high[i];  
   }

   // Finish setting addresses; the start high bytes, end low and high bytes
   // are offset from start low bytes by the specified amounts below. 
   for(i=0;i<N;i++){
      myFPGA->fSignalAddrStartHi[i] = myFPGA->fSignalAddrStartLo[i] + 0x0002; 
      myFPGA->fSignalAddrEndLo[i]   = myFPGA->fSignalAddrStartLo[i] + 0x0004; 
      myFPGA->fSignalAddrEndHi[i]   = myFPGA->fSignalAddrStartLo[i] + 0x0006; 
   }

   int bit_pattern_flag = GetBitPattern(N,module,flag); 
   myFPGA->fBitPatternFlag = bit_pattern_flag; 

   // get recieve gate time: the ADC needs this in the global variable below 
   int ReceiveGateCounts = 0;

   char *rec_gate     = "rf_sw_3";
   const int cSIZE    = 2000;
   char *fpgaName     = (char *)malloc( sizeof(char)*cSIZE );
   char *signalUnits  = (char *)malloc( sizeof(char)*cSIZE );

   double rec_gate_cnts = 0.; 

   int NF = myFPGA->fNSignals;
   for(i=0;i<NF;i++){
      fpgaName = myFPGA->fSignalName[i];
      if( AreEquivStrings(fpgaName,rec_gate) ){
         rec_gate_cnts     = (double)( myFPGA->fSignalPulseTimeLo[i] ) + (double)( pow(2,16)*myFPGA->fSignalPulseTimeHi[i] );
         ReceiveGateCounts = (int)rec_gate_cnts; 
         // ReceiveGateCounts = myFPGA->fSignalPulseTimeLo[i] + pow(2,16)*myFPGA->fSignalPulseTimeHi[i];
         strcpy(signalUnits,myFPGA->fSignalUnits[i]);
      }
   }

   double ReceiveGateTime = GetTimeInUnits(ReceiveGateCounts,ClockFreq,second);

   RECEIVE_GATE_COUNTS    = ReceiveGateCounts; 
   RECEIVE_GATE_TIME_SEC  = ReceiveGateTime; 
   strcpy(RECEIVE_GATE_INPUT_TIME_UNITS,signalUnits);

   // printf("RECEIVE_GATE_COUNTS   = %d  \n",RECEIVE_GATE_COUNTS); 
   // printf("RECEIVE_GATE_TIME_SEC = %lf \n",RECEIVE_GATE_TIME_SEC); 

   if(gIsDebug) printf("----------------------------------------- \n"); 

}
//______________________________________________________________________________
int TimingCheck(const struct fpga myFPGA){

   // make sure all timings are correct. 

   char *mech_sw_1    = "mech_sw_1"; 
   char *mech_sw_2    = "mech_sw_2"; 
   char *mech_sw_3    = "mech_sw_3"; 
   char *mech_sw_4    = "mech_sw_4"; 
   char *trans_gate   = "rf_sw_2";
   char *rec_gate     = "rf_sw_3";
   char *rf_gate      = "rf_gate";
   const int cSIZE    = 2000;
   char *fpgaName     = (char *)malloc( sizeof(char)*cSIZE );
   char *mech_sw_gate = (char *)malloc( sizeof(char)*cSIZE );

   int mech_sw_counter=0; 

   int trans_gate_start_counts  = 0; 
   int trans_gate_end_counts    = 0; 
   int rec_gate_start_counts    = 0; 
   int rec_gate_end_counts      = 0; 
   int rf_gate_start_counts     = 0; 
   int rf_gate_end_counts       = 0; 
   int mech_sw_gate_start_counts= 0;
   int mech_sw_gate_end_counts  = 0;

   // gather times 

   int i=0; 
   int NF = myFPGA.fNSignals;
   for(i=0;i<NF;i++){
      fpgaName = myFPGA.fSignalName[i];
      // mechanical switch 
      if( AreEquivStrings(fpgaName,mech_sw_1) || 
          AreEquivStrings(fpgaName,mech_sw_2) || 
          AreEquivStrings(fpgaName,mech_sw_3) || 
          AreEquivStrings(fpgaName,mech_sw_4) ){
         mech_sw_counter++; 
         mech_sw_gate              = fpgaName; 
         mech_sw_gate_start_counts = myFPGA.fSignalStartTimeLo[i] +  pow(2,16)*myFPGA.fSignalStartTimeHi[i];
         mech_sw_gate_end_counts   = myFPGA.fSignalEndTimeLo[i]   +  pow(2,16)*myFPGA.fSignalEndTimeHi[i];
      }
      // transmit gate (RF switch)  
      if( AreEquivStrings(fpgaName,trans_gate) ){
         trans_gate_start_counts = myFPGA.fSignalStartTimeLo[i] + pow(2,16)*myFPGA.fSignalStartTimeHi[i];
         trans_gate_end_counts   = myFPGA.fSignalEndTimeLo[i]   + pow(2,16)*myFPGA.fSignalEndTimeHi[i];
      }
      // receive gate (RF switch)  
      if( AreEquivStrings(fpgaName,rec_gate) ){
         rec_gate_start_counts = myFPGA.fSignalStartTimeLo[i] + pow(2,16)*myFPGA.fSignalStartTimeHi[i];
         rec_gate_end_counts   = myFPGA.fSignalEndTimeLo[i]   + pow(2,16)*myFPGA.fSignalEndTimeHi[i];
      }
      // TOMCO pulse  
      if( AreEquivStrings(fpgaName,rf_gate) ){
         rf_gate_start_counts = myFPGA.fSignalStartTimeLo[i] + pow(2,16)*myFPGA.fSignalStartTimeHi[i];
         rf_gate_end_counts   = myFPGA.fSignalEndTimeLo[i]   + pow(2,16)*myFPGA.fSignalEndTimeHi[i];
      }
   }

   // start the check

   int fail=0;
   int ret_code = 0; 
   if(mech_sw_counter>1){
      printf("[AcromagFPGA::TimingCheck]: Too many (%d) mechanical switches initialized!  Exiting... \n",mech_sw_counter);
      ret_code = -1;
   }

   double ClockFreq               = myFPGA.fClockFrequency; 
   double mech_sw_gate_start_time = GetTimeInUnits(mech_sw_gate_start_counts,ClockFreq,second);
   double mech_sw_gate_end_time   = GetTimeInUnits(mech_sw_gate_end_counts  ,ClockFreq,second);
   double rec_gate_start_time     = GetTimeInUnits(rec_gate_start_counts    ,ClockFreq,second);
   double rec_gate_end_time       = GetTimeInUnits(rec_gate_end_counts      ,ClockFreq,second);
   double trans_gate_start_time   = GetTimeInUnits(trans_gate_start_counts  ,ClockFreq,second);
   double trans_gate_end_time     = GetTimeInUnits(trans_gate_end_counts    ,ClockFreq,second);
   double rf_gate_start_time      = GetTimeInUnits(rf_gate_start_counts     ,ClockFreq,second);
   double rf_gate_end_time        = GetTimeInUnits(rf_gate_end_counts       ,ClockFreq,second);

   // is the transmit gate inside the mechanical switch gate? 
   if( (trans_gate_start_time > mech_sw_gate_start_time) && 
       (trans_gate_end_time < mech_sw_gate_end_time) ){
      // do nothing  
   }else{
      printf("[AcromagFPGA::TimingCheck]: %s gate is not inside %s gate! \n",trans_gate,mech_sw_gate); 
      printf("%s: %lf  %lf \n%s: %lf %lf \n",trans_gate,trans_gate_start_time,trans_gate_end_time,mech_sw_gate,mech_sw_gate_start_time,mech_sw_gate_end_time);  
      fail++; 
   } 
   // is the rf gate inside the transmit gate? 
   if( (rf_gate_start_time > trans_gate_start_time) && 
       (rf_gate_end_time < trans_gate_end_time) ){
      // do nothing  
   }else{
      printf("[AcromagFPGA::TimingCheck]: %s gate is not inside %s gate! \n",rf_gate,trans_gate); 
      printf("%s: %lf  %lf \n%s: %lf %lf \n",rf_gate,rf_gate_start_time,rf_gate_end_time,trans_gate,trans_gate_start_time,trans_gate_end_time);  
      fail++; 
   } 
   // is the receive gate AFTER the transmit gate? 
   if(rec_gate_start_time > trans_gate_end_time){
      // do nothing  
   }else{
      printf("[AcromagFPGA::TimingCheck]: %s gate starts before the end of the %s gate! \n",rec_gate,trans_gate);
      printf("%s: %lf \n%s: %lf \n",rec_gate,rec_gate_start_time,trans_gate,trans_gate_end_time);  
      fail++; 
   }
   // is the receive gate inside the mechanical switch gate? 
   if( (rec_gate_start_time > mech_sw_gate_start_time) && 
       (rec_gate_end_time < mech_sw_gate_end_time) ){
      // do nothing  
   }else{
      printf("[AcromagFPGA::TimingCheck]: %s gate is not inside %s gate! \n",rec_gate,mech_sw_gate); 
      printf("%s: %lf  %lf \n%s: %lf %lf \n",rec_gate,rec_gate_start_time,rec_gate_end_time,mech_sw_gate,mech_sw_gate_start_time,mech_sw_gate_end_time);  
      fail++; 
   } 
   // print error statement if necessary 
   if(fail>0){
      printf("[AcromagFPGA::TimingCheck]: Timing check failed %d times!  Exiting... \n",fail); 
      ret_code = -2;
   }

   return ret_code;
}
//______________________________________________________________________________
int GetBitPattern(int N,char **module_list,int *flag){

   // Set the bit pattern to be sent to the FPGA; this 
   // will enable/disable each individual signal   

   int i=0;
   const int MAX = mlMAX; 
   int MasterFlag[MAX];
   for(i=0;i<MAX;i++){
      MasterFlag[i] = 0; 
   }

   // compare module_list to MasterList and set the bit pattern according to flags
   int j=0;
   for(i=0;i<MAX;i++){       // master list loop 
      for(j=0;j<N;j++){      // module list loop 
         if( AreEquivStrings(module_list[j],gMasterList[i]) ){
            MasterFlag[i] = flag[j]; 
            if(gIsDebug && gVerbosity>=2) printf("master index i = %d input index j = %d flag = %d master flag = %d\n",i,j,flag[j],MasterFlag[i]); 
         }
      }
   }

   // print bit pattern
   // we invert the order here because we want MSB,...,LSB
   if(gIsDebug && gVerbosity>=2){ 
      printf("bit pattern: "); 
      for(i=MAX-1;i>=0;i--){
         printf("%d",MasterFlag[i]); 
      }
      printf("\n"); 
   }

   // compute decimal equivalent of the bit pattern 
   int arg=0;
   int decimal_flag = 0;  
   for(i=0;i<MAX;i++){  
      arg           = MasterFlag[i]*pow(2,i); 
      decimal_flag += arg;
      if(gIsDebug && gVerbosity>=2) printf("i = %d master_flag = %d arg = %d \n",i,MasterFlag[i],arg);  
   }

   // convert to hex (this is a check; this gets converted in WriteMemoryDataReg)  
   int hex_flag  = (u_int16_t)decimal_flag; 
   if(gIsDebug && gVerbosity>=2) printf("[AcromagFPGA::GetBitPattern]: Representation of flags: 0x%04x (hex) %d (decimal) \n",hex_flag,decimal_flag); 

   return decimal_flag; 
}
//______________________________________________________________________________
int GetAddress(char *module){

   // Since the input list may not be in the default order, 
   // we have to set the address list appropriately 
   int i=0,addr=0;
   for(i=0;i<mlMAX;i++){        // scan through the master list 
      if( AreEquivStrings(module,gMasterList[i]) ){
         if(gIsDebug && gVerbosity>=2) printf("ENTRY FOUND! i = %d master = %s module = %s addr = 0x%04x \n",i,gMasterList[i],module,gMasterAddrList[i]); 
         addr = gMasterAddrList[i];
      }
   }

   return addr; 

}
//______________________________________________________________________________
void ComputeLowAndHighBytes(int counts,int *v){

   // dividing the 32-bit integer counts into  
   // two 16-bit integers, stored in the array v  

   int counts_16l = counts & 0x0000ffff;            // low order bytes
   int num        = counts & 0xffff0000;            // high order bytes
   int den        = pow(2,16); 
   int counts_16h = num/den;                         
   v[0]           = counts_16l; 
   v[1]           = counts_16h;

}
//______________________________________________________________________________
int ReadFPGAMemory(int p,int carrier_addr,int daughter_addr,int my_mem_addr){

   // This write access requires four wait states.  I'm assuming 
   // that time corresponds to the 8 MHz clock on the board.  Let's use that. 
   // Then, four wait states is 4*125ns = 500 ns = .5 us.  Use 1 us (8 wait states), 
   // since usleep takes integers as arguments.  
   int time = 1; // wait 1 us 

   u_int16_t data16;
   // u_int16_t mem_addr = (u_int16_t)my_mem_addr;  
   int base_addr = carrier_addr + daughter_addr;
   int addr      = base_addr + my_mem_addr;  
   int ret_code  = vme_A16D16_read(p,addr,&data16);         // read from memory data register 
   if(gIsDebug && gVerbosity>=4) Print("ReadFPGAMemory","Reading...",addr,data16,ret_code); 
   usleep(time);  

   CheckStatus(addr,data16,ret_code); 

   int bit_pattern = (int)data16;
   return bit_pattern;  

}
//______________________________________________________________________________
int ReadMemoryDataReg(int p,int carrier_addr,int daughter_addr,int my_mem_addr){

   // printf("[AcromagFPGA::ReadMemoryDataReg]: Reading from the Memory Data Register... "); 
   // printf("First, writing to Memory Address Register... \n"); 

   // This write access requires four wait states.  I'm assuming 
   // that time corresponds to the 8 MHz clock on the board.  Let's use that. 
   // Then, four wait states is 4*125ns = 500 ns = .5 us.  Use 1 us (8 wait states), 
   // since usleep takes integers as arguments.  
   int time = 1; // wait 1 us 

   u_int16_t data16;
   u_int16_t mem_addr_data = (u_int16_t)my_mem_addr;  
   int base_addr = carrier_addr + daughter_addr; 
   int ret_code  = vme_A16D16_write(p,base_addr+0x16,mem_addr_data);   // write to memory addr register 
   if(gIsDebug && gVerbosity>=4) Print("ReadMemoryDataReg","Writing...",base_addr+0x16,mem_addr_data,ret_code); 
   usleep(time);  
   ret_code      = vme_A16D16_read( p,base_addr+0x14,&data16);         // read from memory data register 
   if(gIsDebug && gVerbosity>=4) Print("ReadMemoryDataReg","Reading...",base_addr+0x14,data16,ret_code); 
   usleep(time);  

   CheckStatus(base_addr+0x14,data16,ret_code); 

   int bit_pattern = (int)data16;
   return bit_pattern;  

}
//______________________________________________________________________________
void WriteMemoryDataReg(int p,int carrier_addr,int daughter_addr,int my_mem_addr,int bit_pattern){

   // printf("[AcromagFPGA::ReadMemoryDataReg]: Reading from the Memory Data Register... "); 
   // printf("First, writing to Memory Address Register... \n"); 

   // This write access requires four wait states.  I'm assuming 
   // that time corresponds to the 8 MHz clock on the board.  Let's use that. 
   // Then, four wait states is 4*125ns = 500 ns = .5 us.  Use 1 us (8 wait states), 
   // since usleep takes integers as arguments.   
   int time = 1; // wait 1 us 

   u_int16_t data16;
   u_int16_t mem_addr_data    = (u_int16_t)my_mem_addr;  
   u_int16_t bit_pattern_data = (u_int16_t)bit_pattern; 
   int base_addr = carrier_addr + daughter_addr; 
   int ret_code  = vme_A16D16_write(p,base_addr+0x16,mem_addr_data);       // write to memory addr register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Writing...",base_addr+0x16,mem_addr_data,ret_code); 
   usleep(time);  
   ret_code      = vme_A16D16_read(p,base_addr+0x16,&data16);              // read from memory addr register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Reading...",base_addr+0x16,data16,ret_code); 
   usleep(time);  
   ret_code      = vme_A16D16_write(p,base_addr+0x14,bit_pattern_data);    // write to memory data register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Writing...",base_addr+0x14,bit_pattern_data,ret_code); 
   usleep(time);  
   ret_code      = vme_A16D16_read(p,base_addr+0x14,&data16);              // read from memory data register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Reading...",base_addr+0x14,data16,ret_code); 
   usleep(time);  

   CheckStatus(base_addr+0x14,data16,ret_code); 

}
//______________________________________________________________________________
void SetClockSpeed(int p,int carrier_addr,int daughter_addr,int choice){
   // set the desired clock speed (MHz)
   // choice = 0 => 24 MHz
   // choice = 1 => 32 MHz
   // choice = 2 => 40 MHz
   // choice = 3 => 50 MHz
   int bit_pattern[3];
   int speed = 0;  
   switch(choice){
      case 0: // 24 MHz 
         bit_pattern[0] = 0x0010; 
         bit_pattern[1] = 0x242c; 
         bit_pattern[2] = 0x00;
         speed = 24; 
         break;
      case 1: // 32 MHz 
         bit_pattern[0] = 0x000c; 
         bit_pattern[1] = 0x242c; 
         bit_pattern[2] = 0x00;
         speed = 32; 
         break;
      case 2: // 40 MHz 
         bit_pattern[0] = 0x000a; 
         bit_pattern[1] = 0x242e; 
         bit_pattern[2] = 0x00;
         speed = 40; 
         break;
      case 3: // 50 MHz 
         bit_pattern[0] = 0x0008; 
         bit_pattern[1] = 0x242e; 
         bit_pattern[2] = 0x00;
         speed = 50; 
         break;
      default:
         printf("[AcromagFPGA::SetClockSpeed] Invalid clock speed!  Exiting... \n");
         exit(1); 
   }

   printf("[AcromagFPGA::SetClockSpeed]: Setting the programmable clock to %d MHz... \n",speed); 

   int base_addr = carrier_addr + daughter_addr; 
   int addr_1    = base_addr + 0x18;          // manual says 0x19, but we're in little-endian 
   int addr_2    = base_addr + 0x1a;          // manual says 0x1b, but we're in little-endian
   int addr_3    = base_addr + 0x1c;          // manual says 0x1d, but we're in little-endian 

   u_int16_t data_ctrl_reg_1 = (u_int16_t)bit_pattern[0]; 
   u_int16_t data_ctrl_reg_2 = (u_int16_t)bit_pattern[1]; 
   u_int16_t data_ctrl_reg_3 = (u_int16_t)bit_pattern[2];

   int ret_code = 0x0; 
   // int ret_code = vme_A16D16_read(p,my_addr,&data16);  
   // Print("SetClockSpeed","Reading...",my_addr,data16,ret_code); 
   ret_code = vme_A16D16_write(p,addr_1,data_ctrl_reg_1);  
   Print("SetClockSpeed","Writing...",addr_1,data_ctrl_reg_1,ret_code); 
   ret_code = vme_A16D16_write(p,addr_2,data_ctrl_reg_2);  
   Print("SetClockSpeed","Writing...",addr_2,data_ctrl_reg_2,ret_code); 
   ret_code = vme_A16D16_write(p,addr_3,data_ctrl_reg_3);  
   Print("SetClockSpeed","Writing...",addr_3,data_ctrl_reg_3,ret_code); 

   // now, we write 0x0 (manual says 0x1) to clock trigger register
   int clock_trig_reg_addr = base_addr + 0x1e;          // manual says 0x1f, but we're in little-endian 
   u_int16_t value = 0x0; 
   ret_code = vme_A16D16_write(p,clock_trig_reg_addr,value);  
   Print("SetClockSpeed","Writing...",clock_trig_reg_addr,value,ret_code);

   // the programming takes ~ 1.2 ms = 1200 us. 
   int time = 1200;
   usleep(time);  

   printf("[AcromagFPGA::SetClockSpeed]: Done. \n"); 

   CheckStatus(clock_trig_reg_addr,value,ret_code); 

}
//______________________________________________________________________________
void SetCtrlRegIDBits(int p,int carrier_addr,int daughter_addr,int value){
   u_int16_t data16;  
   u_int16_t my_data = (u_int16_t)value;  
   int my_addr       = carrier_addr + daughter_addr;    
   if(!gIsDebug) printf("[AcromagFPGA]: Setting board type to IP EP-201... "); 
   if(gIsDebug && gVerbosity>=3)  printf("[AcromagFPGA]: Setting board type to IP EP-201... \n"); 
   int ret_code      = vme_A16D16_read(p,my_addr,&data16); // high word 
   if(gIsDebug && gVerbosity>=3) Print("SetCtrlRegIDBits","Reading...",my_addr,data16,ret_code); 
   ret_code          = vme_A16D16_write(p,my_addr,my_data); // high word 
   if(gIsDebug && gVerbosity>=3) Print("SetCtrlRegIDBits","Writing...",my_addr,my_data,ret_code); 
   ret_code          = vme_A16D16_read(p,my_addr,&data16); // high word 
   if(gIsDebug && gVerbosity>=3) Print("SetCtrlRegIDBits","Reading...",my_addr,data16,ret_code); 
   if(!gIsDebug) printf(" Done. \n"); 
   CheckStatus(my_addr,data16,ret_code); 
}
//______________________________________________________________________________
int CheckMode(int p,int carrier_addr,int fpga_addr){

   if(!gIsDebug) printf("[AcromagFPGA]: Checking mode of FPGA... \n"); 
   u_int16_t data16 = 0;
   int mode     = -1; 
   int offset   = 0x0a; 
   int my_addr  = carrier_addr + fpga_addr + offset; 
   int ret_code = vme_A16D16_read(p,my_addr, &data16); // high word 
   if(gIsDebug && gVerbosity>=3) Print("CheckMode","",my_addr,data16,ret_code);
   // why are the 8 most significant bits 1's?  
   if(data16==0xff48){       // binary => 1111 1111 0100 1000 
      if(!gIsDebug) printf("[AcromagFPGA]: CONFIGURATION MODE ENABLED\n"); 
      mode = 0; 
   }else if(data16==0xff49){ // binary => 1111 1111 0100 1001 
      if(!gIsDebug) printf("[AcromagFPGA]: USER MODE ENABLED\n"); 
      mode = 1; 
   }else{
      ret_code = -1;
   }
   CheckStatus(my_addr,data16,ret_code); 
   return mode;
}
//______________________________________________________________________________
void SoftwareReset(int p,int carrier_addr,int daughter_addr){
   // software reset of the FPGA 
   if(!gIsDebug) printf("[AcromagFPGA]: Resetting the software on the FPGA... "); 
   if(gIsDebug)  printf("[AcromagFPGA::SoftwareReset]: Resetting the software on the FPGA... \n"); 
   u_int16_t data16;        
   int offset       = 0x0;  
   int my_addr      = carrier_addr + daughter_addr + offset;
   int ret_code     = vme_A16D16_read(p,my_addr, &data16); // high word 
   if(gIsDebug && gVerbosity>=3) Print("SoftwareReset","Reading...",my_addr,data16,ret_code);
   data16           = 0x8000;       // setting bit-15 high issues a software reset  
   ret_code         = vme_A16D16_write(p,my_addr,data16); // high word 
   if(gIsDebug && gVerbosity>=3) Print("SoftwareReset","Writing...",my_addr,data16,ret_code);
   ret_code         = vme_A16D16_read(p,my_addr, &data16); // high word 
   if(gIsDebug && gVerbosity>=3) Print("SoftwareReset","Reading...",my_addr,data16,ret_code);
   // PrintIOSpace(p,carrier_addr,daughter_addr); 
   CheckStatus(my_addr,data16,ret_code); 
   if(!gIsDebug) printf(" Done. \n"); 
}
//______________________________________________________________________________
void BoardReset(int p,int carrier_addr,int daughter_addr){
   // Put the FPGA in user mode 
   printf("[AcromagFPGA::BoardReset]: In FLASH mode, setting the FPGA to USER mode... \n"); 
   u_int16_t data16;        
   int addr_0x00 = carrier_addr + daughter_addr + 0x00;
   int addr_0x8a = carrier_addr + daughter_addr + 0x80 + 0x0a;   
   int ret_code  = vme_A16D16_read(p,addr_0x8a,&data16);  
   if(gIsDebug && gVerbosity>=3) Print("BoardReset","Reading...",addr_0x8a,data16,ret_code);
   data16           = 0x1;       // setting bit-0 high puts the board in user mode  
   ret_code         = vme_A16D16_write(p,addr_0x00,data16);  
   if(gIsDebug && gVerbosity>=3) Print("BoardReset","Writing...",addr_0x00,data16,ret_code);
   ret_code         = vme_A16D16_read(p,addr_0x8a,&data16);  
   if(gIsDebug && gVerbosity>=3) Print("BoardReset","Reading...",addr_0x8a,data16,ret_code);
   if(data16==0xff48){
      printf("[AcromagFPGA::BoardReset]: Operation failed! The FPGA is in CONFIGURATION mode; is the FLASH memory programmed properly? \n"); 
      exit(1); 
   }else if(data16==0xff49){
      printf("[AcromagFPGA::BoardReset]: The FPGA has been switched to USER mode, and is ready for operation.\n"); 
   }
   // PrintIOSpace(p,carrier_addr,daughter_addr); 
   CheckStatus(addr_0x8a,data16,ret_code); 
   if(!gIsDebug) printf(" Done. \n"); 
}
//______________________________________________________________________________
void PrintIDProm(int p,int carrier_addr,int fpga_addr){
   u_int16_t data16    = 0;
   u_int16_t ascii_val = -1;
   int i         =  0; 
   // int start    = carrier_addr + fpga_addr + 0x1; // big-Endian  
   int start     = carrier_addr + fpga_addr + 0x0; // little-Endian  
   int end       = start + 0x3f;
   int ret_code  = 0; 

   printf("[AcromagFPGA::PrintIDProm]: Begin ID PROM read...\n");
   // printf("start addr = 0x%08x \n",start); 
   for (i=start;i<end;i=i+2){  
      ret_code =  vme_A16D16_read(p, i, &data16); /* high word */
      if(i<=start+0x06){
         ascii_val = data16; 
      }else{
         ascii_val = ' ';    // such values are undefined, so we label them ' '.  
      }
      printf("[AcromagFPGA::PrintIDProm]: addr = 0x%08x data = 0x%02x  (ASCII = %c)  ret_code = 0x%08x\n",
            i,data16,ascii_val,ret_code);
   }
   printf("[AcromagFPGA::PrintIDProm]: End ID PROM read.\n");

   // CheckStatus(ret_code);
}
//______________________________________________________________________________
void PrintIOSpace(int p,int carrier_addr,int daughter_addr){
   u_int16_t data16 = 0;
   int i,ret_code;
   int start = carrier_addr + daughter_addr + 0x2; // little-Endian  
   int end   = start + 0x6;
   printf("[AcromagFPGA::PrintIOSpace]: Begin IO SPACE read...\n");
   for (i=start;i<end;i++){
      ret_code =  vme_A16D16_read(p, i, &data16); /* high word */
      Print("PrintIOSpace","",i,data16,ret_code);
   }
   printf("[AcromagFPGA::PrintIOSpace]: End IO SPACE read.\n");
   // CheckStatus(ret_code);
}
//______________________________________________________________________________
void QueryIOSpace(int p,int carrier_addr,int fpga_addr){
   u_int16_t data16 = 0;
   int ret_code = 0,dummy =0;
   int i;
   int start = carrier_addr; 
   int end   = 0x80; 
   for (i=start;i<end;i++){
      ret_code =  vme_A16D16_read(p, i, &data16); /* high word */
      if (data16 != 0xff33) {
         ret_code = -1; 
         break;
      }
   }
   dummy *= ret_code; // to remove compilation warnings 
   // CheckStatus(ret_code);
}
//______________________________________________________________________________
void SetMode(int p,int carrier_addr,int fpga_addr,int choice){
   // config mode = 0; user mode = 1 
   if(choice==0) printf("[AcromagFPGA::SetMode]: Setting to configuration mode...\n"); 
   if(choice==1) printf("[AcromagFPGA::SetMode]: Setting to user mode...\n"); 
   u_int16_t data16 = 0; 
   // read module information 
   // we want to read base_addr + 0a(hex) to see what mode we're currently in
   // remember: the board uses little-Endian, NOT big-Endian like the manual uses.
   // If big-Endian, use base_addr + 0b(hex)  
   // int addr     = carrier_addr + fpga_addr + 0x0a;   
   int addr_0x0a = carrier_addr + fpga_addr + 0x0a;   
   int addr_0x00 = carrier_addr + fpga_addr + 0x0;   // we need to write to this bit  
   int ret_code  = vme_A16D16_read(p,addr_0x0a,&data16); // high word 
   if(gIsDebug) Print("SetMode","Reading...",addr_0x0a,data16,ret_code);
   if(choice==0 && data16==0xff48) printf("[AcromagFPGA::SetMode]: Currently in CONFIGURATION mode, switching to CONFIGURATION mode...\n"); 
   if(choice==1 && data16==0xff48) printf("[AcromagFPGA::SetMode]: Currently in CONFIGURATION mode, switching to USER mode...\n"); 
   if(choice==0 && data16==0xff49) printf("[AcromagFPGA::SetMode]: Currently in USER mode, switching to CONFIGURATION mode...\n"); 
   if(choice==1 && data16==0xff49) printf("[AcromagFPGA::SetMode]: Currently in USER mode, switching to USER mode...\n");
   // we don't care what data16 was now, set to what we want it to be  
   if(choice==0) data16 = 0xff48; 
   if(choice==1) data16 = 0xff49; 
   ret_code = vme_A16D16_write(p,addr_0x00,data16);
   if(gIsDebug) Print("SetMode","Writing...",addr_0x00,data16,ret_code);
   // if(choice==0) data16 = (data16 & 0x30);         // try to set config mode, set bit 0 high                          
   // ret_code = vme_A16D16_read(p,addr,&data16); // high word 
   // printf("[SetMode]: Reading... vme_A16D16_mod_base=0x%08x data= 0x%02x  ret_code=0x%08x\n",addr,data16,ret_code);
   // printf("--------------------------------------------------------------------\n"); 
   CheckStatus(addr_0x00,data16,ret_code); 
}
//______________________________________________________________________________
void SetControlRegister(int p,int carrier_addr,int fpga_addr,int state){
   // the variable 'value' is the value of bit-0: low (0) or high (1) or flip (2)  
   if(state==0) printf("[AcromagFPGA::SetControlRegister]: Setting bit-0 to logic LOW (user mode)... \n");
   if(state==1) printf("[AcromagFPGA::SetControlRegister]: Setting bit-0 to logic HIGH (configuration mode)... \n");
   if(state==2) printf("[AcromagFPGA::SetControlRegister]: Flipping bit-0 logic...\n");
   // read module information 
   u_int16_t data16 = 0;
   int offset       = 0x0; 
   int my_addr      = carrier_addr + fpga_addr + offset;    
   int ret_code     = vme_A16D16_read(p,my_addr,&data16); // high word 
   if(gIsDebug) Print("SetControlRegister","Reading...",my_addr,data16,ret_code);
   // set the appropriate value for data16 based on its previous value 
   // and the input variable 'state'   
   if(state==0) data16 = data16 & 0xfffe; // set bit-0 low      
   if(state==1) data16 = data16 | 0x0001; // set bit-0 high     
   if(state==2) data16 = data16 ^ 0x0001; // flips the bit-0 state     
   ret_code    = vme_A16D16_write(p,my_addr,data16);
   if(gIsDebug) Print("SetControlRegister","Writing...",my_addr,data16,ret_code);
   // set the appropriate value for data16 based on its previous value 
   ret_code     = vme_A16D16_read(p,my_addr,&data16); // high word 
   if(gIsDebug) Print("SetControlRegister","Reading...",my_addr,data16,ret_code);
   // set the appropriate value for data16 based on its previous value 
   CheckStatus(my_addr,data16,ret_code); 
}
//______________________________________________________________________________
void SetDirCtrlReg(int p,int carrier_addr,int daughter_addr,int bit_pattern){
   // Set this AFTER the I/O registers! 
   // set direction control register to all outputs 
   // logic low  => input  
   // logic high => output 
   u_int16_t data16;          
   u_int16_t my_data = (u_int16_t)bit_pattern;    
   int offset        = 0x8; 
   int my_addr       = carrier_addr + daughter_addr + offset;    
   if(gIsDebug && gVerbosity>=3) printf("[AcromagFPGA::SetDirCtrlReg]: Setting data flow direction for pins according to bit pattern 0x%08x... \n",bit_pattern); 
   int ret_code      = vme_A16D16_read(p,my_addr,&data16);
   if(gIsDebug && gVerbosity>=3) Print("SetDirCtrlReg","Reading...",my_addr,data16,ret_code);
   ret_code          = vme_A16D16_write(p,my_addr,my_data);
   if(gIsDebug && gVerbosity>=3) Print("SetDirCtrlReg","Writing...",my_addr,my_data,ret_code);
   ret_code          = vme_A16D16_read(p,my_addr,&data16);
   if(gIsDebug && gVerbosity>=3) Print("SetDirCtrlReg","Reading...",my_addr,data16,ret_code);
   CheckStatus(my_addr,data16,ret_code); 
}
//______________________________________________________________________________
void SetIOBits(int p,int carrier_addr,int daughter_addr,int bit_pattern){
   // set I/O bits  
   u_int16_t data16 = (u_int16_t)bit_pattern;           
   int base  = carrier_addr + daughter_addr; 
   // printf("carrier = 0x%08x "
   int start = base + 0x2; 
   int end   = start + 0x4;  
   int i,i_addr,ret_code;
   for (i=start;i<end;i++){
      i_addr   = i; 
      ret_code =  vme_A16D16_write(p,i_addr,data16);
      if(gIsDebug) Print("SetIOBits","Writing...",i_addr,data16,ret_code);
   }
   // CheckStatus(ret_code); 
}
//______________________________________________________________________________
void SetIOBitsAlt(int p,int carrier_addr,int fpga_addr,int daughter_segment,int bit_pattern){
   // set I/O bits for a specific group of I/O pins  
   // daughter_segment => group of I/O pins
   u_int16_t data16;           
   u_int16_t my_data = (u_int16_t)bit_pattern;           
   int addr          = carrier_addr + fpga_addr + daughter_segment;  
   int ret_code      = vme_A16D16_read(p,addr,&data16);
   if(gIsDebug && gVerbosity>=3) Print("SetIOBitsAlt","Reading...",addr,data16,ret_code);
   ret_code          = vme_A16D16_write(p,addr,my_data);
   if(gIsDebug && gVerbosity>=3) Print("SetIOBitsAlt","Writing...",addr,my_data,ret_code);
   ret_code          = vme_A16D16_read(p,addr,&data16);
   if(gIsDebug && gVerbosity>=3) Print("SetIOBitsAlt","Reading...",addr,data16,ret_code);
   CheckStatus(addr,data16,ret_code);
}
//______________________________________________________________________________
void PrintBits(u_int16_t data16){
   int i;
   for(i=15;i>=0;i--) {
      if (CHECK_BIT(data16,i)) printf("1 ");
      else printf("0 ");
   }
   printf("\n");

}
//______________________________________________________________________________
void CheckStatus(int addr,int data,int code){
   if(code != 0x0){
      printf("[AcromagFPGA::CheckStatus]: Operation failed! \n"); 
      printf("[AcromagFPGA::CheckStatus]: addr = 0x%08x  data = 0x%04x  return code = 0x%08x \n",addr,data,code); 
      printf("[AcromagFPGA::CheckStatus]: Exiting... \n"); 
      exit(1); 
   }
   if(gIsDebug) printf("--------------------------------------------------------------------\n"); 
}
// TODO: Add this function... 
// //______________________________________________________________________________
// void StartPulses(int p,const struct fpga myFPGA){
// 
//    int carrier_addr     = myFPGA.fCarrierAddr; 
//    int fpga_io_sp       = myFPGA.fIOSpaceAddr;
// 
//    // start the counter from zero on the FPGA by writing 0x1 to 0x0056.  
//    WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,0x0056,0x1); 
// 
//    // set the 
// 
// }
//______________________________________________________________________________
int IsFPGATimingSet(int p,int carrier_addr,int daughter_addr){

   int data=0;
   int addr = carrier_addr + daughter_addr + gDigitizerAddr;  

   printf("[AcromagFPGA]: Asking the FPGA if the timing of the signals is set... \n"); 
   data = ReadFPGAMemory(p,carrier_addr,daughter_addr,gDigitizerAddr);
   // printf("[AcromagFPGA]: addr = 0x%04x data = 0x%04x \n",carrier_addr+daughter_addr+gDigitizerAddr,data);
   if(data==0x0){
      printf("[AcromagFPGA]: FPGA is not ready. \n"); 
   }else if(data==0x1){
      printf("[AcromagFPGA]: FPGA is ready. \n"); 
   }else{
      printf("[AcromagFPGA]: Cannot determine FPGA readiness!   \n");
      printf("[AcromagFPGA]: addr = 0x%04x data = 0x%04x \n",addr,data);
   }

   return data; 

}
//______________________________________________________________________________
int IsReturnGateClosed(int p,int carrier_addr,int daughter_addr){

   // check to see if the circuit is set to receive mode
   // receive mode means that the path from the NMR probe to 
   // the amplifier is connected; this is connected by rf_sw_3 
   // in the input file; this RF switch is closed usually for 
   // tens of milliseconds.  

   int data=0; 
   int full_addr = carrier_addr + daughter_addr + gDigitizerAddr2; 

   data = ReadFPGAMemory(p,carrier_addr,daughter_addr,gDigitizerAddr2);

   // gDigitizerAddr2 is tied to the first certain bits of DIO in the FPGA code; 
   // DIO contains the gate logic that we programmed onto the board via 
   // SRAM.  Therefore, we want the value that corresponds to rf_sw_3.  
   // Do bitwise operations on the variable data:

   // WARNING: In bit operations, we start from ZERO, not 1! 

   int io_dm_bit          = GetBit(0,data); 
   int mech_sw_bit        = GetBit(1,data); 
   int global_on_off_bit  = GetBit(2,data); 
   int mech_sw_on_off_bit = GetBit(3,data); 
   int timing_bit         = GetBit(4,data);  
   int rf_sw_3_bit        = GetBit(5,data);  

   // printf("data = %d bit of interest = %d \n",data,the_bit);

   if(gIsDebug && gVerbosity >=5){ 
      printf("[AcromagFPGA]: addr = 0x%04x data = 0x%04x bits: IO_DM(1) = %d mech_sw_value = %d global_on_off = %d mech_sw_on_off = %d is_ready = %d rf_sw_3_value = %d \n",
            full_addr,data,io_dm_bit,mech_sw_bit,global_on_off_bit,mech_sw_on_off_bit,timing_bit,rf_sw_3_bit);
   }

   if(rf_sw_3_bit==0x0){
      // printf("[AcromagFPGA]: FPGA is not ready. \n"); 
      // return 0;
   }else if(rf_sw_3_bit==0x1){
      // printf("[AcromagFPGA]: FPGA is ready. \n"); 
      // return 1;
   }else{
      printf("[AcromagFPGA]: Cannot determine return gate state!   \n");
      printf("[AcromagFPGA]: addr = 0x%04x data = 0x%04x \n",full_addr,data);
      // data = -1; 
   }

   return rf_sw_3_bit; 

}
//______________________________________________________________________________
void PrintSummary(const struct fpga myFPGA){

   printf("[AcromagFPGA]: The following commands have been processed: \n");

   int start_count,pulse_count,flag; 
   double start=0,pulse=0; 
   char *status,*units,*name;

   int N = myFPGA.fNSignals; 

   double ClockFreq = myFPGA.fClockFrequency; 

   int i=0;
   for(i=0;i<N;i++){                     
      name  = myFPGA.fSignalName[i]; 
      units = myFPGA.fSignalUnits[i];  
      flag  = myFPGA.fFlag[i]; 
      if(flag==0){
         status = "off"; 
      }else if(flag==1){
         status = "on"; 
      }else{
         status = "ND"; 
      }
      start_count = myFPGA.fSignalStartTimeLo[i] + pow(2,16)*myFPGA.fSignalStartTimeHi[i]; 
      pulse_count = myFPGA.fSignalPulseTimeLo[i] + pow(2,16)*myFPGA.fSignalPulseTimeHi[i]; 
      start       = GetTimeInUnits(start_count,ClockFreq,units); 
      pulse       = GetTimeInUnits(pulse_count,ClockFreq,units); 
      printf("[AcromagFPGA]: signal name = %-15s status = %-3s \t time offset = %-8.2f %-2s \t pulse duration = %-8.2f %-2s \n",name,status,start,units,pulse,units);
   }

}
//______________________________________________________________________________
void Print(char *function,char *daughter_type,int addr,int data,int code){
   printf("[AcromagFPGA::%s]: %s addr = 0x%08x  data = 0x%04x  return code = 0x%08x \n",function,daughter_type,addr,data,code); 
}

