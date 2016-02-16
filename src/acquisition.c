#include "acquisition.h"
//______________________________________________________________________________
int AcquireData(int p,struct fpga myFPGA,struct adc myADC,unsigned long **timestamp,char *output_dir,int *MECH){

   int ret_code = 0; 

   int adcID = myADC.fID; 
   if(adcID==3302) ret_code = AcquireDataSIS3302(p,myFPGA,myADC,timestamp,output_dir,MECH); 
   if(adcID==3316) ret_code = AcquireDataSIS3316(p,myFPGA,myADC,timestamp,output_dir,MECH); 

   return ret_code; 

}
//______________________________________________________________________________
int AcquireDataSIS3302(int p,struct fpga myFPGA,struct adc myADC,unsigned long **timestamp,char *output_dir,int *MECH){

   int MAX             = PULSES_PER_READ;        // number of pulses to acquire before printing to file
   double clock_freq   = myADC.fClockFrequency; 
   double sig_len      = myADC.fSignalLength; 
   int NEvents         = myADC.fNumberOfEvents; 
   int NSample         = sig_len*clock_freq;     // total number of samples 
   int fpga_state_flag = -2;
   int counter         =  0;
   int counter_total   =  0;
   int counter_offset  =  0;
   int fpga_data       =  0;
   const int NDATA     =  6;
   int i               =  0;
   unsigned long timeinfo[NDATA]; 
   for(i=0;i<NDATA;i++){
      timeinfo[i]    = 0.; 
   } 

   // get recieve gate time 
   int ReceiveGateTime = (int)( RECEIVE_GATE_TIME_SEC*1E+6 );   // time in microseconds 

   int sleep_time = 1;  // 1 us 
   if(ADC_MULTIEVENT_STATE==1) sleep_time = ReceiveGateTime; 
   if(gIsDebug) printf("[NMRDAQ]: Sleep time = %d us \n",sleep_time);

   printf("[NMRDAQ]: Will record %d samples for %d events... \n",NSample,NEvents); 

   int offset=0;
   int NumPulsesToWrite=0; 

   do{
      fpga_state_flag = IsReturnGateClosed(p,myFPGA.fCarrierAddr,myFPGA.fIOSpaceAddr,&fpga_data); 
      if( fpga_state_flag==1 ){  // RF receive gate  
         counter++; 
         counter_total++; 
         // GetDateAndTime(counter,timeinfo); 
         GetTimeStamp_usec(timeinfo); 
         // printf("[NMRDAQ]: Event %d found at %d:%d:%d!  Recording... \n",counter,timeinfo[3],timeinfo[4],timeinfo[5]); 
         printf("[NMRDAQ]: Event %d found!  Recording... \n",counter_total); 
         for(i=0;i<NDATA;i++) timestamp[counter_total-1][i] = timeinfo[i]; 
         SIS3302SampleNMRPulse(p);
         if(ADC_MULTIEVENT_STATE==0){
            // SIS3302WriteNMRPulse(p,counter_total,myADC,output_dir);       // if we're in single-event mode 
            // SIS3302WriteNMRPulseAlt(p,counter_total,myADC,output_dir);    // if we're in single-event mode 
            SIS3302WriteNMRPulseBin(p,counter_total,myADC,output_dir);    // if we're in single-event mode 
         }else if(ADC_MULTIEVENT_STATE==1){
            if(  (counter==MAX) || (counter_total==NEvents) ){
               // offset           = MAX*(counter_total/MAX); 
               offset           = MAX*counter_offset; 
               NumPulsesToWrite = MAX; 
               if(counter_total==NEvents) NumPulsesToWrite = NEvents - offset; 
               SIS3302WriteNMRPulsesAltNew(p,offset,NumPulsesToWrite,myADC,output_dir);
               counter = 0; 
               counter_offset++; 
               SIS3302Init(p,&myADC);   // re-initialize the ADC; clears the memory 
            }
         }
         usleep(sleep_time); 
      }else{
         // no RF receive gate found, do nothing 
         // printf("[NMRDAQ]: Receive gate is OPEN -- no signal. \n"); 
         usleep(1);   // are we sure we want to delay by some amount of time? 
      } 
   }while( counter_total < NEvents ); 

   printf("[NMRDAQ]: Recorded %d of %d desired number of events. \n",counter_total,NEvents);  

   // all finished with data acquisition; clear the gDATA array  
   if(ADC_MULTIEVENT_STATE==0) free(gDATA); 

   // successful run => 0, fail => 1 
   int ret_code = 1;
   if(counter_total==NEvents) ret_code = 0;  
   return ret_code; 

}
//______________________________________________________________________________
int AcquireDataSIS3316(int p,struct fpga myFPGA,struct adc myADC,unsigned long **timestamp,char *output_dir,int *MECH){

   int ret_code        = 1;
   // int MAX             = PULSES_PER_READ;        // number of pulses to acquire before printing to file
   double clock_freq   = myADC.fClockFrequency; 
   double sig_len      = myADC.fSignalLength; 
   int NEvents         = myADC.fNumberOfEvents; 
   int NSample         = sig_len*clock_freq;     // total number of samples 
   int fpga_state_flag = -2;
   int counter         =  0;
   int mech_sw[4]      = {0,0,0,0}; 
   int fpga_data       =  0;
   const int NDATA     =  6;
   int i               =  0;
   unsigned long timeinfo[NDATA]; 
   for(i=0;i<NDATA;i++){
      timeinfo[i]    = 0.; 
   } 

   // get recieve gate time 
   int ReceiveGateTime = (int)( RECEIVE_GATE_TIME_SEC*1E+6 );   // time in microseconds 
   int sleep_time      = ReceiveGateTime; 
   if(gIsDebug) printf("[NMRDAQ]: Sleep time = %d us \n",sleep_time);

   printf("[NMRDAQ]: Will record %d samples for %d events... \n",NSample,NEvents); 

   int armed_bank_flag = 0;  

   do{
      fpga_state_flag = IsReturnGateClosed(p,myFPGA.fCarrierAddr,myFPGA.fIOSpaceAddr,&fpga_data); 
      mech_sw[0] = GetBit(7, fpga_data); 
      mech_sw[1] = GetBit(8 ,fpga_data); 
      mech_sw[2] = GetBit(9 ,fpga_data); 
      mech_sw[3] = GetBit(10,fpga_data);
      printf("[NMRDAQ]: Mechanical switches: sw-1: %d sw-2: %d sw-3: %d sw-4: %d \n",mech_sw[0],mech_sw[1],mech_sw[2],mech_sw[3]);  
      if( fpga_state_flag==1 ){  // RF receive gate  
	 if(mech_sw[0]!=0) MECH[counter] = 1;  
	 if(mech_sw[1]!=0) MECH[counter] = 2;  
	 if(mech_sw[2]!=0) MECH[counter] = 3;  
	 if(mech_sw[3]!=0) MECH[counter] = 4;  
         counter++; 
	 // GetDateAndTime(counter,timeinfo); 
         GetTimeStamp_usec(timeinfo); 
         // printf("[NMRDAQ]: Event %d found at %d:%d:%d!  Recording... \n",counter,timeinfo[3],timeinfo[4],timeinfo[5]); 
         printf("[NMRDAQ]: Event %d found on mech-sw %d!  Recording... \n",counter,MECH[counter]); 
         for(i=0;i<NDATA;i++) timestamp[counter-1][i] = timeinfo[i]; 
         ret_code = SIS3316SampleData(p,myADC,output_dir,counter,&armed_bank_flag);  // note that data is printed to file in here! 
         // printf("armed_bank_flag = %d \n",armed_bank_flag);
         if(ret_code==-97) counter--;  // no data found, decrease counter by 1 
         usleep(sleep_time); 
         printf("---------------------------------------------- \n");
      }else{
         // no RF receive gate found, do nothing 
         // printf("[NMRDAQ]: Receive gate is OPEN -- no signal. \n"); 
         usleep(1);   // are we sure we want to delay by some amount of time? 
      } 
   }while( counter < NEvents ); 

   printf("[NMRDAQ]: Recorded %d of %d desired number of events. \n",counter,NEvents);  

   // successful run => 0, fail => 1 
   ret_code = 1; // assume fail to start  
   if(counter==NEvents) ret_code = 0;  
   return ret_code; 

}
//______________________________________________________________________________
void NextAction(int p,struct FuncGen *myFuncGen,struct fpga *myFPGA){

   const int SIZE = 1; 
   char user_response[SIZE];
   char *quit = "q";  
 
   do{
      printf("[NMRDAQ]: The DAQ is running.  Choose the next action: \n"); 
      // printf("1 = Change the frequency on the SG-382 \n");   // TODO: add this function... others?  
      printf("[NMRDAQ]: q = Quit \n");
      printf("[NMRDAQ]: Choice: "); 
      scanf("%s",user_response); 
      if( AreEquivStrings(user_response,quit) ){
         ShutDownSystem(p,myFuncGen,myFPGA); 
         break; 
      }

   }while( !AreEquivStrings(user_response,quit) );
 
}
//______________________________________________________________________________
void ShutDownSystem(int p,struct FuncGen *myFuncGen,struct fpga *myFPGA){

   printf("[NMRDAQ]: Shutting down the system... \n"); 
   BlankFPGA(p,myFPGA);
   BlankFuncGen(myFuncGen); 
   printf("[NMRDAQ]: Done. \n");  

}

