#include "keithley_interface.h"
//______________________________________________________________________________
int keithley_interface_open_connection(void){
   int SIZE = 128;
   char DEV_PATH[SIZE],DEV_FULL_PATH[SIZE];
   sprintf(DEV_PATH,"%s0","/dev/usbtmc");

   // FIXME: is there a better way to do this?  
   std::stringstream ss;
   std::string inStr;
   std::ifstream in(DEV_PATH);

   // look for the keithley 
   if( in.good() ){
      ss << in.rdbuf();
      in.close();
   }else{
      printf("No USBTMC devices found \n");
      return 1;
   }

   char dev_name[512];
   sprintf(dev_name,"%s","KEITHLEY"); 
   for(int i=0;i<16;i++){
      std::getline(ss,inStr,'\n');
      if(inStr.find(dev_name) != inStr.size() - 1){
         printf("%s \n",inStr.c_str());
         sprintf(DEV_FULL_PATH, "/dev/usbtmc%i", i);
      }
   }

   printf("Device found at: %s \n",DEV_FULL_PATH);

   int portNo=-1;
   portNo = open(DEV_FULL_PATH,O_RDWR);
   return portNo;   
}
//______________________________________________________________________________
int keithley_interface_close_connection(int portNo){
   int rc = close(portNo);
   return rc;
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
   char cmd[SIZE],response[SIZE]; 
   sprintf(cmd,"SYST:ERR?\n");
   int rc = keithley_interface_query(portNo,cmd,response);
   // parse the string; it's going to be an error code and a message
   std::istringstream ss(response);
   std::string token,entry[2];
   int i=0;
   while( std::getline(ss,token,',') ){
      entry[i] = token;
      i++;
   }
   // now return the data 
   rc = atoi(entry[0].c_str());         // zeroth entry is the error code
   strcpy( err_msg,entry[1].c_str() );  // copy the message to the buffer 
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
   int rc = 0;
   int SIZE = 512; 
   int nBytes_wr = keithley_interface_write_cmd(portNo,cmd);  
   usleep(30000);  
   int nBytes_rd = read(portNo,response,SIZE);
   if(nBytes_wr==0||nBytes_rd==0){
      printf("[keithley_interface_query]: Write response = %d \n",nBytes_wr);
      printf("[keithley_interface_query]: Read response  = %d \n",nBytes_rd);
      strcpy(response,"NO RESPONSE");    // comms failed  
      rc = 1;
   } 
   return rc;
}
