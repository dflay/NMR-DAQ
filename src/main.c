// This code runs the DAQ for the NMR system, incorporating the FPGA, 
// digitizer (ADC) and Stanford Research Systems SG-382 function generator.
// See the README.md file for details. 

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "util.h"
#include "nmr_math.h"
#include "run.h"
#include "adc.h"
#include "fpga.h"
#include "fpgaPulseSequence.h"
#include "FuncGen.h"
#include "acromag_ipep201.h"
#include "sg382.h"
#include "struck_adc.h"
#include "acquisition.h"
#include "diagnostics.h"

int OpenVME(int argc, char* argv[]);

int main(int argc, char* argv[]){

   int i=0;

   // import debug mode options 
   ImportUtilityData(); 

   // get output directory 
   const int MAX    = 2000; 
   char *output_dir = (char*)malloc( sizeof(char)*(MAX+1) );  

   struct run myRun; 

   if(gIsTest==0 || gIsTest==5){ 
      output_dir = GetDirectoryName(&myRun);
      printf("[NMRDAQ]: --------------------------- STARTING RUN %05d ---------------------------  \n",myRun.fRunNumber);
      printf("[NMRDAQ]: The date is: %02d %02d %d \n",myRun.fMonth,myRun.fDay,myRun.fYear);
      printf("[NMRDAQ]: The time is: %02d:%02d:%02d \n",myRun.fHour_start,myRun.fMinute_start,myRun.fSecond_start);
      printf("[NMRDAQ]: Output directory: %s \n" ,output_dir);  
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
   for(i=0;i<NPULSE;i++) timestamp[i] = (unsigned long *)malloc( sizeof(unsigned long)*NDATA );

   int *MECH = (int *)malloc( sizeof(int)*NPULSE );
   for(i=0;i<NPULSE;i++){
      MECH[i] = 0;
   }

   // set up the SG-382 function generator 
   int ret_val_fg=0;
   struct FuncGen myFuncGen; 
   ret_val_fg = InitFuncGen(&myFuncGen);       // pass by reference to modify contents of myFuncGen 

   if(ret_val_fg!=0){
      printf("[NMRDAQ]: SG382 initialization FAILED.  Exiting... \n"); 
      exit(1);
   }

   if(gIsTest<2 || gIsTest==4 || gIsTest==5){
      ret_val_fg = ProgramFuncGen(myFuncGen); 
   }

   if(ret_val_fg!=0){
      printf("[NMRDAQ]: SG382 programming FAILED.  Do you need to reattach the connection to the SG382? \n"); 
      printf("[NMRDAQ]: Run the following: sudo chmod 666 /dev/ttyUSB0 \n"); 
      printf("[NMRDAQ]: Exiting... \n"); 
      printf("============================================================ \n"); 
      exit(1);
   }

   usleep(100000); // wait for 100 ms to let SG382 settle in

   // open the VME connection 
   int p = OpenVME(argc,argv); 

   // set up the FPGA 
   int ret_val_fpga=0;
   struct fpga myFPGA;
   struct fpgaPulseSequence myPulseSequence; 
   // initialize and program the FPGA 
   ret_val_fpga = InitFPGA(p,&myFPGA,&myPulseSequence);             // pass by reference to modify contents of myFPGA 
   if(ret_val_fpga!=0){
      printf("[NMRDAQ]: Acromag FPGA initialization FAILED.  Exiting... \n"); 
      exit(1);
   }

   // SIS ADC struct 
   struct adc myADC; 
   int ret_val_adc = 0;
   ret_val_adc = SISLoad(&myADC);    // load in data from input file 

   // return codes 
   int ret_val_daq = 0;
  
   if(gIsTest==2 || gIsTest==3){
      // just want to read the I/O space of the ADC 
      ret_val_adc = SISInit(p,&myADC,0);
   }

   // initialize all the stuff that won't change on the ADC 
   // ret_val_adc = SISInit(p,&myADC,1);
   ret_val_adc = SISBaseInit(p,&myADC); 
   // double rf_rec_pulse = 60E-3; 
   // ReconfigADCStruct(rf_rec_pulse,&myADC);
   // ret_val_adc = SISReInit(p,&myADC,0); 

   // this actually works. 
   // SISInit(p,&myADC,1); 

   if(ret_val_adc!=0){ 
      printf("[NMRDAQ]: ADC initialization FAILED.  Stopping the run. \n");
      return 1;
   }

   const int NEvents = myADC.fNumberOfEvents;   // total number of pulses 
   int *SwList = (int *)malloc( sizeof(int)*NEvents ); 
   for(i=0;i<NEvents;i++){
      SwList[i] = 0;
   }

   GetMechSwitchList(myPulseSequence,NEvents,SwList);    

   if(gIsTest==0){
      // regular operation  
      ret_val_daq = AcquireDataNew(p,myPulseSequence,&myADC,timestamp,output_dir,MECH); 
      // shut down the system and print data to file  
      ShutDownSystemNew(p,&myFuncGen,&myPulseSequence); 
      // print data to file(s) 
      if(ret_val_daq==0){
         GetTime(0,&myRun);  // get end time  
	 printf("[NMRDAQ]: Printing diagnostic data to file(s)... \n");  
	 PrintDiagnosticsNew(output_dir,NumComment,comment,myRun,myFuncGen,myPulseSequence,myADC);
	 PrintRunSummaryNew(output_dir,myRun,myFuncGen,myADC);
	 PrintTimeStampMicroSec(output_dir,myADC,timestamp); 
	 PrintMechSwIndex(output_dir,myRun,myADC,MECH); 
	 close(p);
      }else{
	 printf("[NMRDAQ]: Something is wrong with the software or the system!"); 
	 printf("  No data recorded to files. \n"); 
	 close(p);
      }
   }else if(gIsTest==1){
      // assumes output of system is sent to a scope; nothing is done with the ADC! 
      for(i=0;i<NEvents;i++){
	 ProgramSignalsToFPGANew(p,SwList[i],myPulseSequence);
      }
      ShutDownSystemNew(p,&myFuncGen,&myPulseSequence);
   }else if(gIsTest==2){
      // ADC test 
      ret_val_adc = SISInit(p,&myADC,0); 
   }

   printf("============================================================ \n"); 
   
   free(output_dir);
   free(MECH); 
   free(SwList); 

   for(i=0;i<cSIZE;i++){
      free(comment[i]); 
   }
   free(comment); 

   free(gDATA); 

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

