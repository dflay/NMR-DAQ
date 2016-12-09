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
void InitFPGAPulseSequenceStruct(struct fpgaPulseSequence *myPulseSequence){

   const int SIZE = 100; 
   myPulseSequence->fBoardName    = (char *)malloc( sizeof(char)*SIZE ); 

   myPulseSequence->fNSequences   = 0; 
   myPulseSequence->fGlobalEnable = 0; 
   myPulseSequence->fCarrierAddr  = 0; 
   myPulseSequence->fIOSpaceAddr  = 0; 
   myPulseSequence->fIDSpaceAddr  = 0; 

   int i=0;
   for(i=0;i<4;i++){
      myPulseSequence->fMechSwID[i]           = 0; 
      myPulseSequence->fEnableFlag[i]         = 0; 
      myPulseSequence->fTomcoEnable[i]        = 0; 
      myPulseSequence->fMechSwStartTimeLo[i]  = 0; 
      myPulseSequence->fMechSwStartTimeHi[i]  = 0; 
      myPulseSequence->fMechSwEndTimeLo[i]    = 0; 
      myPulseSequence->fMechSwEndTimeHi[i]    = 0; 
      myPulseSequence->fRFTransStartTimeLo[i] = 0; 
      myPulseSequence->fRFTransStartTimeHi[i] = 0; 
      myPulseSequence->fRFTransEndTimeLo[i]   = 0; 
      myPulseSequence->fRFTransEndTimeHi[i]   = 0; 
      myPulseSequence->fRFRecStartTimeLo[i]   = 0; 
      myPulseSequence->fRFRecStartTimeHi[i]   = 0; 
      myPulseSequence->fRFRecEndTimeLo[i]     = 0; 
      myPulseSequence->fRFRecEndTimeHi[i]     = 0; 
      myPulseSequence->fTomcoStartTimeLo[i]   = 0; 
      myPulseSequence->fTomcoStartTimeHi[i]   = 0; 
      myPulseSequence->fTomcoEndTimeLo[i]     = 0; 
      myPulseSequence->fTomcoEndTimeHi[i]     = 0;
   } 
}
//______________________________________________________________________________
void InitFPGAGlobalVariables(void){

   // FLASH programming; turn this on 
   // if the VHDL code was programmed to FLASH memory. 
   gIsFLASH = 1; 

   // clock frequency 
   gFPGAClockFreq = FPGA_CLOCK_FREQ;  
   // addresses
   gOffset        = 0x0000;  
   gModBase       = MODULE_BASE_ADDR;       
   // I/O Space
   gIPAIOSpace    = IP_A_IO_SPACE_ADDR; 
   gIPBIOSpace    = IP_B_IO_SPACE_ADDR; 
   gIPCIOSpace    = IP_C_IO_SPACE_ADDR; 
   gIPDIOSpace    = IP_D_IO_SPACE_ADDR; 
   // ID space
   gIPAIDSpace    = IP_A_IO_SPACE_ADDR + IO_SPACE_OFFSET;  
   gIPBIDSpace    = IP_B_IO_SPACE_ADDR + IO_SPACE_OFFSET;  
   gIPCIDSpace    = IP_C_IO_SPACE_ADDR + IO_SPACE_OFFSET;  
   gIPDIDSpace    = IP_D_IO_SPACE_ADDR + IO_SPACE_OFFSET;  

   // int i=0;
   // for(i=0;i<4;i++){
   //    gSwitchFlag[i] = 0;
   // }

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

   int i=0;
   for(i=0;i<16;i++){
      gMasterList[i] = "--"; 
   }

   gMasterList[0]   = GLOBAL_ON_OFF_NAME;  
   gMasterList[1]   = MECH_SWITCH_1_NAME;        // second-to-least significant bit
   gMasterList[2]   = MECH_SWITCH_2_NAME;        // second-to-least significant bit
   gMasterList[3]   = MECH_SWITCH_3_NAME;        // second-to-least significant bit
   gMasterList[4]   = MECH_SWITCH_4_NAME;        // second-to-least significant bit
   gMasterList[6]   = RF_TRANSMIT_NAME; 
   gMasterList[7]   = RF_RECEIVE_NAME;
   gMasterList[10]  = TOMCO_NAME;        

   // The FPGA code uses the labels starting from 0, going up through F.  However, 
   // it's actually offset by 16, so we start with 0x20 (decimal 32) instead of 0x10 (decimal 16).   
   gDigitizerAddr     = DIGITIZER_ADDR_1;           // NOTE: this is NOT in the same space as the above (which looks at SRAM)   
   gDigitizerAddr2    = DIGITIZER_ADDR_2;           // NOTE: this is NOT in the same space as the above (which looks at SRAM)   

   for(i=0;i<16;i++){
      gMasterAddrList[i] = FLAG_ADDR;
   }

   gMasterAddrList[0] = FLAG_ADDR;
   gMasterAddrList[1] = gOffset + MECHANICAL_SWITCH_ADDR; 
   gMasterAddrList[2] = gOffset + RF_SWITCH_TRANS_ADDR; 
   gMasterAddrList[3] = gOffset + RF_SWITCH_REC_ADDR; 
   gMasterAddrList[4] = gOffset + TOMCO_ADDR; 

}
//______________________________________________________________________________
int InitFPGA(int p,struct fpga *myFPGA,struct fpgaPulseSequence *myPulseSequence){

   InitFPGAGlobalVariables();                          // this must be called first!  
   InitFPGAAddresses();
   InitFPGAStruct(myFPGA);                             // no need for dereference, this function takes a pointer 
   InitFPGAPulseSequenceStruct(myPulseSequence);       // no need for dereference, this function takes a pointer 

   int carrier_addr = gModBase; 
   int fpga_io_sp   = gIPBIOSpace;          
   int fpga_id_sp   = fpga_io_sp + 0x80;  

   // set the name of the FPGA 
   myPulseSequence->fBoardName   = "Acromag IP-EP201";
   // set the carrier, IO space and ID space addresses 
   myPulseSequence->fCarrierAddr = carrier_addr; 
   myPulseSequence->fIOSpaceAddr = fpga_io_sp; 
   myPulseSequence->fIDSpaceAddr = fpga_id_sp; 

   // import the signals 
   // char *pulse_fn = "./input/pulse-data.dat";
   // ImportPulseData(pulse_fn,myFPGA);                   // must pass by reference so we keep the data in the struct!  
   //                                                     // InitFPGA takes myFPGA as a pointer originally  

   // import all the signals 
   char *pulse_fn = "./input/pulse-data.dat";
   ImportPulseSequenceData(pulse_fn,myPulseSequence);

   char *glo_fn = "./input/global_on_off.dat";
   ImportGlobalOnOff(glo_fn,myPulseSequence);

   int ret_code = 0;
   ret_code = TimingCheckNew(*myPulseSequence); 
   // printf("[AcromagFPGA::InitFPGA]: WARNING: No timing check! Be careful...\n"); 

   if(ret_code!=0) return ret_code; 

   if(gIsDebug && gVerbosity >=4) PrintFPGANew(*myPulseSequence);  // PrintFPGA takes the struct by value, so we dereference the pointer  

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
   // myFPGA->fFlag[0] = 0; 
   // set all bits to zero 
   int i=0;  
   for(i=0;i<20;i++){
      myFPGA->fFlag[i] = 0; 
   } 
    
   // reconstruct bit pattern flag 
   const int N             = myFPGA->fNSignals;  
   int bit_pattern_flag    = GetBitPattern(N,myFPGA->fSignalName,myFPGA->fFlag); 
   myFPGA->fBitPatternFlag = bit_pattern_flag; 

   // dereference the pointer, program new bit pattern to FPGA
   ProgramSignalsToFPGA(p,*myFPGA);    

}
//______________________________________________________________________________
void BlankFPGANew(int p,struct fpgaPulseSequence *myPulseSequence){
   // dereference the pointer, program using Switch = -1 (bit pattern = 0x0000)  
   ProgramSignalsToFPGANew(p,-1,*myPulseSequence);    
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
   WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,FLAG_ADDR,bit_pattern_flag);    
   if(!gIsDebug) printf(" Done. \n"); 

   // blank the signals as we're going to set the timing. 
   // writing 0x0 to 0x0054 will turn off output, as this 
   // flips the value of the timing flag on the FPGA.  
   WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,UPDATE_ADDR,0x0); 

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
   WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,UPDATE_ADDR,0x1);  

   if(!gIsDebug) printf(" Done. \n"); 

   // read back what we wrote to the SRAM as a double-check 
   int data=0,dummy=0;  // dummy is used to remove warnings in compilation concerning data  
   data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,FLAG_ADDR);
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
   data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,UPDATE_ADDR); 

   // print processed signals to screen 
   PrintSummary(myFPGA); 

   fpga_timing_flag = IsFPGATimingSet(p,carrier_addr,fpga_io_sp); 

   return fpga_timing_flag; 
}
//______________________________________________________________________________
int ProgramSignalsToFPGANew(int p,int Switch,const struct fpgaPulseSequence myPulseSequence){

   // Switch = desired mechanical switch to activate   

   int carrier_addr = myPulseSequence.fCarrierAddr; 
   int fpga_io_sp   = myPulseSequence.fIOSpaceAddr;

   // find the desired switch
   int is = GetMechSwitchIndex(Switch,myPulseSequence);

   u_int16_t my_flags = 0x0; 

   if( (gIsDebug) && (gVerbosity>0) ) printf("[AcromagFPGA]: Programming signals for switch %d (index %d)...  \n",Switch,is); 
   
   // blank the signals as we're going to set the timing first.  
   // if(!gIsDebug) printf("[AcromagFPGA]: Setting the bit pattern (flags)... \n"); 
   // WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,FLAG_ADDR,0x0);    
   if( (gIsDebug) && (gVerbosity>0) ) printf("[AcromagFPGA]: First, disabling output... \n"); 
   WriteFPGAMemory(p,carrier_addr,fpga_io_sp,NEW_FLAG_ADDR,0x0);    

   // writing 0x0 to 0x0054 will turn off output, as this 
   // flips the value of the timing flag on the FPGA.  
   // we also disable the counter 
   WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,UPDATE_ADDR,0x0); 
   WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,COUNTER_ENABLE_ADDR,0x0);  
   if( (gIsDebug) && (gVerbosity>0) ) printf("[AcromagFPGA]: Done. \n"); 

   // now get the bit pattern
   u_int16_t bit_pattern = GetBitPatternNew(Switch,myPulseSequence);

   if(gIsDebug && gVerbosity>0){
      printf("[AcromagFPGA]: The bit pattern to be programmed is: "); 
      PrintBits16(bit_pattern);
   } 
 
   if(gIsDebug && gVerbosity>0) printf("[AcromagFPGA]: Programming signals... \n"); 
   // Write signals onto the SRAM for the FPGA to use 

   u_int16_t mech_sw_start_addr_lo  = MECHANICAL_SWITCH_ADDR; 
   u_int16_t mech_sw_start_addr_hi  = MECHANICAL_SWITCH_ADDR + 0x0002; 
   u_int16_t mech_sw_end_addr_lo    = MECHANICAL_SWITCH_ADDR + 0x0004; 
   u_int16_t mech_sw_end_addr_hi    = MECHANICAL_SWITCH_ADDR + 0x0006; 

   u_int16_t rf_trans_start_addr_lo = RF_SWITCH_TRANS_ADDR; 
   u_int16_t rf_trans_start_addr_hi = RF_SWITCH_TRANS_ADDR + 0x0002; 
   u_int16_t rf_trans_end_addr_lo   = RF_SWITCH_TRANS_ADDR + 0x0004; 
   u_int16_t rf_trans_end_addr_hi   = RF_SWITCH_TRANS_ADDR + 0x0006; 

   u_int16_t rf_rec_start_addr_lo   = RF_SWITCH_REC_ADDR; 
   u_int16_t rf_rec_start_addr_hi   = RF_SWITCH_REC_ADDR + 0x0002; 
   u_int16_t rf_rec_end_addr_lo     = RF_SWITCH_REC_ADDR + 0x0004; 
   u_int16_t rf_rec_end_addr_hi     = RF_SWITCH_REC_ADDR + 0x0006; 

   u_int16_t tomco_start_addr_lo    = TOMCO_ADDR; 
   u_int16_t tomco_start_addr_hi    = TOMCO_ADDR + 0x0002; 
   u_int16_t tomco_end_addr_lo      = TOMCO_ADDR + 0x0004; 
   u_int16_t tomco_end_addr_hi      = TOMCO_ADDR + 0x0006; 

   // program timing ONLY IF we have a valid switch.  otherwise, leave it. if Switch<=0, the bit pattern will be 0x0000  
   if(Switch>0){
      // mechanical switch 
      if(gIsDebug && gVerbosity>=3) printf("MECHANICAL SWITCH \n"); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,mech_sw_start_addr_lo,myPulseSequence.fMechSwStartTimeLo[is]  ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,mech_sw_start_addr_hi,myPulseSequence.fMechSwStartTimeHi[is]  ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,mech_sw_end_addr_lo  ,myPulseSequence.fMechSwEndTimeLo[is]    ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,mech_sw_end_addr_hi  ,myPulseSequence.fMechSwEndTimeHi[is]    );
      // rf transmit  
      if(gIsDebug && gVerbosity>=3) printf("RF TRANS SWITCH \n"); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_trans_start_addr_lo,myPulseSequence.fRFTransStartTimeLo[is]); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_trans_start_addr_hi,myPulseSequence.fRFTransStartTimeHi[is]); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_trans_end_addr_lo  ,myPulseSequence.fRFTransEndTimeLo[is]  ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_trans_end_addr_hi  ,myPulseSequence.fRFTransEndTimeHi[is]  );
      // rf receive  
      if(gIsDebug && gVerbosity>=3) printf("RF REC SWITCH \n"); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_rec_start_addr_lo,myPulseSequence.fRFRecStartTimeLo[is]    ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_rec_start_addr_hi,myPulseSequence.fRFRecStartTimeHi[is]    ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_rec_end_addr_lo  ,myPulseSequence.fRFRecEndTimeLo[is]      ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_rec_end_addr_hi  ,myPulseSequence.fRFRecEndTimeHi[is]      );
      // tomco  
      if(gIsDebug && gVerbosity>=3) printf("TOMCO \n"); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,tomco_start_addr_lo,myPulseSequence.fTomcoStartTimeLo[is]     ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,tomco_start_addr_hi,myPulseSequence.fTomcoStartTimeHi[is]     ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,tomco_end_addr_lo  ,myPulseSequence.fTomcoEndTimeLo[is]       ); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,tomco_end_addr_hi  ,myPulseSequence.fTomcoEndTimeHi[is]       );
   }else{
      // do nothing 
   }
   if( (gIsDebug) && (gVerbosity>0) ) printf("[AcromagFPGA]: Done. \n"); 
 
   int i=0; 
   int dummy=0;
   u_int16_t data = 0x0; 

   if(gIsDebug){ 
      // data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,FLAG_ADDR);
      data = ReadFPGAMemory(p,carrier_addr,fpga_io_sp,NEW_FLAG_ADDR);
      printf("[AcromagFPGA]: The programmed bit pattern was: 0x%04x \n",data);
      for(i=mlMAX-1;i>=0;i--){
         dummy = GetBit(i,data);
         printf("%d ",dummy); 
      }
      printf("\n"); 
      // read back what we wrote to the SRAM as a double-check 
      // mechanical switch 
      if(gVerbosity>=0){
	 printf("addr = 0x%04x start_lo = 0x%04x start_hi = 0x%04x end_lo = 0x%04x end_hi = 0x%04x \n",
                mech_sw_start_addr_lo, 
                myPulseSequence.fMechSwStartTimeLo[is],myPulseSequence.fMechSwStartTimeHi[is],
                myPulseSequence.fMechSwEndTimeLo[is]  ,myPulseSequence.fMechSwEndTimeHi[is]);
      }
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,mech_sw_start_addr_lo); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,mech_sw_start_addr_hi); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,mech_sw_end_addr_lo  ); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,mech_sw_end_addr_hi  );
      // rf transmit  
      if(gVerbosity>=0){
	 printf("addr = 0x%04x start_lo = 0x%04x start_hi = 0x%04x end_lo = 0x%04x end_hi = 0x%04x \n",
                rf_trans_start_addr_lo, 
                myPulseSequence.fRFTransStartTimeLo[is],myPulseSequence.fRFTransStartTimeHi[is],
                myPulseSequence.fRFTransEndTimeLo[is]  ,myPulseSequence.fRFTransEndTimeHi[is]);
      }
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_trans_start_addr_lo); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_trans_start_addr_hi); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_trans_end_addr_lo  ); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_trans_end_addr_hi  );
      // rf receive  
      if(gVerbosity>=0){
	 printf("addr = 0x%04x start_lo = 0x%04x start_hi = 0x%04x end_lo = 0x%04x end_hi = 0x%04x \n",
                rf_rec_start_addr_lo, 
                myPulseSequence.fRFRecStartTimeLo[is],myPulseSequence.fRFRecStartTimeHi[is],
                myPulseSequence.fRFRecEndTimeLo[is]  ,myPulseSequence.fRFRecEndTimeHi[is]);
      }
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_rec_start_addr_lo); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_rec_start_addr_hi); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_rec_end_addr_lo  ); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,rf_rec_end_addr_hi  );
      // tomco  
      if(gVerbosity>=0){
	 printf("addr = 0x%04x start_lo = 0x%04x start_hi = 0x%04x end_lo = 0x%04x end_hi = 0x%04x \n",
                tomco_start_addr_lo, 
                myPulseSequence.fTomcoStartTimeLo[is],myPulseSequence.fTomcoStartTimeHi[is],
                myPulseSequence.fTomcoEndTimeLo[is]  ,myPulseSequence.fTomcoEndTimeHi[is]);
      }
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,tomco_start_addr_lo); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,tomco_start_addr_hi); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,tomco_end_addr_lo  ); 
      data = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,tomco_end_addr_hi  );
      dummy *= data;
   } 

   // update signal timestamps (allow output on pins) 
   if(Switch>0){
      if(gIsDebug) printf("[AcromagFPGA]: Turning on pin output... \n"); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,UPDATE_ADDR,0x1);  
      if(gIsDebug) printf("[AcromagFPGA]: Done. \n"); 
      // usleep(4000);
      if(gIsDebug) printf("[AcromagFPGA]: Enabling the counter... \n"); 
      WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,COUNTER_ENABLE_ADDR,0x1);  
      if(gIsDebug) printf("[AcromagFPGA]: Done. \n"); 
      if(gIsDebug) printf("[AcromagFPGA]: Setting the bit pattern (flags)... \n"); 
      // WriteMemoryDataReg(p,carrier_addr,fpga_io_sp,FLAG_ADDR,bit_pattern);    
      WriteFPGAMemory(p,carrier_addr,fpga_io_sp,NEW_FLAG_ADDR,bit_pattern);  
      if(gIsDebug) printf("[AcromagFPGA]: Done. \n");
   }

   if(gIsDebug && gVerbosity>=1){  
      my_flags = ReadFPGAMemory(p,carrier_addr,fpga_io_sp,NEW_FLAG_ADDR);
      printf("[AcromagFPGA]: The flags were: "); 
      PrintBits16(my_flags);   
   }

   // print processed signals to screen 
   if(gIsDebug && gVerbosity>=1){ 
      if(Switch>0) PrintSummaryNew(Switch,myPulseSequence);
   } 

   int fpga_timing_flag = IsFPGATimingSet(p,carrier_addr,fpga_io_sp); 

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
void PrintFPGANew(const struct fpgaPulseSequence myPulseSequence){

   int N = myPulseSequence.fNSequences;
 
   if(N==0){
      printf("[AcromagFPGA]: No data found in myFPGA!  Exiting... \n");
      exit(1);
   }else if(N>4){
      printf("[AcromagFPGA]: Invalid number of entries in myFPGA!  N = %d.  Exiting... \n",N); 
      exit(1);
   }

   printf("FPGA Board Characteristics: \n");
   printf("name: %s \ncarrier addr: 0x%04x \nIO space addr: 0x%04x \nID space addr: 0x%04x \n",
          myPulseSequence.fBoardName,myPulseSequence.fCarrierAddr,myPulseSequence.fIOSpaceAddr,myPulseSequence.fIDSpaceAddr);

   printf("------------------------------- \n");  

   printf("global enable = %d \n",myPulseSequence.fGlobalEnable); 

   int msw_start_time_cnts=0,msw_end_time_cnts=0;
   int rft_start_time_cnts=0,rft_end_time_cnts=0;
   int rfr_start_time_cnts=0,rfr_end_time_cnts=0;
   int tom_start_time_cnts=0,tom_end_time_cnts=0;
   double msw_start_time_sec=0,msw_end_time_sec=0;
   double rft_start_time_sec=0,rft_end_time_sec=0;
   double rfr_start_time_sec=0,rfr_end_time_sec=0;
   double tom_start_time_sec=0,tom_end_time_sec=0;
   double ClockFreq = FPGA_CLOCK_FREQ;  

   int i=0;
   for(i=0;i<N;i++){
      msw_start_time_cnts = myPulseSequence.fMechSwStartTimeLo[i] + pow(2,16)*myPulseSequence.fMechSwStartTimeHi[i];
      msw_start_time_sec  = GetTimeInSeconds(msw_start_time_cnts,ClockFreq);  
      rft_start_time_cnts = myPulseSequence.fRFTransStartTimeLo[i] + pow(2,16)*myPulseSequence.fRFTransStartTimeHi[i];
      rft_start_time_sec  = GetTimeInSeconds(rft_start_time_cnts,ClockFreq);  
      tom_start_time_cnts = myPulseSequence.fTomcoStartTimeLo[i] + pow(2,16)*myPulseSequence.fTomcoStartTimeHi[i];
      tom_start_time_sec  = GetTimeInSeconds(tom_start_time_cnts,ClockFreq);  
      rfr_start_time_cnts = myPulseSequence.fRFRecStartTimeLo[i] + pow(2,16)*myPulseSequence.fRFRecStartTimeHi[i];
      rfr_start_time_sec  = GetTimeInSeconds(rfr_start_time_cnts,ClockFreq);  
      msw_end_time_cnts   = myPulseSequence.fMechSwEndTimeLo[i] + pow(2,16)*myPulseSequence.fMechSwEndTimeHi[i];
      msw_end_time_sec    = GetTimeInSeconds(msw_end_time_cnts,ClockFreq);  
      rft_end_time_cnts   = myPulseSequence.fRFTransEndTimeLo[i] + pow(2,16)*myPulseSequence.fRFTransEndTimeHi[i];
      rft_end_time_sec    = GetTimeInSeconds(rft_end_time_cnts,ClockFreq);  
      tom_end_time_cnts   = myPulseSequence.fTomcoEndTimeLo[i] + pow(2,16)*myPulseSequence.fTomcoEndTimeHi[i];
      tom_end_time_sec    = GetTimeInSeconds(tom_end_time_cnts,ClockFreq);  
      rfr_end_time_cnts   = myPulseSequence.fRFRecEndTimeLo[i] + pow(2,16)*myPulseSequence.fRFRecEndTimeHi[i];
      rfr_end_time_sec    = GetTimeInSeconds(rfr_end_time_cnts,ClockFreq);  
      printf("name                 = s%d    \n"   ,myPulseSequence.fMechSwID[i]); 
      printf("enable flag          = %-1d   \n"   ,myPulseSequence.fEnableFlag[i]); 
      printf("mech sw start t (l)  = %-6d cnts \n",myPulseSequence.fMechSwStartTimeLo[i]);  
      printf("mech sw start t (h)  = %-6d cnts \n",myPulseSequence.fMechSwStartTimeHi[i]);  
      printf("mech sw start t      = %-6lf sec \n",msw_start_time_sec);  
      printf("mech sw end t (l)    = %-6d cnts \n",myPulseSequence.fMechSwEndTimeLo[i]);  
      printf("mech sw end t (h)    = %-6d cnts \n",myPulseSequence.fMechSwEndTimeHi[i]);  
      printf("mech sw end t        = %-6lf sec \n",msw_end_time_sec);  
      printf("rf trans start t (l) = %-6d cnts \n",myPulseSequence.fRFTransStartTimeLo[i]);  
      printf("rf trans start t (h) = %-6d cnts \n",myPulseSequence.fRFTransStartTimeHi[i]);  
      printf("rf trans start t     = %-6lf sec \n",rft_start_time_sec);  
      printf("rf trans end t (l)   = %-6d cnts \n",myPulseSequence.fRFTransEndTimeLo[i]);  
      printf("rf trans end t (h)   = %-6d cnts \n",myPulseSequence.fRFTransEndTimeHi[i]);  
      printf("rf trans end t       = %-6lf sec \n",rft_end_time_sec);  
      printf("tomco start t (l)    = %-6d cnts \n",myPulseSequence.fTomcoStartTimeLo[i]);  
      printf("tomco start t (h)    = %-6d cnts \n",myPulseSequence.fTomcoStartTimeHi[i]);  
      printf("tomco start t        = %-6lf sec \n",tom_start_time_sec);  
      printf("tomco end t (l)      = %-6d cnts \n",myPulseSequence.fTomcoEndTimeLo[i]);  
      printf("tomco end t (h)      = %-6d cnts \n",myPulseSequence.fTomcoEndTimeHi[i]);  
      printf("tomco end t          = %-6lf sec \n",tom_end_time_sec);  
      printf("rf rec start t (l)   = %-6d cnts \n",myPulseSequence.fRFRecStartTimeLo[i]);  
      printf("rf rec start t (h)   = %-6d cnts \n",myPulseSequence.fRFRecStartTimeHi[i]);  
      printf("rf rec start t       = %-6lf sec \n",rfr_start_time_sec);  
      printf("rf rec end t (l)     = %-6d cnts \n",myPulseSequence.fRFRecEndTimeLo[i]);  
      printf("rf rec end t (h)     = %-6d cnts \n",myPulseSequence.fRFRecEndTimeHi[i]);  
      printf("rf rec end t         = %-6lf sec \n",rfr_end_time_sec);  
      printf("------------------------------- \n");  
   }

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

   module = (char**)malloc( sizeof(char*)*(mMAX+1) );
   unit   = (char**)malloc( sizeof(char*)*(mMAX+1) );

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

   myFPGA->fSignalName  = (char**)malloc( sizeof(char*)*(mMAX+1) );
   myFPGA->fSignalUnits = (char**)malloc( sizeof(char*)*(mMAX+1) );

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

   // u_int16_t bit_pattern_flag = GetBitPattern(N,module,flag); 
   // myFPGA->fBitPatternFlag = bit_pattern_flag; 

   // get recieve gate time: the ADC needs this in the global variable below 
   int ReceiveGateCounts = 0;

   char *rec_gate     = RF_RECEIVE_NAME;
   const int cSIZE    = 2000;
   char *fpgaName     = (char *)malloc( sizeof(char)*(cSIZE+1) );
   char *signalUnits  = (char *)malloc( sizeof(char)*(cSIZE+1) );

   double rec_gate_cnts = 0.; 

   int NF = myFPGA->fNSignals;
   for(i=0;i<NF;i++){
      fpgaName = myFPGA->fSignalName[i];
      if( AreEquivStrings(fpgaName,rec_gate) ){
         rec_gate_cnts     = (double)( myFPGA->fSignalPulseTimeLo[i] ) + (double)( pow(2,16)*myFPGA->fSignalPulseTimeHi[i] );
         ReceiveGateCounts = (int)rec_gate_cnts; 
         strcpy(signalUnits,myFPGA->fSignalUnits[i]);
      }
   }

   double ReceiveGateTime = GetTimeInUnits(ReceiveGateCounts,ClockFreq,second);

   RECEIVE_GATE_COUNTS    = ReceiveGateCounts; 
   RECEIVE_GATE_TIME_SEC  = ReceiveGateTime; 
   strcpy(RECEIVE_GATE_INPUT_TIME_UNITS,signalUnits);

   free(fpgaName);
   free(signalUnits); 
   for(i=0;i<N;i++){
      free(module[i]);
      free(unit[i]);
   }
   free(module); 
   free(unit);  

   if(gIsDebug) printf("----------------------------------------- \n"); 

}
//______________________________________________________________________________
void ImportPulseSequenceData(char *filename,struct fpgaPulseSequence *myPulseSequence){

   // start_count = start of pulse
   // end_count   = end of pulse
   // count       = total duration of pulse 

   double istart_mech    ,ipulse_mech; 
   double istart_rf_trans,ipulse_rf_trans; 
   double istart_rf_rec  ,ipulse_rf_rec; 
   double istart_tomco   ,ipulse_tomco; 

   int iid; 
   int istart_mech_cnt    ,ipulse_mech_cnt    ,iend_mech_cnt; 
   int istart_rf_trans_cnt,ipulse_rf_trans_cnt,iend_rf_trans_cnt; 
   int istart_rf_rec_cnt  ,ipulse_rf_rec_cnt  ,iend_rf_rec_cnt; 
   int istart_tomco_cnt   ,ipulse_tomco_cnt   ,iend_tomco_cnt; 
   int itomco_enable;  

   const int S4 = 4; 
   int *mech_start_low      = (int *)malloc( sizeof(int)*S4 ); 
   int *rf_trans_start_low  = (int *)malloc( sizeof(int)*S4 ); 
   int *rf_rec_start_low    = (int *)malloc( sizeof(int)*S4 ); 
   int *tomco_start_low     = (int *)malloc( sizeof(int)*S4 ); 

   int *mech_start_high     = (int *)malloc( sizeof(int)*S4 );  
   int *rf_trans_start_high = (int *)malloc( sizeof(int)*S4 ); 
   int *rf_rec_start_high   = (int *)malloc( sizeof(int)*S4 ); 
   int *tomco_start_high    = (int *)malloc( sizeof(int)*S4 ); 

   int *mech_pulse_low      = (int *)malloc( sizeof(int)*S4 );   
   int *rf_trans_pulse_low  = (int *)malloc( sizeof(int)*S4 ); 
   int *rf_rec_pulse_low    = (int *)malloc( sizeof(int)*S4 ); 
   int *tomco_pulse_low     = (int *)malloc( sizeof(int)*S4 ); 

   int *mech_pulse_high     = (int *)malloc( sizeof(int)*S4 );   
   int *rf_trans_pulse_high = (int *)malloc( sizeof(int)*S4 ); 
   int *rf_rec_pulse_high   = (int *)malloc( sizeof(int)*S4 ); 
   int *tomco_pulse_high    = (int *)malloc( sizeof(int)*S4 ); 
                                                                
   int *mech_end_low        = (int *)malloc( sizeof(int)*S4 );  
   int *rf_trans_end_low    = (int *)malloc( sizeof(int)*S4 ); 
   int *rf_rec_end_low      = (int *)malloc( sizeof(int)*S4 ); 
   int *tomco_end_low       = (int *)malloc( sizeof(int)*S4 ); 

   int *mech_end_high       = (int *)malloc( sizeof(int)*S4 );  
   int *rf_trans_end_high   = (int *)malloc( sizeof(int)*S4 ); 
   int *rf_rec_end_high     = (int *)malloc( sizeof(int)*S4 ); 
   int *tomco_end_high      = (int *)malloc( sizeof(int)*S4 ); 

   int *flag                = (int *)malloc( sizeof(int)*S4 ); 
   int *ID                  = (int *)malloc( sizeof(int)*S4 ); 
   int *tomco_enable        = (int *)malloc( sizeof(int)*S4 ); 

   int i=0,j=0,k=0,N=0; 
   const int MAX = 2000; 
   const int uMAX=2;
   const int fMAX=3;
   const int mMAX=20;  
   const int cMAX=2; 
   int istart_cnt_v[cMAX],iend_cnt_v[cMAX],ipulse_cnt_v[cMAX]; 
   char buf[MAX],iflag[fMAX];
   char iunit_mech[uMAX],iunit_rf_t[uMAX],iunit_rf_r[uMAX],iunit_tomc[uMAX]; 
   char *mode = "r"; 

   char **module,**unit_mech,**unit_rf_t,**unit_rf_r,**unit_tomc;
   module    = (char**)malloc( sizeof(char*)*mMAX );
   unit_mech = (char**)malloc( sizeof(char*)*mMAX );
   unit_rf_t = (char**)malloc( sizeof(char*)*mMAX );
   unit_rf_r = (char**)malloc( sizeof(char*)*mMAX );
   unit_tomc = (char**)malloc( sizeof(char*)*mMAX );

   for(i=0;i<S4;i++){
      flag[i]               = 0;
      ID[i]                 = 0;
      tomco_enable[i]       = 0;
      mech_start_low[i]     = 0;  
      mech_pulse_low[i]     = 0;  
      mech_end_low[i]       = 0;  
      rf_trans_start_low[i] = 0;  
      rf_trans_pulse_low[i] = 0;  
      rf_trans_end_low[i]   = 0;  
      rf_rec_start_low[i]   = 0;  
      rf_rec_pulse_low[i]   = 0;  
      rf_rec_end_low[i]     = 0;  
      tomco_start_low[i]    = 0;  
      tomco_pulse_low[i]    = 0;  
      tomco_end_low[i]      = 0;  
   }

   double ClockFreq = FPGA_CLOCK_FREQ; 

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
            fscanf(infile,"%d %s %lf %lf %s %lf %lf %s %lf %lf %s %d %lf %lf %s",
                   &iid,iflag,
                   &istart_mech    ,&ipulse_mech    ,iunit_mech,
                   &istart_rf_trans,&ipulse_rf_trans,iunit_rf_t,
                   &istart_tomco   ,&ipulse_tomco   ,iunit_tomc,&itomco_enable,
                   &istart_rf_rec  ,&ipulse_rf_rec  ,iunit_rf_r); 
            if( iid!=eof_tag_alt ){ 
               // convert times to clock counts 
               // mechanical switch 
               istart_mech_cnt     = GetClockCounts(istart_mech,ClockFreq,iunit_mech); 
               ipulse_mech_cnt     = GetClockCounts(ipulse_mech,ClockFreq,iunit_mech); 
               iend_mech_cnt       = istart_mech_cnt + ipulse_mech_cnt; 
               // rf switch (transmit)  
               istart_rf_trans_cnt = GetClockCounts(istart_rf_trans,ClockFreq,iunit_rf_t); 
               ipulse_rf_trans_cnt = GetClockCounts(ipulse_rf_trans,ClockFreq,iunit_rf_t); 
               iend_rf_trans_cnt   = istart_rf_trans_cnt + ipulse_rf_trans_cnt; 
               // rf switch (receive)  
               istart_rf_rec_cnt   = GetClockCounts(istart_rf_rec,ClockFreq,iunit_rf_r); 
               ipulse_rf_rec_cnt   = GetClockCounts(ipulse_rf_rec,ClockFreq,iunit_rf_r); 
               iend_rf_rec_cnt     = istart_rf_rec_cnt + ipulse_rf_rec_cnt; 
               // tomco   
               istart_tomco_cnt    = GetClockCounts(istart_tomco,ClockFreq,iunit_tomc); 
               ipulse_tomco_cnt    = GetClockCounts(ipulse_tomco,ClockFreq,iunit_tomc); 
               iend_tomco_cnt      = istart_tomco_cnt + ipulse_tomco_cnt; 
               // divide numbers into low- and high-order bytes 
               // mechanical switch 
               ComputeLowAndHighBytes(istart_mech_cnt,istart_cnt_v); 
               ComputeLowAndHighBytes(ipulse_mech_cnt,ipulse_cnt_v); 
               ComputeLowAndHighBytes(iend_mech_cnt  ,iend_cnt_v);
               mech_start_low[j]  = istart_cnt_v[0]; 
               mech_pulse_low[j]  = ipulse_cnt_v[0]; 
               mech_end_low[j]    = iend_cnt_v[0]; 
               mech_start_high[j] = istart_cnt_v[1]; 
               mech_pulse_high[j] = ipulse_cnt_v[1]; 
               mech_end_high [j]  = iend_cnt_v[1]; 
               // rf switch (transmit)  
               ComputeLowAndHighBytes(istart_rf_trans_cnt,istart_cnt_v); 
               ComputeLowAndHighBytes(ipulse_rf_trans_cnt,ipulse_cnt_v); 
               ComputeLowAndHighBytes(iend_rf_trans_cnt  ,iend_cnt_v); 
               rf_trans_start_low[j]  = istart_cnt_v[0]; 
               rf_trans_pulse_low[j]  = ipulse_cnt_v[0]; 
               rf_trans_end_low[j]    = iend_cnt_v[0]; 
               rf_trans_start_high[j] = istart_cnt_v[1]; 
               rf_trans_pulse_high[j] = ipulse_cnt_v[1]; 
               rf_trans_end_high[j]   = iend_cnt_v[1]; 
               // rf switch (receive)  
               ComputeLowAndHighBytes(istart_rf_rec_cnt,istart_cnt_v); 
               ComputeLowAndHighBytes(ipulse_rf_rec_cnt,ipulse_cnt_v); 
               ComputeLowAndHighBytes(iend_rf_rec_cnt  ,iend_cnt_v); 
               rf_rec_start_low[j]  = istart_cnt_v[0]; 
               rf_rec_pulse_low[j]  = ipulse_cnt_v[0]; 
               rf_rec_end_low[j]    = iend_cnt_v[0]; 
               rf_rec_start_high[j] = istart_cnt_v[1]; 
               rf_rec_pulse_high[j] = ipulse_cnt_v[1]; 
               rf_rec_end_high[j]   = iend_cnt_v[1]; 
               // tomco   
               ComputeLowAndHighBytes(istart_tomco_cnt,istart_cnt_v); 
               ComputeLowAndHighBytes(ipulse_tomco_cnt,ipulse_cnt_v); 
               ComputeLowAndHighBytes(iend_tomco_cnt  ,iend_cnt_v); 
               tomco_start_low[j]  = istart_cnt_v[0]; 
               tomco_pulse_low[j]  = ipulse_cnt_v[0]; 
               tomco_end_low[j]    = iend_cnt_v[0]; 
               tomco_start_high[j] = istart_cnt_v[1]; 
               tomco_pulse_high[j] = ipulse_cnt_v[1]; 
               tomco_end_high[j]   = iend_cnt_v[1]; 
               // mechanical switch ID 
               ID[j]               = iid; 
               // get the module name (using the switch ID)  
               module[j]           = (char*)malloc( sizeof(char)*(mMAX+1) );  
               sprintf(module[j],"S%d",ID[j]); 
               // strcpy(module[j],itag);   
               // get the time units 
               unit_mech[j]        = (char*)malloc( sizeof(char)*(mMAX+1) );  
               strcpy(unit_mech[j],iunit_mech);  
               unit_rf_t[j]        = (char*)malloc( sizeof(char)*(mMAX+1) );  
               strcpy(unit_rf_t[j],iunit_rf_t);  
               unit_rf_r[j]        = (char*)malloc( sizeof(char)*(mMAX+1) );  
               strcpy(unit_rf_r[j],iunit_rf_r);  
               unit_tomc[j]        = (char*)malloc( sizeof(char)*(mMAX+1) );  
               strcpy(unit_tomc[j],iunit_tomc);  
               // set on/off flag 
               if( AreEquivStrings(iflag,on) || AreEquivStrings(iflag,ON) ){  
                  flag[j] = 1; 
               }else if( AreEquivStrings(iflag,off) || AreEquivStrings(iflag,OFF) ){
                  flag[j] = 0;
               }
               // tomco enable 
               tomco_enable[j]      = itomco_enable; 
               // move on to next entry 
               if(gIsDebug && gVerbosity>=1) printf("Adding: index = %d \t"   ,j); 
               if(gIsDebug && gVerbosity>=1) printf("ID = %s \t"              ,module[j]);
               if(gIsDebug && gVerbosity>=1) printf("flag = %d \n"            ,flag[j]);
               if(gIsDebug && gVerbosity>=1) printf("tomco enable = %d \n"    ,tomco_enable[j]);
               // if(gIsDebug && gVerbosity>=1) printf("start (l) = %d \t"       ,start_count_low[j]); 
               // if(gIsDebug && gVerbosity>=1) printf("start (h) = %d \t"       ,start_count_high[j]);
               // if(gIsDebug && gVerbosity>=1) printf("pulse (l) = %d \t"       ,pulse_count_low[j]); 
               // if(gIsDebug && gVerbosity>=1) printf("pulse (h) = %d \t"       ,pulse_count_high[j]);
               // if(gIsDebug && gVerbosity>=1) printf("end (l) = %d \t"         ,end_count_low[j]);
               // if(gIsDebug && gVerbosity>=1) printf("end (h) = %d \n"         ,end_count_high[j]); 
               // if(gIsDebug && gVerbosity>=1) printf("(pulse time = %lf %s) \n",ipulse_time,iunit);
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

   // now add to myPulseSequence 
   myPulseSequence->fNSequences = N; 

   if(gIsDebug && gVerbosity>=1){
      printf("[AcromagFPGA::ImportPulseSequences]: Number of sequences: %d \n",N); 
   }

   // initialize string arrays for labels and units  
   myPulseSequence->fSeqName      = (char**)malloc( sizeof(char*)*FPS4 );
   myPulseSequence->fMechSwUnits  = (char**)malloc( sizeof(char*)*FPS4 );
   myPulseSequence->fRFTransUnits = (char**)malloc( sizeof(char*)*FPS4 );
   myPulseSequence->fRFRecUnits   = (char**)malloc( sizeof(char*)*FPS4 );
   myPulseSequence->fTomcoUnits   = (char**)malloc( sizeof(char*)*FPS4 );

   for(i=0;i<N;i++){
      myPulseSequence->fSeqName[i]  = (char*)malloc( sizeof(char)*(mMAX+1) );
      strcpy(myPulseSequence->fSeqName[i],module[i]); 
      myPulseSequence->fMechSwUnits[i] = (char*)malloc( sizeof(char)*(mMAX+1) );
      strcpy(myPulseSequence->fMechSwUnits[i],unit_mech[i]);
      myPulseSequence->fRFTransUnits[i] = (char*)malloc( sizeof(char)*(mMAX+1) );
      strcpy(myPulseSequence->fRFTransUnits[i],unit_rf_t[i]);
      myPulseSequence->fRFRecUnits[i] = (char*)malloc( sizeof(char)*(mMAX+1) );
      strcpy(myPulseSequence->fRFRecUnits[i],unit_rf_r[i]);
      myPulseSequence->fTomcoUnits[i] = (char*)malloc( sizeof(char)*(mMAX+1) );
      strcpy(myPulseSequence->fTomcoUnits[i],unit_tomc[i]);
      myPulseSequence->fMechSwID[i]           = ID[i]; 
      myPulseSequence->fEnableFlag[i]         = flag[i]; 
      myPulseSequence->fTomcoEnable[i]        = tomco_enable[i]; 
      myPulseSequence->fMechSwStartTimeLo[i]  = mech_start_low[i];  
      myPulseSequence->fMechSwStartTimeHi[i]  = mech_start_high[i];  
      myPulseSequence->fMechSwEndTimeLo[i]    = mech_end_low[i];  
      myPulseSequence->fMechSwEndTimeHi[i]    = mech_end_high[i];  
      myPulseSequence->fRFTransStartTimeLo[i] = rf_trans_start_low[i];  
      myPulseSequence->fRFTransStartTimeHi[i] = rf_trans_start_high[i];  
      myPulseSequence->fRFTransEndTimeLo[i]   = rf_trans_end_low[i];  
      myPulseSequence->fRFTransEndTimeHi[i]   = rf_trans_end_high[i];  
      myPulseSequence->fRFRecStartTimeLo[i]   = rf_rec_start_low[i];  
      myPulseSequence->fRFRecStartTimeHi[i]   = rf_rec_start_high[i];  
      myPulseSequence->fRFRecEndTimeLo[i]     = rf_rec_end_low[i];  
      myPulseSequence->fRFRecEndTimeHi[i]     = rf_rec_end_high[i];  
      myPulseSequence->fTomcoStartTimeLo[i]   = tomco_start_low[i];  
      myPulseSequence->fTomcoStartTimeHi[i]   = tomco_start_high[i];  
      myPulseSequence->fTomcoEndTimeLo[i]     = tomco_end_low[i];  
      myPulseSequence->fTomcoEndTimeHi[i]     = tomco_end_high[i];  
   }

   if(gIsDebug) printf("----------------------------------------- \n"); 

   // free memory 
   free( mech_start_low     ); 
   free( rf_trans_start_low ); 
   free( rf_rec_start_low   ); 
   free( tomco_start_low    ); 

   free( mech_start_high    );  
   free( rf_trans_start_high); 
   free( rf_rec_start_high  ); 
   free( tomco_start_high   ); 

   free( mech_pulse_low     );   
   free( rf_trans_pulse_low ); 
   free( rf_rec_pulse_low   ); 
   free( tomco_pulse_low    ); 

   free( mech_pulse_high    );   
   free( rf_trans_pulse_high); 
   free( rf_rec_pulse_high  ); 
   free( tomco_pulse_high   );
 
   free( mech_end_low       );  
   free( rf_trans_end_low   ); 
   free( rf_rec_end_low     ); 
   free( tomco_end_low      ); 

   free( mech_end_high      );  
   free( rf_trans_end_high  ); 
   free( rf_rec_end_high    ); 
   free( tomco_end_high     );

   free( flag               );  
   free( ID                 ); 

   for(i=0;i<mMAX;i++){
      free(module[i]);
      free(unit_mech[i]);
      free(unit_rf_t[i]);
      free(unit_rf_r[i]);
      free(unit_tomc[i]); 
   }
 
   free(module);
   free(unit_mech);
   free(unit_rf_t);
   free(unit_rf_r);
   free(unit_tomc); 

}
//______________________________________________________________________________
void ImportGlobalOnOff(char *filename,struct fpgaPulseSequence *myPulseSequence){

   int iflag=0,global_on_off=0; 

   int k=0; 
   const int MAX = 2000; 
   const int tMAX=20;  
   char buf[MAX],itag[tMAX]; 
   char *mode = "r"; 

   FILE *infile; 
   infile = fopen(filename,mode);

   if(infile==NULL){
      printf("[AcromagFPGA::ImportGlobalOnOff]: Cannot open the file: %s.  Exiting... \n",filename);
      exit(1);  
   }else{
      if(gIsDebug) printf("[AcromagFPGA::ImportGlobalOnOff]: Opening the file: %s... \n",filename);
      while( !feof(infile) ){
	 if(k==0){
	    fgets(buf,MAX,infile);
	 }else{
	    fscanf(infile,"%s %d",itag,&iflag);
	    if( !AreEquivStrings(itag,eof_tag) ){ 
	       global_on_off = iflag;
               if(gIsDebug && gVerbosity>=1) printf("%s %d \n",itag,global_on_off); 
	    }else{
               break;
            }
	 }
	 k++;
      }
      fclose(infile); 
   }

   myPulseSequence->fGlobalEnable = global_on_off;

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

   int trans_gate_start_counts      = 0; 
   int trans_gate_end_counts        = 0; 
   int rec_gate_start_counts        = 0; 
   int rec_gate_end_counts          = 0; 
   int rf_gate_start_counts         = 0; 
   int rf_gate_end_counts           = 0; 
   int mech_sw_gate_start_counts[4] = {0,0,0,0}; 
   int mech_sw_gate_end_counts[4]   = {0,0,0,0}; 

   // gather times 

   int i=0; 
   int NF = myFPGA.fNSignals;
   for(i=0;i<NF;i++){
      fpgaName = myFPGA.fSignalName[i];
      // mechanical switch 
      if( AreEquivStrings(fpgaName,mech_sw_1) ){
	 mech_sw_gate_start_counts[0] = myFPGA.fSignalStartTimeLo[i] +  pow(2,16)*myFPGA.fSignalStartTimeHi[i];
	 mech_sw_gate_end_counts[0]   = myFPGA.fSignalEndTimeLo[i]   +  pow(2,16)*myFPGA.fSignalEndTimeHi[i];
      }else if( AreEquivStrings(fpgaName,mech_sw_2) ){
	 mech_sw_gate_start_counts[1] = myFPGA.fSignalStartTimeLo[i] +  pow(2,16)*myFPGA.fSignalStartTimeHi[i];
	 mech_sw_gate_end_counts[1]   = myFPGA.fSignalEndTimeLo[i]   +  pow(2,16)*myFPGA.fSignalEndTimeHi[i];
      }else if( AreEquivStrings(fpgaName,mech_sw_3) ){
	 mech_sw_gate_start_counts[2] = myFPGA.fSignalStartTimeLo[i] +  pow(2,16)*myFPGA.fSignalStartTimeHi[i];
	 mech_sw_gate_end_counts[2]   = myFPGA.fSignalEndTimeLo[i]   +  pow(2,16)*myFPGA.fSignalEndTimeHi[i];
      }else if( AreEquivStrings(fpgaName,mech_sw_4) ){
	 mech_sw_gate_start_counts[3] = myFPGA.fSignalStartTimeLo[i] +  pow(2,16)*myFPGA.fSignalStartTimeHi[i];
	 mech_sw_gate_end_counts[3]   = myFPGA.fSignalEndTimeLo[i]   +  pow(2,16)*myFPGA.fSignalEndTimeHi[i];
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


   int fail=0;
   int ret_code = 0; 

   double ClockFreq                  = myFPGA.fClockFrequency; 
   double mech_sw_gate_start_time[4] = {0,0,0,0}; 
   double mech_sw_gate_end_time[4]   = {0,0,0,0}; 
   for(i=0;i<4;i++){
      mech_sw_gate_start_time[i] = GetTimeInUnits(mech_sw_gate_start_counts[i],ClockFreq,second);
      mech_sw_gate_end_time[i]   = GetTimeInUnits(mech_sw_gate_end_counts[i]  ,ClockFreq,second);
   }
   double rec_gate_start_time     = GetTimeInUnits(rec_gate_start_counts    ,ClockFreq,second);
   double rec_gate_end_time       = GetTimeInUnits(rec_gate_end_counts      ,ClockFreq,second);
   double trans_gate_start_time   = GetTimeInUnits(trans_gate_start_counts  ,ClockFreq,second);
   double trans_gate_end_time     = GetTimeInUnits(trans_gate_end_counts    ,ClockFreq,second);
   double rf_gate_start_time      = GetTimeInUnits(rf_gate_start_counts     ,ClockFreq,second);
   double rf_gate_end_time        = GetTimeInUnits(rf_gate_end_counts       ,ClockFreq,second);

   u_int16_t bit_pattern = myFPGA.fBitPatternFlag; 
   int mech_sw_state[4] = {0,0,0,0}; 

   // bit   description
   // -----------------------------  
   // 0     GLOBAL ON/OFF  
   // 1     MECHANICAL_SWITCH_1
   // 2     MECHANICAL_SWITCH_2
   // 3     MECHANICAL_SWITCH_3
   // 4     MECHANICAL_SWITCH_4
   // 5     RF_SWITCH_1
   // 6     RF_SWITCH_2
   // 7     RF_SWITCH_3
   // 8     RF_CLEAR
   // 9     RF_PULSE
   // 10    RF_GATE
   // 11    DIGITIZER_FLAG_1
   // 12    DIGITIZER_FLAG_2

   // find out which mechanical switches are activated 

   int mech_sw_cntr=0; 
   for(i=0;i<4;i++){
      mech_sw_state[i] = GetBit(i+1,bit_pattern);
      if(mech_sw_state[i]==1) mech_sw_cntr++;  
   }   

   if(mech_sw_cntr==0){
      fail++;
      printf("[AcromagFPGA::TimingCheck]: No mechanical switches activated! \n");
      ret_code = -3;
      return ret_code;   
   } 

   if(gIsDebug && gVerbosity>1){
      printf("[AcromagFPGA::TimingCheck]: Number of mechanical switches activated: %d \n",mech_sw_cntr);
      for(i=0;i<4;i++){
         printf("[AcromagFPGA::TimingCheck]: mech_sw_%d state: %d \n",i+1,mech_sw_state[i]); 
      } 
   }

   // start the check

   // find the start time for the earliest mechanical switch; this is what we base our checks on  

   int start_index=-1; 
   double min_start_time = 1E+5; 
   for(i=0;i<4;i++){
      if( (mech_sw_state[i]==1) && (mech_sw_gate_start_time[i] < min_start_time) ){
	 min_start_time = mech_sw_gate_start_time[i]; 
	 start_index = i; 
	 sprintf(mech_sw_gate,"mech_sw_%d",start_index+1);
      }
   }

   // require ALL mechanical switch times to be identical
   int mech_sw_counts_base = mech_sw_gate_end_counts[start_index] - mech_sw_gate_start_counts[start_index];
   int mech_sw_counts=0; 
   double time_base = GetTimeInUnits(mech_sw_counts_base,ClockFreq,second); 
   double time=0; 
   for(i=0;i<4;i++){
      if(mech_sw_state[i]==1){
         mech_sw_counts = mech_sw_gate_end_counts[i] - mech_sw_gate_start_counts[i];
         if( mech_sw_counts != mech_sw_counts_base ){
            fail++;
            time = GetTimeInUnits(mech_sw_counts,ClockFreq,second); 
            printf("[AcromagFPGA::TimingCheck]: mech_sw_%d gate differs from mech_sw_%d gate! \n",i+1,start_index+1);  
            printf("                            mech_sw_%d: %lf s   mech_sw_%d: %lf s \n",start_index+1,time_base,i+1,time);  
         } 
      } 
   }
 
   // is the transmit gate inside the FIRST mechanical switch gate (if multiple switches being used)?
   if( (trans_gate_start_time > mech_sw_gate_start_time[start_index]) && 
       (trans_gate_end_time < mech_sw_gate_end_time[start_index]) ){
      // do nothing  
   }else{
      printf("[AcromagFPGA::TimingCheck]: %s gate is not inside %s gate! \n",trans_gate,mech_sw_gate); 
      printf("                            %s: %lf  %lf \n%s: %lf %lf \n",trans_gate,trans_gate_start_time,trans_gate_end_time,
                                                                         mech_sw_gate,mech_sw_gate_start_time[start_index],mech_sw_gate_end_time[start_index]);  
      fail++; 
   } 
   // is the rf gate inside the transmit gate? 
   if( (rf_gate_start_time > trans_gate_start_time) && 
       (rf_gate_end_time < trans_gate_end_time) ){
      // do nothing  
   }else{
      printf("[AcromagFPGA::TimingCheck]: %s gate is not inside %s gate! \n",rf_gate,trans_gate); 
      printf("                            %s: %lf  %lf \n%s: %lf %lf \n",rf_gate,rf_gate_start_time,rf_gate_end_time,trans_gate,trans_gate_start_time,trans_gate_end_time);  
      fail++; 
   } 
   // is the receive gate AFTER the transmit gate? 
   if(rec_gate_start_time > trans_gate_end_time){
      // do nothing  
   }else{
      printf("[AcromagFPGA::TimingCheck]: %s gate starts before the end of the %s gate! \n",rec_gate,trans_gate);
      printf("                            %s: %lf \n%s: %lf \n",rec_gate,rec_gate_start_time,trans_gate,trans_gate_end_time);  
      fail++; 
   }
   // is the receive gate inside the mechanical switch gate? 
   if( (rec_gate_start_time > mech_sw_gate_start_time[start_index]) && 
       (rec_gate_end_time < mech_sw_gate_end_time[start_index]) ){
      // do nothing  
   }else{
      printf("[AcromagFPGA::TimingCheck]: %s gate is not inside %s gate! \n",rec_gate,mech_sw_gate); 
      printf("                            %s: %lf  %lf \n%s: %lf %lf \n",rec_gate,rec_gate_start_time,rec_gate_end_time,
                                                                         mech_sw_gate,mech_sw_gate_start_time[start_index],mech_sw_gate_end_time[start_index]);  
      fail++; 
   } 
   // print error statement if necessary 
   if(fail>0){
      printf("[AcromagFPGA::TimingCheck]: Timing check failed %d time(s)!  Exiting... \n",fail); 
      ret_code = -2;
   }

   free(fpgaName); 
   free(mech_sw_gate); 

   return ret_code;
}
//______________________________________________________________________________
int TimingCheckNew(const struct fpgaPulseSequence myPulseSequence){

   // make sure all timings are correct. 

   int mech_sw_start,mech_sw_end;
   int rf_trans_start,rf_trans_end;
   int rf_rec_start,rf_rec_end;
   int tomco_start,tomco_end;

   double mech_sw_start_time,mech_sw_end_time;
   double rf_trans_start_time,rf_trans_end_time;
   double rf_rec_start_time,rf_rec_end_time;
   double tomco_start_time,tomco_end_time;

   double ClockFreq = FPGA_CLOCK_FREQ; 

   int fail=0,ret_code=0; 

   int i=0; 
   int NF = myPulseSequence.fNSequences;
   for(i=0;i<NF;i++){
      // get mechanical switch 
      mech_sw_start      = myPulseSequence.fMechSwStartTimeLo[i] + pow(2,16)*myPulseSequence.fMechSwStartTimeHi[i]; 
      mech_sw_end        = myPulseSequence.fMechSwEndTimeLo[i]   + pow(2,16)*myPulseSequence.fMechSwEndTimeHi[i]; 
      mech_sw_start_time = GetTimeInUnits(mech_sw_start,ClockFreq,second);  
      mech_sw_end_time   = GetTimeInUnits(mech_sw_end  ,ClockFreq,second);  
      // get rf transmit switch  
      rf_trans_start      = myPulseSequence.fRFTransStartTimeLo[i] + pow(2,16)*myPulseSequence.fRFTransStartTimeHi[i]; 
      rf_trans_end        = myPulseSequence.fRFTransEndTimeLo[i]   + pow(2,16)*myPulseSequence.fRFTransEndTimeHi[i]; 
      rf_trans_start_time = GetTimeInUnits(rf_trans_start,ClockFreq,second);  
      rf_trans_end_time   = GetTimeInUnits(rf_trans_end  ,ClockFreq,second);  
      // get rf receive switch  
      rf_rec_start        = myPulseSequence.fRFRecStartTimeLo[i]   + pow(2,16)*myPulseSequence.fRFRecStartTimeHi[i]; 
      rf_rec_end          = myPulseSequence.fRFRecEndTimeLo[i]     + pow(2,16)*myPulseSequence.fRFRecEndTimeHi[i]; 
      rf_rec_start_time   = GetTimeInUnits(rf_rec_start,ClockFreq,second);  
      rf_rec_end_time     = GetTimeInUnits(rf_rec_end  ,ClockFreq,second);  
      // get tomco  
      tomco_start         = myPulseSequence.fTomcoStartTimeLo[i]   + pow(2,16)*myPulseSequence.fTomcoStartTimeHi[i]; 
      tomco_end           = myPulseSequence.fTomcoEndTimeLo[i]     + pow(2,16)*myPulseSequence.fTomcoEndTimeHi[i]; 
      tomco_start_time    = GetTimeInUnits(tomco_start,ClockFreq,second);  
      tomco_end_time      = GetTimeInUnits(tomco_end  ,ClockFreq,second); 
      // now check 
      // mech sw makes sense? 
      if(mech_sw_end_time>mech_sw_start_time){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: Mechanical switch gate is incorrect!  \n"); 
         printf("                            mech sw start:  %.3lf s    mech sw end:  %.3lf s \n",mech_sw_start_time,mech_sw_end_time); 
	 fail++;
      }
      // transmit gate starts within the mechanical switch gate?  
      if( (rf_trans_start_time>mech_sw_start_time)&&(rf_trans_start_time<mech_sw_end_time) ){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: RF transmit gate is OUTSIDE mechanical switch gate!  \n"); 
         printf("                            mech sw start:  %.3lf s    mech sw end:  %.3lf s \n",mech_sw_start_time,mech_sw_end_time); 
         printf("                            rf trans start: %.3lf s \n",rf_trans_start_time); 
	 fail++; 
      }
      if( (rf_trans_end_time>mech_sw_start_time)&&(rf_trans_end_time<mech_sw_end_time) ){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: RF transmit gate is OUTSIDE mechanical switch gate!  \n"); 
         printf("                            mech sw start: %.3lf s    mech sw end:  %.3lf s \n",mech_sw_start_time,mech_sw_end_time); 
         printf("                            rf trans end:  %.3lf s \n",rf_trans_end_time); 
	 fail++; 
      }
      // transmit end is after transmit start? 
      if(rf_trans_end_time>rf_trans_start_time){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: RF transmit gate is incorrect!  \n"); 
         printf("                            rf trans start: %.3lf s \nrf trans end: %.3lf s \n",rf_trans_start_time,rf_trans_end_time); 
	 fail++;
      } 
      // tomco gate makes sense? 
      if(tomco_end_time>tomco_start_time){
	 // passed, do nothing
      }else{
	 printf("[AcromagFPGA::TimingCheck]: Tomco gate is incorrect!  \n"); 
         printf("                            tomco start: %.3lf s \ntomco end: %.3lf s \n",tomco_start_time,tomco_end_time); 
	 fail++; 
      }
      // tomco gate inside transmit gate?  
      if( (tomco_start_time>rf_trans_start_time)&&(tomco_start_time<rf_trans_end_time) ){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: Tomco gate is NOT inside RF transmit gate! \n"); 
         printf("                            rf trans start: %.3lf s    rf trans end: %.3lf s \n",rf_trans_start_time,rf_trans_end_time); 
         printf("                            tomco start:    %.3lf s\n",tomco_start_time); 
	 fail++; 
      } 
      if( (tomco_end_time>rf_trans_start_time)&&(tomco_end_time<rf_trans_end_time) ){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: Tomco gate is NOT inside RF transmit gate!  \n"); 
         printf("                            rf trans start: %.3lf s \nrf trans end: %.3lf s \n",rf_trans_start_time,rf_trans_end_time); 
         printf("                            tomco end:      %.3lf s\n",tomco_end_time); 
	 fail++; 
	 fail++; 
      } 
      // receive gate makes sense? 
      if(rf_rec_end_time>rf_rec_start_time){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: RF receive gate is incorrect!  \n"); 
         printf("                            ref rec start: %.3lf s \nrf rec end: %.3lf s \n",rf_rec_start_time,rf_rec_end_time); 
	 fail++;
      }   
      // receive gate after transmit gate? 
      if( (rf_rec_start_time>rf_trans_start_time)&&(rf_rec_start_time>rf_trans_end_time) ){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: RF receive gate is BEFORE RF transmit gate!  \n"); 
         printf("                            rf trans start: %.3lf s    rf trans end: %.3lf s \n",rf_trans_start_time,rf_trans_end_time); 
         printf("                            rf rec start:   %.3lf s \n",rf_rec_start_time); 
	 fail++;
      }   
      if( (rf_rec_end_time>rf_trans_start_time)&&(rf_rec_end_time>rf_trans_end_time) ){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: RF receive gate is BEFORE RF transmit gate!  \n"); 
         printf("                            rf trans start: %.3lf s    rf trans end: %.3lf s \n",rf_trans_start_time,rf_trans_end_time); 
         printf("                            rf rec end:     %.3lf s \n",rf_rec_end_time); 
	 fail++;
      }
      // receive gate inside mechanical switch? 
      if( (rf_rec_start_time>mech_sw_start_time)&&(rf_rec_start_time<mech_sw_end_time) ){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: RF receive gate is OUTSIDE mechanical switch gate!  \n"); 
         printf("                            mech sw start: %.3lf s    mech sw end:  %.3lf s \n",mech_sw_start_time,mech_sw_end_time); 
         printf("                            rf rec start:  %.3lf s \n",rf_rec_start_time); 
	 fail++;
      }        
      if( (rf_rec_end_time>mech_sw_start_time)&&(rf_rec_end_time<mech_sw_end_time) ){
	 // passed; do nothing 
      }else{
	 printf("[AcromagFPGA::TimingCheck]: RF receive gate is OUTSIDE mechanical switch gate!  \n"); 
         printf("                            mech sw start: %.3lf s    mech sw end:  %.3lf s \n",mech_sw_start_time,mech_sw_end_time); 
         printf("                            rf rec end:    %.3lf s \n",rf_rec_end_time); 
	 fail++;
      }        
   }
 
   // print error statement if necessary 
   if(fail>0){
      printf("[AcromagFPGA::TimingCheck]: Timing check failed %d time(s)!  Exiting... \n",fail); 
      ret_code = -2;
   }

   return ret_code;
}
//______________________________________________________________________________
u_int16_t GetBitPattern(int N,char **module_list,int *flag){

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
         printf("%d ",MasterFlag[i]); 
      }
      printf("\n"); 
   }

   // compute the bit pattern 
   int arg=0;
   u_int16_t bit_pattern = 0;  
   for(i=0;i<MAX;i++){  
      arg          = MasterFlag[i]*pow(2,i); 
      bit_pattern += arg;
      if(gIsDebug && gVerbosity>=2) printf("i = %d master_flag = %d arg = %d \n",i,MasterFlag[i],arg);  
   }

   // check it 
   int myBit=0; 
   if(gIsDebug && gVerbosity>=2){
      printf("[AcromagFPGA::GetBitPattern]: Representation of flags: 0x%04x (hex) %d (decimal) \n",bit_pattern,bit_pattern); 
      printf("[AcromagFPGA::GetBitPattern]: Individual bits (MSB--LSB):  \n"); 
      for(i=15;i>=0;i--){
         myBit = GetBit(i,bit_pattern);
         printf("%d ",myBit); 
      }
      printf("\n"); 
   }

   return bit_pattern; 
}
//______________________________________________________________________________
u_int16_t GetBitPatternNew(int Switch,const struct fpgaPulseSequence myPulseSequence){

   // Set the bit pattern to be sent to the FPGA; this 
   // will enable/disable each individual signal  
   
   // first find which switch is turned on 
   const int NS = myPulseSequence.fNSequences;

   int cntr       = 0;
   int mech_sw_id = 0;
   int is         = 0;  // mechanical switch index

   // find the index of the desired switch 
   int i=0;
   for(i=0;i<NS;i++){
      mech_sw_id = myPulseSequence.fMechSwID[i]; 
      if(Switch==mech_sw_id){ 
	 cntr++;
	 is = i;
      }
   }

   if(cntr==0){
      // nobody is turned on; return 0 
      return 0x0;
   }else if(cntr>1){
      printf("[AcromagFPGA::GetBitPatternNew]: ERROR! More than one mechanical switch activated!  No output allowed. \n");
      return 0x0;
   }

   // checks are done.  now build the pattern
   int global_on_off = myPulseSequence.fGlobalEnable;
   int enable        = myPulseSequence.fEnableFlag[is];  // is the desired switch turned on? Should be if it's in the list... 
 
   int mech_sw_1=0,mech_sw_2=0,mech_sw_3=0,mech_sw_4=0;

   if(Switch==1){
      if(enable==1) mech_sw_1 = 1; 
   }else if(Switch==2){
      if(enable==1) mech_sw_2 = 1; 
   }else if(Switch==3){
      if(enable==1) mech_sw_3 = 1; 
   }else if(Switch==4){
      if(enable==1) mech_sw_4 = 1; 
   } 
  
   int rf_trans = 1;                                 // always on 
   int rf_rec   = 1;                                 // always on 
   int tomco_en = myPulseSequence.fTomcoEnable[is];  // is the tomco enabled?  

   if(gIsDebug && gVerbosity>=1){ 
      if(tomco_en==1) printf("[AcromagFPGA::GetBitPatternNew]: The Tomco is ENABLED \n"); 
   }

   // now build the array of bits 
   int *myBit = (int *)malloc( sizeof(int)*mlMAX ); 
   for(i=0;i<mlMAX;i++){
      myBit[i] = 0; 
   }

   myBit[0]  = global_on_off; 
   myBit[1]  = mech_sw_1; 
   myBit[2]  = mech_sw_2; 
   myBit[3]  = mech_sw_3; 
   myBit[4]  = mech_sw_4; 
   myBit[5]  = 0; 
   myBit[6]  = rf_trans; 
   myBit[7]  = rf_rec; 
   myBit[8]  = 0; 
   myBit[9]  = 0; 
   myBit[10] = tomco_en; 

   // compute the bit pattern 
   int arg=0;
   u_int16_t bit_pattern = 0;  
   for(i=0;i<mlMAX;i++){  
      arg          = myBit[i]*pow(2,i); 
      bit_pattern += arg;
   }

   if(gIsDebug && gVerbosity>=2){ 
      printf("[AcromagFPGA::GetBitPatternNew]: The bit pattern is: "); 
      PrintBits16(bit_pattern); 
   }

   // check it 
   int aBit=0; 
 
   int *myBitAfter = (int *)malloc( sizeof(int)*mlMAX ); 
   for(i=0;i<mlMAX;i++) myBitAfter[i] = 0;
   
   if(gIsDebug && gVerbosity>=2) printf("[AcromagFPGA::GetBitPattern]: Representation of flags: 0x%04x (hex) %d (decimal) \n",bit_pattern,bit_pattern); 
   for(i=mlMAX-1;i>=0;i--){
      aBit          = GetBit(i,bit_pattern);
      myBitAfter[i] = aBit;  
   }

   int fail=0; 
   for(i=0;i<mlMAX;i++){
      if(myBit[i]!=myBitAfter[i]) fail++;  
   }

   if(fail>0){
      printf("[AcromagFPGA::GetBitPatternNew]: ERROR!  Bit pattern generation FAILED! \n");
      printf("Desired:   "); 
      for(i=mlMAX-1;i>=0;i--){
	 printf("%d ",myBit[i]);
      }
      printf("\n"); 
      printf("Generated: "); 
      for(i=mlMAX-1;i>=0;i--){
	 printf("%d ",myBitAfter[i]);
      }
      printf("\n");
      printf("[AcromagFPGA::GetBitPatternNew]: Setting bit pattern to 0x0000. \n");
      bit_pattern = 0x0000;  
   }

   free(myBit); 
   free(myBitAfter); 

   return bit_pattern; 
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
int WriteFPGAMemory(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t my_mem_addr,u_int16_t data16){

   // This write access requires four wait states.  I'm assuming 
   // that time corresponds to the 8 MHz clock on the board.  Let's use that. 
   // Then, four wait states is 4*125ns = 500 ns = .5 us.  Use 1 us (8 wait states), 
   // since usleep takes integers as arguments.  
   int time = 1; // wait 1 us 
   int ret_code=1; 

   u_int16_t base_addr = carrier_addr + daughter_addr;
   u_int16_t addr      = base_addr + my_mem_addr;  
   ret_code            = vme_A16D16_write(p,addr,data16);         // read from memory data register 
   if(gIsDebug && gVerbosity>=4) Print("ReadFPGAMemory","Reading...",addr,data16,ret_code); 
   usleep(time);  

   CheckStatus(addr,data16,ret_code); 

   return ret_code; 

}
//______________________________________________________________________________
u_int16_t ReadFPGAMemory(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t my_mem_addr){

   // This write access requires four wait states.  I'm assuming 
   // that time corresponds to the 8 MHz clock on the board.  Let's use that. 
   // Then, four wait states is 4*125ns = 500 ns = .5 us.  Use 1 us (8 wait states), 
   // since usleep takes integers as arguments.  
   int time = 1; // wait 1 us 

   u_int16_t data16;
   u_int16_t base_addr = carrier_addr + daughter_addr;
   u_int16_t addr      = base_addr + my_mem_addr;  
   int ret_code        = vme_A16D16_read(p,addr,&data16);         // read from memory data register 
   if(gIsDebug && gVerbosity>=4) Print("ReadFPGAMemory","Reading...",addr,data16,ret_code); 
   usleep(time);  

   CheckStatus(addr,data16,ret_code); 

   // int bit_pattern = (int)data16;
   // return bit_pattern;  
   return data16; 

}
//______________________________________________________________________________
u_int16_t ReadMemoryDataReg(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t my_mem_addr){

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

   return data16;  

}
//______________________________________________________________________________
int WriteMemoryDataReg(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t my_mem_addr,u_int16_t bit_pattern){

   // printf("[AcromagFPGA::ReadMemoryDataReg]: Reading from the Memory Data Register... "); 
   // printf("First, writing to Memory Address Register... \n"); 

   // This write access requires four wait states.  I'm assuming 
   // that time corresponds to the 8 MHz clock on the board.  Let's use that. 
   // Then, four wait states is 4*125ns = 500 ns = .5 us.  Use 1 us (8 wait states), 
   // since usleep takes integers as arguments.   
   int time_1 = 1; // wait 1 us 
   int time_2 = 100; // wait 1 us 

   u_int16_t data16;
   u_int16_t base_addr = carrier_addr + daughter_addr; 
   int ret_code        = vme_A16D16_write(p,base_addr+0x16,my_mem_addr);   // write to memory addr register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Writing...",base_addr+0x16,my_mem_addr,ret_code); 
   usleep(time_1);  
   ret_code      = vme_A16D16_read(p,base_addr+0x16,&data16);              // read from memory addr register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Reading...",base_addr+0x16,data16,ret_code); 
   usleep(time_1);  
   ret_code      = vme_A16D16_write(p,base_addr+0x14,bit_pattern);    // write to memory data register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Writing...",base_addr+0x14,bit_pattern,ret_code); 
   usleep(time_1);  
   ret_code        = vme_A16D16_write(p,base_addr+0x16,my_mem_addr);   // write to memory addr register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Writing...",base_addr+0x16,my_mem_addr,ret_code); 
   usleep(time_1);  
   ret_code      = vme_A16D16_read(p,base_addr+0x14,&data16);              // read from memory data register 
   if(gIsDebug && gVerbosity>=4) Print("WriteMemoryDataReg","Reading...",base_addr+0x14,data16,ret_code); 
   usleep(time_2);  

   CheckStatus(base_addr+0x14,data16,ret_code);

   return ret_code;  

}
//______________________________________________________________________________
void SetClockSpeed(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t choice){
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

   u_int16_t base_addr = carrier_addr + daughter_addr; 
   u_int16_t addr_1    = base_addr + 0x18;          // manual says 0x19, but we're in little-endian 
   u_int16_t addr_2    = base_addr + 0x1a;          // manual says 0x1b, but we're in little-endian
   u_int16_t addr_3    = base_addr + 0x1c;          // manual says 0x1d, but we're in little-endian 

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
   u_int16_t clock_trig_reg_addr = base_addr + 0x1e;          // manual says 0x1f, but we're in little-endian 
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
void SetCtrlRegIDBits(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t my_data){
   u_int16_t data16;  
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
int CheckMode(int p,u_int16_t carrier_addr,u_int16_t fpga_addr){

   if(!gIsDebug) printf("[AcromagFPGA]: Checking mode of FPGA... \n"); 
   u_int16_t data16  = 0;
   int mode          = -1; 
   u_int16_t offset  = 0x0a; 
   u_int16_t my_addr = carrier_addr + fpga_addr + offset; 
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
void SoftwareReset(int p,u_int16_t carrier_addr,u_int16_t daughter_addr){
   // software reset of the FPGA 
   if(!gIsDebug) printf("[AcromagFPGA]: Resetting the software on the FPGA... "); 
   if(gIsDebug)  printf("[AcromagFPGA::SoftwareReset]: Resetting the software on the FPGA... \n"); 
   u_int16_t data16;        
   u_int16_t offset  = 0x0;  
   u_int16_t my_addr = carrier_addr + daughter_addr + offset;
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
void BoardReset(int p,u_int16_t carrier_addr,u_int16_t daughter_addr){
   // Put the FPGA in user mode 
   printf("[AcromagFPGA::BoardReset]: In FLASH mode, setting the FPGA to USER mode... \n"); 
   u_int16_t data16;        
   u_int16_t addr_0x00 = carrier_addr + daughter_addr + 0x00;
   u_int16_t addr_0x8a = carrier_addr + daughter_addr + 0x80 + 0x0a;   
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
void PrintIDProm(int p,u_int16_t carrier_addr,u_int16_t fpga_addr){
   u_int16_t data16    = 0;
   u_int16_t ascii_val = -1;
   int i         =  0; 
   // int start    = carrier_addr + fpga_addr + 0x1; // big-Endian  
   u_int16_t start = carrier_addr + fpga_addr + 0x0; // little-Endian  
   u_int16_t end   = start + 0x3f;
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
void PrintIOSpace(int p,u_int16_t carrier_addr,u_int16_t daughter_addr){
   u_int16_t data16 = 0;
   int i,ret_code;
   u_int16_t start = carrier_addr + daughter_addr + 0x2; // little-Endian  
   u_int16_t end   = start + 0x6;
   printf("[AcromagFPGA::PrintIOSpace]: Begin IO SPACE read...\n");
   for (i=start;i<end;i++){
      ret_code =  vme_A16D16_read(p, i, &data16); /* high word */
      Print("PrintIOSpace","",i,data16,ret_code);
   }
   printf("[AcromagFPGA::PrintIOSpace]: End IO SPACE read.\n");
   // CheckStatus(ret_code);
}
//______________________________________________________________________________
void QueryIOSpace(int p,u_int16_t carrier_addr,u_int16_t fpga_addr){
   u_int16_t data16 = 0;
   int ret_code = 0,dummy =0;
   int i;
   u_int16_t start = carrier_addr; 
   u_int16_t end   = 0x80; 
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
void SetMode(int p,u_int16_t carrier_addr,u_int16_t fpga_addr,int choice){
   // config mode = 0; user mode = 1 
   if(choice==0) printf("[AcromagFPGA::SetMode]: Setting to configuration mode...\n"); 
   if(choice==1) printf("[AcromagFPGA::SetMode]: Setting to user mode...\n"); 
   u_int16_t data16 = 0; 
   // read module information 
   // we want to read base_addr + 0a(hex) to see what mode we're currently in
   // remember: the board uses little-Endian, NOT big-Endian like the manual uses.
   // If big-Endian, use base_addr + 0b(hex)  
   // int addr     = carrier_addr + fpga_addr + 0x0a;   
   u_int16_t  addr_0x0a = carrier_addr + fpga_addr + 0x0a;   
   u_int16_t  addr_0x00 = carrier_addr + fpga_addr + 0x0;   // we need to write to this bit  
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
void SetControlRegister(int p,u_int16_t carrier_addr,u_int16_t fpga_addr,u_int16_t state){
   // the variable 'value' is the value of bit-0: low (0) or high (1) or flip (2)  
   if(state==0) printf("[AcromagFPGA::SetControlRegister]: Setting bit-0 to logic LOW (user mode)... \n");
   if(state==1) printf("[AcromagFPGA::SetControlRegister]: Setting bit-0 to logic HIGH (configuration mode)... \n");
   if(state==2) printf("[AcromagFPGA::SetControlRegister]: Flipping bit-0 logic...\n");
   // read module information 
   u_int16_t data16  = 0;
   u_int16_t offset  = 0x0; 
   u_int16_t my_addr = carrier_addr + fpga_addr + offset;    
   int ret_code      = vme_A16D16_read(p,my_addr,&data16); // high word 
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
void SetDirCtrlReg(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t bit_pattern){
   // Set this AFTER the I/O registers! 
   // set direction control register to all outputs 
   // logic low  => input  
   // logic high => output 
   u_int16_t data16;          
   u_int16_t offset  = 0x8; 
   u_int16_t my_addr = carrier_addr + daughter_addr + offset;    
   if(gIsDebug && gVerbosity>=3) printf("[AcromagFPGA::SetDirCtrlReg]: Setting data flow direction for pins according to bit pattern 0x%08x... \n",bit_pattern); 
   int ret_code      = vme_A16D16_read(p,my_addr,&data16);
   if(gIsDebug && gVerbosity>=3) Print("SetDirCtrlReg","Reading...",my_addr,data16,ret_code);
   ret_code          = vme_A16D16_write(p,my_addr,bit_pattern);
   if(gIsDebug && gVerbosity>=3) Print("SetDirCtrlReg","Writing...",my_addr,bit_pattern,ret_code);
   ret_code          = vme_A16D16_read(p,my_addr,&data16);
   if(gIsDebug && gVerbosity>=3) Print("SetDirCtrlReg","Reading...",my_addr,data16,ret_code);
   CheckStatus(my_addr,data16,ret_code); 
}
//______________________________________________________________________________
void SetIOBits(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t bit_pattern){
   // set I/O bits  
   u_int16_t data16 = (u_int16_t)bit_pattern;           
   u_int16_t base  = carrier_addr + daughter_addr; 
   // printf("carrier = 0x%08x "
   u_int16_t start = base + 0x2; 
   u_int16_t end   = start + 0x4;  
   int i=0,ret_code=0; 
   u_int16_t i_addr;
   for (i=start;i<end;i++){
      i_addr   = i; 
      ret_code =  vme_A16D16_write(p,i_addr,bit_pattern);
      if(gIsDebug) Print("SetIOBits","Writing...",i_addr,data16,ret_code);
   }
   // CheckStatus(ret_code); 
}
//______________________________________________________________________________
void SetIOBitsAlt(int p,u_int16_t carrier_addr,u_int16_t fpga_addr,u_int16_t daughter_segment,u_int16_t bit_pattern){
   // set I/O bits for a specific group of I/O pins  
   // daughter_segment => group of I/O pins
   u_int16_t data16;           
   u_int16_t addr    = carrier_addr + fpga_addr + daughter_segment;  
   int ret_code      = vme_A16D16_read(p,addr,&data16);
   if(gIsDebug && gVerbosity>=3) Print("SetIOBitsAlt","Reading...",addr,data16,ret_code);
   ret_code          = vme_A16D16_write(p,addr,bit_pattern);
   if(gIsDebug && gVerbosity>=3) Print("SetIOBitsAlt","Writing...",addr,bit_pattern,ret_code);
   ret_code          = vme_A16D16_read(p,addr,&data16);
   if(gIsDebug && gVerbosity>=3) Print("SetIOBitsAlt","Reading...",addr,data16,ret_code);
   CheckStatus(addr,data16,ret_code);
}
//______________________________________________________________________________
void CheckStatus(u_int16_t addr,u_int16_t data,int code){
   if(code != 0x0){
      printf("[AcromagFPGA::CheckStatus]: Operation failed! \n"); 
      printf("[AcromagFPGA::CheckStatus]: addr = 0x%08x  data = 0x%04x  return code = 0x%08x \n",addr,data,code); 
      printf("[AcromagFPGA::CheckStatus]: Exiting... \n"); 
      exit(1); 
   }
   // if(gIsDebug) printf("--------------------------------------------------------------------\n"); 
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
int IsFPGATimingSet(int p,u_int16_t carrier_addr,u_int16_t daughter_addr){

   u_int16_t data=0;
   u_int16_t addr = carrier_addr + daughter_addr + gDigitizerAddr;  

   if(gIsDebug && gVerbosity>=1) printf("[AcromagFPGA]: Asking the FPGA if the timing of the signals is set... \n"); 
   data = ReadFPGAMemory(p,carrier_addr,daughter_addr,gDigitizerAddr);
   // printf("[AcromagFPGA]: addr = 0x%04x data = 0x%04x \n",carrier_addr+daughter_addr+gDigitizerAddr,data);
   if(data==0x0){
      if(gIsDebug && gVerbosity>=1) printf("[AcromagFPGA]: FPGA is not ready. \n"); 
   }else if(data==0x1){
      if(gIsDebug && gVerbosity>=1) printf("[AcromagFPGA]: FPGA is ready. \n"); 
   }else{
      printf("[AcromagFPGA]: Cannot determine FPGA readiness!   \n");
      printf("[AcromagFPGA]: addr = 0x%04x  \n",addr); 
      printf("[AcromagFPGA]: data = 0x%04x: \n",data);
      PrintBits16(data);
   }

   return data; 

}
//______________________________________________________________________________
int IsReturnGateClosed(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t *fpga_data){

   // check to see if the circuit is set to receive mode
   // receive mode means that the path from the NMR probe to 
   // the amplifier is connected; this is connected by rf_sw_3 
   // in the input file; this RF switch is closed usually for 
   // tens of milliseconds.  

   u_int16_t data=0; 
   u_int16_t full_addr = carrier_addr + daughter_addr + gDigitizerAddr2; 

   data = ReadFPGAMemory(p,carrier_addr,daughter_addr,gDigitizerAddr2);

   *fpga_data = data; 

   // gDigitizerAddr2 is tied to the first certain bits of DIO in the FPGA code; 
   // DIO contains the gate logic that we programmed onto the board via 
   // SRAM.  Therefore, we want the value that corresponds to rf_sw_3.  
   // Do bitwise operations on the variable data:

   // WARNING: In bit operations, we start from ZERO, not 1! 

   // int myBit=0; 
   // int i=0;
   // for(i=15;i>=0;i--){
   //    myBit = GetBit(i,data);
   //    printf("%d ",myBit); 
   // }
   // printf("\n"); 

   // old bit definitions 
   // int io_dm_bit          = GetBit(0 ,data); 
   // int mech_sw_bit        = GetBit(1 ,data); 
   // int global_on_off_bit  = GetBit(2 ,data); 
   // int mech_sw_on_off_bit = GetBit(3 ,data); 
   // int timing_bit         = GetBit(4 ,data);  
   // int rf_sw_3_bit        = GetBit(5 ,data);  
   // int mech_sw_1_bit      = GetBit(7 ,data); 
   // int mech_sw_2_bit      = GetBit(8 ,data); 
   // int mech_sw_3_bit      = GetBit(9 ,data); 
   // int mech_sw_4_bit      = GetBit(10,data);
   // new bit definitions (2/22/16)  
   int rf_sw_3_bit        = GetBit(6 ,data);  

   // printf("data = %d bit of interest = %d \n",data,the_bit);

   // if(gIsDebug && gVerbosity >=5){ 
   //    printf("[AcromagFPGA]: addr = 0x%04x data = 0x%04x bits: IO_DM(1) = %d mech_sw_value = %d global_on_off = %d mech_sw_on_off = %d is_ready = %d rf_sw_3_value = %d \n",
   //          full_addr,data,io_dm_bit,mech_sw_bit,global_on_off_bit,mech_sw_on_off_bit,timing_bit,rf_sw_3_bit);
   // }

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
int IsReturnGateClosedNew(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t *fpga_data){

   // check to see if the circuit is set to receive mode
   // receive mode means that the path from the NMR probe to 
   // the amplifier is connected; this is connected by rf_sw_3 
   // in the input file; this RF switch is closed usually for 
   // tens of milliseconds.  

   u_int16_t my_flags   = 0x0; 
   u_int16_t fpga_io_sp = IP_B_IO_SPACE_ADDR; 

   // my_flags = ReadMemoryDataReg(p,carrier_addr,fpga_io_sp,FLAG_ADDR);
   my_flags = ReadFPGAMemory(p,carrier_addr,fpga_io_sp,NEW_FLAG_ADDR);

   if(gIsDebug && gVerbosity>=2){
      printf("[AcromagFPGA]: The programmed bit pattern was: 0x%04x \n",my_flags);
      PrintBits16(my_flags); 
   } 

   u_int16_t data=0; 
   u_int16_t full_addr = carrier_addr + daughter_addr + gDigitizerAddr2; 

   data = ReadFPGAMemory(p,carrier_addr,daughter_addr,gDigitizerAddr2);

   *fpga_data = data; 

   // gDigitizerAddr2 is tied to the first certain bits of DIO in the FPGA code; 
   // DIO contains the gate logic that we programmed onto the board via 
   // SRAM.  Therefore, we want the value that corresponds to rf_sw_3.  
   // Do bitwise operations on the variable data:

   int i=0;
   int *Bit = (int *)malloc( sizeof(int)*mlMAX ); 
   for(i=0;i<mlMAX;i++) Bit[i] = 0; 

   // WARNING: In bit operations, we start from ZERO, not 1! 
   for(i=0;i<mlMAX;i++){
      Bit[i] = GetBit(i,data);
   }

   if(gIsDebug && gVerbosity>=3){ 
      printf("[AcromagFPGA]: addr = 0x%04x data = 0x%04x \n",full_addr,data); 
      printf("bits: \n"); 
      for(i=mlMAX-1;i>=0;i--){
	 printf("%d ",Bit[i]);
      }
      printf("\n"); 
   }

   // bit definitions  
   int mech_sw_state  = Bit[0];  
   int rf_trans_state = Bit[1];  
   int rf_rec_state   = Bit[2];  
   int tomco_state    = Bit[3]; 
   int mech_sw_1_en   = Bit[4];   
   int mech_sw_2_en   = Bit[5];   
   int mech_sw_3_en   = Bit[6];   
   int mech_sw_4_en   = Bit[7];   
   int rf_trans_en    = Bit[8];   
   int rf_rec_en      = Bit[9];   
   int tomco_en       = Bit[10];   
   int is_ready       = Bit[11];  
   int global_on_off  = Bit[12];  

   if(gIsDebug && gVerbosity>=3){
      printf("[AcromagFPGA::IsReturnGateClosedNew]: Flags: \n"); 
      printf("mech-sw-1: %d  mech-sw-2: %d   mech-sw-3: %d   mech-sw-4: %d \n",mech_sw_1_en,mech_sw_2_en,mech_sw_3_en,mech_sw_4_en); 
      printf("mech_sw_state:   %d   \n",mech_sw_state); 
      printf("tomco_enable:    %d   tomco_state:    %d \n",tomco_en,tomco_state); 
      printf("rf_trans_enable: %d   rf_trans_state: %d \n",rf_trans_en,rf_trans_state); 
      printf("rf_rec_enable:   %d   rf_rec_state:   %d \n",rf_rec_en,rf_rec_state); 
      printf("global_on_off:   %d   is_ready:       %d \n",global_on_off,is_ready); 
   } 

   // now check to see if we found a receive gate
   int ret_val=0; 
   if( rf_rec_state==1 && rf_rec_en==1 && is_ready==1 ) ret_val = 1; 

   if(ret_val==0x0){
      // printf("[AcromagFPGA]: FPGA is not ready. \n"); 
      // return 0;
   }else if(ret_val==0x1){
      // printf("[AcromagFPGA]: FPGA is ready. \n"); 
      // return 1;
   }else{
      printf("[AcromagFPGA]: Cannot determine return gate state!   \n");
      printf("[AcromagFPGA]: addr = 0x%04x data = 0x%04x \n",full_addr,data);
      // data = -1; 
   }

   // free allocated memory 
   free(Bit); 

   return ret_val; 

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
void PrintSummaryNew(int Switch,const struct fpgaPulseSequence myPulseSequence){

   printf("[AcromagFPGA]: The following commands have been processed: \n");
 
   int is=0; 
   int mech_sw_id=0;
 
   int N = myPulseSequence.fNSequences;
   int i=0; 
   for(i=0;i<N;i++){
      mech_sw_id = myPulseSequence.fMechSwID[i];
      if(Switch==mech_sw_id) is = i; 
   }
 
   double ClockFreq = FPGA_CLOCK_FREQ;

   int mech_sw_start_cnt  = myPulseSequence.fMechSwStartTimeLo[is]  + pow(2,16)*myPulseSequence.fMechSwStartTimeHi[is]; 
   int mech_sw_end_cnt    = myPulseSequence.fMechSwEndTimeLo[is]    + pow(2,16)*myPulseSequence.fMechSwEndTimeHi[is]; 
   int rf_trans_start_cnt = myPulseSequence.fRFTransStartTimeLo[is] + pow(2,16)*myPulseSequence.fRFTransStartTimeHi[is]; 
   int rf_trans_end_cnt   = myPulseSequence.fRFTransEndTimeLo[is]   + pow(2,16)*myPulseSequence.fRFTransEndTimeHi[is]; 
   int rf_rec_start_cnt   = myPulseSequence.fRFRecStartTimeLo[is]   + pow(2,16)*myPulseSequence.fRFRecStartTimeHi[is]; 
   int rf_rec_end_cnt     = myPulseSequence.fRFRecEndTimeLo[is]     + pow(2,16)*myPulseSequence.fRFRecEndTimeHi[is]; 
   int tomco_start_cnt    = myPulseSequence.fTomcoStartTimeLo[is]   + pow(2,16)*myPulseSequence.fTomcoStartTimeHi[is]; 
   int tomco_end_cnt      = myPulseSequence.fTomcoEndTimeLo[is]     + pow(2,16)*myPulseSequence.fTomcoEndTimeHi[is]; 

   double mech_sw_start  = GetTimeInUnits(mech_sw_start_cnt ,ClockFreq,myPulseSequence.fMechSwUnits[is]);  
   double mech_sw_end    = GetTimeInUnits(mech_sw_end_cnt   ,ClockFreq,myPulseSequence.fMechSwUnits[is]);  
   double rf_trans_start = GetTimeInUnits(rf_trans_start_cnt,ClockFreq,myPulseSequence.fRFTransUnits[is]);  
   double rf_trans_end   = GetTimeInUnits(rf_trans_end_cnt  ,ClockFreq,myPulseSequence.fRFTransUnits[is]);  
   double rf_rec_start   = GetTimeInUnits(rf_rec_start_cnt  ,ClockFreq,myPulseSequence.fRFRecUnits[is]);  
   double rf_rec_end     = GetTimeInUnits(rf_rec_end_cnt    ,ClockFreq,myPulseSequence.fRFRecUnits[is]);  
   double tomco_start    = GetTimeInUnits(tomco_start_cnt   ,ClockFreq,myPulseSequence.fTomcoUnits[is]);  
   double tomco_end      = GetTimeInUnits(tomco_end_cnt     ,ClockFreq,myPulseSequence.fTomcoUnits[is]);  

   printf("[AcromagFPGA]: Switch = S%d \n",myPulseSequence.fMechSwID[is]);
   printf("               mech sw:  offset = %5.3lf duration = %5.3lf %-2s \n",mech_sw_start ,mech_sw_end-mech_sw_start  ,myPulseSequence.fMechSwUnits[is]); 
   printf("               rf trans: offset = %5.3lf duration = %5.3lf %-2s \n",rf_trans_start,rf_trans_end-rf_trans_start,myPulseSequence.fRFTransUnits[is]); 
   printf("               rf rec:   offset = %5.3lf duration = %5.3lf %-2s \n",rf_rec_start  ,rf_rec_end-rf_rec_start    ,myPulseSequence.fRFRecUnits[is]); 
   printf("               tomco:    offset = %5.3lf duration = %5.3lf %-2s \n",tomco_start   ,tomco_end-tomco_start      ,myPulseSequence.fTomcoUnits[is]); 

}
//______________________________________________________________________________
void Print(char *function,char *daughter_type,u_int16_t addr,u_int16_t data,int code){
   printf("[AcromagFPGA::%s]: %s addr = 0x%08x  data = 0x%04x  return code = 0x%08x \n",function,daughter_type,addr,data,code); 
}

