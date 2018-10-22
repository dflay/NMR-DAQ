#ifndef SIS3302_UM_H
#define SIS3302_UM_H

// Functions for the SIS3302 ADC.
// The 'UM' tag stands for UMass.  

// #define _GNU_SOURCE

#include <cstdlib>
#include <iostream> 
#include <math.h>
#include <stdlib.h>
#include <stdint.h> 
#include <sys/time.h> 

#include "sis3100_vme_calls.h"
#include "sis1100_var.h"
#include "sis3302_var.h"

#include "adc.h"
#include "util.h"
#include "sis_util.h"
#include "struck_adc.h"

int SIS3302BaseInit(int vme_handle,struct adc *myADC);
int SIS3302ReInit(int vme_handle,struct adc *myADC);
int SIS3302SetClockFreq(int vme_handle,int clock_state,int freq_mhz);
int SIS3302WriteNMRPulses(int vme_handle,const struct adc myADC,char *outdir);
int SIS3302WriteNMRPulsesAlt(int vme_handle,const struct adc myADC,char *outdir);
int SIS3302WriteNMRPulsesAltNew(int vme_handle,int PulseOffset,int NumPulsesToWrite,const struct adc myADC,char *outdir);
int SIS3302WriteNMRPulse(int vme_handle,int PulseNum,const struct adc myADC,char *outdir);
int SIS3302WriteNMRPulseAlt(int vme_handle,int PulseNum,const struct adc myADC,char *outdir);
int SIS3302WriteNMRPulseBin(int vme_handle,int PulseNum,const struct adc myADC,char *outdir);
int SIS3302SampleNMRPulse(int vme_handle);

int SIS3302SampleData(int vme_handle,const struct adc myADC,char *output_dir,int EventNum,int *armed_bank_flag); 

#endif
