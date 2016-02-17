#ifndef ACQUISITION_H 
#define ACQUISITION_H 

// Data acquisition functions 

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "util.h"
#include "adc.h"
#include "fpga.h"
#include "FuncGen.h"
#include "acromag_ipep201.h"
#include "sg382.h"
#include "struck_adc.h"
#include "sis_util.h"

void NextAction(int p,struct FuncGen *myFuncGen,struct fpga *myFPGA);
void ShutDownSystem(int p,struct FuncGen *myFuncGen,struct fpga *myFPGA); 

int AcquireData(int p,struct fpga myFPGA,struct adc myADC,unsigned long **timestamp,char *output_dir,int *MECH);
int AcquireDataSIS3302(int p,struct fpga myFPGA,struct adc myADC,unsigned long **timestamp,char *output_dir,int *MECH);
int AcquireDataSIS3316(int p,struct fpga myFPGA,struct adc myADC,unsigned long **timestamp,char *output_dir,int *MECH);

#endif 
