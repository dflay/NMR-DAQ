#include "acquisition.h"
//______________________________________________________________________________
int AcquireDataNew(int p,
                   struct fpgaPulseSequence myPulseSequence,
                   struct FuncGen *myFuncGenPi2,
                   struct adc *myADC,
                   keithley_t *myKeithley,
                   event_t *myEvent,
                   char *output_dir){

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
   
   int MECH=0; 

   // need the armed_bank_flag here; if we delcare it in the 
   // SIS3316AcquireDataNew function, we'll keep looking at the 
   // same bank over and over!  
   int armed_bank_flag = 0;  
   int *abfPtr         = &armed_bank_flag;  

   double dt=0,dt_acq=0; 

   unsigned long long timeStamp=0; 

   unsigned long timeStart=0;
   unsigned long timePoll=0,timePoll_acq=0;
   unsigned long timePoll_adc_1=0,timePoll_adc_2=0; 

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

   double resistance = 0;  // for temperature measurements 
 
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
      timeStart = get_sys_time_us(); 
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
      timePoll = get_sys_time_us();  
      dt = (double)( timePoll-timeStart ); 
      if(gIsDebug && gVerbosity>=1) printf("The required time delay is:     %d us    \n",delay_desired); 
      if(gIsDebug && gVerbosity>=1) printf("The additional time delay is:   %d us    \n",delay_addl); 
      if(gIsDebug && gVerbosity>=1) printf("The total time delay is:        %d us    \n",delay_tot); 
      if(gIsDebug && gVerbosity>=1) printf("Elapsed time (before ADC init): %.3lf us \n",dt); 
      // re-initialize the ADC [signal length (number of samples) has changed]  
      ReconfigADCStruct(rf_rec_pulse,myADC);
      SISReInit(p,myADC,i); 
      // program the FPGA for a given mechanical switch and corresponding timing sequence   
      rc_fpga = ProgramSignalsToFPGANew(p,SwList[i],myPulseSequence);
      timePoll = get_sys_time_us();  
      dt = (double)( timePoll-timeStart ); 
      if(gIsDebug && gVerbosity>=1) printf("Elapsed time (after FPGA programming): %.3lf ms \n",dt); 
      if(rc_fpga==1){ 
	 // record data on the ADC
         if(gIsDebug && gVerbosity>=1) printf("[NMRDAQ]: Trying to record data with the ADC... \n"); 
         timePoll_adc_1 = get_sys_time_us();  
	 if(adcID==3316) AcquireDataSIS3316New(p,i+1,myPulseSequence,*myADC,timeStamp,MECH,output_dir,abfPtr);
         timePoll_adc_2 = get_sys_time_us();  
         dt = (double)( timePoll_adc_2-timePoll_adc_1 ); 
         // printf("ADC elapsed time: %.3lf ms \n",dt); 
         // get new timestamp (in nanoseconds)  
         // timestamp_ns[i] = get_sys_time_us()*1E+3; 
         // get the temperature 
         if(myKeithley->enable==1){
            resistance = keithley_interface_get_resistance(myKeithley->portNo);
         }else{
            resistance = 0.;
         }
         // accumulate data into event object 
         myEvent[i].timestamp   = timeStamp; 
         myEvent[i].temperature = resistance; 
         myEvent[i].chNum       = MECH; 
         myEvent[i].pulseNum    = i; 
      }else{
	 rc = 1;
	 break;
      }
      // compute the time it took to get to this point 
      timePoll_acq = get_sys_time_us(); 
      dt_acq = (double)( timePoll_acq-timeStart ); // dt_acq is in us
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
      timePoll = get_sys_time_us();  
      dt = (double)( timePoll-timeStart ); 
      // if(gIsDebug && gVerbosity>=1) printf("Elapsed time: %.3lf ms \n",dt); 
      // printf("End of event %3d; Elapsed time: %.3lf ms (delay = %d us) \n",i+1,dt,delay_tot); 
      if(gIsDebug && gVerbosity>=1) printf("[NMRDAQ]: ------------------------------ End of Event ------------------------------ \n"); 
      delay_prev = delay_tot; 
   }

   // kill compiler warnings 
   delay +=0;

   printf("[NMRDAQ]: Done. \n"); 

   free(SwList);

   return rc; 

}
//______________________________________________________________________________
int AcquireDataSIS3316New(int p,int i,
                          struct fpgaPulseSequence myPulseSequence,
                          struct adc myADC,
                          unsigned long long &timestamp,int &MECH,char *output_dir,int *armed_bank_flag){

   // NOTE: i = ith event 
 
   int j=0; 
   int ret_code = 1;  // assume fail to start  

   u_int16_t fpga_data = 0x0; 

   int rf_rec_gate = 0;

   int *Bit = (int *)malloc( sizeof(int)*mlMAX );
   for(j=0;j<mlMAX;j++) Bit[j] = 0;  

   int *mech_sw = (int *)malloc( sizeof(int)*4 ); 
   for(j=0;j<4;j++) mech_sw[j] = 0;  

   // const int NDATA = 6; 
   // unsigned long *timeinfo = (unsigned long *)malloc( sizeof(unsigned long)*NDATA ); 
   // for(j=0;j<NDATA;j++){
   //    timeinfo[j] = 0.; 
   // } 

   do{ 
      // GetTimeStamp_usec(time1); 
      rf_rec_gate = IsReturnGateClosedNew(p,myPulseSequence.fCarrierAddr,myPulseSequence.fIOSpaceAddr,&fpga_data); 
      if( rf_rec_gate==1 ){  // RF receive gate is closed  
         if(gIsDebug && gVerbosity>=2) printf("[acquisition]: RF Rec. Gate is HIGH \n"); 
	 // get time stamp 
	 // GetTimeStamp_usec(timeinfo);
         timestamp = get_sys_time_us()*1E+3;  // UTC time in ns 
         // read the ADC  
	 ret_code = SIS3316SampleData(p,myADC,output_dir,i,armed_bank_flag);            // note that data is printed to file in here! 
         if(gIsDebug && gVerbosity>=2) printf("[acquisition]: bank1 armed flag = %d \n",*armed_bank_flag);
	 // for(j=0;j<NDATA;j++) timestamp[i-1][j] = timeinfo[j];                 // finish timestamp stuff 
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
	 if(mech_sw[0]!=0) MECH = 1;  // if we start at pulse 1, we want array index 0. 
	 if(mech_sw[1]!=0) MECH = 2;  // if we start at pulse 1, we want array index 0. 
	 if(mech_sw[2]!=0) MECH = 3;  // if we start at pulse 1, we want array index 0. 
	 if(mech_sw[3]!=0) MECH = 4;  // if we start at pulse 1, we want array index 0. 
	 if(gIsDebug && gVerbosity>=2) printf("[NMRDAQ]: Event %d found on mech-sw %d!  Recording... \n",i,MECH); 
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
   // free(timeinfo);

   // successful run => 0, fail => 1 
   return ret_code; 

}
// //______________________________________________________________________________
// void NextAction(int p,struct FuncGen *myFuncGen,struct fpga *myFPGA){
// 
//    const int SIZE = 1; 
//    char user_response[SIZE];
//    char quit[10];
//    sprintf(quit,"%s","q"); 
//    // char *quit = "q";  
//  
//    do{
//       printf("[NMRDAQ]: The DAQ is running.  Choose the next action: \n"); 
//       // printf("1 = Change the frequency on the SG-382 \n");   // TODO: add this function... others?  
//       printf("[NMRDAQ]: q = Quit \n");
//       printf("[NMRDAQ]: Choice: "); 
//       scanf("%s",user_response); 
//       if( AreEquivStrings(user_response,quit) ){
//          ShutDownSystem(p,myFuncGen,myFPGA); 
//          break; 
//       }
// 
//    }while( !AreEquivStrings(user_response,quit) );
//  
// }
//______________________________________________________________________________
int ShutDownSystemNew(int p,
                      struct FuncGen *myFuncGen,struct FuncGen *myFuncGenPi2,
                      struct fpgaPulseSequence *myPulseSequence,
                      keithley_t *myKeithley){
   printf("[NMRDAQ]: Disarming the system... \n"); 
   BlankFPGANew(p,myPulseSequence);
   BlankFuncGen(constants_t::SG382_LO_DEV_PATH.c_str() ,myFuncGen); 
   BlankFuncGen(constants_t::SG382_PI2_DEV_PATH.c_str(),&myFuncGenPi2[0]); 
   keithley_interface_close_connection(myKeithley->portNo); 
   printf("[NMRDAQ]: Done. \n");  

   return 0;

}
