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

#define SG382_RET_BUF_SIZE 2048 

// Library to control Stanford Research Systems RG382 
// RF Signal Generator over USB->SG382 port

void InitFuncGenStruct(struct FuncGen *myFuncGen); 
void ImportSG382Data(char *filename,struct FuncGen *myFuncGen); 
void PrintFuncGen(const struct FuncGen myFuncGen); 
void BlankFuncGen(struct FuncGen *myFuncGen); 

int InitFuncGen(struct FuncGen *myFuncGen);
int ProgramFuncGen(const struct FuncGen myFuncGen);
int SG382Init(void);
int SG382ClearError(int rs232_handle);
int SG382GetError(int rs232_handle);
int SG382FlushErrors(int rs232_handle);
int SG382CheckInput(const struct FuncGen myFuncGen); 
int SG382Close(int rs232_handle);
int SG382Write(int rs232_handle, char *buffer, int buffer_size);
int SG382Read(int rs232_handle, char *in_buffer, int in_size, char *out_buffer, int out_size);
int SG382SetFreq(int rs232_handle, char *freq); 
int SG382SetNTypeAmp(int rs232_handle, char *amp); 
int SG382SetBNCAmp(int rs232_handle, char *amp); 
int SG382SetBNCOutput(int rs232_handle, int flag);
int SG382SetNTypeOutput(int rs232_handle, int flag);
int SG382SetModulation(int rs232_handle, int flag);
int SG382SetModulationRate(int rs232_handle, double freq);
int SG382SetModulationFunction(int rs232_handle, int flag);
int SG382Enable(char *freq, char *bnc_amp, char *ntype_amp,
                int bnc_enable, int ntype_enable);

#endif
