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

   if(gVerbosity>1) printf("[NMRDAQ]: Will record %d samples for %d events... \n",NSample,NEvents); 
   printf("[NMRDAQ]: Acquiring data... \n");  

   int offset=0;
   int NumPulsesToWrite=0;
   u_int16_t fpga_data = 0x0000;  

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
   const int NDATA     =  6;
   int i               =  0;
   unsigned long timeinfo[NDATA]; 
   for(i=0;i<NDATA;i++){
      timeinfo[i]    = 0.; 
   } 

   u_int16_t hex_flag = 0;
   int myBit          = 0;

   // get recieve gate time 
   int ReceiveGateTime = (int)( RECEIVE_GATE_TIME_SEC*1E+6 );   // time in microseconds 
   int sleep_time      = ReceiveGateTime; 
   if(gIsDebug) printf("[NMRDAQ]: Sleep time = %d us \n",sleep_time);

   if(gVerbosity>1) printf("[NMRDAQ]: Will record %d samples for %d events... \n",NSample,NEvents);
   printf("[NMRDAQ]: Acquiring data... \n");  

   int armed_bank_flag = 0;  
   u_int16_t fpga_data = 0x0000;  

   do{
      fpga_state_flag = IsReturnGateClosed(p,myFPGA.fCarrierAddr,myFPGA.fIOSpaceAddr,&fpga_data); 
      if( fpga_state_flag==1 ){  // RF receive gate  
         mech_sw[0] = GetBit(0 , fpga_data); 
         mech_sw[1] = GetBit(1 ,fpga_data); 
         mech_sw[2] = GetBit(2 ,fpga_data); 
         mech_sw[3] = GetBit(3 ,fpga_data);
         if(gVerbosity>1){
            hex_flag = (u_int16_t)fpga_data;
            printf("FPGA flag bit pattern: hex: 0x%04x MSB--LSB: [",hex_flag);
            for(i=15;i>=0;i--){
               myBit = GetBit(i,fpga_data); 
               printf("%d ",myBit);
            }
            printf("] \n"); 
            printf("[NMRDAQ]: Mechanical switches: sw-1: %d sw-2: %d sw-3: %d sw-4: %d \n",mech_sw[0],mech_sw[1],mech_sw[2],mech_sw[3]);  
         }
	 if(mech_sw[0]!=0) MECH[counter] = 1;  
	 if(mech_sw[1]!=0) MECH[counter] = 2;  
	 if(mech_sw[2]!=0) MECH[counter] = 3;  
	 if(mech_sw[3]!=0) MECH[counter] = 4;  
         counter++; 
	 // GetDateAndTime(counter,timeinfo); 
         GetTimeStamp_usec(timeinfo); 
         // printf("[NMRDAQ]: Event %d found at %d:%d:%d!  Recording... \n",counter,timeinfo[3],timeinfo[4],timeinfo[5]); 
         if(gVerbosity>1) printf("[NMRDAQ]: Event %d found on mech-sw %d!  Recording... \n",counter,MECH[counter-1]); 
         for(i=0;i<NDATA;i++) timestamp[counter-1][i] = timeinfo[i]; 
         ret_code = SIS3316SampleData(p,myADC,output_dir,counter,&armed_bank_flag);  // note that data is printed to file in here! 
         // printf("armed_bank_flag = %d \n",armed_bank_flag);
         if(ret_code==-97) counter--;  // no data found, decrease counter by 1 
         usleep(sleep_time); 
         if(gVerbosity>1) printf("---------------------------------------------- \n");
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
int AcquireDataNew(int p,
                   struct fpgaPulseSequence myPulseSequence,
                   struct FuncGen *myFuncGenPi2,
                   struct adc *myADC,
                   keithley_t *myKeithley,
                   double *resistance,
                   unsigned long **timestamp,unsigned long long *timestamp_ns,char *output_dir,int *MECH){

   printf("[NMRDAQ]: Acquiring data... \n"); 

   // We may do this later... 
   // program the pi/2 SG382: FREQUENCY ONLY [consider this fixed for now] 
   // we may move this over to the event loop eventually 
   // sleep time of 10E3 us = 10 ms 
   // int rc_fg=0;       
   // rc_fg = ProgramFuncGen(SG382_ENABLE_FREQ_ONLY,SG382_PI2_DEV_PATH,myFuncGenPi2[0],10000); 

   int rc=0,rc_fpga=0;  
   int adcID    = myADC->fID;
   int NEvents  = myADC->fNumberOfEvents; 

   int i=0;  
   int *SwList  = (int *)malloc( sizeof(int)*NEvents );
   for(i=0;i<NEvents;i++){
      SwList[i] = 0;
   } 

   // need the armed_bank_flag here; if we delcare it in the 
   // SIS3316AcquireDataNew function, we'll keep looking at the 
   // same bank over and over!  
   int armed_bank_flag = 0;  
   int *abfPtr         = &armed_bank_flag;  

   double dt=0,dt_acq=0; 
   unsigned long *timeStart      = (unsigned long *)malloc( sizeof(unsigned long)*6 );  
   unsigned long *timePoll       = (unsigned long *)malloc( sizeof(unsigned long)*6 );  
   unsigned long *timePoll_acq   = (unsigned long *)malloc( sizeof(unsigned long)*6 );  
   unsigned long *timePoll_adc_1 = (unsigned long *)malloc( sizeof(unsigned long)*6 );  
   unsigned long *timePoll_adc_2 = (unsigned long *)malloc( sizeof(unsigned long)*6 );  

   for(i=0;i<6;i++) timeStart[i]      = 0; 
   for(i=0;i<6;i++) timePoll[i]       = 0; 
   for(i=0;i<6;i++) timePoll_acq[i]   = 0; 
   for(i=0;i<6;i++) timePoll_adc_1[i] = 0; 
   for(i=0;i<6;i++) timePoll_adc_2[i] = 0; 

   // get array of mechanical switches of dimension NEvents 
   // for example: if we have S1 and S2 activated, the order 
   // will be 1, 2, 1, 2,... for i = 1,...,NEvents.  
   GetMechSwitchList(myPulseSequence,NEvents,SwList);  

   int isw=0;
   int rf_rec_start_cnt=0,rf_rec_end_cnt=0,rf_rec_pulse_cnt=0; 
   int mech_sw_end_cnt=0; 
   double rf_rec_pulse=0,rf_rec_end=0,mech_sw_end=0; 
   double ClockFreq = FPGA_CLOCK_FREQ; 

   int delay=0;             // time delay to let mechanical switch close [us]  
   int delay_addl = 1E+6; // 25000;  // additional delay of 25 ms to give the mechanical switch a chance to recover [us] 
   // int long_delay = 1E+6;   // 2 seconds (in us)  
   int delay_desired = (int)( gDelayTime*1.0E+6 );   // delay time (in us) between pulses; gDelayTime is in seconds  
   int delay_prev=0; 
   int delay_tot=0;  
   double delay_sec=0;
   double delay_usec=0; 

   if(gIsDebug && gVerbosity>=1) printf("[NMRDAQ]: Delay time between pulses: %.3lf s \n",gDelayTime); 

   if(gIsDebug && gVerbosity>=1) printf("[NMRDAQ]: The desired delay time is: %d us \n",delay_desired); 

   for(i=0;i<NEvents;i++){
      rc = TimingCheckNew(myPulseSequence);
      if(rc!=0){
         rc = 1;
         printf("[NMRDAQ]: ERROR!  Timing is not correct.  Exiting... \n");
         PrintFPGANew(myPulseSequence);   
         break; 
      }
      // PrintFPGANew(myPulseSequence);   
      if(gIsDebug && gVerbosity>=1) printf("[NMRDAQ]: ------------------------------ Event %d ------------------------------ \n",i+1); 
      GetTimeStamp_usec(timeStart); 
      // find the switch we want to send pulses to 
      isw              = GetMechSwitchIndex(SwList[i],myPulseSequence);
      // NOTE: We leave out programming the pi/2 function generator here (may do this later)  
      // set the pi/2 SG382 to the appropriate settings [only amplitude: output ENABLED]  
      // rc_fg            = ProgramFuncGen(SG382_ENABLE_AMPL_ONLY,SG382_PI2_DEV_PATH,myFuncGenPi2[isw],100000); 
      // if(rc_fg!=0){ 
      //    printf("[NMRDAQ]: ERROR! Cannot program pi/2 SG382!  Stopping the run...");
      //    break; 
      // } 
      // get duration of signal in clock counts  
      rf_rec_start_cnt = myPulseSequence.fRFRecStartTimeLo[isw] + pow(2,16)*myPulseSequence.fRFRecStartTimeHi[isw];  
      rf_rec_end_cnt   = myPulseSequence.fRFRecEndTimeLo[isw]   + pow(2,16)*myPulseSequence.fRFRecEndTimeHi[isw]; 
      rf_rec_pulse_cnt = rf_rec_end_cnt - rf_rec_start_cnt;
      mech_sw_end_cnt  = myPulseSequence.fMechSwEndTimeLo[isw]  + pow(2,16)*myPulseSequence.fMechSwEndTimeHi[isw]; 
      // convert to time in seconds: recall that we converted clock counts from units to seconds upon importing the data
      rf_rec_pulse     = GetTimeInSeconds(rf_rec_pulse_cnt,ClockFreq);     
      rf_rec_end       = GetTimeInSeconds(rf_rec_end_cnt  ,ClockFreq);     
      mech_sw_end      = GetTimeInSeconds(mech_sw_end_cnt ,ClockFreq);     
      // compute time delay to wait before starting the next pulse 
      delay_sec        = mech_sw_end - rf_rec_end;
      delay_usec       = ConvertTimeFromSecondsToUnits(delay_sec,constants_t::microsecond.c_str());
      delay            = (int)delay_usec;  
      // delay_addl       = delay_desired - delay;        // this allows for a TOTAL delay of 1 second AFTER the close of the mechanical switch 
      // delay_tot        = delay + delay_addl;  
      // delay_tot        = long_delay; 
      // delay_tot        = delay_addl - delay;  
      GetTimeStamp_usec(timePoll); 
      dt = (double)( timePoll[5]-timeStart[5] ); 
      if(gIsDebug && gVerbosity>=1) printf("The required time delay is:     %d us    \n",delay_desired); 
      if(gIsDebug && gVerbosity>=1) printf("The additional time delay is:   %d us    \n",delay_addl); 
      if(gIsDebug && gVerbosity>=1) printf("The total time delay is:        %d us    \n",delay_tot); 
      if(gIsDebug && gVerbosity>=1) printf("Elapsed time (before ADC init): %.3lf us \n",dt); 
      // re-initialize the ADC [signal length (number of samples) has changed]  
      ReconfigADCStruct(rf_rec_pulse,myADC);
      SISReInit(p,myADC,i); 
      // program the FPGA for a given mechanical switch and corresponding timing sequence   
      rc_fpga = ProgramSignalsToFPGANew(p,SwList[i],myPulseSequence);
      GetTimeStamp_usec(timePoll); 
      dt = (double)( timePoll[4]-timeStart[4] ); 
      if(gIsDebug && gVerbosity>=1) printf("Elapsed time (after FPGA programming): %.3lf ms \n",dt); 
      if(rc_fpga==1){ 
	 // record data on the ADC
         if(gIsDebug && gVerbosity>=1) printf("[NMRDAQ]: Trying to record data with the ADC... \n"); 
         GetTimeStamp_usec(timePoll_adc_1); 
	 if(adcID==3316) AcquireDataSIS3316New(p,i+1,myPulseSequence,*myADC,timestamp,output_dir,MECH,abfPtr);
         GetTimeStamp_usec(timePoll_adc_2); 
         dt = (double)( timePoll_adc_2[4]-timePoll_adc_1[4] ); 
         // printf("ADC elapsed time: %.3lf ms \n",dt); 
         // get new timestamp (in nanoseconds)  
         timestamp_ns[i] = get_sys_time_us()*1E+3; 
         // get the temperature 
         resistance[i] = 0; // keithley_interface_get_resistance(myKeithley->portNo);
      }else{
	 rc = 1;
	 break;
      }
      // compute the time it took to get to this point 
      GetTimeStamp_usec(timePoll_acq); 
      dt_acq = (double)( timePoll_acq[4]-timeStart[4] )*1E+3;// dt_acq is in ms, so we convert to us
      // now compute the delay time so that there's an equal amount of time between each pulse, regardless of pulse sequencing 
      delay_tot = delay_desired - (int)dt_acq;   
      if(delay_tot<0){
         printf("[NMRDAQ]: WARNING: Timing incorrect for pulse %d! \n",i+1); 
         printf("          desired delay:    %d us \n",delay_desired);
         printf("          acquisition time: %d us \n",(int)dt_acq);
         printf("          computed delay:   %d us \n",delay_tot);
         printf("          adjusted delay:   %d us \n",delay_prev);
         delay_tot = 0; // delay_prev;
      }  
      usleep(delay_tot);                                // give the mechanical switch a chance to close 
      GetTimeStamp_usec(timePoll); 
      dt = (double)( timePoll[4]-timeStart[4] ); 
      // if(gIsDebug && gVerbosity>=1) printf("Elapsed time: %.3lf ms \n",dt); 
      // printf("End of event %3d; Elapsed time: %.3lf ms (delay = %d us) \n",i+1,dt,delay_tot); 
      if(gIsDebug && gVerbosity>=1) printf("[NMRDAQ]: ------------------------------ End of Event ------------------------------ \n"); 
      delay_prev = delay_tot; 
   }

   // kill compiler warnings 
   delay +=0;

   printf("[NMRDAQ]: Done. \n"); 

   free(SwList); 
   free(timeStart); 
   free(timePoll); 
   free(timePoll_acq); 
   free(timePoll_adc_1); 
   free(timePoll_adc_2); 

   return rc; 

}
//______________________________________________________________________________
int AcquireDataSIS3316Test(int p,int i,struct fpgaPulseSequence myPulseSequence,struct adc myADC,
                          unsigned long **timestamp,char *output_dir,int *MECH){


   int myTest = 0; 
   int *myPtr = &myTest;

   printf("test bit before = %d \n",myTest);   
   InvertBit(myPtr);
   printf("test bit after = %d \n",myTest);   

   return 0;
 
}
//______________________________________________________________________________
int AcquireDataSIS3316New(int p,int i,
                          struct fpgaPulseSequence myPulseSequence,
                          struct adc myADC,
                          unsigned long **timestamp,char *output_dir,int *MECH,int *armed_bank_flag){

   // NOTE: i = ith event 
 
   int j=0; 
   int ret_code = 1;  // assume fail to start  

   u_int16_t fpga_data = 0x0; 

   int rf_rec_gate = 0;

   int *Bit = (int *)malloc( sizeof(int)*mlMAX );
   for(j=0;j<mlMAX;j++) Bit[j] = 0;  

   int *mech_sw = (int *)malloc( sizeof(int)*4 ); 
   for(j=0;j<4;j++) mech_sw[j] = 0;  

   const int NDATA = 6; 
   unsigned long *timeinfo = (unsigned long *)malloc( sizeof(unsigned long)*NDATA ); 
   for(j=0;j<NDATA;j++){
      timeinfo[j] = 0.; 
   } 

   do{ 
      // GetTimeStamp_usec(time1); 
      rf_rec_gate = IsReturnGateClosedNew(p,myPulseSequence.fCarrierAddr,myPulseSequence.fIOSpaceAddr,&fpga_data); 
      if( rf_rec_gate==1 ){  // RF receive gate is closed  
         if(gIsDebug && gVerbosity>=2) printf("[acquisition]: RF Rec. Gate is HIGH \n"); 
	 // get time stamp 
	 GetTimeStamp_usec(timeinfo);
         // read the ADC  
	 ret_code = SIS3316SampleData(p,myADC,output_dir,i,armed_bank_flag);            // note that data is printed to file in here! 
         if(gIsDebug && gVerbosity>=2) printf("[acquisition]: bank1 armed flag = %d \n",*armed_bank_flag);
	 for(j=0;j<NDATA;j++) timestamp[i-1][j] = timeinfo[j];                 // finish timestamp stuff 
	 for(j=0;j<mlMAX;j++){
	    Bit[j] = GetBit(j,fpga_data);
	 }
	 mech_sw[0] = Bit[0] & Bit[4]; // AND of mech_sw_state (bit 0) and mech_sw_1 enable (bit 4) 
	 mech_sw[1] = Bit[0] & Bit[5]; // AND of mech_sw_state (bit 0) and mech_sw_2 enable (bit 5) 
	 mech_sw[2] = Bit[0] & Bit[6]; // AND of mech_sw_state (bit 0) and mech_sw_3 enable (bit 6) 
	 mech_sw[3] = Bit[0] & Bit[7]; // AND of mech_sw_state (bit 0) and mech_sw_4 enable (bit 7)
	 if(gIsDebug && gVerbosity>=2){
	    printf("FPGA flag bit pattern: hex: 0x%04x MSB--LSB: ",fpga_data);
            PrintBits16(fpga_data); 
	    printf("[NMRDAQ]: Mechanical switches: sw-1: %d sw-2: %d sw-3: %d sw-4: %d \n",mech_sw[0],mech_sw[1],mech_sw[2],mech_sw[3]);  
	 }
	 if(mech_sw[0]!=0) MECH[i-1] = 1;  // if we start at pulse 1, we want array index 0. 
	 if(mech_sw[1]!=0) MECH[i-1] = 2;  // if we start at pulse 1, we want array index 0. 
	 if(mech_sw[2]!=0) MECH[i-1] = 3;  // if we start at pulse 1, we want array index 0. 
	 if(mech_sw[3]!=0) MECH[i-1] = 4;  // if we start at pulse 1, we want array index 0. 
	 if(gIsDebug && gVerbosity>=2) printf("[NMRDAQ]: Event %d found on mech-sw %d!  Recording... \n",i,MECH[i-1]); 
	 // printf("armed_bank_flag = %d \n",armed_bank_flag);
	 // if(ret_code==-97) i--;  // no data found, decrease counter by 1 
	 // usleep(sleep_time); 
	 if(gIsDebug && gVerbosity>=2) printf("---------------------------------------------- \n");
         ret_code = 0; 
      }else{
	 // no RF receive gate found, do nothing 
	 usleep(1);    
      }
   }while( rf_rec_gate==0 );

   // free allocated memory
   free(Bit); 
   free(mech_sw); 
   free(timeinfo);

   // successful run => 0, fail => 1 
   return ret_code; 

}
//______________________________________________________________________________
void NextAction(int p,struct FuncGen *myFuncGen,struct fpga *myFPGA){

   const int SIZE = 1; 
   char user_response[SIZE];
   char quit[10];
   sprintf(quit,"%s","q"); 
   // char *quit = "q";  
 
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
   BlankFuncGen(constants_t::SG382_LO_DEV_PATH.c_str(),myFuncGen); 
   printf("[NMRDAQ]: Done. \n");  
}
//______________________________________________________________________________
void ShutDownSystemNew(int p,
                       struct FuncGen *myFuncGen,struct FuncGen *myFuncGenPi2,
                       struct fpgaPulseSequence *myPulseSequence,
                       keithley_t *myKeithley){
   printf("[NMRDAQ]: Shutting down the system... \n"); 
   BlankFPGANew(p,myPulseSequence);
   BlankFuncGen(constants_t::SG382_LO_DEV_PATH.c_str() ,myFuncGen); 
   BlankFuncGen(constants_t::SG382_PI2_DEV_PATH.c_str(),&myFuncGenPi2[0]); 
   // keithley_interface_close_connection(myKeithley->portNo); 
   printf("[NMRDAQ]: Done. \n");  
}
