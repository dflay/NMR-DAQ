#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <math.h> 
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ctype.h> 

#include "run.h" 
#include "fpgaPulseSequence.h" 

// on/off 
#define on  ( (char *)"on"  )
#define off ( (char *)"off" )
#define ON  ( (char *)"ON"  )
#define OFF ( (char *)"OFF" )
// frequencies
#define kHz ( (char *)"kHz" ) 
#define MHz ( (char *)"MHz" )
#define GHz ( (char *)"GHz" )
// times 
#define second      ( (char *)"s" ) 
#define millisecond ( (char *)"ms")
#define microsecond ( (char *)"us")
#define nanosecond  ( (char *)"ns")
#define picosecond  ( (char *)"ps")
// voltage & power 
#define Vpp         ( (char *)"Vpp" ) 
#define Vp          ( (char *)"Vp" ) 
#define Watts       ( (char *)"Watts" ) 
#define dBm         ( (char *)"dBm" ) 
// miscellaneous
#define eof_tag     ( (char *)"end_of_file" )
#define eof_tag_alt 99  
#define notdef      ( (char *)"ND" )
#define RUN_ACTIVE 0 
#define RUN_STOPPED 0 

struct timeval gStart,gStop,gTime; 

int gIsDebug;      // debug flag 
int gIsTest;       // test mode; if true, no ADC, waits for user to shut down. 
int gVerbosity;    // verbosity flag
int gIsFLASH;      // FLASH prorgramming flag 

double gFreq_RF;   // expected frequency of the NMR signal
double gDelayTime; // how long to wait between pulse sequences  

void InvertBit(int *j); 
void AsciiToBinary(int N,char *ascii,int a[]);
void Test(void); 
void ImportUtilityData(void); 
void GetDateAndTime(int pulse,unsigned long *output); 
void GetTimeStamp_usec(unsigned long *output);
void GetMechSwitchList(const struct fpgaPulseSequence myPulseSequence,int N,int *List); 
void PrintBits(int N,int *data); 
void PrintBits16(u_int16_t data); 
void PrintBits32(u_int32_t data);

char *BinaryToAscii(int N,int binary[]); 
char *GetDirectoryName(struct run *myRun);
char *trimwhitespace(char *str);  

int GetDate(struct run *myRun); 
int GetTime(int IsStart,struct run *myRun); 
int GetMechSwitchIndex(int Switch,const struct fpgaPulseSequence myPulseSequence);  
int ImportComments(char **comment); 
int GetNextRunNumber(char *myDIR); 
int GetBit(int,u_int16_t); 
int GetBit32(int,u_int32_t); 
int AreEquivStrings(const char *s1,const char *s2);
int GetClockCounts(const double time,const double ClockFreq,const char *units); 
int HexToBinary(char hex[]);
int find_string(const char *string_to_search,const char *text);  

unsigned int BinaryToHex16(int binary[]);
 
unsigned long GetTimeStamp(void); 
unsigned long BinaryToHex32(int binary[]); 

double GetScale(const char *units); 
double ConvertTimeFromSecondsToUnits(const double time_seconds,const char *units);
double ConvertTimeFromUnitsToSeconds(const double time_in_units,const char *units);
double GetTimeInSeconds(const int counts,const double ClockFreq); 
double GetTimeInUnits(const int counts,const double ClockFreq,const char *units); 

#endif
