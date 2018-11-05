#ifndef UTIL_H
#define UTIL_H

#include <cstdlib> 
#include <iostream> 
// #include <stdlib.h> 
#include <stdio.h> 
#include <string> 
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

#include "constants.h"
 
// miscellaneous
#define eof_tag_alt          99  
#define RUN_ACTIVE           0 
#define RUN_STOPPED          1
#define ADC_COMM_FAILED      2 
#define KEITHLEY_COMM_FAILED 3 
#define SRS_COMM_FAILED      4 
#define FPGA_COMM_FAILED     5 
// file modes 
#define READ_MODE            "r"  
#define WRITE_MODE           "w"  

extern struct timeval gStart,gStop,gTime; 

extern int gIsFNAL;       // FNAL or ANL? 
extern int gIsDebug;      // debug flag 
extern int gIsTest;       // test mode; if true, no ADC, waits for user to shut down. 
extern int gVerbosity;    // verbosity flag
extern int gIsFLASH;      // FLASH prorgramming flag 
extern double gFreq_RF;   // expected frequency of the NMR signal
extern double gDelayTime; // how long to wait between pulse sequences 

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

int InitConstants();
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
int CopyFile(const char *src_path,const char *dst_path); 

unsigned int BinaryToHex16(int binary[]);
 
unsigned long GetTimeStamp(void); 
unsigned long BinaryToHex32(int binary[]); 

unsigned long long int get_sys_time_us(); 

double GetScale(const char *units); 
double ConvertTimeFromSecondsToUnits(const double time_seconds,const char *units);
double ConvertTimeFromUnitsToSeconds(const double time_in_units,const char *units);
double GetTimeInSeconds(const int counts,const double ClockFreq); 
double GetTimeInUnits(const int counts,const double ClockFreq,const char *units); 

std::string GetTimeStampString(unsigned long unix_time); 

#endif
