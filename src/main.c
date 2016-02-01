// This code runs the DAQ for the NMR system, incorporating the FPGA, 
// digitizer (ADC) and Stanford Research Systems SG-382 function generator.
// See the README.md file for details. 

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "util.h"
#include "run.h"
#include "adc.h"
#include "fpga.h"
#include "FuncGen.h"
#include "acromag_ipep201.h"
#include "sg382.h"
#include "struck_adc.h"
#include "acquisition.h"
#include "diagnostics.h"

int OpenVME(int argc, char* argv[]);

int main(int argc, char* argv[]){

   // GetTimeStamp(); 
   // CheckHardware(); 

   // import debug mode options 
   ImportUtilityData(); 

   // get output directory 
   const int MAX    = 2000; 
   char *output_dir = (char*)malloc( sizeof(char)*(MAX+1) );  

   struct run myRun; 

   if(gIsTest==0 || gIsTest==5){ 
      output_dir = GetDirectoryName(&myRun);
      printf("[NMRDAQ]: Output directory: %s \n",output_dir);  
   }

   // import comments about the run 
   const int cSIZE = 1000; 
   char **comment; 
   comment = (char**)malloc( sizeof(char*)*cSIZE );
   int NumComment = ImportComments(comment); 

   // time stamp for each pulse
   const int NPULSE = 2000; // arbitrary large number 
   const int NDATA  = 6;    // 6 entries for date info 
   unsigned long **timestamp = (unsigned long **)malloc( sizeof(unsigned long *)*NPULSE ); 
   int i=0;
   for(i=0;i<NPULSE;i++) timestamp[i] = (unsigned long *)malloc( sizeof(unsigned long)*NDATA );

   // set up the SG-382 function generator 
   int ret_val_fg=0;
   struct FuncGen myFuncGen; 
   ret_val_fg = InitFuncGen(&myFuncGen);       // pass by reference to modify contents of myFuncGen 

   if(ret_val_fg!=0){
      printf("[NMRDAQ]: SG382 initialization FAILED.  Exiting... \n"); 
      exit(1);
   }

   if(gIsTest<2 || gIsTest==4 || gIsTest==5){
      ProgramFuncGen(myFuncGen); 
   }

   // open the VME connection 
   int p = OpenVME(argc,argv); 

   // set up the FPGA 
   int ret_val_fpga=0;
   struct fpga myFPGA;
   ret_val_fpga = InitFPGA(p,&myFPGA);             // pass by reference to modify contents of myFPGA 

   if(ret_val_fpga!=0){
      printf("[NMRDAQ]: Acromag FPGA initialization FAILED.  Exiting... \n"); 
      exit(1);
   }

   if(gIsTest<2 || gIsTest==4 || gIsTest==5){
      ProgramSignalsToFPGA(p,myFPGA); 
   }

   // SIS ADC struct  
   struct adc myADC; 

   // return codes 
   int ret_val_daq = 0;
   int ret_val_adc = 0;
  
   if(gIsTest==2 || gIsTest==3){
      // just want to read the I/O space of the ADC 
      ret_val_adc = SISInit(p,&myADC);
   }

   if(gIsTest==1||gIsTest==4){
      // for testing 
      NextAction(p,&myFuncGen,&myFPGA);
   }else if(gIsTest==0 || gIsTest==5){
      // initialize the ADC 
      ret_val_adc = SISInit(p,&myADC);
      if(ret_val_adc!=0){
         ShutDownSystem(p,&myFuncGen,&myFPGA); 
      }else{
         // acquire data
         ret_val_daq = AcquireData(p,myFPGA,myADC,timestamp,output_dir); 

         // shut down the system and print data to file  
         ShutDownSystem(p,&myFuncGen,&myFPGA); 

         // print data to file(s) 
         if(ret_val_daq==0){
            printf("[NMRDAQ]: Printing diagnostic data to file(s)... \n");  
            // if(ADC_MULTIEVENT_STATE==1) SISWriteNMRPulses(p,myADC,output_dir);
            // if(ADC_MULTIEVENT_STATE==1) SISWriteNMRPulsesAlt(p,myADC,output_dir);  // binary output
            PrintDiagnostics(output_dir,NumComment,comment,myFuncGen,myFPGA,myADC);
            PrintRunSummary(output_dir,myRun,myFuncGen,myFPGA,myADC);
            PrintTimeStampMicroSec(output_dir,myADC,timestamp); 
            close(p);
         }else{
            printf("[NMRDAQ]: Something is wrong with the software or the system!"); 
            printf("  No data recorded to files. \n"); 
            close(p);
         }
      }
   }

   printf("============================================================ \n"); 

   return 0; 
}
//______________________________________________________________________________
int OpenVME(int argc, char *argv[]){

   // Open VME connection 
   int p; 
   if(argc<3){
      fprintf(stderr, "usage: %s  path  VME_BASE_ADDR\n", argv[0]);
      return -1;
   }

   if((p=open(argv[1], O_RDWR, 0))<0){
      perror("open");
      printf("[NMRDAQ]: Cannot open VME connection. \n"); 
      return 1;
   }else{
      printf("[NMRDAQ]: VME connection opened successfully. \n"); 
   }

   return p; 

}

