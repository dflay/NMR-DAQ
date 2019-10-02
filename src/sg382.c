#include "sg382.h"
//______________________________________________________________________________
int SG382Init(const char *device_path) {
   int rs232_handle=0;
   rs232_handle = open(device_path, O_RDWR | O_NOCTTY | O_NDELAY);
   // rs232_handle = open(device_path, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
   if (rs232_handle < 0) { 
      printf("[SG382]: ERROR: Failed to open usb->serial port. \n");
      return -1; 
   }

   if ( tcgetattr(rs232_handle, &old_termios) != 0 ) {
      printf("[SG382]: ERROR: Failed to read original serial settings.\n");
      close(rs232_handle);
      exit(1);
   }

   // 8 data bits, no parity, 1 stop bit, 9600 baud, hdw flow control
   struct termios new_termios;
   // new_termios.c_cflag = CS8 | B9600 | CRTSCTS;
   // new_termios.c_cflag = CS8 | B115200 | CRTSCTS;
   new_termios.c_cflag     &=  ~PARENB;        // Make 8n1  
   new_termios.c_cflag     &=  ~CSTOPB;
   new_termios.c_cflag     &=  ~CSIZE;
   new_termios.c_cflag     |=  CS8;
   new_termios.c_cflag     &=  ~CRTSCTS;       // no flow control 

   // set baud rate 
   cfsetospeed(&new_termios,B115200);
   cfsetispeed(&new_termios,B115200);

   int rc = tcsetattr(rs232_handle, TCSANOW, &new_termios);
   if(rc<0){
      printf("[SG382]: Something's wrong. ERROR %d \n",rc);
      return -1;
   }

   tcflush(rs232_handle,TCIOFLUSH);

   usleep(1E5);
   return rs232_handle;
}
//______________________________________________________________________________
int SG382Close(int rs232_handle){
   tcsetattr(rs232_handle,TCSANOW,&old_termios);            // restore old settings
   return close(rs232_handle);
}
//______________________________________________________________________________
int SG382Write(int rs232_handle,const char *buffer){
   int return_code=0;
   int buffer_size = (int)( strlen(buffer) ); 
   return_code = write(rs232_handle, buffer, buffer_size); 
   usleep(SG382_SLEEP_TIME);
   write(rs232_handle, "*WAI\n", 5); 
   usleep(SG382_SLEEP_TIME);
   return return_code; 
}
//______________________________________________________________________________
int SG382ClearErrorAlt(const char *device_path){
   int rs232_handle = SG382Init(device_path);
   char buffer[512]; 
   sprintf(buffer,"%s","*CLS\n");  
   int rc = SG382Write(rs232_handle,buffer); 
   return rc; 
}
//______________________________________________________________________________
int SG382ClearError(int rs232_handle){
   char buffer[512]; 
   sprintf(buffer,"%s","*CLS\n");  
   int rc = SG382Write(rs232_handle,buffer); 
   return rc; 
}
//______________________________________________________________________________
int SG382GetError(int rs232_handle){
   char buffer[512]; 
   sprintf(buffer,"%s","LERR?\n"); 
   char *ans    = (char *)malloc( sizeof(char)*(SG382_RET_BUF_SIZE+1) ); 
   int rc       = SG382Read(rs232_handle,buffer,ans,SG382_RET_BUF_SIZE); 
   int err_code = atoi(ans); 
   free(ans);
   rc *= 1; 
   return err_code;  
}
//______________________________________________________________________________
int SG382GetIDN(int rs232_handle,std::string &idn){
  char buffer[512];
  sprintf(buffer,"%s","*IDN?\n");
  char *ans    = (char *)malloc( sizeof(char)*(SG382_RET_BUF_SIZE+1) ); 
  int rc       = SG382Read(rs232_handle,buffer,ans,SG382_RET_BUF_SIZE);
  idn          = ans; 
  return rc;
}
//______________________________________________________________________________
int SG382Read(int rs232_handle,const char *in_buffer,char *out_buffer, int out_size){
   int in_size = (int)( strlen(in_buffer) ); 
   if (out_size < SG382_RET_BUF_SIZE){
      printf("[SG382]: ERROR: out_buffer size insufficient (< SG382_RET_BUF_SIZE) for SG382Read.\n");
      printf("[SG382]: out_size = %d \t SG382_RET_BUF_SIZE = %d \n",out_size,SG382_RET_BUF_SIZE); 
      return -1;
   }
   write(rs232_handle, in_buffer, in_size);
   usleep(SG382_SLEEP_TIME);
   int rc = read(rs232_handle, out_buffer, out_size);
   usleep(SG382_SLEEP_TIME);
   return rc;
}
//______________________________________________________________________________
int SG382SetFreq(int rs232_handle, char *freq) {
   char freq_str[100];
   sprintf(freq_str, "FREQ %s\n", freq);
   return SG382Write(rs232_handle, freq_str);
}
//______________________________________________________________________________
int SG382SetBNCAmp(int rs232_handle, char *amp) {
   char amp_str[100];
   sprintf(amp_str, "AMPL %s\n", amp);
   return SG382Write(rs232_handle, amp_str);
}
//______________________________________________________________________________
int SG382SetNTypeAmp(int rs232_handle, char *amp) {
   char amp_str[100];
   sprintf(amp_str, "AMPR %s\n", amp);
   return SG382Write(rs232_handle, amp_str);
}
//______________________________________________________________________________
int SG382SetBNCOutput(int rs232_handle, int flag) {
   int return_code;
   switch (flag) {
      case 0:
         return_code = SG382Write(rs232_handle,"ENBL0\n"); 
         break;
      case 1:
         return_code = SG382Write(rs232_handle,"ENBL1\n"); 
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
         return_code = SG382Write(rs232_handle, "ENBR0\n"); 
         break;
      case 1:
         return_code = SG382Write(rs232_handle, "ENBR1\n"); 
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
         return_code = SG382Write(rs232_handle, "MODL0\n"); 
         break;
      case 1:   // enable 
         return_code = SG382Write(rs232_handle, "MODL1\n"); 
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
         return_code = SG382Write(rs232_handle, "MFNC0\n"); 
         break;
      case 1:  // ramp 
         return_code = SG382Write(rs232_handle, "MFNC1\n"); 
         break;
      case 2:  // triangle 
         return_code = SG382Write(rs232_handle, "MFNC2\n"); 
         break;
      case 3:  // square 
         return_code = SG382Write(rs232_handle, "MFNC3\n"); 
         break;
      case 4:  // noise 
         return_code = SG382Write(rs232_handle, "MFNC4\n"); 
         break;
      case 5:  // external 
         return_code = SG382Write(rs232_handle, "MFNC5\n"); 
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
   return_code = SG382Write(rs232_handle,freq_str); 
   return return_code;
}
//______________________________________________________________________________
int SG382Enable(u_int16_t bit_pattern,const char *device_path,
                char *freq, char *bnc_amp, char *ntype_amp,
                int bnc_enable, int ntype_enable){

   int rc=0; 
   int rs232_handle = SG382Init(device_path);
   if(rs232_handle < 0){
      printf("[SG382]: ERROR: Failed to open USB->SERIAL_PORT.\n");
      return -1;
   }else{
      printf("[SG382]: Opened USB->SERIAL_PORT successfully (rs232_handle=%d)\n", rs232_handle);
   }

   int err_code = SG382GetError(rs232_handle);
   if(err_code!=0){
      printf("[SG382]: Error code = %d \n",err_code); 
   }

   int cntr=0;
   while(err_code!=0){ 
      err_code = SG382GetError(rs232_handle);
      cntr++;
      if(cntr>10){
         printf("[SG382]: Error code = %d \n",err_code); 
         printf("[SG382]: Something's not working; can't clear errors.  Exiting... \n");
         rc = 1; 
         break; 
      }
   }  

   // disable outputs to start 
   SG382SetBNCOutput(rs232_handle,0);
   SG382SetNTypeOutput(rs232_handle,0);

   if(rc!=0) return rc;  

   rc = SG382ClearError(rs232_handle); 

   int i=0; 
   const int NBITS = 16; 
   int *myBit = static_cast<int *>( malloc( sizeof(int)*NBITS ) );
   for(i=0;i<NBITS;i++){
      myBit[i] = GetBit(i,bit_pattern); 
   }

   if(gIsDebug && gVerbosity>=1){  
      printf("[SG382]: Bit pattern is "); 
      for(i=NBITS-1;i>=0;i--){ 
         printf("%d ",myBit[i]); 
      }
      printf("\n"); 
   }

   int aFunc       = 0;             
   double mod_freq = 0; 

   if(gIsTest==4||gIsTest==5){
      aFunc    = 2;                 // 0 = sine, 1 = ramp, 2 = triangle, 3 = square, 4 = noise, 5 = external
      // 10--15 ms duration signal
      if(aFunc==1) mod_freq = 66.00; // in Hz 
      if(aFunc==2) mod_freq = 33.00; // in Hz 
      // 40 ms duration signal
      // if(aFunc==1) mod_freq = 20.00; // in Hz 
      // if(aFunc==2) mod_freq = 10.00; // in Hz 
      SG382SetModulation(rs232_handle,1);                // turn on modulation  
      SG382SetModulationFunction(rs232_handle,aFunc);   
      SG382SetModulationRate(rs232_handle,mod_freq);   
   }else{
      SG382SetModulation(rs232_handle, 0);
   }

   // program frequency  
   if(myBit[1]==1){
      // printf("[SG382]: SETTING FREQUENCY! \n"); 
      SG382SetFreq(rs232_handle,freq);
   }
   // program amplitudes  
   if(myBit[0]==1){
      // printf("[SG382]: SETTING AMPLTUDES! \n"); 
      SG382SetBNCAmp(rs232_handle, bnc_amp);
      SG382SetNTypeAmp(rs232_handle, ntype_amp);
      SG382SetBNCOutput(rs232_handle, bnc_enable);
      SG382SetNTypeOutput(rs232_handle, ntype_enable);
   }
   // both frequency and amplitude are disabled 
   if(myBit[0]==0 && myBit[1]==0){ 
      // printf("[SG382]: DISABLING OUTPUT! \n"); 
      SG382SetBNCOutput(rs232_handle,0);
      SG382SetNTypeOutput(rs232_handle,0);
   }

   // const int BUF_SIZE = 100; 
   // char *query = (char *)malloc( sizeof(char)*(BUF_SIZE+1) ); 
   // char *ans   = (char *)malloc( sizeof(char)*(SG382_RET_BUF_SIZE+1) ); 

   // sprintf(query,"%s\n","FREQ?");
   // rc = SG382Read(rs232_handle,query,ans,SG382_RET_BUF_SIZE);
   // printf("[SG382]: query = %s"  ,query);  
   // printf("[SG382]: ans   = %s\n",ans); 
 
   // sprintf(query,"%s\n","AMPR?");
   // rc = SG382Read(rs232_handle,query,ans,SG382_RET_BUF_SIZE);
   // printf("[SG382]: query = %s"  ,query);  
   // printf("[SG382]: ans   = %s\n",ans);  

   free(myBit);
   // free(query);  
   // free(ans);  
  
   // clear errors  
   rc = SG382ClearError(rs232_handle); 

   rc = SG382Close(rs232_handle); 
   return rc;
}
//_____________________________________________________________________________
int ProgramFuncGen(u_int16_t bit_pattern,const char *device_path,const struct FuncGen myFuncGen,int sleep_time){
   int rc = 0;

   SG382_SLEEP_TIME = sleep_time; 

   rc= SG382Enable(bit_pattern,device_path,
                   myFuncGen.fFreqCommand ,
                   myFuncGen.fBNCCommand  ,
                   myFuncGen.fNTypeCommand,
                   myFuncGen.fIntBNCState ,
                   myFuncGen.fIntNTypeState);
      
   // PrintFuncGen(myFuncGen);

   usleep(SG382_SLEEP_TIME); 

   if(rc!=0){    
      printf("[SG382]: Cannot enable output! \n"); 
      PrintFuncGen(myFuncGen);
   }

   return rc; 
}
//_____________________________________________________________________________
void BlankFuncGen(const char *device_path,struct FuncGen *myFuncGen){
   // turn off BNC and N-Type output 
   sprintf(myFuncGen->fBNCState  ,"%s","off"); 
   sprintf(myFuncGen->fNTypeState,"%s","off"); 
   myFuncGen->fIntBNCState   = 0;
   myFuncGen->fIntNTypeState = 0;

   int ret_code = 0;  
   int dummy    = 0;    
   ret_code     = SG382Enable(SG382_DISABLE,device_path,
                              myFuncGen->fFreqCommand ,
                              myFuncGen->fBNCCommand  ,
                              myFuncGen->fNTypeCommand,
                              myFuncGen->fIntBNCState ,
                              myFuncGen->fIntNTypeState);
   dummy *= ret_code; 
}
//_____________________________________________________________________________
int InitFuncGenLO(struct FuncGen *myFuncGen){
   int rc=0;
   
   // clear all errors 
   SG382ClearErrorAlt(constants_t::SG382_LO_DEV_PATH.c_str()); 

   // zero out all data members of myFuncGen 
   InitFuncGenStruct(myFuncGen);
 
   // FIXME: Read SG382 for name of device
   strcpy(myFuncGen->fName,"Stanford Research Systems SG382 [LO]");   

   // import function generator settings 
   char func_gen_fn[512]; 
   sprintf(func_gen_fn,"%s","./input/sg382.dat"); 
   ImportSG382Data_LO(func_gen_fn,myFuncGen);

   rc = SG382CheckInput(*myFuncGen); 

   return rc; 
}
//_____________________________________________________________________________
int InitFuncGenPi2(int NCH,struct FuncGen *myFuncGen){
   int i=0,rc=0;
   
   // clear all errors 
   SG382ClearErrorAlt(constants_t::SG382_PI2_DEV_PATH.c_str()); 

   // zero out all data members of myFuncGen 
   for(i=0;i<NCH;i++){ 
      InitFuncGenStruct(&myFuncGen[i]);    // to get a pointer to the ith element, use an ampersand 
   }

   char func_gen_fn[512]; 
   sprintf(func_gen_fn,"%s","./input/sg382_pi2.dat"); 
   ImportSG382Data_pi2(func_gen_fn,NCH,myFuncGen);

   double volt_dBm=0,volt_limit=0;
   // check the input 
   for(i=0;i<NCH;i++){ 
      sprintf(myFuncGen[i].fName,"%s","Stanford Research Systems SG382 [pi/2]");    // FIXME: Read SG382 for name of device 
      // PrintFuncGen(myFuncGen[i]);            
      // test settings against SG382 limits  
      rc = SG382CheckInput(myFuncGen[i]); 
      // additional check: input voltage must be LESS than 0 dBm for Tomco input.
      volt_dBm = ConvertVoltageFrom_Vpp_to_dBm(myFuncGen[i].fNTypeVoltage); 
      if(volt_dBm>=volt_limit){
         printf("[SG382]: pi/2 input voltage too large for Tomco!\n");
         printf("[SG382]: input = %.3lf dBm  limit = %.3lf dBm \n",volt_dBm,volt_limit);
         printf("[SG382]: Exiting...\n");
	 rc = 1;
	 break;
      }
   }

   return rc; 
}
//_____________________________________________________________________________
int SG382CheckInput(const struct FuncGen myFuncGen){

   char volt_units[6],freq_units[6]; 

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
      sprintf(volt_units,"%s",myFuncGen.fBNCVoltageUnits); 
   }else if(!is_bnc && is_ntype){
      v_lo       = ntype_voltage_lo;
      v_hi       = ntype_voltage_hi;
      freq_lo    = ntype_freq_lo; 
      freq_hi    = ntype_freq_hi; 
      VOLT       = myFuncGen.fNTypeVoltage; 
      sprintf(volt_units,"%s",myFuncGen.fNTypeVoltageUnits); 
   }else if(!is_bnc && !is_ntype){
      // default to N-Type 
      v_lo       = ntype_voltage_lo;
      v_hi       = ntype_voltage_hi;
      freq_lo    = ntype_freq_lo; 
      freq_hi    = ntype_freq_hi; 
      VOLT       = myFuncGen.fNTypeVoltage; 
      sprintf(volt_units,"%s",myFuncGen.fNTypeVoltageUnits); 
   }

   FREQ       = myFuncGen.fFrequency;
   sprintf(freq_units,"%s",myFuncGen.fFreqUnits); 

   // scale FREQ to Hz 
   if( AreEquivStrings( freq_units,constants_t::kHz.c_str() ) ) FREQ *= 1E+3; 
   if( AreEquivStrings( freq_units,constants_t::MHz.c_str() ) ) FREQ *= 1E+6; 
   if( AreEquivStrings( freq_units,constants_t::GHz.c_str() ) ) FREQ *= 1E+9; 
 
   double R       = 50.;
   double P_in_mW = 1E-3; 
   double T1=0,T2=0,exp_arg=0,arg=0,G=0; 

   // convert voltage to Vrms 
   if( AreEquivStrings(volt_units,"Vpp") ){
      // input is in Vpp
      if(gIsDebug) printf("[SG382::CheckInput]: V = %.4lf Vpp \n",VOLT);  
      VOLT *= 1./(2.*sqrt(2));
      if(gIsDebug) printf("[SG382::CheckInput]: V = %.4lf Vrms \n",VOLT);  
   }else if( AreEquivStrings(volt_units,"rms") ){
      // already in RMS, nothing to do
      VOLT *= 1.;
   }else if( AreEquivStrings(volt_units,"dBm") ){
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
   myFuncGen->fMACAddress        = (char*)malloc( sizeof(char)*(SIZE+1) );
   myFuncGen->fFreqUnits         = (char*)malloc( sizeof(char)*(SIZE+1) );
   myFuncGen->fBNCVoltageUnits   = (char*)malloc( sizeof(char)*(SIZE+1) );
   myFuncGen->fNTypeVoltageUnits = (char*)malloc( sizeof(char)*(SIZE+1) );
   myFuncGen->fBNCState          = (char*)malloc( sizeof(char)*(SIZE+1) );
   myFuncGen->fNTypeState        = (char*)malloc( sizeof(char)*(SIZE+1) );
   myFuncGen->fFreqCommand       = (char*)malloc( sizeof(char)*(SIZE+1) );
   myFuncGen->fBNCCommand        = (char*)malloc( sizeof(char)*(SIZE+1) );
   myFuncGen->fNTypeCommand      = (char*)malloc( sizeof(char)*(SIZE+1) );
   strcpy(myFuncGen->fName             ,"UNKNOWN"); 
   strcpy(myFuncGen->fMACAddress       ,"UNKNOWN"); 
   strcpy(myFuncGen->fFreqUnits        ,"ND");
   strcpy(myFuncGen->fBNCVoltageUnits  ,"ND");
   strcpy(myFuncGen->fNTypeVoltageUnits,"ND");
   strcpy(myFuncGen->fBNCState         ,"off");
   strcpy(myFuncGen->fNTypeState       ,"off");
   strcpy(myFuncGen->fFreqCommand      ,"ND");
   strcpy(myFuncGen->fBNCCommand       ,"ND");
   strcpy(myFuncGen->fNTypeCommand     ,"ND");
   myFuncGen->fMechSwID          = 0; 
   myFuncGen->fFrequency         = 0; 
   myFuncGen->fBNCVoltage        = 0; 
   myFuncGen->fNTypeVoltage      = 0; 
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
}
//_____________________________________________________________________________
void ImportSG382Data_LO(char *filename,struct FuncGen *myFuncGen){

   // SG382 data for the local oscillator 
 
   double ivalue;
   int j=0,k=0,N=0;
   const int MAX = 2000;
   const int uMAX= 10;
   const int tMAX= 30;
   const int sMAX= 3; 
   char buf[MAX+1],itag[tMAX+1],iunit[uMAX+1],istate[sMAX+1];
   char ntype[10],bnc[10],freq[20];
   sprintf(ntype,"%s","ntype"    ); 
   sprintf(bnc  ,"%s","bnc"      );
   sprintf(freq ,"%s","frequency");  

   // memory allocation 
   // myFuncGen->fBNCState          = (char*)malloc( sizeof(char)*(sMAX+1) );
   // myFuncGen->fNTypeState        = (char*)malloc( sizeof(char)*(sMAX+1) );
   // myFuncGen->fFreqUnits         = (char*)malloc( sizeof(char)*(uMAX+1) );
   // myFuncGen->fBNCVoltageUnits   = (char*)malloc( sizeof(char)*(uMAX+1) );
   // myFuncGen->fNTypeVoltageUnits = (char*)malloc( sizeof(char)*(uMAX+1) );
   // myFuncGen->fFreqCommand       = (char*)malloc( sizeof(char)*(tMAX+1) );
   // myFuncGen->fBNCCommand        = (char*)malloc( sizeof(char)*(tMAX+1) );
   // myFuncGen->fNTypeCommand      = (char*)malloc( sizeof(char)*(tMAX+1) );

   double volt_peak=0,volt_dbm=0;

   FILE *infile;
   infile = fopen(filename,READ_MODE);

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
            if(gIsDebug && gVerbosity>=1) printf("%s %s %.4lf %s \n",itag,istate,ivalue,iunit); 
            if( !AreEquivStrings(itag,constants_t::eof_tag.c_str()) ){
               // LO details 
               if( AreEquivStrings(itag,freq) ){ 
                  myFuncGen->fFrequency = ivalue; 
                  strcpy(myFuncGen->fFreqUnits,iunit);
               }else if( AreEquivStrings(itag,bnc) ){
                  myFuncGen->fBNCVoltage = ivalue; 
                  strcpy(myFuncGen->fBNCState       ,istate); 
                  strcpy(myFuncGen->fBNCVoltageUnits,iunit); 
                  if( AreEquivStrings(istate,constants_t::on.c_str()) || AreEquivStrings(istate,constants_t::ON.c_str()) ){
                     myFuncGen->fIntBNCState = 1; 
                  }else if( AreEquivStrings(istate,constants_t::off.c_str()) || AreEquivStrings(istate,constants_t::OFF.c_str()) ){
                     myFuncGen->fIntBNCState = 0; 
                  } 
               }else if( AreEquivStrings(itag,ntype) ){
                  // just in case someone uses Watts... 
                  if( AreEquivStrings(iunit,"Watts") ){ 
                     volt_peak = GetVoltageUsingPower(ivalue,_50_OHMS); 
                     volt_dbm  = ConvertVoltageFrom_Vp_to_dBm(volt_peak); 
                     ivalue    = volt_dbm; 
                  }
                  myFuncGen->fNTypeVoltage = ivalue; 
                  strcpy(myFuncGen->fNTypeState       ,istate); 
                  strcpy(myFuncGen->fNTypeVoltageUnits,iunit);
                  if( AreEquivStrings(istate,constants_t::on.c_str()) || AreEquivStrings(istate,constants_t::ON.c_str()) ){
                     myFuncGen->fIntNTypeState = 1; 
                  }else if( AreEquivStrings(istate,constants_t::off.c_str()) || AreEquivStrings(istate,constants_t::OFF.c_str()) ){
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
   // LO 
   char *freq_cmd  = (char*)malloc( sizeof(char)*(tMAX+1) ); 
   char *bnc_cmd   = (char*)malloc( sizeof(char)*(tMAX+1) ); 
   char *ntype_cmd = (char*)malloc( sizeof(char)*(tMAX+1) ); 

   sprintf(freq_cmd ,"%.14lf %s",myFuncGen->fFrequency   ,myFuncGen->fFreqUnits);
   sprintf(bnc_cmd  ,"%.14lf %s",myFuncGen->fBNCVoltage  ,myFuncGen->fBNCVoltageUnits);
   sprintf(ntype_cmd,"%.14lf %s",myFuncGen->fNTypeVoltage,myFuncGen->fNTypeVoltageUnits);
   strcpy(myFuncGen->fFreqCommand ,freq_cmd);
   strcpy(myFuncGen->fBNCCommand  ,bnc_cmd);
   strcpy(myFuncGen->fNTypeCommand,ntype_cmd);
  
   free(freq_cmd); 
   free(bnc_cmd); 
   free(ntype_cmd); 
 
   if(gIsDebug) printf("----------------------------------------- \n");

}
//_____________________________________________________________________________
void ImportSG382Data_pi2(char *filename,int NCH,struct FuncGen *myFuncGen){

   // SG382 data for the local oscillator 
 
   int iid=0,i=0,j=0,k=0,N=0,istate=0;
   const int MAX = 2000;
   const int uMAX=10;
   const int tMAX=30;
   // const int sMAX=3; 
   double ifreq=0,iampl=0,pwr=0,vp_input=0;
   char buf[MAX+1]; 
   char ifreq_unit[uMAX+1],iampl_unit[uMAX+1];

   // memory allocation  
   // for(i=0;i<NCH;i++){
   //    myFuncGen[i].fBNCState          = (char*)malloc( sizeof(char)*(sMAX+1) );
   //    myFuncGen[i].fNTypeState        = (char*)malloc( sizeof(char)*(sMAX+1) );
   //    myFuncGen[i].fFreqUnits         = (char*)malloc( sizeof(char)*(uMAX+1) );
   //    myFuncGen[i].fBNCVoltageUnits   = (char*)malloc( sizeof(char)*(uMAX+1) );
   //    myFuncGen[i].fNTypeVoltageUnits = (char*)malloc( sizeof(char)*(uMAX+1) );
   //    myFuncGen[i].fFreqCommand       = (char*)malloc( sizeof(char)*(tMAX+1) );
   //    myFuncGen[i].fBNCCommand        = (char*)malloc( sizeof(char)*(tMAX+1) );
   //    myFuncGen[i].fNTypeCommand      = (char*)malloc( sizeof(char)*(tMAX+1) );
   // }

   double VOLTAGE=0;   

   FILE *infile;
   infile = fopen(filename,READ_MODE);

   if(infile==NULL){
      printf("[SG382::ImportSG382Data]: Cannot open the file: %s.  Exiting... \n",filename);
      exit(1);
   }else{
      if(gIsDebug) printf("[SG382::ImportSG382Data]: Opening the file: %s... \n",filename);
      while( !feof(infile) ){
         if(k==0){
            fgets(buf,MAX,infile);
         }else{
            fscanf(infile,"%d %d %lf %s %lf %s",&iid,&istate,&ifreq,ifreq_unit,&iampl,iampl_unit);
            if(gIsDebug && gVerbosity>=1) printf("%d %.5lf %s %.5lf %s\n",iid,ifreq,ifreq_unit,iampl,iampl_unit); 
	    if( iid!=eof_tag_alt ){
               // set mechanical switch ID 
               myFuncGen[j].fMechSwID  = iid; 
	       // set frequency 
	       myFuncGen[j].fFrequency = ifreq; 
	       strcpy(myFuncGen[j].fFreqUnits,ifreq_unit);
	       // disable BNC (always disconnected)  
	       myFuncGen[j].fBNCVoltage = 1E-3; 
               myFuncGen[j].fBNCPower   = 0; 
	       strcpy(myFuncGen[j].fBNCState,"off"); 
	       strcpy(myFuncGen[j].fBNCVoltageUnits,constants_t::Vpp.c_str());
	       myFuncGen[j].fIntBNCState = 0; 
	       // set N-Type 
               // first determine appropriate peak-to-peak voltage 
               // appropriate for the TOMCO such that the output from 
               // the TOMCO gives the requested value (recall, the power of the tomco is 250 W)  
               if( AreEquivStrings(iampl_unit,constants_t::Watts.c_str()) ){
                  pwr      = iampl; 
               }else if( AreEquivStrings(iampl_unit,constants_t::Vpp.c_str()) ){
                  pwr      = GetPower(iampl/2.,_50_OHMS);   
               }else if( AreEquivStrings(iampl_unit,constants_t::Vp.c_str()) ){
                  pwr      = GetPower(iampl,_50_OHMS);   
               }else if( AreEquivStrings(iampl_unit,constants_t::dBm.c_str()) ){ 
                  iampl    = ConvertVoltageFrom_dBm_to_Vp(iampl); 
                  pwr      = GetPower(iampl,_50_OHMS);  
               }
               vp_input = CalculateVinForTOMCO(pwr,_50_OHMS);
               VOLTAGE  = 2.*vp_input;   // convert to Vpp
               if(pwr>constants_t::PI2_POWER_LIMIT){
                 std::cout << "[SG382]: WARNING!  Unsafe requested power of " << pwr << " W.  Changing to 3.5 W." << std::endl;
                 pwr     = 3.5; 
                 VOLTAGE = 2.*CalculateVinForTOMCO(pwr,_50_OHMS); 
               }
               // VOLTAGE    = ConvertVoltageFrom_Vp_to_dBm(vp_input); // convert to dBm  
	       myFuncGen[j].fNTypeVoltage = VOLTAGE; 
               myFuncGen[j].fNTypePower   = pwr; 
	       strcpy(myFuncGen[j].fNTypeVoltageUnits,constants_t::Vpp.c_str());
	       myFuncGen[j].fIntNTypeState = istate; 
               if(istate==1){ 
		  strcpy(myFuncGen[j].fNTypeState,"on");
               }else if(istate==0){
		  strcpy(myFuncGen[j].fNTypeState,"off");
               }  
	    } 
	    j++;
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

   for(i=0;i<NCH;i++){ 
      sprintf(freq_cmd ,"%.14lf %s",myFuncGen[i].fFrequency   ,myFuncGen[i].fFreqUnits);
      sprintf(bnc_cmd  ,"%.14lf %s",myFuncGen[i].fBNCVoltage  ,myFuncGen[i].fBNCVoltageUnits);
      sprintf(ntype_cmd,"%.14lf %s",myFuncGen[i].fNTypeVoltage,myFuncGen[i].fNTypeVoltageUnits);
      strcpy(myFuncGen[i].fFreqCommand ,freq_cmd);
      strcpy(myFuncGen[i].fBNCCommand  ,bnc_cmd);
      strcpy(myFuncGen[i].fNTypeCommand,ntype_cmd);
   }

   if(gIsDebug) printf("----------------------------------------- \n");

   // clear memory 
   free(freq_cmd); 
   free(bnc_cmd); 
   free(ntype_cmd); 

}

