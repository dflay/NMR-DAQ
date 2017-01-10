#include "sg382.h"
const int RET_BUF_SIZE = 2048;
struct termios old_termios;
// //______________________________________________________________________________
// int SG382Init(void) {
//    const char *device="/dev/ttyUSB1";
// 
//    int rs232_handle;
//    rs232_handle = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
//    if (rs232_handle < 0) { 
//       printf("[SG382]: ERROR: Failed to open usb->serial port.\n");
//       return rs232_handle; 
//    }
// 
//    if ( tcgetattr(rs232_handle, &old_termios) != 0 ) {
//       printf("[SG382]: ERROR: Failed to read original serial settings.\n");
//       close(rs232_handle);
//       exit(1);
//    }
// 
//    // 8 data bits, no parity, 1 stop bit, 9600 baud, hdw flow control
//    struct termios new_termios;
//    new_termios.c_cflag = CS8 | B9600 | CRTSCTS;  
//    tcsetattr(rs232_handle, TCSANOW, &new_termios);
// 
//    usleep(1E5);
//    return rs232_handle;
// }
//______________________________________________________________________________
int SG382Init(void) {
   const char *device="/dev/ttyUSB1";
   int rc=0,rs232_handle=0;

   // rs232_handle = open(device_path, O_RDWR | O_NOCTTY | O_NDELAY);
   rs232_handle = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
   if (rs232_handle < 0) {
      printf("[SG382]: ERROR: Failed to open usb->serial port. \n");
      return -1;
   }

   rc = SG382ClearError(rs232_handle); 
   rc += 0; 

   if ( tcgetattr(rs232_handle, &old_termios) != 0 ) {
      printf("[SG382]: ERROR: Failed to read original serial settings.\n");
      close(rs232_handle);
      exit(1);
   }

   rc = SG382ClearError(rs232_handle); 
   rc += 0; 

   if(rc<0) return -1;

   // 8 data bits, no parity, 1 stop bit, 9600 baud, hdw flow control
   struct termios new_termios;
   // new_termios.c_cflag = CS8 | B115200 | CRTSCTS;
   new_termios.c_cflag &=  ~PARENB;        // Make 8n1  
   new_termios.c_cflag &=  ~CSTOPB;
   new_termios.c_cflag &=  ~CSIZE;
   new_termios.c_cflag |=  CS8;         
   new_termios.c_cflag &=  ~CRTSCTS;       // no flow control 

   // set baud rate to 115200 (both SG382s are set to this)  
   cfsetospeed(&new_termios,B115200);
   cfsetispeed(&new_termios,B115200);

   rc = tcsetattr(rs232_handle, TCSANOW, &new_termios);
   if(rc<0){
      printf("[SG382]: Something's wrong. tcsetattr error = %d \n",rc);
      return -1;
   }

   tcflush(rs232_handle,TCIOFLUSH);

   rc = SG382ClearError(rs232_handle); 
   rc += 0; 

   usleep(1E5);
   return rs232_handle;
}
//______________________________________________________________________________
int SG382Close(int rs232_handle) {
   tcsetattr(rs232_handle,TCSANOW,&old_termios);            // restore old settings
   return close(rs232_handle);
}
//______________________________________________________________________________
int SG382Write(int rs232_handle, char *buffer, int buffer_size) {
   int return_code;
   return_code = write(rs232_handle, buffer, buffer_size); 
   usleep(3E5);
   write(rs232_handle, "*WAI\n", 5); 
   usleep(3E5);
   return return_code; 
}
//______________________________________________________________________________
int SG382ClearError(int rs232_handle){
   char *buffer     = "*CLS\n"; 
   int buffer_size  = (int)( strlen(buffer) ); 
   int rc           = SG382Write(rs232_handle,buffer,buffer_size); 
   return rc; 
}
//______________________________________________________________________________
int SG382FlushErrors(int rs232_handle){
   int i=0,ec=0; 
   const int N = 20; 
   for(i=0;i<N;i++){ 
      ec = SG382GetError(rs232_handle);
      if(ec!=0){
         printf("[SG382]: Flush errors iteration %d; Error code = %d \n",i,ec);
         break;
      }
   }
   return ec; 
}
//______________________________________________________________________________
int SG382GetError(int rs232_handle){
   char *buffer = "LERR?\n"; 
   int buf_size = (int)( strlen(buffer) ); 
   char *ans    = (char *)malloc( sizeof(char)*(SG382_RET_BUF_SIZE+1) ); 
   int rc       = SG382Read(rs232_handle,buffer,buf_size,ans,SG382_RET_BUF_SIZE); 
   int err_code = atoi(ans); 
   free(ans);
   rc *= 1; 
   return err_code;  
}
//______________________________________________________________________________
int SG382SetFreq(int rs232_handle, char *freq) {
   char freq_str[100];
   sprintf(freq_str, "FREQ %s\n", freq);
   return SG382Write(rs232_handle, freq_str, (int)strlen(freq_str));
}
//______________________________________________________________________________
int SG382SetBNCAmp(int rs232_handle, char *amp) {
   char amp_str[100];
   sprintf(amp_str, "AMPL %s\n", amp);
   return SG382Write(rs232_handle, amp_str, (int)strlen(amp_str));
}
//______________________________________________________________________________
int SG382SetNTypeAmp(int rs232_handle, char *amp) {
   char amp_str[100];
   sprintf(amp_str, "AMPR %s\n", amp);
   return SG382Write(rs232_handle, amp_str, (int)strlen(amp_str));
}
//______________________________________________________________________________
int SG382SetBNCOutput(int rs232_handle, int flag) {
   int return_code;
   switch (flag) {
      case 0:
         return_code = SG382Write(rs232_handle, "ENBL0\n", 6); 
         break;
      case 1:
         return_code = SG382Write(rs232_handle, "ENBL1\n", 6); 
         break;
      default:
         printf("SG382: ERROR: INVALID FLAG PASSED TO SG382SetBNCOutput\n");
         exit(1);
   }

   return return_code;
}
//______________________________________________________________________________
int SG382SetNTypeOutput(int rs232_handle, int flag) {
   int return_code;
   switch (flag) {
      case 0:
         return_code = SG382Write(rs232_handle, "ENBR0\n", 6); 
         break;
      case 1:
         return_code = SG382Write(rs232_handle, "ENBR1\n", 6); 
         break;
      default:
         printf("SG382: ERROR: INVALID FLAG PASSED TO SG382SetNTypeOutput\n");
         exit(1);
   }

   return return_code;
}
//______________________________________________________________________________
int SG382SetModulation(int rs232_handle, int flag) {
   int return_code;
   switch (flag) {
      case 0:   // disable
         return_code = SG382Write(rs232_handle, "MODL0\n", 6); 
         break;
      case 1:   // enable 
         return_code = SG382Write(rs232_handle, "MODL1\n", 6); 
         break;
      default:
         printf("[SG382]: ERROR: INVALID FLAG PASSED TO SG382SetModulation\n");
         exit(1);
   }

   return return_code;
}
//______________________________________________________________________________
int SG382SetModulationFunction(int rs232_handle, int flag) {
   int return_code;
   switch (flag) {
      case 0:  // sine 
         return_code = SG382Write(rs232_handle, "MFNC0\n", 6); 
         break;
      case 1:  // ramp 
         return_code = SG382Write(rs232_handle, "MFNC1\n", 6); 
         break;
      case 2:  // triangle 
         return_code = SG382Write(rs232_handle, "MFNC2\n", 6); 
         break;
      case 3:  // square 
         return_code = SG382Write(rs232_handle, "MFNC3\n", 6); 
         break;
      case 4:  // noise 
         return_code = SG382Write(rs232_handle, "MFNC4\n", 6); 
         break;
      case 5:  // external 
         return_code = SG382Write(rs232_handle, "MFNC5\n", 6); 
         break;
      default:
         printf("[SG382]: ERROR: INVALID FLAG PASSED TO SG382SetModulationFunction\n");
         exit(1);
   }

   return return_code;
}
//______________________________________________________________________________
int SG382SetModulationRate(int rs232_handle,double freq) {
   int return_code = 0;
   char freq_str[100];
   sprintf(freq_str, "RATE %.14lf\n", freq);
   return_code = SG382Write(rs232_handle,freq_str, (int)strlen(freq_str) ); 
   return return_code;
}
//______________________________________________________________________________
int SG382Read(int rs232_handle, char *in_buffer, int in_size,
              char *out_buffer, int out_size) {
   /* FIXME: find systematic way to impose read/write timings */
   usleep(1E5);

   if (out_size < RET_BUF_SIZE) 
   {
      printf("SG382: ERROR: out_buffer size insufficient (< RET_BUF_SIZE) for SG382Read.\n");
      exit(1);
   }

   write(rs232_handle, in_buffer, in_size);
   usleep(1E6); // Long wait time, but ensures the communication is finished
   int return_code = read(rs232_handle, out_buffer, out_size);
   usleep(1E6);
   return return_code;
}
//______________________________________________________________________________
int SG382Enable(char *freq, char *bnc_amp, char *ntype_amp,
                int bnc_enable, int ntype_enable) {

   int rs232_handle = SG382Init();
   if (rs232_handle < 0) {
      printf("[SG382]: ERROR: Failed to open USB->SERIAL_PORT.\n");
      return -1;
   } else {
      printf("[SG382]: Opened USB->SERIAL_PORT successfully (rs232_handle=%d)\n", rs232_handle);
   }

   int aFunc = 2;                 // 0 = sine, 1 = ramp, 2 = triangle, 3 = square, 4 = noise, 5 = external
   double mod_freq = 0; 

   // 10--15 ms duration signal
   if(aFunc==1) mod_freq = 66.00; // in Hz 
   if(aFunc==2) mod_freq = 33.00; // in Hz '
   // 40 ms duration signal
   // if(aFunc==1) mod_freq = 20.00; // in Hz 
   // if(aFunc==2) mod_freq = 10.00; // in Hz 

   if(gIsTest==4||gIsTest==5){
      SG382SetModulation(rs232_handle,1);                // turn on modulation  
      SG382SetModulationFunction(rs232_handle,aFunc);   
      SG382SetModulationRate(rs232_handle,mod_freq);   
   }else{
      SG382SetModulation(rs232_handle, 0);
   } 
   SG382SetFreq(rs232_handle,freq);
   SG382SetBNCAmp(rs232_handle, bnc_amp);
   SG382SetNTypeAmp(rs232_handle, ntype_amp);
   SG382SetBNCOutput(rs232_handle, bnc_enable);
   SG382SetNTypeOutput(rs232_handle, ntype_enable);

   SG382ClearError(rs232_handle); 

   return SG382Close(rs232_handle);
}
//_____________________________________________________________________________
int ProgramFuncGen(const struct FuncGen myFuncGen){

   int ret_code     = 0;
   ret_code         = SG382Enable(myFuncGen.fFreqCommand ,
                                  myFuncGen.fBNCCommand  ,
                                  myFuncGen.fNTypeCommand,
                                  myFuncGen.fIntBNCState ,
                                  myFuncGen.fIntNTypeState);

   int sleep_time = 10000; 
   usleep(sleep_time); 

   if(ret_code==0){    
      PrintFuncGen(myFuncGen);
   }

   return ret_code; 

}
//_____________________________________________________________________________
void BlankFuncGen(struct FuncGen *myFuncGen){

   // turn off BNC and N-Type output 
   myFuncGen->fBNCState      = "off"; 
   myFuncGen->fNTypeState    = "off"; 
   myFuncGen->fIntBNCState   = 0;
   myFuncGen->fIntNTypeState = 0;

   int ret_code = 0;  
   int dummy    = 0;    
   ret_code     = SG382Enable(myFuncGen->fFreqCommand ,
                              myFuncGen->fBNCCommand  ,
                              myFuncGen->fNTypeCommand,
                              myFuncGen->fIntBNCState ,
                              myFuncGen->fIntNTypeState);

   dummy *= ret_code; 

}
//_____________________________________________________________________________
int InitFuncGen(struct FuncGen *myFuncGen){
   
   int ret_code = 0; 

   // zero out all data members of myFuncGen 
   InitFuncGenStruct(myFuncGen); 

   myFuncGen->fName = "Stanford Research Systems SG-382"; 

   // import function generator settings 
   char *func_gen_fn = "./input/sg382.dat";
   ImportSG382Data(func_gen_fn,myFuncGen);

   ret_code = SG382CheckInput(*myFuncGen); 

   return ret_code; 

}
//_____________________________________________________________________________
int SG382CheckInput(const struct FuncGen myFuncGen){

   const int SIZE = 5; 
   char *volt_units = malloc( sizeof(char)*(SIZE+1) ); 
   char *freq_units = malloc( sizeof(char)*(SIZE+1) ); 

   int ret_code = 0; 

   // voltage limits 
   double bnc_voltage_hi   = 1.0;    // Vrms 
   double bnc_voltage_lo   = 0.001;  // Vrms 
   double ntype_voltage_hi = 1.5;    // Vrms  
   double ntype_voltage_lo = 0.7E-6; // Vrms  

   // frequency limits (Hz) 
   double bnc_freq_lo      = 0.;      
   double bnc_freq_hi      = 62.5E+6;  
   double ntype_freq_lo    = 950E+3; 
   double ntype_freq_hi    = 2.025E+9;  
   
   double freq_lo = 0;
   double freq_hi = 0;
   double v_lo = 0;
   double v_hi = 0;

   double FREQ = 0;
   double VOLT = 0; 

   int is_bnc   = myFuncGen.fIntBNCState; 
   int is_ntype = myFuncGen.fIntNTypeState; 

   if( is_bnc && !is_ntype){
      v_lo       = bnc_voltage_lo;
      v_hi       = bnc_voltage_hi;
      freq_lo    = bnc_freq_lo; 
      freq_hi    = bnc_freq_hi; 
      VOLT       = myFuncGen.fBNCVoltage;
      volt_units = myFuncGen.fBNCVoltageUnits; 
   }else if(!is_bnc && is_ntype){
      v_lo       = ntype_voltage_lo;
      v_hi       = ntype_voltage_hi;
      freq_lo    = ntype_freq_lo; 
      freq_hi    = ntype_freq_hi; 
      VOLT       = myFuncGen.fNTypeVoltage; 
      volt_units = myFuncGen.fNTypeVoltageUnits; 
   }else if(!is_bnc && !is_ntype){
      // default to N-Type 
      v_lo       = ntype_voltage_lo;
      v_hi       = ntype_voltage_hi;
      freq_lo    = ntype_freq_lo; 
      freq_hi    = ntype_freq_hi; 
      VOLT       = myFuncGen.fNTypeVoltage; 
      volt_units = myFuncGen.fNTypeVoltageUnits; 
   }

   FREQ       = myFuncGen.fFrequency;
   freq_units = myFuncGen.fFreqUnits; 

   // scale FREQ to Hz 
   if( AreEquivStrings(freq_units,kHz) ) FREQ *= 1E+3; 
   if( AreEquivStrings(freq_units,MHz) ) FREQ *= 1E+6; 
   if( AreEquivStrings(freq_units,GHz) ) FREQ *= 1E+9; 

   char *vpp = "Vpp"; 
   char *rms = "rms";
   char *dbm = "dBm";  
 
   double R       = 50.;
   double P_in_mW = 1E-3; 
   double T1=0,T2=0,exp_arg=0,arg=0,G=0; 

   // convert voltage to Vrms 
   if( AreEquivStrings(volt_units,vpp) ){
      // input is in Vpp
      if(gIsDebug) printf("[SG382::CheckInput]: V = %.4lf Vpp \n",VOLT);  
      VOLT *= 1./(2.*sqrt(2));
      if(gIsDebug) printf("[SG382::CheckInput]: V = %.4lf Vrms \n",VOLT);  
   }else if( AreEquivStrings(volt_units,rms) ){
      // already in RMS, nothing to do
      VOLT *= 1.;
   }else if( AreEquivStrings(volt_units,dbm) ){
      // input is in dBm
      if(gIsDebug) printf("[SG382::CheckInput]: V = %.4lf dBm \n",VOLT);  
      G       = VOLT;
      T1      = 2.*R*P_in_mW;
      exp_arg = G/10.;
      T2      = pow(10.,exp_arg);
      arg     = T1*T2;
      VOLT    = sqrt(arg)/sqrt(2.);
      if(gIsDebug) printf("[SG382::CheckInput]: V = %.4lf Vrms \n",VOLT);  
   }

   int fail=0; 
   int fail_freq=0;
   int fail_volt=0;

   if( FREQ > freq_lo && FREQ < freq_hi ){
      // good frequency, do nothing
   }else{
      fail++;
      fail_freq = 1; 
   }

   if( (VOLT > v_lo) && (VOLT < v_hi) ){
      // good voltage, do nothing
   }else{
      fail++;
      fail_volt = 1; 
   }

   if(fail_freq==1){
      printf("[SG382::CheckInput]: Frequency out of range! \n");
      printf("[SG382::CheckInput]: freq_lo = %lf Hz \n",freq_lo); 
      printf("[SG382::CheckInput]: freq    = %lf Hz \n",FREQ); 
      printf("[SG382::CheckInput]: freq_hi = %lf Hz \n",freq_hi); 
   }

   if(fail_volt==1){
      printf("[SG382::CheckInput]: Voltage out of range! \n");
      printf("[SG382::CheckInput]: v_lo = %lf Vrms \n",v_lo); 
      printf("[SG382::CheckInput]: v    = %lf Vrms \n",VOLT); 
      printf("[SG382::CheckInput]: v_hi = %lf Vrms \n",v_hi); 
   }

   if(fail!=0) printf("[SG382::CheckInput]: Exiting... \n"); 

   ret_code = fail;

   return ret_code; 

}
//_____________________________________________________________________________
void InitFuncGenStruct(struct FuncGen *myFuncGen){

   const int SIZE                = 100; 
   myFuncGen->fName              = (char*)malloc( sizeof(char)*(SIZE+1) );
   strcpy(myFuncGen->fName,"UNKNOWN"); 
   myFuncGen->fFrequency         = 0; 
   myFuncGen->fFreqUnits         = "ND";
   myFuncGen->fBNCVoltage        = 0; 
   myFuncGen->fBNCVoltageUnits   = "ND";
   myFuncGen->fNTypeVoltage      = 0; 
   myFuncGen->fNTypeVoltageUnits = "ND";
   myFuncGen->fBNCState          = "off";
   myFuncGen->fNTypeState        = "off";
   myFuncGen->fIntBNCState       = 0;
   myFuncGen->fIntNTypeState     = 0;
}
//_____________________________________________________________________________
void PrintFuncGen(const struct FuncGen myFuncGen){

   printf("[SG382]: Function Generator Characteristics: \n");
   printf("[SG382]: Name           = %s      \n",myFuncGen.fName); 
   printf("[SG382]: Frequency      = %s      \n",myFuncGen.fFreqCommand); 
   printf("[SG382]: BNC voltage    = %s      \n",myFuncGen.fBNCCommand); 
   printf("[SG382]: N-Type voltage = %s      \n",myFuncGen.fNTypeCommand); 
   printf("[SG382]: BNC state      = %d (%s) \n",myFuncGen.fIntBNCState  ,myFuncGen.fBNCState);
   printf("[SG382]: N-Type state   = %d (%s) \n",myFuncGen.fIntNTypeState,myFuncGen.fNTypeState);
   // printf("[SG382]: Frequency              = %.2f %s \n",myFuncGen.fFrequency   ,myFuncGen.fFreqUnits); 
   // printf("[SG382]: BNC voltage            = %.2f %s \n",myFuncGen.fBNCVoltage  ,myFuncGen.fBNCVoltageUnits); 
   // printf("[SG382]: N-Type voltage         = %.2f %s \n",myFuncGen.fNTypeVoltage,myFuncGen.fNTypeVoltageUnits); 

}
//_____________________________________________________________________________
void ImportSG382Data(char *filename,struct FuncGen *myFuncGen){
 
   double ivalue;
   int j=0,k=0,N=0;
   const int MAX = 2000;
   const int uMAX=10;
   const int tMAX=20;
   const int sMAX=3; 
   char buf[MAX],itag[tMAX],iunit[uMAX],istate[sMAX];
   char *mode    = "r";
   char *ntype   = "ntype"; 
   char *bnc     = "bnc";
   char *freq    = "frequency";  

   // memory allocation 
   myFuncGen->fBNCState          = (char*)malloc( sizeof(char)*(sMAX+1) );
   myFuncGen->fNTypeState        = (char*)malloc( sizeof(char)*(sMAX+1) );
   myFuncGen->fFreqUnits         = (char*)malloc( sizeof(char)*(uMAX+1) );
   myFuncGen->fBNCVoltageUnits   = (char*)malloc( sizeof(char)*(uMAX+1) );
   myFuncGen->fNTypeVoltageUnits = (char*)malloc( sizeof(char)*(uMAX+1) );
   myFuncGen->fFreqCommand       = (char*)malloc( sizeof(char)*(tMAX+1) );
   myFuncGen->fBNCCommand        = (char*)malloc( sizeof(char)*(tMAX+1) );
   myFuncGen->fNTypeCommand      = (char*)malloc( sizeof(char)*(tMAX+1) );

   FILE *infile;
   infile = fopen(filename,mode);

   if(infile==NULL){
      printf("[SG382::ImportSG382Data]: Cannot open the file: %s.  Exiting... \n",filename);
      exit(1);
   }else{
      if(gIsDebug) printf("[SG382::ImportSG382Data]: Opening the file: %s... \n",filename);
      while( !feof(infile) ){
         if(k==0){
            fgets(buf,MAX,infile);
         }else{
            fscanf(infile,"%s %s %lf %s",itag,istate,&ivalue,iunit);
            if(gIsDebug && gVerbosity>=1) printf("%s %s %.2lf %s \n",itag,istate,ivalue,iunit); 
            if( !AreEquivStrings(itag,eof_tag) ){
               if( AreEquivStrings(itag,freq) ){ 
                  myFuncGen->fFrequency = ivalue; 
                  strcpy(myFuncGen->fFreqUnits,iunit);
               }else if( AreEquivStrings(itag,bnc) ){
                  myFuncGen->fBNCVoltage = ivalue; 
                  strcpy(myFuncGen->fBNCState       ,istate); 
                  strcpy(myFuncGen->fBNCVoltageUnits,iunit); 
                  if( AreEquivStrings(istate,on) || AreEquivStrings(istate,ON) ){
                     myFuncGen->fIntBNCState = 1; 
                  }else if( AreEquivStrings(istate,off) || AreEquivStrings(istate,OFF) ){
                     myFuncGen->fIntBNCState = 0; 
                  } 
               }else if( AreEquivStrings(itag,ntype) ){
                  myFuncGen->fNTypeVoltage = ivalue; 
                  strcpy(myFuncGen->fNTypeState       ,istate); 
                  strcpy(myFuncGen->fNTypeVoltageUnits,iunit);
                  if( AreEquivStrings(istate,on) || AreEquivStrings(istate,ON) ){
                     myFuncGen->fIntNTypeState = 1; 
                  }else if( AreEquivStrings(istate,off) || AreEquivStrings(istate,OFF) ){
                     myFuncGen->fIntNTypeState = 0; 
                  } 
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
         printf("[SG382::ImportSG382Data]: No data!  Exiting... \n");
         exit(1);
      }
   }

   // construct commands for SG-382 
   char *freq_cmd  = (char*)malloc( sizeof(char)*(tMAX+1) ); 
   char *bnc_cmd   = (char*)malloc( sizeof(char)*(tMAX+1) ); 
   char *ntype_cmd = (char*)malloc( sizeof(char)*(tMAX+1) ); 

   sprintf(freq_cmd ,"%.14lf %s",myFuncGen->fFrequency   ,myFuncGen->fFreqUnits);
   sprintf(bnc_cmd  ,"%.14lf %s",myFuncGen->fBNCVoltage  ,myFuncGen->fBNCVoltageUnits);
   sprintf(ntype_cmd,"%.14lf %s",myFuncGen->fNTypeVoltage,myFuncGen->fNTypeVoltageUnits);

   // printf("FREQ = %s \n",freq_cmd); 

   strcpy(myFuncGen->fFreqCommand ,freq_cmd);
   strcpy(myFuncGen->fBNCCommand  ,bnc_cmd);
   strcpy(myFuncGen->fNTypeCommand,ntype_cmd);

   if(gIsDebug) printf("----------------------------------------- \n");

}

