#include "keithley_interface.h"
//______________________________________________________________________________
int keithley_interface_open_connection(void){
   int SIZE = 128;
   char devPath[SIZE]; 
   sprintf(devPath,"dev/usbtmc0"); 

   // FIXME: get rid of these C++ objects! 
   // std::stringstream ss;
   // std::string s;
   // std::ifstream in(devPath);

   FILE *in; 
   in = fopen(devPath,"r"); 

   // look for the keithley 
   char ss[SIZE]; 
   if( in!=NULL ){
      // ss << in->rdbuf();
      fgets(ss,SIZE,in); 
      fclose(in);
   }else{
      printf("No USBTMC devices found!\n"); 
      return 1;
   }

   char s[SIZE],devName[SIZE]; 
   char mfg[] = "KEITHLEY";
   int isFound = 0; 
   for(int i=0;i<16;++i){
      // getline(in,s,'\n');
      fscanf(in,"%s",s);
      isFound = find_string(mfg,s);
      if(isFound==1) { 
	 printf("%s \n",s);
         sprintf(devName, "/dev/usbtmc%i", i);
      }
      strcpy(s,""); 
   }

   int portNo=-1; 
   portNo = open(devName,O_RDWR);
   return portNo;   
}
//______________________________________________________________________________
int keithley_interface_get_device_id(int portNo,char *response){
   const int SIZE = 512;
   char cmd[SIZE];
   sprintf(cmd,"*IDN?\n"); 
   int rc = keithley_interface_query(portNo,cmd,response);
   return rc; 
}
//______________________________________________________________________________
int keithley_interface_get_mode(int portNo,char *response){
   const int SIZE = 512; 
   char cmd[SIZE]; 
   sprintf(cmd,"SENS:FUNC?\n"); 
   int rc = keithley_interface_query(portNo,cmd,response);
   return rc;   
}
//______________________________________________________________________________
int keithley_interface_check_errors(int portNo,char *err_msg){
   const int SIZE = 512; 
   char cmd[SIZE]; 
   sprintf(cmd,"SYST:ERR?\n");
   int rc = keithley_interface_query(portNo,cmd,err_msg);
   // FIXME: parse the string; it's going to be an error code and a message
   printf("keithley error message: %s \n",err_msg); 
   return rc;
}
//______________________________________________________________________________
int keithley_interface_set_to_remote_mode(int portNo){
   const int SIZE = 512;
   char cmd[SIZE];
   sprintf(cmd,"SYST:REM\n"); 
   int rc   = keithley_interface_write_cmd(portNo,cmd);  
   return rc;
}
//______________________________________________________________________________
int keithley_interface_set_range(int portNo,double maxRange){
   const int SIZE = 512;
   char cmd[SIZE];
   sprintf(cmd,"CONF:RES %.3lf\n",maxRange); 
   int rc   = keithley_interface_write_cmd(portNo,cmd);  
   return rc;
}
//______________________________________________________________________________
double keithley_interface_get_resistance(int portNo){
   const int SIZE = 512;
   char cmd[SIZE],response[SIZE];
   sprintf(cmd,"MEAS:RES?\n"); 
   int rc   = keithley_interface_query(portNo,cmd,response);  
   double R = atof(response);
   rc += 0.;  
   return R; 
}
//______________________________________________________________________________
int keithley_interface_write_cmd(int portNo,const char *cmd){
   int rc = write( portNo,cmd,strlen(cmd) );  
   return rc; 
}
//______________________________________________________________________________
int keithley_interface_query(int portNo,const char *cmd,char *response){
   int SIZE = 512; 
   int r = keithley_interface_write_cmd(portNo,cmd);   
   int b = read(portNo,&response,SIZE);
   if(r!=0||b!=0) strcpy(response,"NO RESPONSE");    // comms failed   
   return r;
}
