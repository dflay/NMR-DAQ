#include "struck_adc.h"
//______________________________________________________________________________
int SISInitGlobalVariables(const struct adc myADC){

   // initialize ADC parameters for the rest of the code  
   int ret_val          = 0; 
   int adcID            = myADC.fID; 

   PULSES_PER_READ      = 5;           // every 5 pulses do a block-read of the memory  
   ADC_MULTIEVENT_STATE = myADC.fMultiEventState; 

   if(adcID==3302){
      MOD_BASE = SIS3302_MOD_BASE;
   }else if(adcID==3316){
      MOD_BASE = SIS3316_MOD_BASE;
   }else{
      printf("[StruckADC]: Invalid ADC ID = %d!  Exiting... \n",adcID);
      ret_val = 1; 
   }
   
   if(gIsDebug) printf("[StruckADC]: Module base address = 0x%08x \n",MOD_BASE); 

   return ret_val;

}
//______________________________________________________________________________
int SISInit(int vme_handle,struct adc *myADC){

   int ret_code = 1;
   u_int32_t data32=0;  

   // Initialize (or reset) the StruckADC to NMR signal-gathering configuration
   InitADCStruct(myADC); 

   char *filename = "./input/struck_adc.dat"; 
   SISImportData(filename,myADC); 
   PrintADC(*myADC); 

   int adcID = myADC->fID;
 
   // array for output data
   const int NUM_SAMPLES = myADC->fNumberOfSamples; 
   gDATA                 = (u_int32_t *)malloc( sizeof(u_int32_t)*NUM_SAMPLES );
   gDATA_us              = (unsigned short *)malloc( sizeof(unsigned short)*NUM_SAMPLES );

   int global_vars_set   = SISInitGlobalVariables(*myADC); 

   if(gIsTest<2 || gIsTest==5){
      if(adcID==3302) ret_code = SIS3302Init(vme_handle,myADC); 
      if(adcID==3316) ret_code = SIS3316Init(vme_handle,*myADC);
   }else if(gIsTest==2){
      // ADC test mode, don't need to go any further
      // read the module ID and quit 
      // SISIOSpaceRead(vme_handle,*myADC);
      if(adcID==3302) data32 = SIS3302_MODID;  
      if(adcID==3316) data32 = SIS3316_MODID;  
      SISMODID(vme_handle,data32);  
      return 0;
   }else if(gIsTest==3){
      // test the SIS3316  
      ret_code = SIS3316Test(vme_handle,*myADC); 
   }

   if(global_vars_set!=0){
      // global variables not set properly; return 1 (error) 
      return 1;
   }

   return ret_code;
}
//_____________________________________________________________________________
void PrintADC(const struct adc myADC){

   char *clock_type = (char *)malloc( sizeof(char)*100 ); 
   clock_type = "Internal"; 
   if(myADC.fClockType==1) clock_type = "External";  

   printf("[StruckADC]: ADC Name:                      %s      \n",myADC.fName); 
   printf("[StruckADC]: Channel number:                %d      \n",myADC.fChannelNumber); 
   printf("[StruckADC]: Number of Events (NMR pulses): %d      \n",myADC.fNumberOfEvents); 
   printf("[StruckADC]: Clock Type:                    %s      \n",clock_type); 
   printf("[StruckADC]: Clock Frequency:               %.2E Hz \n",myADC.fClockFrequency); 
   printf("[StruckADC]: Clock Frequency Input Units:   %s      \n",myADC.fClockFreqUnits); 
   printf("[StruckADC]: Clock Period:                  %.2E s  \n",myADC.fClockPeriod); 
   printf("[StruckADC]: Signal Length:                 %.2E s  \n",myADC.fSignalLength); 
   printf("[StruckADC]: Signal Length Input Units:     %s      \n",myADC.fSignalLengthUnits); 
}
//_____________________________________________________________________________
void InitADCStruct(struct adc *myADC){
   const int SIZE            =  100;
   // char *adc_name            = (char *)malloc( sizeof(char)*(SIZE+1) );
   myADC->fName              = (char *)malloc( sizeof(char)*(SIZE+1) );
   // sprintf(name,"Struck SIS %d ADC",myADC->fID); 
   // strcpy(myADC->fName,adc_name);
   myADC->fID                =  0; 
   myADC->fClockFrequency    =  0;
   myADC->fClockPeriod       =  0; 
   myADC->fSignalLength      =  0;
   myADC->fClockType         =  0; // 0 = internal, 1 = external   
   myADC->fMultiEventState   = -1; // 0 = disabled, 1 = enabled 
}
//_____________________________________________________________________________
void SISImportData(char *filename,struct adc *myADC){

   double ivalue;
   int j=0,k=0,N=0;
   const int MAX =2000;
   const int uMAX=10;
   const int tMAX=20;
   char buf[MAX],itag[tMAX],iunit[uMAX];
   char *mode        = "r";
   char *freq        = "frequency";
   // char *sig_len     = "signal_length";
   char *ext_clk     = "external_clock";
   char *num_events  = "number_of_events"; 
   char *multi_event = "multi_event_state";
   char *adc_id      = "adc_id";
   char *ch_num      = "channel_number"; 

   // memory allocation 
   myADC->fClockFreqUnits    = (char*)malloc( sizeof(char)*(uMAX+1) );
   myADC->fSignalLengthUnits = (char*)malloc( sizeof(char)*(uMAX+1) );

   FILE *infile;
   infile = fopen(filename,mode);

   if(infile==NULL){
      printf("[StruckADC::ImportStruckADCData]: Cannot open the file: %s.  Exiting... \n",filename);
      exit(1);
   }else{
      if(gIsDebug) printf("[StruckADC::ImportStruckADCData]: Opening the file: %s... \n",filename);
      while( !feof(infile) ){
         if(k==0){
            fgets(buf,MAX,infile);
         }else{
            fscanf(infile,"%s %lf %s",itag,&ivalue,iunit);
            if(gIsDebug && gVerbosity>=1) printf("%s %.2f %s \n",itag,ivalue,iunit);
            if( !AreEquivStrings(itag,eof_tag) ){
               // ADC ID 
               if( AreEquivStrings(itag,adc_id) ){
                  myADC->fID = (int)ivalue; 
               // channel number
               }else if ( AreEquivStrings(itag,ch_num) ){ 
                  myADC->fChannelNumber = (int)ivalue; 
               // number of events 
               }else if( AreEquivStrings(itag,num_events) ){
                  myADC->fNumberOfEvents = (int)ivalue; 
               // clock frequency 
               }else if( AreEquivStrings(itag,freq) ){
                  if( AreEquivStrings(iunit,kHz) ){
                     ivalue *= 1E+3; 
                  }else if( AreEquivStrings(iunit,MHz) ){
                     ivalue *= 1E+6; 
                  }else if( AreEquivStrings(iunit,GHz) ){
                     ivalue *= 1E+9; 
                  }
                  myADC->fClockFrequency = ivalue;
                  strcpy(myADC->fClockFreqUnits,iunit);
               // external clock
               }else if( AreEquivStrings(itag,ext_clk) ){
                  if( AreEquivStrings(iunit,kHz) ){
                     ivalue *= 1E+3; 
                  }else if( AreEquivStrings(iunit,MHz) ){
                     ivalue *= 1E+6; 
                  }else if( AreEquivStrings(iunit,GHz) ){
                     ivalue *= 1E+9; 
                  }
                  if(ivalue>0){
                     // we're using an external clock
                     myADC->fClockType = 1; 
                     // OVERWRITE fClockFrequency since we're using an external clock 
                     myADC->fClockFrequency = ivalue;  
                     strcpy(myADC->fClockFreqUnits,iunit);
                  }
               // single or multi-event state? 
               }else if( AreEquivStrings(itag,multi_event) ){
                  myADC->fMultiEventState = (int)ivalue;
               }
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
         printf("[StruckADC::ImportStruckADCData]: No data!  Exiting... \n");
         exit(1);
      }
   }

   if(gIsDebug) printf("RECEIVE_GATE_TIME_SEC = %lf \n",RECEIVE_GATE_TIME_SEC); 

   myADC->fSignalLength    = RECEIVE_GATE_TIME_SEC; 
   strcpy(myADC->fSignalLengthUnits,RECEIVE_GATE_INPUT_TIME_UNITS);

   myADC->fClockPeriod     = 1./myADC->fClockFrequency; 
   myADC->fNumberOfSamples = (int)( (myADC->fClockFrequency)*(myADC->fSignalLength) ); 

   const int SIZE = 100; 
   char *name = (char *)malloc( sizeof(char)*(SIZE+1) );
   sprintf(name,"Struck SIS%d ADC",myADC->fID); 
   strcpy(myADC->fName,name);

   if(gIsDebug) printf("----------------------------------------- \n");

}
//_____________________________________________________________________________
int FIFORead(int vme_handle,int an_addr,u_int32_t NUM_SAMPLES){
   int ret_code = 0; 
   int addr = MOD_BASE + an_addr; 
   u_int32_t NumWords = 0;
   ret_code = vme_A32_2EVME_read(vme_handle,addr,&gDATA[0],NUM_SAMPLES/2,&NumWords);
   return ret_code; 
}
//______________________________________________________________________________
int SISIOSpaceRead(int vme_handle,u_int32_t MODID,const struct adc myADC){

   int ret_code;
   u_int32_t data32 = 0; 
   ret_code = SISRead32(vme_handle,MODID,&data32);

   printf("[StruckADC]: I/O space read: \n");

   int adcID = myADC.fID; 

   const int NBITS = 32; 

   const int SIZE = 4; 
   char *my_ascii   = (char *)malloc( sizeof(char)*(SIZE+1) );
   char *my_ascii_2 = (char *)malloc( sizeof(char)*(SIZE+1) );
   sprintf(my_ascii,"%d",adcID);  

   const int N = SIZE*8 + 1;
   int binary_code[N];  // binary when we convert adcID from ascii   
 
   printf("[StruckADC]: SIS%d ADC should be: ",adcID); 
   AsciiToBinary(SIZE,my_ascii,binary_code);

   my_ascii_2             = BinaryToAscii(NBITS,binary_code);
   unsigned long my_hex_2 = BinaryToHex32(binary_code); 

   int counter=0;
   int i=0;
   for(i=NBITS-1;i>=0;i--){
      counter++; 
      printf("%d",binary_code[i]);
      if(counter==8){
         printf(" "); 
         counter = 0;
      }
   }

   printf("\t"); 
   printf("(ascii = %s) \t",my_ascii_2); 
   printf("(hex = 0x%08lx) \n",my_hex_2); 

   printf("[StruckADC]: I/O space read gives:  "); 

   int binary_adc[NBITS];   // binary we get from the ADC 

   counter = 0; 
   for(i=NBITS-1;i>=0;i--){
      counter++;
      binary_adc[i] = GetBit(i,data32); 
      printf("%d",binary_adc[i]);
      if(counter==8){
         printf(" "); 
         counter = 0;
      }
   }
   printf("\t"); 
   char *my_ascii_adc         = BinaryToAscii(NBITS,binary_adc); 
   unsigned long my_hex_adc   = BinaryToHex32(binary_adc); 
   printf("(ascii = %s) \t",my_ascii_adc); 
   printf("(hex = 0x%08lx) \n",my_hex_adc); 

   // compare the two arrays; if all is good, then these should be identical
   for(i=0;i<NBITS;i++){
      if(binary_code[i]!=binary_adc[i]) ret_code = 1; 
   }

   if(ret_code==0){
      printf("[StruckADC]: ADC is verified as SIS%d     \n",adcID); 
   }else if(ret_code==1){ 
      printf("[StruckADC]: ADC is NOT verified as SIS%d \n",adcID);
   } 

   return ret_code; 

}

