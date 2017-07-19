// This code runs the DAQ for the NMR system, incorporating the FPGA, 
// digitizer (ADC) and Stanford Research Systems SG-382 function generator.
// See the README.md file for details. 

#include <stdlib.h> 
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
#include "keithley.h"
#include "acromag_ipep201.h"
#include "sg382.h"
#include "keithley_interface.h"
#include "struck_adc.h"
#include "acquisition.h"
#include "diagnostics.h"

// global definitions
// utilities  
int gIsDebug; 
int gIsFLASH; 
int gIsTest;
int gVerbosity;
double gFreq_RF; 
double gDelayTime;
struct timeval gStart,gStop,gTime;
// FPGA 
char **gMasterList;                // make sure this array size matches mlMAX 
u_int16_t gMasterAddrList[mlMAX];  // make sure this array size matches mlMAX 
u_int16_t gMechSwitchAddr[SIZE4];
u_int16_t gRFSwitchAddr[SIZE4];
u_int16_t gRFPulseAddr;
u_int16_t gRFGateAddr;
u_int16_t gDigitizerAddr;          // this is not included in the master list 
u_int16_t gDigitizerAddr2;         // this is not included in the master list 
int gMechSwFlag[SIZE4];            // enable bits for mechanical switches  
int RECEIVE_GATE_COUNTS;
double gFPGAClockFreq;
double RECEIVE_GATE_TIME_SEC;
char *RECEIVE_GATE_INPUT_TIME_UNITS;
u_int16_t gModBase;
u_int16_t gIPAIOSpace;
u_int16_t gIPBIOSpace;
u_int16_t gIPCIOSpace;
u_int16_t gIPDIOSpace;
u_int16_t gIPAIDSpace;
u_int16_t gIPBIDSpace;
u_int16_t gIPCIDSpace;
u_int16_t gIPDIDSpace;
u_int16_t gOffset;
// SG382 
int SG382_SLEEP_TIME;
struct termios old_termios;
// SIS ADC 
int ADC_MULTIEVENT_STATE;
int PULSES_PER_READ;                       // how many pulses to read out during a block-read?  
u_int32_t MOD_BASE;
u_int32_t *gDATA;
unsigned short *gDATA_us;

int OpenVME(int argc, char* argv[]);

