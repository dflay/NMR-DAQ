#include "util.h"
//______________________________________________________________________________
void InvertBit(int *j){
   int val = *j; 
   if(val==0) *j = 1; 
   if(val==1) *j = 0; 
   printf("input = %d output = %d \n",val,*j); 
}
//______________________________________________________________________________
void PrintBits16(u_int16_t data){
   int i=0,cntr=0,aBit=0;
   const int N = 16; 
   for(i=N-1;i>=0;i--){
      cntr++; 
      aBit = GetBit(i,data);
      printf("%d",aBit); 
      if(cntr==4){
         printf(" "); 
         cntr = 0; 
      } 
   }
   printf("\n"); 
}
//______________________________________________________________________________
void PrintBits32(u_int32_t data){
   int i=0,cntr=0,aBit=0;
   const int N = 32; 
   for(i=N-1;i>=0;i--){
      cntr++; 
      aBit = GetBit(i,data);
      printf("%d",aBit);  
      if(cntr==4){
         printf(" "); 
         cntr = 0; 
      }
   }
   printf("\n"); 
}
//______________________________________________________________________________
void PrintBits(int N,int *data){
   int i=0,cntr=0;
   const int NB = N; 
   for(i=NB-1;i>=0;i--){
      printf("%d",data[i]); 
      if(cntr==4){
         printf(" "); 
         cntr = 0; 
      } 
   }
   printf("\n"); 
}
//______________________________________________________________________________
int GetMechSwitchIndex(int Switch,const struct fpgaPulseSequence myPulseSequence){
   int i=0,mech_sw=0,isw=0;
   const int NS = myPulseSequence.fNSequences; 
   for(i=0;i<NS;i++){
      mech_sw = myPulseSequence.fMechSwID[i]; 
      if(Switch==mech_sw) isw = i;
   }
   return isw; 
}
//______________________________________________________________________________
void GetMechSwitchList(const struct fpgaPulseSequence myPulseSequence,int N,int *List){
   // fill the array list with the appropriate mechanical switches used 
   
   const int NS = myPulseSequence.fNSequences; 

   // find how many switches enabled 
   int cntr=0;
   int i=0;
   for(i=0;i<NS;i++){
      if(myPulseSequence.fEnableFlag[i]==1) cntr++;
   }

   // make an array for the active switches 
   const int SW_SIZE = cntr;
   int *Switch = (int *)malloc( sizeof(int)*SW_SIZE ); 
   for(i=0;i<SW_SIZE;i++){
      Switch[i] = 0;
   }

   // fill switch array with active switches 
   int j=0;
   for(i=0;i<NS;i++){
      if(myPulseSequence.fEnableFlag[i]==1){
	 Switch[j] = myPulseSequence.fMechSwID[i];
	 j++;
      }
   } 
 
   // fill the final list with the appropriate switches  
   j=0; 
   for(i=0;i<N;i++){
      List[i] = Switch[j];
      j++;
      if(j>=SW_SIZE) j = 0;
   }

   // delete allocated memory 
   free(Switch);  

}
//______________________________________________________________________________
unsigned long GetTimeStamp(void){

   struct timeval  tv;
   struct timezone tz;
   struct tm      *tm;
   unsigned long  start;

   gettimeofday(&tv, &tz);
   tm = localtime(&tv.tv_sec);

   unsigned long hr      = tm->tm_hour; 
   unsigned long min     = tm->tm_min; 
   unsigned long sec     = tm->tm_sec; 
   unsigned long usec    = tv.tv_usec;
   unsigned long tot_sec = hr*3600*1E+0 + min*60*1E+0 + sec*1E+0 + usec/1E+6;
   unsigned long tot_ms  = hr*3600*1E+3 + min*60*1E+3 + sec*1E+3 + usec/1E+3;
   unsigned long tot_us  = hr*3600*1E+6 + min*60*1E+6 + sec*1E+6 + usec;

   printf("[NMRDAQ]: Time stamp: %lu:%02lu:%02lu:%lu (~%lu s, ~%lu ms, ~%lu us)\n",hr,
         min,sec,usec,tot_sec,tot_ms,tot_us);

   start = tot_ms;

   return start;
 
}
//______________________________________________________________________________
void GetTimeStamp_usec(unsigned long *output){

   struct timeval  tv;
   struct timezone tz;
   struct tm      *tm;

   gettimeofday(&tv, &tz);
   tm = localtime(&tv.tv_sec);

   unsigned long hr      = tm->tm_hour; 
   unsigned long min     = tm->tm_min; 
   unsigned long sec     = tm->tm_sec; 
   unsigned long usec    = tv.tv_usec;
   unsigned long tot_sec = hr*3600*1E+0 + min*60*1E+0 + sec*1E+0 + usec/1E+6;
   unsigned long tot_ms  = hr*3600*1E+3 + min*60*1E+3 + sec*1E+3 + usec/1E+3;
   unsigned long tot_us  = hr*3600*1E+6 + min*60*1E+6 + sec*1E+6 + usec;

   if(gIsDebug){
      printf("[NMRDAQ]: Time stamp: %lu:%02lu:%02lu:%lu (~%lu s, ~%lu ms, ~%lu us)\n",hr,
            min,sec,usec,tot_sec,tot_ms,tot_us);
   }

   output[0] = 0; 
   output[1] = 0; 
   output[2] = 0; 
   output[3] = tot_sec; 
   output[4] = tot_ms; 
   output[5] = tot_us; 
 
}
//______________________________________________________________________________
void GetDateAndTime(int pulse,unsigned long *output){

    time_t rawtime;
    struct tm *timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    output[0] = timeinfo->tm_mday; 
    output[1] = timeinfo->tm_mon + 1; 
    output[2] = timeinfo->tm_year + 1900;
    output[3] = timeinfo->tm_hour; 
    output[4] = timeinfo->tm_min; 
    output[5] = timeinfo->tm_sec; 

    // printf("pulse: %d \t date: %d %d %d time: %d:%d:%d \n",pulse,timeinfo->tm_mday,
    //        timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, 
    //        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

}
//______________________________________________________________________________
int GetBit(int k,u_int16_t data){
   int n        = data;
   int mask     = 1 << k;        // move "1" k spaces to the left
   int masked_n = n & mask;      // AND n with mask -- this picks out the bit at position k 
   int the_bit  = masked_n >> k; // move the masked_n k spaces to the right  
   return the_bit; 
}
//______________________________________________________________________________
int GetBit32(int k,u_int32_t data){
   int n        = data;
   int mask     = 1 << k;        // move "1" k spaces to the left
   int masked_n = n & mask;      // AND n with mask -- this picks out the bit at position k 
   int the_bit  = masked_n >> k; // move the masked_n k spaces to the right  
   return the_bit; 
}
//______________________________________________________________________________
char *BinaryToAscii(int N,int binary_data[]){

   // works for bytes (that is, nbits >= 8) 

   int length = N; // strlen(input);     // get length of string

   const int SIZE = length/8; 

   int binary[8];    // array used to store 1 byte of binary number (1 character)
   int asciiNum = 0; // the ascii number after conversion from binary
   char ascii;       // the ascii character itself

   char *ascii_array = (char *)malloc( sizeof(char)*(SIZE+1) ); 

   int power[8];
   int i=0,j=0,z=0; 
   for(i=0;i<8;i++) power[i] = i; 

   double a=0,b=0;

   // printf("Num bits = %d, N = %d \n",length,length/8); 

   for(i=length/8-1;i>=0;i--){
      // reading in bytes. total characters = length / 8
      for(j=0;j<8;j++){    
         // store info into binary[0] through binary[7]
         // binary[j] = data[z] - 48;      // z never resets.  why 48?? 
         binary[j] = binary_data[z];      // z never resets 
         // printf("bit %d: %d \n",j+4*i,binary[j]);
         z++;
      }
      // compute the ascii number
      for(j=0;j<8;j++){
         a = binary[j];            // store the element from binary[] as "a"
         b = power[j];             // store the lement from power[] as "b"
         asciiNum += a*pow(2, b);  // asciiNum = sum of a * 2^power where 0 <= power <= 7, power is int
      }
      ascii    = asciiNum;         // assign the asciiNum value to ascii, to change it into an actual character
      asciiNum = 0;                // reset asciiNum for next loop

      ascii_array[i] = ascii; 

      // printf("%c",ascii);         // display the ascii character
   }

   return ascii_array; 

}
//______________________________________________________________________________
void AsciiToBinary(int N,char *ascii,int binary[]){

   // int SIZE = N*8 + 1;   // 1 extra for the \n 
   int num_value=0;
   // int binary[SIZE]; 

   int length = strlen(ascii);                          
   int count  = 0;
   int pos    = 0;

   int i=0;
   for (i = length-1; i>=0; i--){
      num_value = ascii[i];
      while(count<8){
         if( (num_value%2) == 0){
            binary[pos] = 0;
            num_value = num_value/2;
            count++;
            pos++;
         }else{
            binary[pos] = 1;
            num_value = num_value/2;
            count++;
            pos++;
         } 
      }
      count = 0; /*resets loop counter*/
   }

   // printf("Binary representation of %s: ",ascii);
 
   // int space=0; 
   // int j=0; 
   // for(j = pos-1; j >= 0; j--){
   //    printf("%d", binary[j]);
   //    space++;
   //    if(space == 8){
   //       printf(" "); /*white space between bytes*/
   //       space = 0; 
   //    }
   // } 

   // printf("\n"); 

}
//______________________________________________________________________________
int HexToBinary(char hex[]){
   int i, length, decimal=0, binary=0;
   for(length=0; hex[length]!='\0'; ++length);
   for(i=0; hex[i]!='\0'; ++i, --length){
      if(hex[i]>='0' && hex[i]<='9')
         decimal+=(hex[i]-'0')*pow(16,length-1);
      if(hex[i]>='A' && hex[i]<='F')
         decimal+=(hex[i]-55)*pow(16,length-1);
      if(hex[i]>='a' && hex[i]<='f')
         decimal+=(hex[i]-87)*pow(16,length-1);
   }

   // At this point, the variable decimal contains the hexadecimal number 
   // in decimal format. 

   i=1;
   while (decimal!=0){
      binary+=(decimal%2)*i;
      decimal/=2;
      i*=10;
   }

   return binary;
}
//______________________________________________________________________________
unsigned int BinaryToHex16(int binary[]){

   const int NBITS   = 16; 
   int pwr[NBITS]; 

   int i=0;
   for(i=0;i<NBITS;i++) pwr[i] = i; 

   // convert binary array to decimal  
   int dec=0;
   // start with LSB 
   for(i=NBITS-1;i>=0;i--){
      dec += binary[i]*pow(2,pwr[i]); 
   }

   unsigned int hex = (unsigned int)dec;
   return hex; 

}
//______________________________________________________________________________
unsigned long BinaryToHex32(int binary[]){

   const int NBITS = 32;  
   int pwr[NBITS]; 

   int i=0;
   for(i=0;i<NBITS;i++) pwr[i] = i; 

   // convert binary array to decimal  
   int dec=0;
   // start with LSB 
   for(i=NBITS-1;i>=0;i--){
      dec += binary[i]*pow(2,pwr[i]); 
   }

   unsigned long hex = (unsigned long)dec;
   return hex; 

}
//______________________________________________________________________________
double GetTimeInUnits(const int counts,const double ClockFreq,const char *units){
   // ClockFreq in Hz 
   // time in units 
   double time_seconds = GetTimeInSeconds(counts,ClockFreq); 
   double time_units   = ConvertTimeFromSecondsToUnits(time_seconds,units); 
   if(gIsDebug && gVerbosity>=2) printf("[NMRDAQ::GetTime]: counts = %d clock = %.2E time = %.3f %s  \n",counts,ClockFreq,time_units,units);
   return time_units;
}
//______________________________________________________________________________
double GetTimeInSeconds(const int counts,const double ClockFreq){
   // ClockFreq in Hz 
   // time in seconds 
   double counts_f = (double)counts;
   double time     = counts_f/ClockFreq; 
   if(gIsDebug && gVerbosity>=2) printf("[NMRDAQ::GetTimeInSeconds]: counts = %d clock = %.2E time = %.3f s  \n",counts,ClockFreq,time);
   return time;
}
//______________________________________________________________________________
int GetClockCounts(const double time,const double ClockFreq,const char *units){
   double time_seconds = ConvertTimeFromUnitsToSeconds(time,units);  
   int counts         = (int)( ClockFreq*time_seconds );
   if(gIsDebug && gVerbosity>=2) printf("[NMRDAQ::GetClockCounts]: time = %.3f %s clock = %.2E counts = %d \n",time,units,ClockFreq,counts);
   return counts;
}
//______________________________________________________________________________
double ConvertTimeFromSecondsToUnits(const double time_seconds,const char *units){
   // time is in seconds
   // output time is in units (e.g., ms, us, ns) 
   double scale         = GetScale(units); 
   double time_in_units = time_seconds/scale; 
   return time_in_units; 
}
//______________________________________________________________________________
double ConvertTimeFromUnitsToSeconds(const double time_in_units,const char *units){
   // time_in_units is in units (e.g., ms, us, ns)
   // output time is in seconds  
   double scale = GetScale(units); 
   double time  = time_in_units*scale; 
   return time; 
}
//______________________________________________________________________________
double GetScale(const char *units){
   double scale=0;
   if( AreEquivStrings(units,second) ){
      scale = 1.0;
   }else if( AreEquivStrings(units,millisecond) ){
      scale = 1E-3;
   }else if( AreEquivStrings(units,microsecond) ){
      scale = 1E-6;
   }else if( AreEquivStrings(units,nanosecond) ){
      scale = 1E-9;
   }else if( AreEquivStrings(units,picosecond) ){
      scale = 1E-12;
   }else if( AreEquivStrings(units,notdef) ){
      scale = -1;
   }else{
      printf("[NMRDAQ::GetScale]: Invalid units!  Exiting... \n");
      exit(1);
   }
   return scale; 
}
//______________________________________________________________________________
int AreEquivStrings(const char *s1,const char *s2){

   int ret_val = 0; 

   int res = strcmp(s1,s2);
   if(res==0){
      ret_val = 1; 
   }else if(res==1){
      ret_val = 0; 
   } 

   return ret_val; 

}
//______________________________________________________________________________
void ImportUtilityData(void){

   int ivalue=0;
   int counter=0;
   int k=0,N=0;
   const int MAX = 2000;
   const int tMAX= 20;
   char buf[MAX],itag[tMAX];
   char *mode       = "r";
   char *rf_freq    = "rf_frequency"; 
   char *debug_tag  = "debug_mode";
   char *verb_tag   = "verbosity";
   char *test_tag   = "test_mode";
   char *dt_tag     = "delay_time";  
   char *debug_mode = off;

   char *filename = "./input/utilities.dat";

   FILE *infile;
   infile = fopen(filename,mode);

   if(infile==NULL){
      printf("[NMRDAQ::ImportUtilityData]: Cannot open the file: %s.  Exiting... \n",filename);
      exit(1);
   }else{
      // printf("[NMRDAQ::ImportUtilityData]: Opening the file: %s... \n",filename);
      while( !feof(infile) ){
         if(k==0){
            fgets(buf,MAX,infile);
         }else{
            fscanf(infile,"%s %d",itag,&ivalue);
            if( !AreEquivStrings(itag,eof_tag) ){
               // set debug flag 
               if( AreEquivStrings(itag,debug_tag) ){
                  gIsDebug = (int)ivalue;
               }
               // set verbosity flag 
               if( AreEquivStrings(itag,verb_tag) ){
                  gVerbosity = (int)ivalue;
               }
               // set test flag 
               if( AreEquivStrings(itag,test_tag) ){
                  gIsTest = (int)ivalue;
               }
               // set reference frequency 
               if( AreEquivStrings(itag,rf_freq) ){
                  gFreq_RF = (double)ivalue;
               }
               // set delay time 
               if( AreEquivStrings(itag,dt_tag) ){
                 gDelayTime = (double)ivalue; 
               }
               counter++;
            }else{
               break;
            }
         }
         k++;
      }
      N = counter;
      fclose(infile);
      if(N==0){
         printf("[NMRDAQ::ImportUtilityData]: No data!  Exiting... \n");
         exit(1);
      }
   }

   if(gIsDebug) debug_mode = on;

   if(gIsDebug) printf("debug mode: %d (%s) \n",gIsDebug,debug_mode);
   if(gIsDebug) printf("verbosity:  %d \n",gVerbosity);
   if(gIsDebug) printf("----------------------------------------- \n");

   if(gIsTest==1){
      printf("[NMRDAQ]: Running in TEST MODE. ");
      printf("No data will be recorded by the ADC; the user will be prompted shut down the system. \n"); 
   }else if(gIsTest==2){
      printf("[NMRDAQ]: Running in TEST MODE; the Module ID of the ADC will be read, and nothing else. \n");
   }else if(gIsTest==3){
      printf("[NMRDAQ]: Running in TEST MODE; nothing happens (so far). \n");
   }else if(gIsTest==4){
      printf("[NMRDAQ]: Running in TEST MODE; SG382 is MODULATED by EXTERNAL signal. \n");
      printf("          No data will be recorded by the ADC; the user will be prompted to shut down the system.\n");
   }else if(gIsTest==5){
      printf("[NMRDAQ]: Running in TEST MODE; SG382 is MODULATED by EXTERNAL signal. \n");
      printf("          Data WILL be recorded by the ADC.\n");
   }

}
//______________________________________________________________________________
int ImportComments(char **comment){

   int j=0,N=0;
   const int MAX  = 2000;
   char buf[MAX];
   char *mode     = "r";
   char *line     = (char*)malloc( sizeof(char)*(MAX+1) );
   char *filename = "./input/comments.txt";

   FILE *infile;
   infile = fopen(filename,mode);

   if(infile==NULL){
      printf("[NMRDAQ::ImportComments]: Cannot open the file: %s.  Exiting... \n",filename);
      exit(1);
   }else{
      if(gIsDebug) printf("[NMRDAQ::ImportComments]: Opening the file: %s... \n",filename);
      while( !feof(infile) ){
         fgets(buf,MAX,infile);
         if( !AreEquivStrings(buf,eof_tag) ){
            comment[j] = (char*)malloc( sizeof(char)*(MAX+1) );
            line = trimwhitespace(buf);
            strcpy(comment[j],line);
            if(gIsDebug) printf("line %d: %s \n",j,comment[j]); 
            j++; 
         }else{
            break;
         }
      }
      N = j;
      fclose(infile);
   }

   N -= 1; // total number of lines; the above goes one over the true number of lines. 

   return N;   

} 
//______________________________________________________________________________
int GetNextRunNumber(char *myDIR){

   DIR *d;
   struct dirent *dir; 

   char *current_dir = ".";
   char *parent_dir  = ".."; 

   const int SIZE = 100; 
   char *a_dir = (char*)malloc( sizeof(char)*(SIZE+1) );
   char *p     = (char*)malloc( sizeof(char)*(SIZE+1) );

   int IsCurrentDir = 0; 
   int IsParentDir  = 0; 

   int RunMax    = -30; 
   int RunNumber = -1; 

   long val; 

   d = opendir(myDIR);
   if(d){
      while( (dir=readdir(d)) != NULL ){
         if(dir->d_type==DT_DIR){
            a_dir        = dir->d_name;
            IsCurrentDir = AreEquivStrings(a_dir,current_dir);  
            IsParentDir  = AreEquivStrings(a_dir,parent_dir); 
            p            = a_dir;   
            if( !IsCurrentDir && !IsParentDir ){
               // cycle through the characters of the directory name, 
               // find the number 
               // printf("directory name = %s \n",a_dir); 
               while(*p){
                  if( isdigit(*p) ){  
                     // character is a number, record its value 
                     val       = strtol(p,&p,10); 
                     RunNumber = (int)val;
                     if(RunNumber>RunMax) RunMax = RunNumber;         
                     // printf("run = %d \n",RunMax); 
                  }else{
                     // character is not a number, move on 
                     p++; 
                  }
               }
               // printf("run = %d \n",RunMax); 
            }
         }
      }
   }else{
      RunMax = 0;  // no directories available, run number starts at 0  
   }

   // check to see if we found a number;
   // if we didn't, set RunMax to 0, since it didn't change from -30. 
   if(RunMax<0) RunMax = 0;    

   RunMax++;      // return the next run number 
   return RunMax; 

}
//______________________________________________________________________________
char *GetDirectoryName(struct run *myRun,char *BASE_DIR){

   struct tm *tm;
   time_t t;

   const int SIZE_100  = 100; 
   const int SIZE_200  = 200; 
   const int SIZE_2000 = 2000; 

   char str_time[SIZE_100];
   char str_date[SIZE_100];
   char str_month[SIZE_100];
   char str_year[SIZE_100];
   char year_dir[SIZE_200];
   char month_dir[SIZE_200];
   char date_dir[SIZE_200];

   char the_day[SIZE_100]; 
   char the_month[SIZE_100]; 
   char the_year[SIZE_100]; 
   char the_hour[SIZE_100]; 
   char the_minute[SIZE_100]; 
   char the_second[SIZE_100]; 

   char *prefix = "./data";
   char *data_dir = (char*)malloc( sizeof(char)*(SIZE_2000+1) );

   t  = time(NULL);
   tm = localtime(&t);

   // get labels 
   strftime(str_time ,sizeof(str_time) ,"%H-%M-%S", tm);
   strftime(str_year ,sizeof(str_year) ,"%Y"      , tm);
   strftime(str_month,sizeof(str_month),"%m_%y"   , tm);
   strftime(str_date ,sizeof(str_date) ,"%m_%d_%y", tm);

   strftime(the_year ,sizeof(str_year) ,"%Y", tm);
   strftime(the_month,sizeof(str_month),"%m", tm);
   strftime(the_day  ,sizeof(str_date) ,"%d", tm);

   strftime(the_hour  ,sizeof(str_year) ,"%H", tm);
   strftime(the_minute,sizeof(str_month),"%M", tm);
   strftime(the_second,sizeof(str_date) ,"%S", tm);

   // make strings 
   sprintf(year_dir ,"%s/%s",prefix,str_year);
   sprintf(month_dir,"%s/%s",year_dir,str_month);
   sprintf(date_dir ,"%s/%s",month_dir,str_date);
   // make directories 
   mkdir(year_dir ,0700);
   mkdir(month_dir,0700);
   mkdir(date_dir ,0700);
   // construct directory path with run number 
   int RunNumber     = GetNextRunNumber(date_dir);
   myRun->fRunNumber = RunNumber; 
   myRun->fDay       = atoi(the_day);  
   myRun->fMonth     = atoi(the_month);  
   myRun->fYear      = atoi(the_year); 
   myRun->fHour      = atoi(the_hour); 
   myRun->fMinute    = atoi(the_minute); 
   myRun->fSecond    = atoi(the_second); 
   sprintf(data_dir,"%s/run-%d",date_dir,RunNumber);
   sprintf(BASE_DIR,"%s",date_dir); 

   mkdir(data_dir ,0700);
 
   if(gIsDebug && gVerbosity >=1) printf("[NMRDAQ]: Printing data to the directory: %s \n",data_dir); 
 
   return data_dir; 

}
//______________________________________________________________________________
char *trimwhitespace(char *str){

   // Note: This function returns a pointer to a substring of the original string.
   // If the given string was allocated dynamically, the caller must not overwrite
   // that pointer with the returned value, since the original pointer must be
   // deallocated using the same allocator with which it was allocated.  The return
   // value must NOT be deallocated using free() etc.

   char *end;

   // Trim leading space
   while(isspace(*str)) str++;

   if(*str == 0)  // All spaces?
      return str;

   // Trim trailing space
   end = str + strlen(str) - 1;
   while(end > str && isspace(*end)) end--;

   // Write new null terminator
   *(end+1) = 0;

   return str;
}

