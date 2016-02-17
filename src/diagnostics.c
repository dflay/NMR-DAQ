#include "diagnostics.h"
//______________________________________________________________________________
void CheckHardware(void){

   const int SIZE = 1; 
   char user_response[SIZE];
   char *no = "n";  
   char *NO = "N";  
   char *yes = "y"; 
   char *YES = "Y"; 

   printf("[NMRDAQ]: Is everything powered on correctly? [y/n]: ");
   scanf("%s",user_response);  

   if( AreEquivStrings(user_response,no) || AreEquivStrings(user_response,NO) ){
      printf("[NMRDAQ]: Check all settings.  Exiting... \n");
      exit(1);
   }else if( AreEquivStrings(user_response,yes) || AreEquivStrings(user_response,YES) ){
      printf("[NMRDAQ]: Ready to go! \n");
   }else{
      printf("[NMRDAQ]: Invalid response.  Exiting... \n");
      exit(1);
   } 

}
//______________________________________________________________________________
void PrintTimeStamp(char *prefix,const struct adc myADC,unsigned long **timestamp){

   int i=0; 
   unsigned long hour=0,min=0,sec=0;

   int N = myADC.fNumberOfEvents; 

   const int cSIZE = 1000;
   char *outpath = (char *)malloc( sizeof(char)*(cSIZE+1) );
   sprintf(outpath,"%s/timestamps.dat",prefix); 

   FILE *outfile;
   outfile = fopen(outpath,"w");
   if(outfile==NULL){
      printf("[NMRDAQ]: Cannot open the file: %s.  The data will NOT be written to file. \n",outpath);
   }else{
      for(i=0;i<N;i++){
         hour = timestamp[i][3]; 
         min  = timestamp[i][4]; 
         sec  = timestamp[i][5]; 
         // printf("%-2d %-2d %-2d %-2d\n",i+1,hour,min,sec);
         fprintf(outfile,"%-2d %-2lu %-2lu %-2lu\n",i+1,hour,min,sec);
      }
      fclose(outfile);
      printf("[NMRDAQ]: Time stamps written to the file: %s \n",outpath);
   }

}
//______________________________________________________________________________
void PrintTimeStampMicroSec(char *prefix,const struct adc myADC,unsigned long **timestamp){

   int i=0; 
   unsigned long hour=0,min=0,sec=0;

   int N = myADC.fNumberOfEvents; 

   const int cSIZE = 1000;
   char *outpath = (char *)malloc( sizeof(char)*(cSIZE+1) );
   sprintf(outpath,"%s/timestamps.dat",prefix); 

   FILE *outfile;
   outfile = fopen(outpath,"w");
   if(outfile==NULL){
      printf("[NMRDAQ]: Cannot open the file: %s.  The data will NOT be written to file. \n",outpath);
   }else{
      for(i=0;i<N;i++){
         hour = timestamp[i][3]; 
         min  = timestamp[i][4]; 
         sec  = timestamp[i][5]; 
         fprintf(outfile,"%-2d %-2lu %-2lu %-2lu\n",i+1,hour,min,sec);
      }
      fclose(outfile);
      printf("[NMRDAQ]: Time stamps written to the file: %s \n",outpath);
   }

}
//______________________________________________________________________________
void PrintMechSwIndex(char *prefix,const struct run myRun,const struct adc myADC,int *MECH){

   int i=0; 
   int N = myADC.fNumberOfEvents;

   const int cSIZE = 1000;
   char *outpath = (char *)malloc( sizeof(char)*(cSIZE+1) );
   sprintf(outpath,"%s/mech-sw.dat",prefix); 

   FILE *outfile;
   outfile = fopen(outpath,"a");
   if(outfile==NULL){
      printf("[NMRDAQ]: Cannot open the file: %s.  The data will NOT be written to file. \n",outpath);
   }else{
      for(i=0;i<N;i++){
         fprintf(outfile,"%-2d %-2d %-2d \n",myRun.fRunNumber,i+1,MECH[i]);
      }
      fclose(outfile);
      printf("[NMRDAQ]: Mechanical switch indices written to the file: %s \n",outpath);
   }

}
//______________________________________________________________________________
void PrintRunSummary(char *outdir,
                     const struct run     myRun,
                     const struct FuncGen myFuncGen, 
                     const struct fpga    myFPGA, 
                     const struct adc     myADC){

   double Freq_LO    = myFuncGen.fFrequency; 
   char *units       = myFuncGen.fFreqUnits; 
   if( AreEquivStrings(units,kHz) ) Freq_LO *= 1E+3; 
   if( AreEquivStrings(units,MHz) ) Freq_LO *= 1E+6; 
   if( AreEquivStrings(units,GHz) ) Freq_LO *= 1E+9; 

   double Freq_IF    = fabs(gFreq_RF - Freq_LO); 

   if(Freq_IF<0) Freq_IF = Freq_LO; // probably a test run if this happens  

   char *mode        = "w";
   char *filename    = "summary.dat";
   const int MAX     = 2000; 
   char *outpath     = (char*)malloc( sizeof(char)*(MAX+1) );  
   sprintf(outpath,"%s/%s",outdir,filename); 

   FILE *outfile;
   outfile = fopen(outpath,mode);

   if(outfile==NULL){
      printf("[NMRDAQ]: Cannot open the file: %s.  Exiting... \n",outpath);
      exit(1);
   }else{
      fprintf(outfile,"run_number            %d    \n",myRun.fRunNumber       );
      fprintf(outfile,"num_pulses            %d    \n",myADC.fNumberOfEvents  );
      fprintf(outfile,"adc_id                %d    \n",myADC.fID              );
      fprintf(outfile,"adc_channel_number    %d    \n",myADC.fChannelNumber   );
      fprintf(outfile,"adc_number_of_samples %d    \n",myADC.fNumberOfSamples );
      fprintf(outfile,"adc_clock_frequency   %.7f  \n",myADC.fClockFrequency  );
      fprintf(outfile,"adc_signal_length     %.7f  \n",myADC.fSignalLength    );
      fprintf(outfile,"expected_IF_frequency %.7f  \n",Freq_IF                );
      fprintf(outfile,"LO_frequency          %.7f  \n",Freq_LO                );
      fprintf(outfile,"RF_frequency          %.7f  \n",gFreq_RF               );
      fprintf(outfile,"bnc_voltage           %.7f  \n",myFuncGen.fBNCVoltage  );
      fprintf(outfile,"ntype_voltage         %.7f  \n",myFuncGen.fNTypeVoltage);
      fclose(outfile); 
      printf("[NMRDAQ]: Run summary written to the file: %s \n",outpath);
   }

}
//______________________________________________________________________________
void PrintDiagnostics(char *outdir,int NumComments,char **comment,
                      const struct FuncGen myFuncGen, 
                      const struct fpga    myFPGA, 
                      const struct adc     myADC){

   int i=0; 
   int N = myFPGA.fNSignals;
   int start_counts = 0; 
   int pulse_counts = 0; 
   int end_counts   = 0; 

   double start_time    = 0; 
   double pulse_time    = 0; 
   double end_time      = 0; 
   double fpgaClockFreq = myFPGA.fClockFrequency; 

   char *int_clk     = "Internal"; 
   char *ext_clk     = "External";
   char *disabled    = "Disabled"; 
   char *enabled     = "Enabled"; 
   char *unknown     = "Unknown"; 
   char *mode        = "w";
   char *filename    = "diagnostics.dat";
   const int MAX     = 2000; 
   char *outpath     = (char*)malloc( sizeof(char)*(MAX+1) );  
   char *clk_type    = (char*)malloc( sizeof(char)*(MAX+1) ); 
   char *multi_event = (char*)malloc( sizeof(char)*(MAX+1) ); 
   sprintf(outpath,"%s/%s",outdir,filename); 

   int ClkType = myADC.fClockType;
   if(ClkType==0){
      sprintf(clk_type,"%s",int_clk); 
   }else if(ClkType==1){
      sprintf(clk_type,"%s",ext_clk); 
   } 

   int MultiEventState = myADC.fMultiEventState;
   if(MultiEventState==0){
      sprintf(multi_event,"%s",disabled);
   }else if(MultiEventState==1){
      sprintf(multi_event,"%s",enabled);
   }else{
      sprintf(multi_event,"%s",unknown);
   }

   FILE *outfile;
   outfile = fopen(outpath,mode);

   if(outfile==NULL){
      printf("[NMRDAQ]: Cannot open the file: %s.  Exiting... \n",outpath);
      exit(1);
   }else{
      fprintf(outfile,"--------------------------- ADC Data ---------------------------\n"                      );
      fprintf(outfile,"ADC Name                     = %s      \n",myADC.fName                                   );
      fprintf(outfile,"Multi-Event Mode             = %s      \n",multi_event                                   );
      fprintf(outfile,"Number of Events (NMR pulses)= %d      \n",myADC.fNumberOfEvents                         );
      fprintf(outfile,"Number of Samples            = %d      \n",myADC.fNumberOfSamples                        );
      fprintf(outfile,"Clock type                   = %s      \n",clk_type                                      ); 
      fprintf(outfile,"Clock Frequency              = %.2E Hz \n",myADC.fClockFrequency                         );
      fprintf(outfile,"Clock Frequency Input Units  = %s      \n",myADC.fClockFreqUnits                         );
      fprintf(outfile,"Clock Period                 = %.2E s  \n",myADC.fClockPeriod                            );
      fprintf(outfile,"Signal Length                = %.2E s  \n",myADC.fSignalLength                           );
      fprintf(outfile,"Signal Length Input Units    = %s      \n",myADC.fSignalLengthUnits                      );
      fprintf(outfile,"-------------------------- FPGA Data ---------------------------\n"                      );
      for(i=0;i<N;i++){
         start_counts = myFPGA.fSignalStartTimeLo[i] + pow(2,16)*myFPGA.fSignalStartTimeHi[i];
         pulse_counts = myFPGA.fSignalPulseTimeLo[i] + pow(2,16)*myFPGA.fSignalPulseTimeHi[i];
         end_counts   = myFPGA.fSignalEndTimeLo[i]   + pow(2,16)*myFPGA.fSignalEndTimeHi[i];
         start_time   = GetTimeInSeconds(start_counts,fpgaClockFreq);
         pulse_time   = GetTimeInSeconds(pulse_counts,fpgaClockFreq);
         end_time     = GetTimeInSeconds(end_counts,fpgaClockFreq);
         fprintf(outfile,"Name                         = %-15s    \n",myFPGA.fSignalName[i]                       );
         fprintf(outfile,"Units                        = %-2s     \n",myFPGA.fSignalUnits[i]                      );
         fprintf(outfile,"Flag                         = %-1d     \n",myFPGA.fFlag[i]                             );
         fprintf(outfile,"Start addr (l)               = 0x%04x   \n",myFPGA.fSignalAddrStartLo[i]                );
         fprintf(outfile,"Start addr (h)               = 0x%04x   \n",myFPGA.fSignalAddrStartHi[i]                );
         fprintf(outfile,"End addr (l)                 = 0x%04x   \n",myFPGA.fSignalAddrEndLo[i]                  );
         fprintf(outfile,"End addr (h)                 = 0x%04x   \n",myFPGA.fSignalAddrEndHi[i]                  );
         fprintf(outfile,"Start time (l)               = %-6d     \n",myFPGA.fSignalStartTimeLo[i]                );
         fprintf(outfile,"Start time (h)               = %-6d     \n",myFPGA.fSignalStartTimeHi[i]                );
         fprintf(outfile,"Start time                   = %-6.3E s \n",start_time                                  );
         fprintf(outfile,"End time (l)                 = %-6d     \n",myFPGA.fSignalEndTimeLo[i]                  );
         fprintf(outfile,"End time (h)                 = %-6d     \n",myFPGA.fSignalEndTimeHi[i]                  );
         fprintf(outfile,"End time                     = %-6.3E s \n",end_time                                    );
         fprintf(outfile,"Pulse time (l)               = %-6d     \n",myFPGA.fSignalPulseTimeLo[i]                );
         fprintf(outfile,"Pulse time (h)               = %-6d     \n",myFPGA.fSignalPulseTimeHi[i]                );
         fprintf(outfile,"Pulse time                   = %-6.3E s \n",pulse_time                                  );
         if(i<N-1) fprintf(outfile,"----------------------------------------\n"); 
      }
      fprintf(outfile,"----------------- Function Generator Data ----------------------\n"                      );
      fprintf(outfile,"Name                         = %s      \n",myFuncGen.fName                               );
      fprintf(outfile,"Frequency                    = %s      \n",myFuncGen.fFreqCommand                        );
      fprintf(outfile,"BNC voltage                  = %s      \n",myFuncGen.fBNCCommand                         );
      fprintf(outfile,"N-Type voltage               = %s      \n",myFuncGen.fNTypeCommand                       );
      fprintf(outfile,"BNC state                    = %d (%s) \n",myFuncGen.fIntBNCState  ,myFuncGen.fBNCState  );
      fprintf(outfile,"N-Type state                 = %d (%s) \n",myFuncGen.fIntNTypeState,myFuncGen.fNTypeState);
      fprintf(outfile,"----------------------------------------\n"); 
      fprintf(outfile,"Comments: \n");
      for(i=0;i<NumComments;i++){
         fprintf(outfile,"%s \n",comment[i]);
       }
      fclose(outfile); 
      printf("[NMRDAQ]: Diagnostic data written to the file: %s \n",outpath);
   }

}