int main(int argc, char* argv[]){

   int i=0;
   SG382_SLEEP_TIME = 10000;  // default to 10 ms 

   // import debug mode options 
   ImportUtilityData(); 

   // initialize variable for output directory  
   const int MAX    = 2000; 
   char *output_dir = (char*)malloc( sizeof(char)*(MAX+1) );  

   struct run myRun; 

   int rc=0; 

   printf("--------------------------- Initializing ---------------------------  \n");

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

   // a new time stamp 
   unsigned long long *timestamp_ns = (unsigned long long *)malloc( sizeof(unsigned long long)*NPULSE );
   for(i=0;i<NPULSE;i++) timestamp_ns[i] = 0;  

   int *MECH = (int *)malloc( sizeof(int)*NPULSE );
   for(i=0;i<NPULSE;i++){
      MECH[i] = 0;
   }

   // set up the SG-382 function generator 
   int ret_val_fg=0;
   struct FuncGen myFuncGen; 
   ret_val_fg = InitFuncGenLO(&myFuncGen);       // pass by reference to modify contents of myFuncGen 

   if(ret_val_fg!=0){
      printf("[NMRDAQ]: Initialization for the LO SG382 FAILED.  Exiting... \n"); 
      BlankFuncGen(constants_t::SG382_LO_DEV_PATH.c_str() ,&myFuncGen);
      return 1;
   }

   if(gIsTest<2 || gIsTest==4 || gIsTest==5){
      ret_val_fg = ProgramFuncGen(SG382_ENABLE_AMPL_AND_FREQ,constants_t::SG382_LO_DEV_PATH.c_str(),myFuncGen,100000);   
      printf("[NMRDAQ]: LO SG382 initialization complete! \n");  
   }

   if(ret_val_fg!=0){
      printf("[NMRDAQ]: LO SG382 programming FAILED.  Do you need to reattach the connection to the SG382? \n"); 
      printf("[NMRDAQ]: Run the following: ./connect_rs232.sh \n"); 
      printf("[NMRDAQ]: Exiting... \n"); 
      printf("============================================================ \n"); 
      return 1;
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
      BlankFuncGen(constants_t::SG382_LO_DEV_PATH.c_str() ,&myFuncGen);
      return 1;
   }else{
      printf("[NMRDAQ]: Acromag FPGA initialization complete! \n"); 
   }

   // pi/2 function generator initialization 
   // create the necessary number of data structs 
   // and load data into the array 
   const int NCH = 1; // myPulseSequence.fNSequences; [assume only ONE value, since we're setting the characteristics once] 
   struct FuncGen *myFuncGenPi2 = static_cast<struct FuncGen *>( malloc( sizeof(struct FuncGen)*NCH ) ); 
   ret_val_fg = InitFuncGenPi2(NCH,myFuncGenPi2); 

   if(ret_val_fg!=0){
      printf("[NMRDAQ]: Initialization for the pi/2 SG382 FAILED.  Exiting... \n"); 
      return 1;
   }

   ret_val_fg = ProgramFuncGen(SG382_ENABLE_AMPL_AND_FREQ,constants_t::SG382_PI2_DEV_PATH.c_str(),myFuncGenPi2[0],100000);   
   if(ret_val_fg==0){
      printf("[NMRDAQ]: Initialization for the pi/2 SG382 complete! \n"); 
   }else{
      printf("[NMRDAQ]: Initialization for the pi/2 SG382 FAILED. \n"); 
      BlankFuncGen(constants_t::SG382_LO_DEV_PATH.c_str() ,&myFuncGen);
      BlankFuncGen(constants_t::SG382_PI2_DEV_PATH.c_str(),&myFuncGenPi2[0]);
      return 1;
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
   ret_val_adc = SISBaseInit(p,&myADC); 

   if(ret_val_adc!=0){ 
      printf("[NMRDAQ]: ADC initialization failed.  Exiting... \n"); 
      BlankFuncGen(constants_t::SG382_LO_DEV_PATH.c_str() ,&myFuncGen);
      BlankFuncGen(constants_t::SG382_PI2_DEV_PATH.c_str(),&myFuncGenPi2[0]);
      return 1; 
   }else{
      printf("[NMRDAQ]: ADC initialization complete! \n"); 
   }

   // initialize the keithley
   double *resistance = (double *)malloc( sizeof(double)*NPULSE ); 
   for(i=0;i<NPULSE;i++){
      resistance[i] = 0.;
   }
   char err_msg[512]; 
   keithley_t myKeithley;
   int ret_val_k = keithley_interface_load_settings(&myKeithley); 
   if(ret_val_k!=0){
      printf("[NMRDAQ]: Keithley initalization FAILED.  Error message:\n%s\n",err_msg); 
      ShutDownSystemNew(p,&myFuncGen,myFuncGenPi2,&myPulseSequence,&myKeithley);
      return 1;
   }
   // continue setting up 
   myKeithley.portNo = keithley_interface_open_connection(); 
   ret_val_k = keithley_interface_set_range(myKeithley.portNo,myKeithley.maxRange);
   ret_val_k = keithley_interface_set_to_remote_mode(myKeithley.portNo); 
   ret_val_k = keithley_interface_check_errors(myKeithley.portNo,err_msg); 
   if(ret_val_k!=0){
      printf("[NMRDAQ]: Keithley initalization FAILED.  Error message:\n%s\n",err_msg); 
      ShutDownSystemNew(p,&myFuncGen,myFuncGenPi2,&myPulseSequence,&myKeithley);
      return 1;
   }else{
      printf("[NMRDAQ]: Keithley initalization complete! \n"); 
   }

   // passed all tests, start the run 

   if(gIsTest==0 || gIsTest==5){ 
      output_dir = GetDirectoryName(&myRun);
      printf("--------------------------- STARTING RUN %05d ---------------------------  \n",myRun.fRunNumber);
      printf("[NMRDAQ]: The date is: %02d %02d %d \n",myRun.fMonth,myRun.fDay,myRun.fYear);
      printf("[NMRDAQ]: The time is: %02d:%02d:%02d \n",myRun.fHour_start,myRun.fMinute_start,myRun.fSecond_start);
      printf("[NMRDAQ]: Output directory: %s \n" ,output_dir);  
      rc = WriteStatus(RUN_ACTIVE);
      if(rc!=0){
         printf("[NMRDAQ]: Cannot update the run status!  Exiting... \n");
         exit(1); 
      } 
   }

   const int NEvents = myADC.fNumberOfEvents;   // total number of pulses 
   int *SwList = (int *)malloc( sizeof(int)*NEvents ); 
   for(i=0;i<NEvents;i++){
      SwList[i] = 0;
   }

   GetMechSwitchList(myPulseSequence,NEvents,SwList);    

   if(gIsTest==0){
      // regular operation  
      ret_val_daq = AcquireDataNew(p,myPulseSequence,myFuncGenPi2,&myADC,&myKeithley,resistance,timestamp,timestamp_ns,output_dir,MECH); 
      // shut down the system and print data to file  
      ShutDownSystemNew(p,&myFuncGen,myFuncGenPi2,&myPulseSequence,&myKeithley);
      // print data to file(s) 
      if(ret_val_daq==0){
         GetTime(0,&myRun);  // get end time  
	 printf("[NMRDAQ]: Printing diagnostic data to file(s)... \n");  
	 PrintDiagnosticsNew(output_dir,NumComment,comment,myRun,myFuncGen,myFuncGenPi2,myPulseSequence,myADC);
	 PrintRunSummary(output_dir,NCH,myRun,myFuncGen,myFuncGenPi2,myADC);
	 PrintTimeStampMicroSec(output_dir,myADC,timestamp); 
	 PrintMechSwIndex(output_dir,myRun,myADC,MECH);
         PrintAuxiliaryData(output_dir,myADC,timestamp_ns,MECH,resistance);  
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
      ShutDownSystemNew(p,&myFuncGen,myFuncGenPi2,&myPulseSequence,&myKeithley);
   }else if(gIsTest==2){
      // ADC test 
      ret_val_adc = SISInit(p,&myADC,0); 
   }

   printf("============================================================ \n"); 
   
   free(output_dir);
   free(MECH); 
   free(SwList); 
   free(gDATA); 
   free(myFuncGenPi2); 

   for(i=0;i<NumComment;i++){
      free(comment[i]); 
   }
   free(comment); 
   
   for(i=0;i<NPULSE;i++){
      free(timestamp[i]);
   } 
   free(timestamp);

   free(resistance); 
   free(timestamp_ns); 

   rc = WriteStatus(RUN_STOPPED);
   if(rc!=0){
      printf("[NMRDAQ]: Cannot update the run status!  Exiting... \n");
      exit(1); 
   } 

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

