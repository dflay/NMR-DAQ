#include "sis3302_um.h"
//______________________________________________________________________________
int SIS3302BaseInit(int vme_handle,struct adc *myADC){

   // Initialize (or reset) the SIS3302 to NMR signal-gathering configuration
   // Note: This is separated from the general SISInit() because 
   //       we want to call this function many times in the main 
   //       part of the code to reset the ADC memory after every block read, 
   //       but don't want to waste time re-reading the input file for the ADC. 

   int rc=0;
   if(gIsDebug) printf("[SIS3302_um]: Configuring... \n"); 

   u_int32_t data32 = 0;

   printf("[SIS3302_um]: Issuing key reset... \n");
   // reset StruckADC to power-up state and clear all sampled data from memory
   rc = SISWrite32(vme_handle,SIS3302_KEY_RESET, 0x0);
   printf("[SIS3302_um]: --> Done \n");

   rc = SISMODID(vme_handle,SIS3302_MODID);

   // general configuration settings
   if(ADC_MULTIEVENT_STATE==0){
      // multi-event state disabled
      printf("[SIS3302_um]: ADC in SINGLE-EVENT mode. \n");
      data32 = SIS3302_ACQ_DISABLE_LEMO_START_STOP 
             + SIS3302_ACQ_DISABLE_AUTOSTART
             + SIS3302_ACQ_DISABLE_MULTIEVENT;
   }else if(ADC_MULTIEVENT_STATE==1){
      // multi-event state enabled
      printf("[SIS3302_um]: ADC in MULTI-EVENT mode. \n");
      data32 = SIS3302_ACQ_DISABLE_LEMO_START_STOP 
             + SIS3302_ACQ_DISABLE_AUTOSTART
             + SIS3302_ACQ_ENABLE_MULTIEVENT;
   }else{
      printf("[SIS3302_um]: ADC event mode not properly set!  Defaulting to single-event mode... \n");
      data32 = SIS3302_ACQ_DISABLE_LEMO_START_STOP 
             + SIS3302_ACQ_DISABLE_AUTOSTART
             + SIS3302_ACQ_DISABLE_MULTIEVENT;
      myADC->fMultiEventState = 0; 
   }
   printf("[SIS3302_um]: Applying settings... \n");
   rc = SISWrite32(vme_handle,SIS3302_ACQUISTION_CONTROL,data32);      
   printf("[SIS3302_um]: --> Done \n");

   int ClockType = myADC->fClockType; 
   int ClockFreq = (int)myADC->fClockFrequency;
   char *units   = myADC->fClockFreqUnits; 

   int ClockFreq_in_units=0;

   if( AreEquivStrings( units,constants_t::kHz.c_str() ) ) ClockFreq_in_units = ClockFreq/1E+3; 
   if( AreEquivStrings( units,constants_t::MHz.c_str() ) ) ClockFreq_in_units = ClockFreq/1E+6; 
   if( AreEquivStrings( units,constants_t::GHz.c_str() ) ) ClockFreq_in_units = ClockFreq/1E+9; 

   SIS3302SetClockFreq(vme_handle,ClockType,ClockFreq_in_units); 

   printf("[SIS3302_um]: Setting start/stop delays... \n");
   rc = SISWrite32(vme_handle,SIS3302_START_DELAY,0); // TODO: later use this to cut out a few of the initial noisy usec of probe signal
   rc = SISWrite32(vme_handle,SIS3302_STOP_DELAY,0);  // we almost certainly will never need a stop delay
   printf("[SIS3302_um]: --> Done \n");
   
   data32 = SIS3302_EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP; // enable automatic event stop after a certain number of samples
   rc = SISWrite32(vme_handle,SIS3302_EVENT_CONFIG_ALL_ADC,data32);      

   if(gIsDebug) printf("[SIS3302_um]: Configuration complete. \n"); 

   rc = SISWrite32(vme_handle,SIS3302_KEY_ARM,0x0);     
 
   return rc;
}
//______________________________________________________________________________
int SIS3302ReInit(int vme_handle,struct adc *myADC){

   // we set this every time we want to read from the ADC 

   // now set the number of samples recorded before each event stops
   // the number of events is the "larger unit" compared to 
   // number of samples... that is, 1 event = N samples.  
   double signal_length   = myADC->fSignalLength; 
   double sampling_period = myADC->fClockPeriod; 
   double event_length_f  = signal_length/sampling_period; // time_of_signal/sample_period = time_of_signal/(time/sample) = total number of samples per event
   u_int32_t event_length = (u_int32_t)event_length_f;        
   int event_length_int   = (int)event_length_f; 

   // printf("[SIS3302_um]: signal_length = %.0lf, sampling period = %.3E, event_length = %.0lf \n",signal_length,sampling_period,event_length_f); 

   printf("[SIS3302_um]: Setting up to record %d samples per event... \n",event_length_int); 

   // set the event length 
   u_int32_t data32 = (event_length - 4) & 0xfffffC;       // what is this wizardry? no idea, from StruckADC manual.
   int rc = SISWrite32(vme_handle,SIS3302_SAMPLE_LENGTH_ALL_ADC,data32);     

   int NumberOfEvents = myADC->fNumberOfEvents;

   if(NumberOfEvents>PULSES_PER_READ){
      rc = SISWrite32(vme_handle,SIS3302_MAX_NOF_EVENT,PULSES_PER_READ);
   }else{
      rc = SISWrite32(vme_handle,SIS3302_MAX_NOF_EVENT,NumberOfEvents);
   } 

   if(gIsDebug) printf("[SIS3302_um]: Configuration complete. \n"); 

   rc = SISWrite32(vme_handle,SIS3302_KEY_ARM,0x0);     
 
   return rc;
}
//______________________________________________________________________________
int SIS3302SetClockFreq(int vme_handle,int clock_state,int freq_mhz){

   u_int32_t data32;

   switch(clock_state){
      case 0: // internal clock 
         switch(freq_mhz){
            case 1: 
               data32 = SIS3302_ACQ_SET_CLOCK_TO_1MHZ;
               printf("[StruckADC]: Clock set to 1 MHz \n"); 
               break;
            case 10: 
               data32 = SIS3302_ACQ_SET_CLOCK_TO_10MHZ;
               printf("[StruckADC]: Clock set to 10 MHz \n"); 
               break;
            case 25: 
               data32 = SIS3302_ACQ_SET_CLOCK_TO_25MHZ;
               printf("[StruckADC]: Clock set to 25 MHz \n"); 
               break;
            case 50: 
               data32 = SIS3302_ACQ_SET_CLOCK_TO_50MHZ; 
               printf("[StruckADC]: Clock set to 50 MHz \n"); 
               break;
            case 100: 
               data32 = SIS3302_ACQ_SET_CLOCK_TO_100MHZ; 
               printf("[StruckADC]: Clock set to 100 MHz \n"); 
               break;
            default:
               data32 = SIS3302_ACQ_SET_CLOCK_TO_1MHZ;
               printf("[SS3302::SISSetClockFreq]: Error!  Invalid frequency passed to SISSetClockFreq, defaulting to 1 MHz.\n");
               printf("                           freq = %d MHz",freq_mhz); 
               break;
         }
         break; 
      case 1: // external clock 
         data32 = SIS3302_ACQ_SET_CLOCK_TO_LEMO_CLOCK_IN; 
         printf("[StruckADC]: Using an EXTERNAL CLOCK of %d MHz \n",freq_mhz);
         break;
      default:
         data32 = SIS3302_ACQ_SET_CLOCK_TO_1MHZ;
         printf("[StruckADC::SISSetClockFreq]: Invalid clock state!  Using the internal clock, set to 1MHz. \n"); 
   }

   int ret_code = SISWrite32(vme_handle,SIS3302_ACQUISTION_CONTROL, data32);

   return ret_code;  
}
//______________________________________________________________________________
int SIS3302SampleNMRPulse(int vme_handle){

   // Samples a single NMR pulse when called. Since the SIS3302 will be in 
   // multi-event mode, this can be called multiple times (up to maxing out the 
   // SIS3302 memory). SISWriteNMRPulses will later write all recorded pulses to 
   // files.
   // TODO: check if max number of events has been recorded, error message if so.

   int return_code;
   u_int32_t data32;

   if(gIsDebug){ 
      return_code = SISRead32(vme_handle,SIS3302_ACTUAL_SAMPLE_ADDRESS_ADC1,&data32);
      printf("[StruckADC]: Current sample address: return code = 0x%08x, data=%u\n",return_code,data32); 
   }

   return_code = SISWrite32(vme_handle,SIS3302_KEY_START, 0x0);

   return return_code;
}
//______________________________________________________________________________
int SIS3302WriteNMRPulses(int vme_handle,const struct adc myADC,char *outdir){

   double signal_length    = myADC.fSignalLength;
   double sampling_period  = myADC.fClockPeriod;   
   double event_length_f   = signal_length/sampling_period;
   u_int32_t event_length = (u_int32_t)event_length_f; 
   u_int32_t total_pulses, data32, data1, data2;

   SISRead32(vme_handle,SIS3302_ACTUAL_EVENT_COUNTER,&total_pulses);
   printf("[StruckADC]: Number of pulses to write: %u \n", total_pulses);

   FILE *pulse_file;
   char pulse_filepath[200];

   const int NPTS = (int)( (event_length/2) ); 
 
   unsigned int pulse_num=0;
   int i=0,t1=0,t2=0;
   unsigned short int v1=0,v2=0;    // 16-bit-long integers, unsigned  
   for(pulse_num = 0; pulse_num < total_pulses; pulse_num++){
      sprintf(pulse_filepath, "%s/%d.dat",outdir,pulse_num+1);
      pulse_file = fopen(pulse_filepath, "w");
      if(pulse_file==NULL){
         printf("[StruckADC]: Cannot open the file: %s.  The data will NOT be written to file. \n",pulse_filepath);
         // exit(1);
      }
      // TODO: read only 16 bits (1 sample) at a time instead of this 
      // data1,data2 + pow(2,16) approach.
      for(i=0;i<NPTS;i++){
         SISRead32(vme_handle,SIS3302_ADC1_OFFSET + 4*(i + (int)(pulse_num*event_length)/2), &data32); 
         data1       =  data32 & 0x0000ffff;              // low bytes 
         data2       = (data32 & 0xffff0000)/pow(2,16);  // high bytes 
         t1          = i*2;   // GetTimeInUnits(i*2  ,ClockFreq,units);
         t2          = i*2+1; // GetTimeInUnits(i*2+1,ClockFreq,units); 
         v1          = (unsigned short int)data1; // SISIntToVoltage(data1);
         v2          = (unsigned short int)data2; // SISIntToVoltage(data2);
         // print out time and voltage  
         // fprintf(pulse_file,"%.7f %.7f\n%.7f %.7f\n",t1,v1,t2,v2);
         fprintf(pulse_file,"%d %hu\n%d %hu\n",t1,v1,t2,v2);
      }
      fclose(pulse_file);
      printf("[StruckADC]: Pulses written to: %s \n",pulse_filepath);
   }

   return 0; 
}
//______________________________________________________________________________
int SIS3302WriteNMRPulsesAlt(int vme_handle,const struct adc myADC,char *outdir){
 
   double signal_length    = myADC.fSignalLength;
   double sampling_period  = myADC.fClockPeriod;   
   double event_length_f   = signal_length/sampling_period;
   u_int32_t event_length = (u_int32_t)event_length_f; 
   u_int32_t total_pulses,data1,data2;
   // u_int32_t data32;  

   SISRead32(vme_handle,SIS3302_ACTUAL_EVENT_COUNTER,&total_pulses);
   printf("[StruckADC]: Number of pulses to write: %u \n", total_pulses);

   int NUM_SAMPLES          = myADC.fNumberOfSamples;
   u_int32_t NUM_SAMPLES_ul = (u_int32_t)NUM_SAMPLES; 

   size_t NDATA=0; 
   FILE *pulse_file_bin;
   char pulse_filepath_bin[200];
   // sprintf(pulse_filepath_bin, "%s/%d.bin",outdir,PulseNum);

   // block read of data from ADC
   unsigned int i=0;
   unsigned long delta_t = 0;  
   int addr           = 0;
   int ret_code       = 0; 
   u_int32_t NumWords = 0;
   int j=0;
   for(i=0;i<total_pulses;i++){ 
      // block read of pulse 
      gettimeofday(&gStart,NULL); 
      addr     = MOD_BASE + SIS3302_ADC1_OFFSET + 4*( (int)(i*event_length)/2 );
      ret_code = vme_A32_2EVME_read(vme_handle,addr,&gDATA[0],NUM_SAMPLES_ul/2,&NumWords);
      gettimeofday(&gStop,NULL); 
      delta_t  = gStop.tv_usec - gStart.tv_usec;
      // anything wrong? 
      if( gIsDebug || ret_code!=0 ){
         if(ret_code==0) printf("[StruckADC]: Block read return code        = %d     \n",ret_code); 
         if(ret_code!=0) printf("[StruckADC]: ERROR! Block read return code = %d     \n",ret_code); 
         printf("           address                       = 0x%08x \n",addr); 
         printf("           Number of words read          = %d     \n",NumWords);
         printf("           Time duration                 = %lu us \n",delta_t);
      }
      // convert to an array of unsigned shorts  
      for(j=0;j<NUM_SAMPLES/2;j++){
         data1           =  gDATA[j] & 0x0000ffff;             // low bytes 
         data2           = (gDATA[j] & 0xffff0000)/pow(2,16);  // high bytes 
         gDATA_us[j*2]   = (unsigned short)data1; 
         gDATA_us[j*2+1] = (unsigned short)data2; 
      }
      // print to file 
      sprintf(pulse_filepath_bin, "%s/%d.bin",outdir,i+1);
      pulse_file_bin = fopen(pulse_filepath_bin, "wb");
      if(pulse_file_bin==NULL){
         printf("[StruckADC]: Cannot open the file: %s.  The data will NOT be written to file. \n",pulse_filepath_bin);
      }else{
         NDATA = fwrite(gDATA_us,sizeof(unsigned short),NUM_SAMPLES,pulse_file_bin); 
         fclose(pulse_file_bin);
         printf("[StruckADC]: Pulse written to: %s \n",pulse_filepath_bin);
      }
      ClearOutputArrays(NUM_SAMPLES); 
   }
   NDATA += 0; 

   return 0; 
}
//______________________________________________________________________________
int SIS3302WriteNMRPulsesAltNew(int vme_handle,int PulseOffset,int NumPulsesToWrite,const struct adc myADC,char *outdir){

   // read all pulses out of the ADC 
   // via a block-read of the memory  

   double signal_length    = myADC.fSignalLength;
   double sampling_period  = myADC.fClockPeriod;   
   double event_length_f   = signal_length/sampling_period;
   u_int32_t event_length = (u_int32_t)event_length_f; 
   u_int32_t data1,data2;
   // u_int32_t data32;  

   u_int32_t total_pulses = NumPulsesToWrite; 

   // SISRead32(vme_handle,SIS3302_ACTUAL_EVENT_COUNTER,&total_pulses);
   printf("[StruckADC]: Number of pulses to write: %u \n", total_pulses);

   int NUM_SAMPLES          = myADC.fNumberOfSamples;
   u_int32_t NUM_SAMPLES_ul = (u_int32_t)NUM_SAMPLES; 

   size_t NDATA=0; 
   FILE *pulse_file_bin;
   char pulse_filepath_bin[200];

   // block read of data from ADC
   unsigned int i=0;
   unsigned long delta_t = 0;  
   int addr           = 0;
   int ret_code       = 0; 
   u_int32_t NumWords = 0;
   int j=0,PulseNum=0;
   for(i=0;i<total_pulses;i++){ 
      // block read of pulse 
      gettimeofday(&gStart,NULL); 
      addr     = MOD_BASE + SIS3302_ADC1_OFFSET + 4*( (int)(i*event_length)/2 );
      ret_code = vme_A32_2EVME_read(vme_handle,addr,&gDATA[0],NUM_SAMPLES_ul/2,&NumWords);
      gettimeofday(&gStop,NULL); 
      delta_t  = gStop.tv_usec - gStart.tv_usec;
      // anything wrong? 
      if( gIsDebug || ret_code!=0 ){
         if(ret_code==0) printf("[StruckADC]: Block read return code        = %d     \n",ret_code); 
         if(ret_code!=0) printf("[StruckADC]: ERROR! Block read return code = %d     \n",ret_code); 
         printf("           address                       = 0x%08x \n",addr); 
         printf("           Number of words read          = %d     \n",NumWords);
         printf("           Time duration                 = %lu us \n",delta_t);
      }
      // convert to an array of type unsigned short 
      for(j=0;j<NUM_SAMPLES/2;j++){
         data1           =  gDATA[j] & 0x0000ffff;             // low bytes 
         data2           = (gDATA[j] & 0xffff0000)/pow(2,16);  // high bytes 
         gDATA_us[j*2]   = (unsigned short)data1; 
         gDATA_us[j*2+1] = (unsigned short)data2; 
      }
      // print to file
      PulseNum = PulseOffset + i + 1;
      sprintf(pulse_filepath_bin,"%s/%d.bin",outdir,PulseNum);
      pulse_file_bin = fopen(pulse_filepath_bin, "wb");
      if(pulse_file_bin==NULL){
         printf("[StruckADC]: Cannot open the file: %s.  The data will NOT be written to file. \n",pulse_filepath_bin);
      }else{
         NDATA = fwrite(gDATA_us,sizeof(unsigned short),NUM_SAMPLES,pulse_file_bin); 
         fclose(pulse_file_bin);
         printf("[StruckADC]: Pulse written to: %s \n",pulse_filepath_bin);
      }
      // set up for next pulse
      ClearOutputArrays(NUM_SAMPLES); 
   }
   NDATA += 0; 

   return ret_code; 
}
//______________________________________________________________________________
int SIS3302WriteNMRPulse(int vme_handle,int PulseNum,const struct adc myADC,char *outdir){

   // write a single pulse to file 

   // double ClockFreq        = myADC.fClockFrequency; 
   double signal_length    = myADC.fSignalLength;
   double sampling_period  = myADC.fClockPeriod;   
   double event_length_f   = signal_length/sampling_period;
   u_int32_t event_length = (u_int32_t)event_length_f; 
   u_int32_t data32, data1, data2;

   // SISRead32(vme_handle, SIS3302_ACTUAL_EVENT_COUNTER, &total_pulses);
   // printf("[SIS3302]: Number of pulses to write: %u \n", total_pulses);

   // // create new directory for output
   // char *data_dir = GetDirectoryName(); 
   // // save it to outdir so we can print a diagnostic file to the directory in the main function 
   // strcpy(outdir,data_dir);

   int t1=0,t2=0;
   unsigned short int v1=0,v2=0; 

   // const int NPTS = (int)( ClockFreq*signal_length ); 

   unsigned int i=0;

   FILE *pulse_file;
   char pulse_filepath[200];
   sprintf(pulse_filepath, "%s/%d.dat",outdir,PulseNum);

   pulse_file = fopen(pulse_filepath, "w");
   if(pulse_file==NULL){
      printf("[StruckADC]: Cannot open the file: %s.  The data will NOT be written to file. \n",pulse_filepath);
      // exit(1);
   }else{
      // TODO: read only 16 bits (1 sample) at a time instead of this 
      // data1,data2 + pow(2,16) approach.
      for(i=0;i<(event_length/2)-2; i++){
         SISRead32(vme_handle,SIS3302_ADC1_OFFSET + 4*(i + (int)(1.*event_length)/2), &data32);   // are we sure this is right?
         data1       =  data32 & 0x0000ffff;              // low bytes 
         data2       = (data32 & 0xffff0000)/pow(2,16);  // high bytes 
         t1          = i*2;   // GetTimeInUnits(i*2  ,ClockFreq,units);
         t2          = i*2+1; // GetTimeInUnits(i*2+1,ClockFreq,units); 
         v1          = (unsigned short int)data1; // SISIntToVoltage(data1);
         v2          = (unsigned short int)data2; // SISIntToVoltage(data2);
         // print out time and voltage  
         // fprintf(pulse_file,"%.7f %.7f\n%.7f %.7f\n",t1,v1,t2,v2);
         fprintf(pulse_file,"%d %hu\n%d %hu\n",t1,v1,t2,v2);
      }
      fclose(pulse_file);
      // printf("[SIS3302]: Pulse written to: %s \n",pulse_filepath);
   }

   return 0; 
}
//______________________________________________________________________________
int SIS3302WriteNMRPulseAlt(int vme_handle,int PulseNum,const struct adc myADC,char *outdir){

   // write a single pulse to file
   // do a block read of the ADC 
   // file format: dat and binary    

   int NUM_SAMPLES          = myADC.fNumberOfSamples;
   u_int32_t NUM_SAMPLES_ul = (u_int32_t)NUM_SAMPLES; 

   // block read of data from ADC
   gettimeofday(&gStart,NULL); 
   int addr           = MOD_BASE + SIS3302_ADC1_OFFSET;
   int ret_code       = 0; 
   u_int32_t NumWords = 0; 
   ret_code           = vme_A32_2EVME_read(vme_handle,addr,&gDATA[0],NUM_SAMPLES_ul/2,&NumWords);
   gettimeofday(&gStop,NULL); 

   unsigned long delta_t = gStop.tv_usec - gStart.tv_usec;

   if( gIsDebug || ret_code!=0 ){
      if(ret_code==0) printf("[StruckADC]: Block read return code        = %d     \n",ret_code); 
      if(ret_code!=0) printf("[StruckADC]: ERROR! Block read return code = %d     \n",ret_code); 
      printf("           Number of words read          = %d     \n",NumWords);
      printf("           Time duration                 = %lu us \n",delta_t);
   }

   int t1=0,t2=0;
   u_int32_t data1, data2;
   unsigned short v1=0,v2=0; 

   // convert to an array of unsigned shorts  
   int i=0;
   for(i=0;i<NUM_SAMPLES/2;i++){
      data1           =  gDATA[i] & 0x0000ffff;             // low bytes 
      data2           = (gDATA[i] & 0xffff0000)/pow(2,16);  // high bytes 
      v1              = (unsigned short)data1; 
      v2              = (unsigned short)data2; 
      gDATA_us[i*2]   = v1;  
      gDATA_us[i*2+1] = v2;  
   }

   // print to files 

   size_t NDATA=0; 
   FILE *pulse_file_bin;
   FILE *pulse_file_dat;
   char pulse_filepath_bin[200];
   char pulse_filepath_dat[200];
   sprintf(pulse_filepath_bin, "%s/%d.bin",outdir,PulseNum);
   sprintf(pulse_filepath_dat, "%s/%d.dat",outdir,PulseNum);

   pulse_file_bin = fopen(pulse_filepath_bin, "wb");
   if(pulse_file_bin==NULL){
      printf("[StruckADC]: Cannot open the file: %s.  The data will NOT be written to file. \n",pulse_filepath_bin);
   }else{
      NDATA = fwrite(gDATA_us,sizeof(unsigned short),NUM_SAMPLES,pulse_file_bin); 
      fclose(pulse_file_bin);
      printf("[StruckADC]: Pulse written to: %s \n",pulse_filepath_bin);
   }
   NDATA += 0; 

   pulse_file_dat = fopen(pulse_filepath_dat, "w");
   if(pulse_file_dat==NULL){
      printf("[StruckADC]: Cannot open the file: %s.  The data will NOT be written to file. \n",pulse_filepath_dat);
      // exit(1);
   }else{
      for(i=0;i<NUM_SAMPLES/2; i++){
         t1          = i*2;  
         t2          = i*2+1;
         v1          = gDATA_us[2*i];  
         v2          = gDATA_us[2*i+1];  
         fprintf(pulse_file_dat,"%d %hu\n%d %hu\n",t1,v1,t2,v2);
      }
      fclose(pulse_file_dat);
      printf("[StruckADC]: Pulse written to: %s \n",pulse_filepath_dat);
   }

   // clear our arrays 
   ClearOutputArrays(NUM_SAMPLES); 

   return 0; 
}
//______________________________________________________________________________
int SIS3302WriteNMRPulseBin(int vme_handle,int PulseNum,const struct adc myADC,char *outdir){

   // write a single pulse to file
   // file format: binary   

   int NUM_SAMPLES          = myADC.fNumberOfSamples;
   u_int32_t NUM_SAMPLES_ul = (u_int32_t)NUM_SAMPLES; 

   // block read of data from ADC
   gettimeofday(&gStart,NULL); 
   int addr           = MOD_BASE + SIS3302_ADC1_OFFSET;
   int ret_code       = 0; 
   u_int32_t NumWords = 0; 
   ret_code           = vme_A32_2EVME_read(vme_handle,addr,&gDATA[0],NUM_SAMPLES_ul/2,&NumWords);
   gettimeofday(&gStop,NULL); 

   unsigned long delta_t = gStop.tv_usec - gStart.tv_usec;

   if( gIsDebug || ret_code!=0 ){
      if(ret_code==0) printf("[StruckADC]: Block read return code        = %d     \n",ret_code); 
      if(ret_code!=0) printf("[StruckADC]: ERROR! Block read return code = %d     \n",ret_code); 
      printf("           Number of words read          = %d     \n",NumWords);
      printf("           Time duration                 = %lu us \n",delta_t);
   }

   u_int32_t data1, data2;
   unsigned short v1=0,v2=0; 

   // convert to an array of unsigned shorts  
   int i=0;
   for(i=0;i<NUM_SAMPLES/2;i++){
      data1           =  gDATA[i] & 0x0000ffff;             // low bytes 
      data2           = (gDATA[i] & 0xffff0000)/pow(2,16);  // high bytes 
      v1              = (unsigned short)data1; 
      v2              = (unsigned short)data2; 
      gDATA_us[i*2]   = v1;  
      gDATA_us[i*2+1] = v2;  
   }

   // print to file 

   size_t NDATA=0; 
   FILE *pulse_file;
   char pulse_filepath[200];
   sprintf(pulse_filepath, "%s/%d.bin",outdir,PulseNum);

   pulse_file = fopen(pulse_filepath, "wb");
   if(pulse_file==NULL){
      printf("[StruckADC]: Cannot open the file: %s.  The data will NOT be written to file. \n",pulse_filepath);
   }else{
      NDATA = fwrite(gDATA_us,sizeof(unsigned short),NUM_SAMPLES,pulse_file); 
      fclose(pulse_file);
      printf("[StruckADC]: Pulse written to: %s \n",pulse_filepath);
   }

   NDATA += 0; 

   // clear our arrays 
   ClearOutputArrays(NUM_SAMPLES); 

   return 0; 
}
//______________________________________________________________________________
int SIS3302SampleData(int vme_handle,const struct adc myADC,char *output_dir,int EventNum,int *armed_bank_flag){

   // write a single pulse to file
   // file format: binary   

   int chNumber             = myADC.fChannelNumber;

   int NUM_SAMPLES          = myADC.fNumberOfSamples;
   u_int32_t NUM_SAMPLES_ul = (u_int32_t)NUM_SAMPLES; 

   u_int32_t *data32       = (u_int32_t *)malloc( sizeof(u_int32_t)*NUM_SAMPLES );
   unsigned short *data_us = (unsigned short *)malloc( sizeof(unsigned short)*NUM_SAMPLES/2 );

   // block read of data from ADC
   gettimeofday(&gStart,NULL);
   int addr=0; 
   if(chNumber==1){
     addr = MOD_BASE + SIS3302_ADC1_OFFSET;
   }else if(chNumber==2){
     addr = MOD_BASE + SIS3302_ADC2_OFFSET;
   }else if(chNumber==3){
     addr = MOD_BASE + SIS3302_ADC3_OFFSET;
   }else if(chNumber==4){
     addr = MOD_BASE + SIS3302_ADC4_OFFSET;
   }else if(chNumber==5){
     addr = MOD_BASE + SIS3302_ADC5_OFFSET;
   }else if(chNumber==6){
     addr = MOD_BASE + SIS3302_ADC6_OFFSET;
   }else if(chNumber==7){
     addr = MOD_BASE + SIS3302_ADC7_OFFSET;
   }else if(chNumber==8){
     addr = MOD_BASE + SIS3302_ADC8_OFFSET;
   }

   int ret_code       = 0; 
   u_int32_t NumWords = 0; 
   ret_code           = vme_A32_2EVME_read(vme_handle,addr,&data32[0],NUM_SAMPLES_ul/2,&NumWords);
   gettimeofday(&gStop,NULL); 

   unsigned long delta_t = gStop.tv_usec - gStart.tv_usec;

   if( gIsDebug || ret_code!=0 ){
      if(ret_code==0) printf("[StruckADC]: Block read return code        = %d     \n",ret_code); 
      if(ret_code!=0) printf("[StruckADC]: ERROR! Block read return code = %d     \n",ret_code); 
      printf("           Number of words read          = %d     \n",NumWords);
      printf("           Time duration                 = %lu us \n",delta_t);
   }

   u_int32_t data1, data2;
   unsigned short v1=0,v2=0; 

   // convert to an array of unsigned shorts  
   int i=0;
   for(i=0;i<NUM_SAMPLES/2;i++){
      data1          =  data32[i] & 0x0000ffff;             // low bytes 
      data2          = (data32[i] & 0xffff0000)/pow(2,16);  // high bytes 
      v1             = (unsigned short)data1; 
      v2             = (unsigned short)data2; 
      data_us[i*2]   = v1;  
      data_us[i*2+1] = v2;  
   }

   // print to file 

   size_t NDATA=0; 
   FILE *pulse_file;
   char pulse_filepath[200];
   sprintf(pulse_filepath,"%s/%d.bin",output_dir,EventNum);

   pulse_file = fopen(pulse_filepath,"wb");
   if(pulse_file==NULL){
      printf("[StruckADC]: Cannot open the file: %s.  The data will NOT be written to file. \n",pulse_filepath);
   }else{
      NDATA = fwrite(data_us,sizeof(unsigned short),NUM_SAMPLES,pulse_file); 
      fclose(pulse_file);
      printf("[StruckADC]: Pulse written to: %s \n",pulse_filepath);
   }

   NDATA += 0; 

   // clear our arrays 
   ClearOutputArrays(NUM_SAMPLES);

   free(data32);
   free(data_us); 

   return 0; 
}

