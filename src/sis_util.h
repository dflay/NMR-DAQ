#ifndef SIS_UTIL_H
#define SIS_UTIL_H

#define _GNU_SOURCE

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h> 
#include <sys/time.h> 

#include "libs/sis3100_vme_calls.h"
#include "libs/sis1100_var.h"
#include "libs/sis3302_var.h"
#include "libs/sis3316_var.h"

#include "util.h"

#define MAX_NUMBER_LWORDS_64MBYTE 0x1000000       // 64MB 
#define SIS3302_MOD_BASE          0x30000000      // default address of the SIS3302 digitizer 
#define SIS3316_MOD_BASE          0x41000000      // default address of the SIS3316 digitizer 

int ADC_MULTIEVENT_STATE;
int PULSES_PER_READ;                                      // how many pulses to read out during a block-read?  

u_int32_t MOD_BASE; 
// u_int32_t MOD_ID; 

u_int32_t *gDATA;
unsigned short *gDATA_us;

void ClearOutputArrays(int NUM_SAMPLES);

int SISMODID(int vme_handle,u_int32_t MOD_ID); 
int SISClose(int vme_handle); 
int SISWrite32(int vme_handle, u_int32_t addr, u_int32_t data32);
int SISRead32(int vme_handle, u_int32_t addr, u_int32_t *data32);
int SI5325Write(int vme_handle, u_int32_t si5325_addr, u_int32_t data32);

#endif
