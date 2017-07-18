#ifndef SG382_H
#define SG382_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "util.h"
#include "FuncGen.h"
#include "nmr_math.h"

// #define SG382_LO_DEV_PATH          "/dev/ttyUSB1"     // for the local oscillator 
// #define SG382_PI2_DEV_PATH         "/dev/ttyUSB0"     // for the pi/2 pulse 
#define SG382_DISABLE              0x0                 
#define SG382_ENABLE_AMPL_ONLY     0x1  
#define SG382_ENABLE_FREQ_ONLY     0x2  
#define SG382_ENABLE_AMPL_AND_FREQ 0x3
#define SG382_RET_BUF_SIZE         2048 

extern int SG382_SLEEP_TIME; 
extern struct termios old_termios;

// Library to control Stanford Research Systems RG382 
// RF Signal Generator over USB->SG382 port

void InitFuncGenStruct(struct FuncGen *myFuncGen); 
void ImportSG382Data_LO(char *filename,struct FuncGen *myFuncGen); 
void ImportSG382Data_pi2(char *filename,int NCH,struct FuncGen *myFuncGen); 
void PrintFuncGen(const struct FuncGen myFuncGen); 
void BlankFuncGen(const char *device_path,struct FuncGen *myFuncGen); 

int SG382Init(const char *device_path);
int SG382ClearErrorAlt(const char *device_path); 
int SG382ClearError(int rs232_handle); 
int SG382GetError(int rs232_handle); 
int InitFuncGenLO(struct FuncGen *myFuncGen);
int InitFuncGenPi2(int NCH,struct FuncGen *myFuncGen);
int ProgramFuncGen(u_int16_t bit_pattern,const char *device_path,const struct FuncGen myFuncGen,int sleep_time);
int SG382CheckInput(const struct FuncGen myFuncGen); 
int SG382Close(int rs232_handle);
int SG382Write(int rs232_handle,const char *buffer);
int SG382Read(int rs232_handle,const char *in_buffer,char *out_buffer,int out_size);
int SG382SetFreq(int rs232_handle, char *freq); 
int SG382SetNTypeAmp(int rs232_handle, char *amp); 
int SG382SetBNCAmp(int rs232_handle, char *amp); 
int SG382SetBNCOutput(int rs232_handle, int flag);
int SG382SetNTypeOutput(int rs232_handle, int flag);
int SG382SetModulation(int rs232_handle, int flag);
int SG382SetModulationRate(int rs232_handle, double freq);
int SG382SetModulationFunction(int rs232_handle, int flag);
int SG382Enable(u_int16_t bit_pattern,const char *device_path,
                char *freq, char *bnc_amp, char *ntype_amp,
                int bnc_enable, int ntype_enable);

#endif
