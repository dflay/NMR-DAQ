#include "sis3316_um.h"
//______________________________________________________________________________
int SIS3316Test(int vme_handle,const struct adc myADC){

   int ret_code = 0; 
  
   ret_code = SIS3316Init(vme_handle,myADC);

   if(ret_code==0){
      ret_code = SIS3316SampleDataTest(vme_handle,myADC);
   }else if(ret_code==-99){
      printf("[StruckADC]: SIS3316 initialization successful.  User has chosen to exit... \n"); 
   }else{
      printf("[StruckADC]: SIS3316 initialization FAILED!  Exiting... \n"); 
      ret_code = -1; 
   }

   return ret_code; 

}
//_____________________________________________________________________________
int SIS3316Init(int vme_handle,const struct adc myADC){

   double dt=0; 
   unsigned long *timeStart = (unsigned long *)malloc( sizeof(unsigned long)*6 ); 
   unsigned long *timeEnd   = (unsigned long *)malloc( sizeof(unsigned long)*6 ); 
   
   int i=0; 
   for(i=0;i<6;i++) timeStart[i] = 0; 
   for(i=0;i<6;i++) timeEnd[i]   = 0; 

   GetTimeStamp_usec(timeStart);
 
   int TestVal = 3;   // test code (used for test mode) 
 
   int ret_code=0; 
   int use_ext_raw_buf = 0;
 
   u_int32_t SixtyFourK=64000;
   u_int32_t read_data=0;
   u_int32_t data32=0; 
   u_int32_t raw_data_buf_reg=0;        
   u_int32_t ext_raw_data_buf_reg=0;

   double sample_size_bytes    = 2.; 

   // u_int32_t raw_buf_max       = 33554430;    // maximum of raw data buffer + extended raw data buffer (I think...) 
   u_int32_t raw_buf_max       = SixtyFourK; 
   u_int32_t tot_buf_max       = 33554430; 

   // general settings and data

   // input from user 
   u_int32_t input_nof_samples      = (u_int32_t)myADC.fNumberOfSamples; // number of samples  
   u_int32_t NEvents                = (u_int32_t)myADC.fNumberOfEvents;  // number of events 
   u_int32_t event_length           = input_nof_samples;                 // number of samples per event  
   int use_ext_clock                = myADC.fClockType;                  // 0 => false; 1 => true 

   // some values that (most likely) won't change 
   unsigned int auto_trigger_enable = 1;                                 // 1 => use internal triggering; 0 => use external triggering 
   unsigned int analog_ctrl_val     = 0 ;                                // 5V Range
   // unsigned int analog_ctrl_val     = analog_ctrl_val + 0x01010101 ;  // set to 2V Range
   // unsigned int analog_ctrl_val     = analog_ctrl_val + 0x04040404 ;  // disable 50 Ohm Termination (enables 1k termination) 
   int adc_125MHz_flag              = 1;                                 // 0 => 250 MHz; 1 => 125 MHz; choosing the 250 MHz (125 MHz) option will use the 14-bit (16-bit) ADC  
   unsigned long int NEventsOnADC   = 1;                                 // we'll print 1 event to file; so we make the address threshold hold 1 event.  
   double trigger_gate_window_sec   = 80E-6;                             // choose the trigger gate window length (seconds); not sure if this matters for us...    

   // bookkeeping
   double input_nof_samples_mb      = ( (double)input_nof_samples )*sample_size_bytes/1E+6;   

   if(input_nof_samples>raw_buf_max){
      use_ext_raw_buf      = 1;
      ext_raw_data_buf_reg = event_length;                                   
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Using EXTENDED raw data buffer (number of samples = %lu) \n",(unsigned long)input_nof_samples);  
   }else{
      // ANDed with 1s to make sure it's 16 bits wide; 
      // bit-shifted to the left by 16 to meet register requirements 
      raw_data_buf_reg     = (input_nof_samples & 0xffff) << 16;                                      
   }

   if(input_nof_samples>tot_buf_max){
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Number of samples too big!  Setting to maximum... \n"); 
      use_ext_raw_buf      = 1;
      ext_raw_data_buf_reg = tot_buf_max - 1;                                    
   }

   unsigned long int addr_thresh        = (unsigned long int)( NEventsOnADC*event_length/2 );  // FIXME: Should be in number of 32-bit words! 
   unsigned long int max_read_nof_words = NEventsOnADC*event_length;

   // get some details about the clock we're using 
   int ClockFreq=0;
   if(adc_125MHz_flag==0) ClockFreq = 250E+6; 
   if(adc_125MHz_flag==1) ClockFreq = 125E+6; 
   if(use_ext_clock==1)   ClockFreq = (int)myADC.fClockFrequency;

   unsigned int trigger_gate_window_length = (unsigned int)( trigger_gate_window_sec*ClockFreq );   // 
     
   printf("[SIS3316_um]: Initializing... \n"); 
 
   if(gIsDebug || gIsTest==TestVal) printf("Event length:                        %lu (%.3lf MB) \n",(unsigned long)event_length,input_nof_samples_mb); 
   if(gIsDebug || gIsTest==TestVal) printf("Number of events:                    %d    \n",NEvents); 
   if(gIsDebug || gIsTest==TestVal) printf("Address threshold:                   %lu 32-bit words \n",addr_thresh ); 
   if(gIsDebug || gIsTest==TestVal) printf("Total number of expected data words: %lu   \n",max_read_nof_words); 
   if(gIsDebug || gIsTest==TestVal) printf("Sampling frequency:                  %d Hz \n",ClockFreq); 
   if(gIsDebug || gIsTest==TestVal) printf("Trigger gate window length:          %u (%.4E sec) \n",trigger_gate_window_length,trigger_gate_window_sec); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Reading the MOD ID... \n"); 
   ret_code = SISMODID(vme_handle,SIS3316_MODID); 
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 
   usleep(1); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Issuing key reset...\n");
   ret_code = SISWrite32(vme_handle,SIS3316_KEY_RESET, 0x0);
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 
   usleep(1); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Issuing key disarm...\n");
   ret_code = SISWrite32(vme_handle,SIS3316_KEY_DISARM, 0x0);
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 
   usleep(1); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Configuring the ADC via SPI... \n"); 
   ret_code = adc_spi_setup(vme_handle,adc_125MHz_flag); 
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 
   usleep(1); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Configuring the clock... \n"); 
   ret_code = SIS3316ConfigureClock(vme_handle,myADC,use_ext_clock,adc_125MHz_flag);
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

   // enable ADC chip outputs
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Turning on the ADC chip outputs... \n"); 
   u_int32_t an_offset=0; 
   int fail=0; 
   for(i=0;i<4;i++){
      an_offset = i*SIS3316_FPGA_ADC_REG_OFFSET; 
      ret_code  = SISWrite32(vme_handle,an_offset + SIS3316_ADC_CH1_4_SPI_CTRL_REG, 0x01000000 ); // enable ADC chip outputs
      if(ret_code!=0) fail++; 
   }
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   usleep(1); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting the LEMO output 'CO'... \n"); 
   data32 = 0x1 ; // Select Sample Clock
   ret_code = SISWrite32(vme_handle,SIS3316_LEMO_OUT_CO_SELECT_REG, data32 ); //
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Enabling the LEMO output 'TO'... \n"); 
   data32 = 0xffff ; // Select all triggers
   ret_code = SISWrite32(vme_handle,SIS3316_LEMO_OUT_TO_SELECT_REG, data32 ); //
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

   // header writes 
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting up the headers... \n"); 
   fail = 0; 
   data32 = 0x0 ;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_CHANNEL_HEADER_REG  , data32 ); //
   if(ret_code!=0) fail++; 
   data32 = 0x00400000;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_CHANNEL_HEADER_REG  , data32 ); //
   if(ret_code!=0) fail++; 
   data32 = 0x00800000 ;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_CHANNEL_HEADER_REG , data32 ); //
   if(ret_code!=0) fail++; 
   data32 = 0x00C00000;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_CHANNEL_HEADER_REG, data32 ); //
   if(ret_code!=0) fail++; 
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   // gain/termination 
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting the gain and termination options... \n"); 
   fail = 0; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_ANALOG_CTRL_REG  ,analog_ctrl_val); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_ANALOG_CTRL_REG  ,analog_ctrl_val); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_ANALOG_CTRL_REG ,analog_ctrl_val); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_ANALOG_CTRL_REG,analog_ctrl_val); 
   if(ret_code!=0) fail++; 
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Intializing the ADC (DAC) offsets... \n"); 
   u_int32_t adc_dac_offset=0; 
   u_int32_t analog_offset_dac_val = 0x8000; // -2.5 < V < 2.5 volts: 32768 (0x8000); 0 < V < 5 volts: 65535; -5 < V < 0 volts: 0 

   //  set ADC offsets (DAC)
   // some details: below in the loop, there are some numbers.  They translate to: 
   // 0x80000000 // DAC CTRL Mode: Write Command
   // 0x2000000  // DAC Command Mode: write to Input
   // 0xf00000   // DAC Address bits: ALL DACs

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Enabling the internal reference... \n");
   fail = 0; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG  ,0x88f00001);
   if(ret_code!=0) fail++; 
   usleep(50); 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_DAC_OFFSET_CTRL_REG  ,0x88f00001);
   if(ret_code!=0) fail++; 
   usleep(50); 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_DAC_OFFSET_CTRL_REG ,0x88f00001);
   if(ret_code!=0) fail++; 
   usleep(50); 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_DAC_OFFSET_CTRL_REG,0x88f00001);
   if(ret_code!=0) fail++; 
   usleep(50); 
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Now implementing configuration... \n"); 
   fail = 0;
   for (i=0;i<4;i++){ // over all 4 ADC-FPGAs
      adc_dac_offset = i*SIS3316_FPGA_ADC_REG_OFFSET;    //                                   write cmd    ??           all DACs   ??
      ret_code = SISWrite32(vme_handle,adc_dac_offset + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG,0x80000000 + 0x8000000 +  0xf00000 + 0x1); // set internal reference 
      if(ret_code!=0) fail++; 
      usleep(50); //unsigned int uint_usec                                                     write cmd  write to input  all DACs           offset setting  
      ret_code = SISWrite32(vme_handle,adc_dac_offset + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG,0x80000000 + 0x2000000 +  0xf00000 + ((analog_offset_dac_val & 0xffff) << 4) );  //
      if(ret_code!=0) fail++; 
      usleep(50); //unsigned int uint_usec                                                     ??
      ret_code = SISWrite32(vme_handle,adc_dac_offset + SIS3316_ADC_CH1_4_DAC_OFFSET_CTRL_REG,0xC0000000 );  //
      if(ret_code!=0) fail++; 
      usleep(50); //unsigned int uint_usec
   }
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting the trigger gate window length register... \n"); 
   fail = 0; 
   data32 = (trigger_gate_window_length - 2) & 0xffff;  
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_TRIGGER_GATE_WINDOW_LENGTH_REG,data32);
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_TRIGGER_GATE_WINDOW_LENGTH_REG,data32);
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_TRIGGER_GATE_WINDOW_LENGTH_REG,data32);
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_TRIGGER_GATE_WINDOW_LENGTH_REG,data32);
   if(ret_code!=0) fail++; 
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting the pre-trigger delay value... \n"); 
   fail = 0; 
   data32 = 0x0; // 2042; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_PRE_TRIGGER_DELAY_REG,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_PRE_TRIGGER_DELAY_REG,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_PRE_TRIGGER_DELAY_REG,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_PRE_TRIGGER_DELAY_REG,data32); 
   if(ret_code!=0) fail++; 
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   // Disable/Enable LEMO Input "TI" as External Trigger
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting the trigger type... \n"); 
   if (auto_trigger_enable==1) {
      data32 = 0x0;  // Disable NIM Input "TI"
   }else{
      data32 = 0x10; // Enable NIM Input "TI"
   }
   ret_code = SISWrite32(vme_handle,SIS3316_NIM_INPUT_CONTROL_REG,data32);
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Now starting event configuration... \n"); 
   fail = 0; 
   // data32 = 0x04040404;     //  internal trigger
   data32 = 0x08080808 ;       //  external trigger
   // data32 = 0x00080008 ;    //  external trigger only ch1, 3, 5, 7 ..
   // data32 = 0x00000008 ;    //  external trigger only ch1, 5, 9, 13
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_EVENT_CONFIG_REG  ,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_EVENT_CONFIG_REG  ,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_EVENT_CONFIG_REG ,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_EVENT_CONFIG_REG,data32); 
   if(ret_code!=0) fail++; 
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting the data format to zero... \n"); 
   fail = 0; 
   data32 = 0x0; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_DATAFORMAT_CONFIG_REG,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_DATAFORMAT_CONFIG_REG,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_DATAFORMAT_CONFIG_REG,data32); 
   if(ret_code!=0) fail++; 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_DATAFORMAT_CONFIG_REG,data32); 
   if(ret_code!=0) fail++; 
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   usleep(1); 

   unsigned long int data_low=0; 
   unsigned long int data_high=0; 
   unsigned long int sum=0;

   if(use_ext_raw_buf==0){
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Writing data to raw data buffer config register... \n"); 
      fail = 0; 
      ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_RAW_DATA_BUFFER_CONFIG_REG,raw_data_buf_reg);
      if(ret_code!=0) fail++; 
      ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_RAW_DATA_BUFFER_CONFIG_REG,raw_data_buf_reg);
      if(ret_code!=0) fail++; 
      ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_RAW_DATA_BUFFER_CONFIG_REG,raw_data_buf_reg);
      if(ret_code!=0) fail++; 
      ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_RAW_DATA_BUFFER_CONFIG_REG,raw_data_buf_reg);
      if(ret_code!=0) fail++;    
      if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

      printf("[SIS3316_um]: Reading data from raw data buffer config register... \n"); 
      fail = 0; 
      ret_code  = SISRead32(vme_handle,SIS3316_ADC_CH1_4_RAW_DATA_BUFFER_CONFIG_REG,&read_data);
      if(ret_code!=0) fail++;    
      data_low  =  read_data & 0x0000ffff;                 // low bytes 
      data_high = (read_data & 0xffff0000)/pow(2,16);      // high bytes 
      sum       =  data_low + data_high; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
      // printf("low bytes:  %lu \n",data_low);  
      // printf("high bytes: %lu \n",data_high);  
      // printf("sum:        %lu \n",sum);  
      ret_code  = SISRead32(vme_handle,SIS3316_ADC_CH5_8_RAW_DATA_BUFFER_CONFIG_REG,&read_data);
      if(ret_code!=0) fail++;    
      data_low  =  read_data & 0x0000ffff;                 // low bytes 
      data_high = (read_data & 0xffff0000)/pow(2,16);      // high bytes 
      sum       =  data_low + data_high; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
      // printf("low bytes:  %lu \n",data_low);  
      // printf("high bytes: %lu \n",data_high);  
      // printf("sum:        %lu \n",sum);  
      ret_code  = SISRead32(vme_handle,SIS3316_ADC_CH9_12_RAW_DATA_BUFFER_CONFIG_REG,&read_data);
      if(ret_code!=0) fail++;    
      data_low  =  read_data & 0x0000ffff;                 // low bytes 
      data_high = (read_data & 0xffff0000)/pow(2,16);      // high bytes 
      sum       =  data_low + data_high; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
      // printf("low bytes:  %lu \n",data_low);  
      // printf("high bytes: %lu \n",data_high);  
      // printf("sum:        %lu \n",sum);  
      ret_code  = SISRead32(vme_handle,SIS3316_ADC_CH13_16_RAW_DATA_BUFFER_CONFIG_REG,&read_data);
      if(ret_code!=0) fail++;    
      data_low  =  read_data & 0x0000ffff;                 // low bytes 
      data_high = (read_data & 0xffff0000)/pow(2,16);      // high bytes 
      sum       =  data_low + data_high; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
      // printf("low bytes:  %lu \n",data_low);  
      // printf("high bytes: %lu \n",data_high);  
      // printf("sum:        %lu \n",sum);  
      if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   }else if(use_ext_raw_buf==1){
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Writing data to EXTENDED raw data buffer config register... \n"); 
      fail = 0; 
      ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG,ext_raw_data_buf_reg);
      if(ret_code!=0) fail++;    
      ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG,ext_raw_data_buf_reg);
      if(ret_code!=0) fail++;    
      ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG,ext_raw_data_buf_reg);
      if(ret_code!=0) fail++;    
      ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG,ext_raw_data_buf_reg);
      if(ret_code!=0) fail++;    
      if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Reading data from EXTENDED raw data buffer config register... \n"); 
      fail = 0; 
      ret_code  = SISRead32(vme_handle,SIS3316_ADC_CH1_4_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG,&read_data);
      if(ret_code!=0) fail++;    
      data_low  =  read_data & 0x00000fff;                 // low bytes 
      data_high = (read_data & 0x00fff000)/pow(2,12);      // high bytes 
      sum       = data_low + data_high; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
      // printf("low bytes:  %lu \n",data_low);  
      // printf("high bytes: %lu \n",data_high);  
      // printf("sum:        %lu \n",sum);  
      ret_code  = SISRead32(vme_handle,SIS3316_ADC_CH5_8_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG,&read_data);
      if(ret_code!=0) fail++;    
      data_low  =  read_data & 0x00000fff;                 // low bytes 
      data_high = (read_data & 0x00fff000)/pow(2,12);      // high bytes 
      sum       = data_low + data_high; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
      // printf("low bytes:  %lu \n",data_low);  
      // printf("high bytes: %lu \n",data_high);  
      // printf("sum:        %lu \n",sum);  
      ret_code  = SISRead32(vme_handle,SIS3316_ADC_CH9_12_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG,&read_data);
      if(ret_code!=0) fail++;    
      data_low  =  read_data & 0x00000fff;                 // low bytes 
      data_high = (read_data & 0x00fff000)/pow(2,12);      // high bytes 
      sum       = data_low + data_high; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
      // printf("low bytes:  %lu \n",data_low);  
      // printf("high bytes: %lu \n",data_high);  
      // printf("sum:        %lu \n",sum);  
      ret_code  = SISRead32(vme_handle,SIS3316_ADC_CH13_16_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG,&read_data);
      if(ret_code!=0) fail++;    
      data_low  =  read_data & 0x00000fff;                 // low bytes 
      data_high = (read_data & 0x00fff000)/pow(2,12);      // high bytes 
      sum       = data_low + data_high; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
      // printf("low bytes:  %lu \n",data_low);  
      // printf("high bytes: %lu \n",data_high);  
      // printf("sum:        %lu \n",sum);  
      if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   }

   // set the address threshold for multi-event operation: should be set to 
   // the total sample length -- that is, NEvents*event_length; must be <= 24 bits wide. 
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting the address threshold... \n");
   fail = 0; 
   // printf("NEvents       = %d \n",NEvents); 
   // printf("Sample length = %d \n",event_length); 
   // data32 = 0x80000000 | ( (u_int32_t)( NEventsOnADC*event_length ) - 1 );  // not sure what the factor of 1 is for.
   // data32 = 0x80000000 | ( (u_int32_t)( NEventsOnADC*event_length )/2 ) - 1;  // factor of 2 to turn the integer into units of "number of 32-bit words"; what's with the (- 1)?  
   data32 = ( (u_int32_t)( addr_thresh ) ) - 1;  // what's with the (- 1)?  
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_ADDRESS_THRESHOLD_REG,data32); 
   if(ret_code!=0) fail++;    
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_ADDRESS_THRESHOLD_REG,data32); 
   if(ret_code!=0) fail++;    
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_ADDRESS_THRESHOLD_REG,data32); 
   if(ret_code!=0) fail++;    
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_ADDRESS_THRESHOLD_REG,data32); 
   if(ret_code!=0) fail++;    
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Write data = %lu (hex: 0x%08x) \n",(unsigned long)data32,data32);
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Reading the address threshold... \n");
   fail = 0; 
   ret_code = SISRead32(vme_handle,SIS3316_ADC_CH1_4_ADDRESS_THRESHOLD_REG,&read_data); 
   if(ret_code!=0) fail++;    
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
   ret_code = SISRead32(vme_handle,SIS3316_ADC_CH5_8_ADDRESS_THRESHOLD_REG,&read_data); 
   if(ret_code!=0) fail++;    
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
   ret_code = SISRead32(vme_handle,SIS3316_ADC_CH9_12_ADDRESS_THRESHOLD_REG,&read_data); 
   if(ret_code!=0) fail++;    
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
   ret_code = SISRead32(vme_handle,SIS3316_ADC_CH13_16_ADDRESS_THRESHOLD_REG,&read_data); 
   if(ret_code!=0) fail++;    
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read data:  %lu (hex: 0x%08x) \n",(unsigned long)read_data,read_data); 
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   // printf("Starting multi-event test... \n"); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Enabling external triggers... \n");
   // data32 = 0x100; // external trigger function as trigger enable   
   // data32 = 0x400; // external timestamp clear enabled  
   data32 = 0x500; // external trigger function as trigger enable + external timestamp clear enabled  
   //data32 = 0x0;
   ret_code = SISWrite32(vme_handle,SIS3316_ACQUISITION_CONTROL_STATUS,data32);
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 
   usleep(1); 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Clearing the timestamp... \n");  
   ret_code = SISWrite32(vme_handle,SIS3316_KEY_TIMESTAMP_CLEAR ,0x0);  
   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   // usleep(500000);   // it's probably best to wait a bit before starting... 
   usleep(500);        // it's probably best to wait a bit before starting... 

   char *no = "n";
   char *NO = "N";
   char ans[1]; 

   if(gIsTest==TestVal){ 
      printf("Is this OK? Enter y to continue, n to exit: ");
      scanf("%s",ans); 

      if( AreEquivStrings(ans,no) || AreEquivStrings(ans,NO) ){
         return ret_code = -99; 
      } 
   }

   if(ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if(ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

   GetTimeStamp_usec(timeEnd);
   dt = (double)( timeEnd[4]-timeStart[4] );
   printf("[SIS3316]: Elapsed time (ADC init): %.3lf ms \n",dt);

   free(timeStart); 
   free(timeEnd); 

   return ret_code; 
}
//_____________________________________________________________________________
int adc_spi_setup(int vme_handle,int adc_125MHz_flag){

   // this needs to be done to turn on all the ADC outputs properly.
   // pulled from the sis3316_class.cpp file.  

   // adc_fpga_group: 0,1,2,3
   // adc_chip: 0 or 1
   //                              -1 : not all adc chips have the same chip ID
   //                              >0 : VME Error Code

   int return_code;
   unsigned int adc_chip_id;
   unsigned int addr, data;
   unsigned i_adc_fpga_group; 
   unsigned i_adc_chip; 

   // disable ADC output
   for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
      addr        = SIS3316_ADC_CH1_4_SPI_CTRL_REG +  ((i_adc_fpga_group & 0x3) * SIS3316_FPGA_ADC_REG_OFFSET) ;
      return_code = SISWrite32(vme_handle,addr,0x0); //  
      if(return_code != 0){
         return return_code ; 
      }
   }

   // dummy loop to access each adc chip one time after power up -- add 12.02.2015
   for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
      for (i_adc_chip = 0; i_adc_chip < 2; i_adc_chip++) {
         adc_spi_read(vme_handle,i_adc_fpga_group,i_adc_chip,1,&data);
      }
   }

   // reset 
   for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
      for (i_adc_chip = 0; i_adc_chip < 2; i_adc_chip++) {
         return_code = adc_spi_write(vme_handle,i_adc_fpga_group,i_adc_chip,0x0,0x24); // soft reset
      }
      usleep(10) ; // after reset
   }

   return_code = adc_spi_read(vme_handle,0,0,1,&adc_chip_id); // read chip Id from adc chips ch1/2

   for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
      for (i_adc_chip = 0; i_adc_chip < 2; i_adc_chip++) {
         adc_spi_read(vme_handle,i_adc_fpga_group, i_adc_chip,1,&data);
         //printf("i_adc_fpga_group = %d   i_adc_chip = %d    data = 0x%08x     adc_chip_id = 0x%08x     \n", i_adc_fpga_group, i_adc_chip, data, adc_chip_id);
         if (data != adc_chip_id) {
            printf("i_adc_fpga_group = %d   i_adc_chip = %d    data = 0x%08x     adc_chip_id = 0x%08x     \n", i_adc_fpga_group, i_adc_chip, data, adc_chip_id);
            return -1 ;
         }
      }
   }

   // adc_125MHz_flag = 0;
   if ((adc_chip_id&0xff)==0x32){
      adc_125MHz_flag = 1;
   }

   // reg 0x14 : Output mode
   if (adc_125MHz_flag==0) { // 250 MHz chip AD9643
      data = 0x04 ;   //  Output inverted (bit2 = 1)
   }else{ 
      // 125 MHz chip AD9268
      data = 0x40 ;   // Output type LVDS (bit6 = 1), Output inverted (bit2 = 0) !
   }

   for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
      for (i_adc_chip = 0; i_adc_chip < 2; i_adc_chip++) {
         adc_spi_write(vme_handle,i_adc_fpga_group,i_adc_chip,0x14,data); 
      }
   }

   // reg 0x18 : Reference Voltage / Input Span
   if (adc_125MHz_flag==0){ 
      // 250 MHz chip AD9643
      data = 0x0 ;    //  1.75V
   }else{ 
      // 125 MHz chip AD9268
      //data = 0x8 ;  //  1.75V
      data = 0xC0 ;   //  2.0V
   }

   for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
      for (i_adc_chip = 0; i_adc_chip < 2; i_adc_chip++) {
         adc_spi_write(vme_handle,i_adc_fpga_group,i_adc_chip,0x18,data); 
      }
   }

   // reg 0xff : register update
   data = 0x01 ;   // update
   for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
      for (i_adc_chip = 0; i_adc_chip < 2; i_adc_chip++) {
         adc_spi_write(vme_handle,i_adc_fpga_group,i_adc_chip,0xff, data); 
      }
   }

   // enable ADC output
   for (i_adc_fpga_group = 0; i_adc_fpga_group < 4; i_adc_fpga_group++) {
      addr        = SIS3316_ADC_CH1_4_SPI_CTRL_REG +  ((i_adc_fpga_group & 0x3) * SIS3316_FPGA_ADC_REG_OFFSET) ;
      return_code = SISWrite32(vme_handle,addr,0x1000000); //  set bit 24
      if (return_code != 0) {return return_code ; }

   }

   return 0 ;

}
//_____________________________________________________________________________
//#define TEST_DEBUG
int adc_spi_read(int vme_handle,unsigned int adc_fpga_group, unsigned int adc_chip, unsigned int spi_addr, unsigned int* spi_data ){

   // this needs to be done to turn on all the ADC outputs properly.
   // pulled from the sis3316_class.cpp file. 

   int return_code;
   unsigned int data ;
   unsigned int addr ;
   unsigned int uint_adc_mux_select;
   unsigned int pollcounter;

   pollcounter = 1000;

   if (adc_fpga_group > 4) {return -1;}
   if (adc_chip > 2) {return -1;}
   if (spi_addr > 0x1fff) {return -1;}

   if (adc_chip == 0) {
      uint_adc_mux_select = 0 ;       // adc chip ch1/ch2     
   }else{
      uint_adc_mux_select = 0x400000 ; // adc chip ch3/ch4            
   }

   // read register to get the information of bit 24 (adc output enabled)
   addr = SIS3316_ADC_CH1_4_SPI_CTRL_REG +  ((adc_fpga_group & 0x3) * SIS3316_FPGA_ADC_REG_OFFSET) ;
   return_code = SISRead32(vme_handle,addr,&data); //  

   if (return_code != 0) {

#ifdef TEST_DEBUG
      printf("adc_spi_read vme_A32D32_read 1: return_code = 0x%08x     \n", return_code);
#endif
      return return_code ; 
   }

   data = data & 0x01000000 ; // save bit 24
   data = data + 0xC0000000 + uint_adc_mux_select + ((spi_addr & 0x1fff) << 8)  ;

   addr = SIS3316_ADC_CH1_4_SPI_CTRL_REG +  ((adc_fpga_group & 0x3) * SIS3316_FPGA_ADC_REG_OFFSET) ;
   return_code = SISWrite32(vme_handle,addr,data); //  

#ifdef TEST_DEBUG
   if (return_code != 0) {
      printf("adc_spi_read vme_A32D32_write 1: return_code = 0x%08x     \n", return_code);
   }
#endif

   addr = SIS3316_ADC_FPGA_SPI_BUSY_STATUS_REG ;

   do{ // the logic is appr. 20us busy 
         return_code = SISRead32(vme_handle,addr,&data); //  
#ifdef TEST_DEBUG
         if (return_code != 0) {
            printf("adc_spi_read vme_A32D32_read 2: return_code = 0x%08x     \n", return_code);
         }
#endif
         pollcounter--;
      //} while (((data & 0x80000000) == 0x80000000) && (pollcounter > 0) && (return_code == 0)); // VME FPGA Version 0x0006 and higher
    } while (((data & 0x0000000f) != 0x00000000) && (pollcounter > 0) && (return_code == 0)); // changed 2.12.2014,  VME FPGA Version 0x0005 and lower

#ifdef TEST_DEBUG
   printf("adc_spi_read pollcounter: pollcounter = 0x%08x     \n", pollcounter);
#endif

   if (return_code != 0) {return return_code ; }
   if (pollcounter == 0) {
      return -2 ; 
   }

   usleep(20) ; //

   //addr = SIS3316_ADC_CH1_4_SPI_READBACK_REG  ; // removed 21.01.2015
   addr = SIS3316_ADC_CH1_4_SPI_READBACK_REG +  ((adc_fpga_group & 0x3) * SIS3316_FPGA_ADC_REG_OFFSET) ; // changed 21.01.2015
   
   return_code = SISRead32(vme_handle,addr,&data); //  

#ifdef TEST_DEBUG
   if (return_code != 0) {
      printf("adc_spi_read vme_A32D32_read 3: return_code = 0x%08x     \n", return_code);
   }
#endif

   if (return_code != 0) {return return_code ; }

   *spi_data = data & 0xff ;

   return 0 ;

}
//______________________________________________________________________________
int adc_spi_write(int vme_handle,unsigned int adc_fpga_group, unsigned int adc_chip, unsigned int spi_addr, unsigned int spi_data ){

   // this needs to be done to turn on all the ADC outputs properly.
   // pulled from the sis3316_class.cpp file. 

   volatile int return_code;  // huh?? 
   unsigned int data ;
   unsigned int addr ;
   unsigned int uint_adc_mux_select;
   unsigned int pollcounter;
   pollcounter = 1000;

   if (adc_fpga_group > 4) {return -1;}
   if (adc_chip > 2) {return -1;}
   if (spi_addr > 0xffff) {return -1;}
   if (adc_chip == 0) {
      uint_adc_mux_select = 0 ;       // adc chip ch1/ch2     
   }else {
      uint_adc_mux_select = 0x400000 ; // adc chip ch3/ch4            
   }

   // read register to get the information of bit 24 (adc output enabled)
   addr = SIS3316_ADC_CH1_4_SPI_CTRL_REG +  ((adc_fpga_group & 0x3) * SIS3316_FPGA_ADC_REG_OFFSET) ;
   return_code = SISRead32(vme_handle,addr,&data); //  

   if (return_code != 0) {return return_code ; }

   data = data & 0x01000000 ; // save bit 24
   data = data + 0x80000000 + uint_adc_mux_select + ((spi_addr & 0xffff) << 8) + (spi_data & 0xff) ;
   addr = SIS3316_ADC_CH1_4_SPI_CTRL_REG +  ((adc_fpga_group & 0x3) * SIS3316_FPGA_ADC_REG_OFFSET) ;
   return_code = SISWrite32(vme_handle,addr,data); //  

   //usleep(1000) ; //

   addr = SIS3316_ADC_FPGA_SPI_BUSY_STATUS_REG ;
   //printf("poll_on_spi_busy: pollcounter = 0x%08x    \n", pollcounter );

   do { // the logic is appr. 20us busy
      return_code = SISRead32(vme_handle,addr,&data); //  
      pollcounter--;
      //} while (((data & 0x80000000) == 0x80000000) && (pollcounter > 0) && (return_code == 0)); // VME FPGA Version 0x0006 and higher
   } while (((data & 0x0000000f) != 0x00000000) && (pollcounter > 0) && (return_code == 0)); // changed 2.12.2014,  VME FPGA Version 0x0005 and lower

   //  printf("poll_on_spi_busy: pollcounter = 0x%08x   data = 0x%08x   return_code = 0x%08x \n", pollcounter, data, return_code);
   
   if (return_code != 0) {return return_code ; }
   if (pollcounter == 0) {return -2 ; }

   return 0 ;

}
//_____________________________________________________________________________
int SIS3316ConfigureClock(int vme_handle,const struct adc myADC,int use_ext_clock,int adc_125MHz_flag){

   int TestVal  = 3;   // test code (used for test mode) 
   int ret_code = 0;
   int fail     = 0;  

   unsigned int clock_N1div = 0; 
   unsigned int clock_HSdiv = 0;
   unsigned int iob_delay_value = 0;  

   u_int32_t read_data = 0; 

   int ClockFreq=0;
   int ClockFreq_in_units=0;
   char *units = (char *)malloc( sizeof(char)*100 );

   if(use_ext_clock==1){
      ClockFreq          = (int)myADC.fClockFrequency;
      units              = myADC.fClockFreqUnits;
      ClockFreq_in_units = 0;
      if( AreEquivStrings(units,kHz) ) ClockFreq_in_units = ClockFreq/1E+3;
      if( AreEquivStrings(units,MHz) ) ClockFreq_in_units = ClockFreq/1E+6;
      if( AreEquivStrings(units,GHz) ) ClockFreq_in_units = ClockFreq/1E+9;
   }

   if(use_ext_clock==0){
      if(adc_125MHz_flag==0){
         // 250  MHz
         if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Using internal clock: 250 MHz... \n");
         clock_N1div      =  4  ;
         clock_HSdiv      =  5  ;
         // iob_delay_value  =  0x48   ; // ADC FPGA version A_0250_0003
         iob_delay_value  =  0x1008 ; // ADC FPGA version A_0250_0004 and higher
      }else{
         if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Using internal clock: 125 MHz... \n");
         clock_N1div      =  8  ;
         clock_HSdiv      =  5  ;
         // iob_delay_value  =  0x7F   ; // ADC FPGA version A_0250_0003
         iob_delay_value  =  0x1020 ; // ADC FPGA version A_0250_0004 and higher
      }
      ret_code = change_frequency_HSdiv_N1div(vme_handle,0, clock_HSdiv, clock_N1div) ;
      if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 
   }else if(use_ext_clock==1){
      iob_delay_value = 0x0; 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Using external clock: %d %s \n",ClockFreq_in_units,units); 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting sample clock distribution control to 3 (external lemo)...\n");
      ret_code = SISWrite32(vme_handle,SIS3316_SAMPLE_CLOCK_DISTRIBUTION_CONTROL, 0x3);
      if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

      ret_code = SISRead32(vme_handle,SIS3316_SAMPLE_CLOCK_DISTRIBUTION_CONTROL, &read_data);
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Read sample clock distribution: 0x%08x\n", read_data);
      if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Bypassing precision clock multiplier...\n");
      ret_code = SI5325Write(vme_handle, 0, 0x2);
      if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Powering down the old clock...\n");
      ret_code = SI5325Write(vme_handle, 11, 0x02);
      if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
      if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 
   }
   usleep(1000);

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Resetting the DCM/PLL of all FPGAs... \n"); 
   ret_code = SISWrite32(vme_handle,SIS3316_KEY_ADC_CLOCK_DCM_RESET,0x0); 
   if( (gIsDebug || gIsTest==TestVal) && ret_code==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || ret_code!=0) printf("[SIS3316_um]: Failed! \n"); 

   usleep(10000);   // wait 10 ms for clock to be stable; should be fine after 5 ms, but we wait longer  

   fail = 0; 
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Setting the input tap delays... \n"); 
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG  , 0xf00 ); // Calibrate IOB _delay Logic
   if(ret_code!=0) fail++;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG  , 0xf00 ); // Calibrate IOB _delay Logic
   if(ret_code!=0) fail++;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG , 0xf00 ); // Calibrate IOB _delay Logic
   if(ret_code!=0) fail++;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0xf00 ); // Calibrate IOB _delay Logic
   if(ret_code!=0) fail++;
   usleep(1000) ;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH1_4_INPUT_TAP_DELAY_REG  , 0x300 + iob_delay_value ); // set IOB _delay Logic
   if(ret_code!=0) fail++;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH5_8_INPUT_TAP_DELAY_REG  , 0x300 + iob_delay_value ); // set IOB _delay Logic
   if(ret_code!=0) fail++;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH9_12_INPUT_TAP_DELAY_REG , 0x300 + iob_delay_value ); // set IOB _delay Logic
   if(ret_code!=0) fail++;
   ret_code = SISWrite32(vme_handle,SIS3316_ADC_CH13_16_INPUT_TAP_DELAY_REG, 0x300 + iob_delay_value ); // set IOB _delay Logic
   if(ret_code!=0) fail++;
   usleep(1000) ;

   if( (gIsDebug || gIsTest==TestVal) && fail==0) printf("[SIS3316_um]: Done. \n"); 
   if( (gIsDebug || gIsTest==TestVal) || fail!=0) printf("[SIS3316_um]: Failed %d times! \n",fail); 

   return ret_code; 

}
//_____________________________________________________________________________
int SIS3316SampleData(int vme_handle,const struct adc myADC,char *output_dir,int EventNum,int *armed_bank_flag){

   printf("[SIS3316::SIS3316SampleData]: line %d \n",__LINE__); 

   // Samples an NMR pulse when called.  After the address threshold is reached 
   // for a single event (i.e., pulse), the current memory bank is disarmed and the idle 
   // one is armed.  From here, the data is written to file, where the pulse number (EventNum)
   // serves as the file name, stored in the appropriate output directory (output_dir). 
   // inputs:
   // - vme_handle:      VME handle
   // - myADC:           ADC struct with all ADC settings 
   // - output_dir:      Output directory path 
   // - EventNum:        The current event number 
   // - armed_bank_flag: The currently armed bank (0 = bank2, 1 = bank1) 
   // outputs 
   // - armed_bank_flag: Is updated to reflect currently armed bank
   // - ret_code: reflects status of the sampling; 0 => success; -97: no data read; -98: addr threshold not reached. 

   int TestVal              = 3;   // test code (used for test mode) 
   int i                    = 0;  
   int ch                   = 0; 
   int ret_code             = 0;  
   int bank1_armed_flag     = 0; 
   int input_nof_samples    = myADC.fNumberOfSamples; 
   int start_ch             = myADC.fChannelNumber - 1;  // ADC channel number; subtract 1 because index runs from 0,..,15. 
   int end_ch               = start_ch + 1;  
   // int start_ch             = 0; 
   // int end_ch               = 1; // SIS3316_MAX_CH; 
   // const int SIS3316_MAX_CH = 16; 
   int max_read_nof_words   = input_nof_samples; 
   int poll_counter         = 0; 
   int poll_counter_max     = 10000; 
   const long int SIZE      = max_read_nof_words; 

   unsigned int got_nof_32bit_words = 0; 

   // apparently this works better for large arrays...
   u_int32_t *adc_buffer    = malloc( sizeof(u_int32_t)*SIZE     );
   u_int16_t *adc_buffer_us = malloc( sizeof(u_int16_t)*(2*SIZE) );

   // u_int32_t adc_buffer[SIZE]; 
   // u_int16_t adc_buffer_us[2*SIZE]; 

   u_int32_t read_data=0,addr_thresh=0;  
   u_int32_t data_low=0,data_high=0; 
   u_int32_t event_length = (u_int32_t)input_nof_samples;

   bank1_armed_flag = *armed_bank_flag;  // keeping track of previous bank.  armed_bank_flag: 0 => bank2 armed; 1 => bank1 armed   

   if(gIsDebug || gIsTest==TestVal) printf("Starting the readout loop... \n");  
   if(EventNum==1){
      // only do this on the first event to get things rolling 
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");  
      ret_code = SISWrite32(vme_handle,SIS3316_KEY_DISARM_AND_ARM_BANK1 ,0x0);  //  Arm Bank1
      bank1_armed_flag = 1; // start condition
   }

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: armed_bank_flag = %d \n",bank1_armed_flag); 

   printf("[SIS3316::SIS3316SampleData]: line %d \n",__LINE__); 

   ret_code = SISWrite32(vme_handle,SIS3316_KEY_TRIGGER,0x0);  
   poll_counter = 0 ;
   do{
      poll_counter++;
      if (poll_counter==poll_counter_max){
         // if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Address threshold has NOT been reached yet... this is taking longer than expected.  Exiting. \n"); 
         printf("[SIS3316_um]: Address threshold has NOT been reached yet... this is taking longer than expected.  Exiting. \n"); 
         ret_code = SISRead32(vme_handle,SIS3316_ADC_CH1_4_ADDRESS_THRESHOLD_REG,&addr_thresh); 
         printf("[SIS3316_um]: address thresh  = %lu \n",(unsigned long int)addr_thresh);
         printf("[SIS3316_um]: acq. ctrl. data = %lu \n",(unsigned long int)read_data);
         return -98;    
      }
      ret_code = SISRead32(vme_handle,SIS3316_ACQUISITION_CONTROL_STATUS,&read_data);
      // usleep(500000); //500ms
      usleep(1);
   } while ( (read_data & 0x80000)==0x0 ); // has the Address Threshold been reached? If 0, then address threshold has NOT been reached. 
 
   printf("[SIS3316::SIS3316SampleData]: line %d \n",__LINE__); 
   ret_code = SISRead32(vme_handle,SIS3316_ADC_CH1_4_ADDRESS_THRESHOLD_REG,&addr_thresh); 
   printf("[SIS3316_um]: address thresh  = %lu \n",(unsigned long int)addr_thresh);

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Address threshold reached!  Switching banks... \n"); 
   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: ACQUISITION CONTROL STATUS: 0x%08x\n", read_data);

   // get ready for next event: disarm current bank and arm the next bank 
   if(bank1_armed_flag == 1){
      ret_code = SISWrite32(vme_handle,SIS3316_KEY_DISARM_AND_ARM_BANK2 ,0x0);  //  Arm Bank2
      bank1_armed_flag = 0; // bank 2 is armed
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: SIS3316_KEY_DISARM_AND_ARM_BANK2 \n");
   }else{
      ret_code = SISWrite32(vme_handle,SIS3316_KEY_DISARM_AND_ARM_BANK1 ,0x0);  //  Arm Bank1
      bank1_armed_flag = 1; // bank 1 is armed
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");
   }
   printf("[SIS3316::SIS3316SampleData]: line %d \n",__LINE__); 

   usleep(10000);  // wait 10 ms 
 
   // Read out data
   for(ch=start_ch;ch<end_ch;ch++){
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Reading channel %d \n",ch+1); 
      ret_code = read_DMA_Channel_PreviousBankDataBuffer(vme_handle,
                                                         bank1_armed_flag,
                                                         ch,           
                                                         max_read_nof_words,   
                                                         &got_nof_32bit_words, 
                                                         adc_buffer,
                                                         event_length);       
      if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: read_DMA_Channel_PreviousBankDataBuffer: "); 
      if(gIsDebug || gIsTest==TestVal) printf("ch = %d  got_nof_32bit_words = 0x%08x (%d) return_code = 0x%08x \n",ch+1,got_nof_32bit_words,got_nof_32bit_words,ret_code);
   }
   printf("[SIS3316::SIS3316SampleData]: line %d \n",__LINE__); 
   if(ret_code!=0x900){  
      if(got_nof_32bit_words>0){
         printf("[SIS3316::SIS3316SampleData]: line %d WE HAVE DATA! \n",__LINE__); 
         for(i=0;i<got_nof_32bit_words;i++){
            data_low             =  adc_buffer[i] & 0x0000ffff; 
            data_high            = (adc_buffer[i] & 0xffff0000)/pow(2,16); 
            adc_buffer_us[i*2]   = (u_int16_t)data_low; 
            adc_buffer_us[i*2+1] = (u_int16_t)data_high; 
         }
         if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Event %d: Recorded %d 32-bit data-words.\n",EventNum,got_nof_32bit_words); 
         if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: Printing to file... \n"); 
         WriteEventToFile(EventNum,adc_buffer_us,2*got_nof_32bit_words,output_dir); 
      }else{
         printf("[SIS3316_um]: No 32-bit words found! No data recorded! Moving on...\n"); 
         ret_code = -97; 
      }
   }else{
      printf("[SIS3316_um]: read_DMA_Channel_PreviousBankDataBuffer return code 0x900.  No data recorded! Moving on...\n"); 
      ret_code = -97; 
   }
   // printf("---------------------------------------------------- \n"); 

   // bookkeeping of armed bank
   *armed_bank_flag = bank1_armed_flag; 

   printf("[SIS3316::SIS3316SampleData]: line %d \n",__LINE__); 
   printf("[SIS3316::SIS3316SampleData]: Return code = %d   \n",ret_code); 
   printf("[SIS3316::SIS3316SampleData]: event length = %lu \n",(unsigned long int)event_length); 

   // usleep(10000);  
   // printf("adc buffer = %hu \n",adc_buffer_us); 

   return ret_code; 
}
//_____________________________________________________________________________
int SIS3316SampleDataTest(int vme_handle,const struct adc myADC){

   // FIXME: Can't read past channel (3+1)=4!  

   int i                    = 0;  
   int ch                   = 0; 
   int ret_code             = 0;  
   int bank1_armed_flag     = 0; 
   int loop_counter         = 0;
   int input_nof_samples    = myADC.fNumberOfSamples; 
   int loop_max             = myADC.fNumberOfEvents; // NEvents;
   int max_read_nof_words   = input_nof_samples; 
   int poll_counter         = 0; 
   // int poll_counter_max     = 100000; 
   // const int SIS3316_MAX_CH = 16; 
   int start_ch             = 0; 
   int end_ch               = 1; // SIS3316_MAX_CH; 
   const long int SIZE      = max_read_nof_words; 

   unsigned int got_nof_32bit_words = 0; 
   // unsigned int adc_buffer[SIZE];
   // unsigned short adc_buffer_us[2*SIZE];
   // unsigned long data_low=0,data_high=0; 

   u_int32_t adc_buffer[SIZE]; 
   u_int16_t adc_buffer_us[2*SIZE]; 

   u_int32_t read_data=0; 
   u_int32_t data_low=0,data_high=0; 
   u_int32_t event_length = (u_int32_t)input_nof_samples; 

   // double nof_events = 0;  

   // get output directory 
   const int MAX    = 2000;
   char *output_dir = (char*)malloc( sizeof(char)*(MAX+1) );
   char *base_dir = (char*)malloc( sizeof(char)*(MAX+1) );

   struct run myRun; 

   output_dir = GetDirectoryName(&myRun,base_dir);
   printf("[NMRDAQ]: Output directory: %s \n",output_dir);

   printf("Starting the readout loop... \n");  
   printf("SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");
   ret_code = SISWrite32(vme_handle,SIS3316_KEY_DISARM_AND_ARM_BANK1 ,0x0);  //  Arm Bank1
   bank1_armed_flag = 1; // start condition

   do{
      ret_code = SISWrite32(vme_handle,SIS3316_KEY_TRIGGER,0x0);  
      poll_counter = 0 ;
      // ret_code = SISRead32(vme_handle,SIS3316_ACQUISITION_CONTROL_STATUS,&read_data);
      do {
         poll_counter++;
         // if (poll_counter==100){
         //    poll_counter = 0 ;
         // }
         ret_code = SISRead32(vme_handle,SIS3316_ACQUISITION_CONTROL_STATUS,&read_data);
         // usleep(500000); //500ms
         usleep(1);
         // printf("in Loop:   SIS3316_ACQUISITION_CONTROL_STATUS = 0x%08x poll counter = %d \n",read_data,poll_counter);
         // if(read_data==0x00300000){
         //    printf("- Status of FP-Bus In Status 1: Sample logic busy \n"); 
         //    printf("- Status of FP-Bus In Status 2: Address threshold flag \n"); 
         // }
         // if(poll_counter>poll_counter_max){
         //    ret_code = -99; 
         //    printf("Data read FAILED.  Exiting loop... \n"); 
         //    break;
         // } 

      } while ( (read_data & 0x80000)==0x0 ); // has the Address Threshold been reached? If 0, then address threshold has NOT been reached.  
      // } while ( (read_data & 0x2000000)==0x0 );

      // if(poll_counter>poll_counter_max) break;
      printf("ACQUISITION CONTROL STATUS: 0x%08x\n", read_data);

      if (bank1_armed_flag == 1) {
         ret_code = SISWrite32(vme_handle,SIS3316_KEY_DISARM_AND_ARM_BANK2 ,0x0);  //  Arm Bank2
         bank1_armed_flag = 0; // bank 2 is armed
         printf("SIS3316_KEY_DISARM_AND_ARM_BANK2 \n");
      }else{
         ret_code = SISWrite32(vme_handle,SIS3316_KEY_DISARM_AND_ARM_BANK1 ,0x0);  //  Arm Bank1
         bank1_armed_flag = 1; // bank 1 is armed
         printf("SIS3316_KEY_DISARM_AND_ARM_BANK1 \n");
      }
      usleep(10000);  // wait 10 ms  
      // Read out data
      for(ch=start_ch;ch<end_ch;ch++){
         printf("Reading channel %d \n",ch+1); 
         ret_code = read_DMA_Channel_PreviousBankDataBuffer(vme_handle,
                                                            bank1_armed_flag,
                                                            ch,           
                                                            max_read_nof_words,   
                                                            &got_nof_32bit_words, 
                                                            adc_buffer,
                                                            event_length);       
         printf("read_DMA_Channel_PreviousBankDataBuffer: ch = %d  got_nof_32bit_words = 0x%08x (%d) return_code = 0x%08x\n",ch+1,got_nof_32bit_words,got_nof_32bit_words,ret_code);
      } 
      if(got_nof_32bit_words>0){
         for(i=0;i<got_nof_32bit_words;i++){
            data_low             =  adc_buffer[i] & 0x0000ffff; 
            data_high            = (adc_buffer[i] & 0xffff0000)/pow(2,16); 
            adc_buffer_us[i*2]   = (u_int16_t)data_low; 
            adc_buffer_us[i*2+1] = (u_int16_t)data_high; 
            // printf("data = %llu data low = %hu data high =%hu \n",(unsigned long long)adc_buffer[j],data_low,data_high); 
         }
         // nof_events = (double)got_nof_32bit_words/(double)event_length; 
         // printf("Event %d: Recorded %d 32-bit data-words (NEvents = %lf).\n",loop_counter,got_nof_32bit_words,nof_events); 
         printf("Event %d: Recorded %d 32-bit data-words.\n",loop_counter+1,got_nof_32bit_words); 
         WriteEventToFile(loop_counter+1,adc_buffer_us,2*got_nof_32bit_words,output_dir); 
      }else{
         printf("No data recorded! Moving on...\n"); 
         continue; 
      }
      printf("---------------------------------------------------- \n"); 
 
      // usleep(10000);  
      // printf("adc buffer = %hu \n",adc_buffer_us); 

      loop_counter++; 
   }while( loop_counter< loop_max ); 
   
   return ret_code; 
}
//_____________________________________________________________________________
int WriteEventToFile(int EventNum,unsigned short* memory_data_array,
                     unsigned int nof_write_length_lwords,char *outdir){

   FILE *outfile;
   char outpath[200];

   int nof_write_elements=0;
   int written=0;

   sprintf(outpath,"%s/%d.bin",outdir,EventNum);
   outfile = fopen(outpath, "wb");

   if(outfile==NULL){
      printf("[SIS3316_um::WriteEventToFile]: Cannot open the file: %s.  The data will NOT be written to file. \n",outpath);
      return -1; 
   }else{
      //int data ;
      //char messages_buffer[256] ;
      // gl_uint_DataEvent_RunFile_EvenSize : length
      nof_write_elements = nof_write_length_lwords ;
      // written            = fwrite(memory_data_array,0x4,nof_write_elements,outfile); // write 3 uint value
      written            = fwrite(memory_data_array,sizeof(unsigned short),nof_write_elements,outfile); 
      if(nof_write_elements != written) {
         printf ("[SIS3316_um::WriteEventToFile]: Data file write error!  \n");
      }else{
         if(gVerbosity>1) printf ("[SIS3316_um::WriteEventToFile]: Data written to: %s  \n",outpath);
      }
      fclose(outfile);
   } 

   return 0;

}
//_____________________________________________________________________________
int change_frequency_HSdiv_N1div(int vme_handle,int osc, unsigned hs_div_val, unsigned n1_div_val){

   // change_frequency_HSdiv_N1div                             
   // hs_div_val: allowed values are [4, 5, 6, 7, 9, 11]       
   // n1_div_val: allowed values are [2, 4, 6, .... 124, 126]  
   // Sample Frequence =  5 GHz / (hs_div_val * n1_div_val)    
   //                                                          
   // example:                                                 
   // hs_div_val = 5                                           
   // n1_div_val = 4                                           
   // Sample Frequence =  5 GHz / 20 = 250 MHz                 

   int rc;
   unsigned i ;
   unsigned N1div ;
   unsigned HSdiv ;
   unsigned HSdiv_reg[6] ;
   unsigned HSdiv_val[6] ;
   unsigned char freqSI570_high_speed_rd_value[6];
   unsigned char freqSI570_high_speed_wr_value[6];

   if(osc > 3 || osc < 0){
      return -100;
   }

   HSdiv_reg[0] =  0 ;
   HSdiv_val[0] =  4 ;

   HSdiv_reg[1] =  1 ;
   HSdiv_val[1] =  5 ;

   HSdiv_reg[2] =  2 ;
   HSdiv_val[2] =  6 ;

   HSdiv_reg[3] =  3 ;
   HSdiv_val[3] =  7 ;

   HSdiv_reg[4] =  5 ;
   HSdiv_val[4] =  9 ;

   HSdiv_reg[5] =  7 ;
   HSdiv_val[5] =  11 ;

   HSdiv = 0xff ;
   for (i=0;i<6;i++){
      if (HSdiv_val[i] == hs_div_val) {
         HSdiv = HSdiv_reg[i] ;
      }
   }
   if (HSdiv > 11) {
      return -101;
   }

   // gtr than 127 or odd then return
   if((n1_div_val > 127) || ((n1_div_val & 0x1) == 1) || (n1_div_val == 0) ) {
      return -102;
   }
   N1div = n1_div_val - 1 ;

   rc = Si570ReadDivider(vme_handle,osc, freqSI570_high_speed_rd_value);
   if(rc){
      printf("Si570ReadDivider = %d \n",rc);
      return rc;
   }
   freqSI570_high_speed_wr_value[0] = ((HSdiv & 0x7) << 5) + ((N1div & 0x7c) >> 2);
   freqSI570_high_speed_wr_value[1] = ((N1div & 0x3) << 6) + (freqSI570_high_speed_rd_value[1] & 0x3F);
   freqSI570_high_speed_wr_value[2] = freqSI570_high_speed_rd_value[2];
   freqSI570_high_speed_wr_value[3] = freqSI570_high_speed_rd_value[3];
   freqSI570_high_speed_wr_value[4] = freqSI570_high_speed_rd_value[4];
   freqSI570_high_speed_wr_value[5] = freqSI570_high_speed_rd_value[5];

   rc = set_frequency(vme_handle,osc,freqSI570_high_speed_wr_value);
   if(rc){
      printf("set_frequency = %d \n",rc);
      return rc;
   }

   return 0;

} 
//_____________________________________________________________________________
int read_DMA_Channel_PreviousBankDataBuffer(int vme_handle,                    /* VME handle */ 
                                            unsigned int bank2_read_flag,      /* which bank? 0 => 1, 1=>2 */ 
                                            unsigned int channel_no,           /* 0 to 15 */
                                            unsigned int max_read_nof_words,   /* max number of words read */  
                                            unsigned int *dma_got_nof_words,   /* obtained number of words */ 
                                            u_int32_t *uint_adc_buffer,        /* output data? */
                                            u_int32_t NumOfSamples             /* number of samples */ 
                                            ){    

   // taken from SIS3316 DVD
   // seems to be the code to read the data out from the ADC... 

   int TestVal = 3; 

   int return_code;
   unsigned int data;
   unsigned int addr;
   unsigned int previous_bank_addr_value;
   unsigned int req_nof_32bit_words;
   unsigned int got_nof_32bit_words;
   unsigned int memory_bank_offset_addr;
   unsigned int max_poll_counter;

   // read previous Bank sample address
   // poll until it is valid.
   addr                 = SIS3316_ADC_CH1_PREVIOUS_BANK_SAMPLE_ADDRESS_REG + ((channel_no & 0x3) * 4) + (((channel_no >> 2) & 0x3) * SIS3316_FPGA_ADC_REG_OFFSET);
   max_poll_counter     = 10000;
   *dma_got_nof_words = 0;
   do {
      // printf("%d \n",__LINE__); 
      // get the previous (memory) bank address 
      return_code = SISRead32(vme_handle,addr,&previous_bank_addr_value); //  

      if(return_code != 0) { 
         printf("[SISRead32]: Error: vme_A32D32_read: return_code = 0x%08x   addr = 0x%08x \n", return_code,SIS3316_MOD_BASE+addr);
         return return_code;
      }

      max_poll_counter--;

      if (max_poll_counter == 0) { 
         printf("[SIS3316_um]: Error: max_poll_counter = 0x%08x   \n", max_poll_counter);
         return 0x900;
      }

   }while( ( (previous_bank_addr_value & 0x1000000) >> 24 )  != bank2_read_flag ); // previous Bank sample address is valid if bit 24 is equal bank2_read_flag 

   if(gIsDebug || gIsTest==TestVal) printf("[SIS3316_um]: addr = 0x%08x previous bank address value = 0x%08x \n",SIS3316_MOD_BASE+addr,previous_bank_addr_value);  
   // check the obtained previous bank address; return zero if
   // we have nothing. 
   if( (previous_bank_addr_value & 0xffffff)==0 ){ // no data sampled !
      printf("[SIS3316_um]: No data sampled! \n");
      printf("[SIS3316_um]: addr = 0x%08x previous bank address value = 0x%08x \n",SIS3316_MOD_BASE+addr,previous_bank_addr_value);  
      *dma_got_nof_words = 0;
      return 1;       
   } 
   // check the obtained previous bank address; return zero if
   // start Readout FSM
   // determine the memory bank offset address 
   if (bank2_read_flag == 1){             
      memory_bank_offset_addr = 0x01000000; // Bank2 offset
   }else{
      memory_bank_offset_addr = 0x00000000; // Bank1 offset
   }
   // check the obtained previous bank address; return zero if
   // now locate the desired channel number 
   if( (channel_no & 0x1) != 0x1 ){    // 0,1         
      memory_bank_offset_addr = memory_bank_offset_addr + 0x00000000; // channel 1 , 3, ..... 15
   }else{
      memory_bank_offset_addr = memory_bank_offset_addr + 0x02000000; // channel 2 , 4, ..... 16
   }
   if( (channel_no & 0x2) != 0x2 ){ // 0,2         
      memory_bank_offset_addr = memory_bank_offset_addr + 0x00000000; // channel 0,1 , 4,5, .....
   }else{
      memory_bank_offset_addr = memory_bank_offset_addr + 0x10000000; // channel 2,3 , 6,7 ..... 
   }
   // check the obtained previous bank address; return zero if

   // in case of using ETHERNET_UDP_INTERFACE it is possible to lose packets 
   // (observed with WIN7 using a "company-net", not observed with WIN7 and direct connection and not observed with LINUX)

   unsigned int retry_counter = 0 ;
   unsigned int retry_flag = 0 ;

   // data transfer control register read/write: my estimation (from page 109 of the manual) 
   // - bits 0--27:  Memory 32-bit start addresses (only have 28 bits to work with, possibly number of 32-bit words?)  
   // - bits 28--29: "space select bits" 
   // - bits 30--31: "command bits"
   // - space select bit table 
   //   bit 1    bit 0    hex            function 
   //   -------------------------------------------------------------------------
   //     0       0       0x0            Memory 1 (ch. 1 and 2) 
   //     0       1       0x40000000     Memory 2 (ch. 3 and 4) 
   //     1       0       0x80000000     Reserved  
   //     1       1       0xC0000000     Statistic counter (128 32-bit words)  
   //   -------------------------------------------------------------------------
   // - command bit table 
   //   bit 1    bit 0    hex            function 
   //   -------------------------------------------------------------------------
   //     0       -       0x0            Reset transfer FSM
   //     1       0       0x80000000     Start read transfer
   //     1       1       0xC0000000     Start write transfer        
   //   -------------------------------------------------------------------------

   // check the obtained previous bank address; return zero if
   u_int32_t START_READ_TRANSFER  = 0x80000000; 
   // u_int32_t START_WRITE_TRANSFER = 0xC0000000; 

   do{
      addr        = SIS3316_DATA_TRANSFER_CH1_4_CTRL_REG + ( ( (channel_no >> 2) & 0x3) * 4 );
      // data        = 0x80000000 + memory_bank_offset_addr;   
      data        = START_READ_TRANSFER + memory_bank_offset_addr;   
      // data = start read transfer (0x80000000) of the given address of channel no.  
      return_code = SISWrite32(vme_handle,addr,data);
      // return_code = i->vme_A32D32_write(this->baseaddress + addr, data);

      if(return_code != 0) { 
         // printf("Error: vme_A32D32_write: return_code = 0x%08x   addr = 0x%08x  data = 0x%08x \n", return_code,  this->baseaddress + addr, data);
         // return return_code;
      }else{
         // readout 
         addr                = SIS3316_MOD_BASE + SIS3316_FPGA_ADC1_MEM_BASE + (((channel_no >> 2) & 0x3 )* SIS3316_FPGA_ADC_MEM_OFFSET);
         req_nof_32bit_words = previous_bank_addr_value & 0xffffff ;  // get the lowest 24 bits 
         // printf("number of 32-bit words: %d (0x%08x) \n",req_nof_32bit_words,req_nof_32bit_words); 

         if(req_nof_32bit_words > max_read_nof_words){
            printf("[SIS3316_um]: Error: exceeded the allowed number of data words! Setting to maximum = %u. \n",max_read_nof_words); 
            req_nof_32bit_words = max_read_nof_words;
         }
         usleep(5); // wait 5 us before reading out data 
         // FIFORead(vme_handle,addr,NumOfSamples);  
         // FIFO: first in, first out 
         return_code = call_vme_A32MBLT64FIFO_read(vme_handle,addr,uint_adc_buffer,((req_nof_32bit_words + 1) & 0xfffffE),&got_nof_32bit_words); // N * 8-byte length  !!! 

         if(return_code != 0){ 
            printf("[SIS3316_um]: Error: vme_A32MBLT64FIFO_read: return_code = 0x%08x   addr = 0x%08x  req_nof_32bit_words = 0x%08x \n", 
                                                   return_code,SIS3316_MOD_BASE + addr,req_nof_32bit_words);
            return return_code;
         }

         // *dma_got_nof_words = req_nof_32bit_words;
         *dma_got_nof_words = got_nof_32bit_words;
      }

      retry_flag = 0 ;        

      if(return_code != 0) { 
         retry_counter++ ;
         retry_flag = 0 ;        
         if (retry_counter < 16) {
            retry_flag = 1 ;        
         }
         if (retry_counter > 1) {
            printf("[SIS3316_um]: Info: retry_counter = %d \n", retry_counter);
         }
      }

      // reset FSM again
      addr        = SIS3316_DATA_TRANSFER_CH1_4_CTRL_REG + (((channel_no >> 2) & 0x3) * 4) ;
      data        = 0x00000000; // Reset
      return_code = SISWrite32(vme_handle,addr,data);
      // return_code = i->vme_A32D32_write(this->baseaddress + addr, data);

      if(return_code != 0) { 
         //printf("Error: vme_A32D32_write: return_code = 0x%08x   addr = 0x%08x  data = 0x%08x \n", return_code,  this->baseaddress + addr, data);
         //return return_code;
      }

   }while(retry_flag==1); 

   if (retry_counter > 15) {
      printf("[SIS3316_um]: Tried too many times. \n"); 
      return -1 ;
   }

   return 0 ;

}
//_____________________________________________________________________________
int call_vme_A32MBLT64FIFO_read(int vme_handle, u_int32_t vme_adr, u_int32_t* vme_data,
                      u_int32_t req_num_of_lwords, u_int32_t* got_num_of_lwords){

   // pulled from sis1100linux_vme_class.cpp
   // inputs: 
   // - vme_handle        = VME handle  
   // - vme_adr           = VME address (including base address) 
   // - vme_data          = data we want to pull from ADC 
   // - req_num_of_lwords = requested number of data words 
   // - got_num_of_lwords = obtained number of data words 
   // output: 
   // - returns error code (0 if successful)  

   struct sis1100_vme_block_req block_req;
   block_req.num=req_num_of_lwords   ; /*  */
   block_req.fifo=1;
   block_req.size=4;
   block_req.am=0x8;
   block_req.addr=vme_adr ;
   block_req.data = (u_int8_t*)vme_data ;
   if (ioctl(vme_handle, SIS3100_VME_BLOCK_READ, &block_req)<0 )  return -1 ;   /* NEW */
   *got_num_of_lwords = block_req.num;
   return block_req.error;            /* NEW */

}
//______________________________________________________________________________
int I2cStart(int vme_handle,int osc){

   int rc;
   int i;
   unsigned int tmp;

   if(osc > 3){
      return -101;
   }

   // start
   rc = SISWrite32(vme_handle,SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), 1<<I2C_START);

   if(rc){
      return rc;
   }

   i = 0;

   do{
      // poll i2c fsm busy
      rc = SISRead32(vme_handle,SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), &tmp);
      if(rc){
         return rc;
      }
      i++;
   }while((tmp & (1<<I2C_BUSY)) && (i < 1000));

   // register access problem
   if(i == 1000){
      return -100;
   }

   return 0;

}
//______________________________________________________________________________
int I2cStop(int vme_handle,int osc){

   int rc;
   int i;
   unsigned int tmp;

   if(osc > 3){
      return -101;
   }

   // stop
   rc = SISWrite32(vme_handle,SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), 1<<I2C_STOP);
   if(rc){
      return rc;
   }

   i = 0;
   do{
      // poll i2c fsm busy
      rc = SISRead32(vme_handle,SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), &tmp);
      if(rc){
         return rc;
      }
      i++;
   }while((tmp & (1<<I2C_BUSY)) && (i < 1000));

   // register access problem
   if(i == 1000){
      return -100;
   }

   return 0;

}
//______________________________________________________________________________
int I2cWriteByte(int vme_handle,int osc, unsigned char data, char *ack){

   int rc;
   int i;
   unsigned int tmp;

   if(osc > 3){
      return -101;
   }

   // write byte, receive ack
   rc = SISWrite32(vme_handle,SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), 1<<I2C_WRITE ^ data);
   if(rc){
      return rc;
   }

   i = 0;
   do{
      // poll i2c fsm busy
      rc = SISRead32(vme_handle,SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), &tmp);
      if(rc){
         return rc;
      }
      i++;
   }while((tmp & (1<<I2C_BUSY)) && (i < 1000));

   // register access problem
   if(i == 1000){
      return -100;
   }

   // return ack value?
   if(ack){
      // yup
      *ack = tmp & 1<<I2C_ACK ? 1 : 0;
   }

   return 0;

}
//______________________________________________________________________________
int I2cReadByte(int vme_handle,int osc, unsigned char *data, char ack){

   int rc;
   int i;
   unsigned int tmp;
   unsigned char char_tmp; 

   if(osc > 3){
      return -101;
   }

   // read byte, put ack
   tmp = 1<<I2C_READ;
   tmp |= ack ? 1<<I2C_ACK : 0;
   rc = SISWrite32(vme_handle,SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), tmp);
   if(rc){
      return rc;
   }

   i = 0;
   do{
      // poll i2c fsm busy
      rc = SISRead32(vme_handle,SIS3316_ADC_CLK_OSC_I2C_REG + (4 * osc), &tmp);
      if(rc){
         return rc;
      }
      i++;
   }while((tmp & (1<<I2C_BUSY)) && (i < 1000));

   // register access problem
   if(i == 1000){
      return -100;
   }

   char_tmp = (unsigned char) (tmp & 0xff) ;

   *data = char_tmp ;

   return 0;

}
//______________________________________________________________________________
int Si570FreezeDCO(int vme_handle,int osc){

   int rc;
   char ack;

   // start
   rc = I2cStart(vme_handle,osc);

   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   // address
   rc = I2cWriteByte(vme_handle,osc,OSC_ADR<<1,&ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // register offset
   rc = I2cWriteByte(vme_handle,osc, 0x89, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // write data
   rc = I2cWriteByte(vme_handle,osc, 0x10, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // stop
   rc = I2cStop(vme_handle,osc);
   if(rc){
      return rc;
   }

   return 0;

}
//______________________________________________________________________________
int Si570ReadDivider(int vme_handle,int osc, unsigned char *data){

   int rc;
   char ack;
   int i;

   // start
   rc = I2cStart(vme_handle,osc);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   // address
   rc = I2cWriteByte(vme_handle,osc, OSC_ADR<<1, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // register offset
   rc = I2cWriteByte(vme_handle,osc, 0x0D, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   rc = I2cStart(vme_handle,osc);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   // address + 1
   rc = I2cWriteByte(vme_handle,osc, (OSC_ADR<<1) + 1, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // read data
   for(i = 0;i < 6;i++){
      ack = 1 ;
      if (i==5){
         ack = 0;
      }
      rc = I2cReadByte(vme_handle,osc, &data[i], ack);
      if(rc){
         I2cStop(vme_handle,osc);
         return rc;
      }
   }

   // stop
   rc = I2cStop(vme_handle,osc);
   if(rc){
      return rc;
   }

   return 0;

}
//______________________________________________________________________________
int Si570Divider(int vme_handle,int osc, unsigned char *data){

   int rc;
   char ack;
   int i;

   // start
   rc = I2cStart(vme_handle,osc);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   // address
   rc = I2cWriteByte(vme_handle,osc, OSC_ADR<<1, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // register offset
   rc = I2cWriteByte(vme_handle,osc, 0x0D, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // write data
   for(i = 0;i < 2;i++){
      rc = I2cWriteByte(vme_handle,osc, data[i], &ack);
      if(rc){
         I2cStop(vme_handle,osc);
         return rc;
      }
      if(!ack){
         I2cStop(vme_handle,osc);
         return -101;
      }
   }

   // stop
   rc = I2cStop(vme_handle,osc);
   if(rc){
      return rc;
   }

   return 0;

}
//______________________________________________________________________________
int Si570UnfreezeDCO(int vme_handle,int osc){

   int rc;
   char ack;

   // start
   rc = I2cStart(vme_handle,osc);

   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   // address
   rc = I2cWriteByte(vme_handle,osc, OSC_ADR<<1, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // register offset
   rc = I2cWriteByte(vme_handle,osc, 0x89, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // write data
   rc = I2cWriteByte(vme_handle,osc, 0x00, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // stop
   rc = I2cStop(vme_handle,osc);
   if(rc){
      return rc;
   }

   return 0;

}
//______________________________________________________________________________
int Si570NewFreq(int vme_handle,int osc){

   int rc;
   char ack;

   // start
   rc = I2cStart(vme_handle,osc);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   // address
   rc = I2cWriteByte(vme_handle,osc,OSC_ADR<<1, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;

   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // register offset
   rc = I2cWriteByte(vme_handle,osc, 0x87, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;

   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // write data
   rc = I2cWriteByte(vme_handle,osc, 0x40, &ack);
   if(rc){
      I2cStop(vme_handle,osc);
      return rc;
   }

   if(!ack){
      I2cStop(vme_handle,osc);
      return -101;
   }

   // stop
   rc = I2cStop(vme_handle,osc);
   if(rc){
      return rc;
   }

   return 0;

}
//______________________________________________________________________________
int set_frequency(int vme_handle,int osc, unsigned char *values){

   int rc;

   if(values == NULL){
      return -100;
   }

   if(osc > 3 || osc < 0){
      return -100;
   }

   rc = Si570FreezeDCO(vme_handle,osc);
   if(rc){
      return rc;
   }

   rc = Si570Divider(vme_handle,osc, values);
   if(rc){
      return rc;
   }

   rc = Si570UnfreezeDCO(vme_handle,osc);
   if(rc){
      return rc;
   }

   rc = Si570NewFreq(vme_handle,osc);
   if(rc){
      return rc;
   }

   // min. 10ms wait
   usleep(15000); // 15 ms

   // DCM Reset  
   rc = SISWrite32(vme_handle,0x438,0);

   if(rc){
      return rc;
   }

   // DCM Reset -> the DCM/PLL of the ADC-FPGAs will be stable after max. 5ms
   //              or check the DCM OK bits (ADC FPGA Status registers bit 20)
   usleep(5000); // 5 ms

   return 0;

}
//______________________________________________________________________________
int SIS3316IsEventAvailable(int vme_handle){

   // Check acquisition register to see if the event is available.
   int rc=0,count=0;
   u_int32_t data32=0; 

   // uint msg = 0;
   int is_event = 0; 
   // static bool is_event;
   // static int count, rc;

   int max = 100; 
 
   do {
      // rc = Read(0x10, msg);
      rc = SISRead32(vme_handle,SIS3316_INTERFACE_ACCESS_ARBITRATION_CONTROL,&data32);
      count++;
   }while( (rc < 0) && (count < max) );   // FIXME: why 100? 

   // is_event = !(msg & 0x10000);
   is_event    = !(data32 & 0x10000);     // FIXME: Why do we do this? 

   // uint armit = 1;
   // u_int32_t armit = 1;  

   // if (is_event && go_time_){          // FIXME: What is "go_time_"? 
   //    // rearm the logic
   //    armit = 1;
   //    count = 0;
   //    rc    = 0;
   //    do {
   //       // rc = Write(0x410, armit);
   //       rc = Write32(vme_handle,SIS3316_KEY_ARM,armit);
   //       count++;
   //    }while( (rc < 0) && (count < max) );
   //    return is_event;
   // }

   return 0;      // 0 = false, 1 = true

}


