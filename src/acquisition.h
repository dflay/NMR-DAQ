#ifndef ACQUISITION_H 
#define ACQUISITION_H 

// Data acquisition functions 

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "event.h"
#include "util.h"
#include "adc.h"
#include "fpga.h"
#include "fpgaPulseSequence.h"
#include "FuncGen.h"
#include "acromag_ipep201.h"
#include "sg382.h"
#include "struck_adc.h"
#include "sis_util.h"
#include "keithley.h"
#include "keithley_interface.h"

// void NextAction(int p,struct FuncGen *myFuncGen,struct fpga *myFPGA);
// void ShutDownSystem(int p,struct FuncGen *myFuncGen,struct fpga *myFPGA); 

int  ShutDownSystemNew(int p,
                       struct FuncGen *myFuncGen,struct FuncGen *myFuncGenPi2,
                       struct fpgaPulseSequence *myPulseSequence,
                       keithley_t *myKeithley); 

// int AcquireDataNew(int p,
//                    struct fpgaPulseSequence myPulseSequence,
//                    struct FuncGen *myFuncGenPi2,
//                    struct adc *myADC,
//                    keithley_t *myKeithley,
//                    double *resistance,
//                    unsigned long long *timestamp,char *output_dir,int *MECH);

int AcquireDataNew(int p,
                   struct fpgaPulseSequence myPulseSequence,
                   struct FuncGen *myFuncGenPi2,
                   struct adc *myADC,
                   keithley_t *myKeithley,
                   event_t *myEvent,
                   char *output_dir);

// int AcquireDataSIS3302New(int p,
//                           struct fpga myFPGA,
//                           struct adc myADC,
//                           unsigned long **timestamp,char *output_dir,int *MECH);

int AcquireDataSIS3316New(int p,int i,
                          struct fpgaPulseSequence myPulseSequence,
                          struct adc myADC,
                          unsigned long long &timestamp,int &MECH,char *output_dir,int *armed_bank_flag);


#endif 
