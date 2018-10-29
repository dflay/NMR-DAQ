#ifndef StruckADC_H
#define StruckADC_H

#include <cstdlib> 
#include <iostream> 
#include <math.h>
#include <stdint.h> 
#include <sys/time.h> 

#include "sis3100_vme_calls.h"
#include "sis1100_var.h"
#include "sis3302_var.h"
#include "sis3316_var.h"

#include "adc.h"
#include "util.h"
#include "acromag_ipep201.h"
#include "sis_util.h"
#include "sis3302_um.h"
#include "sis3316_um.h"

void SISImportData(char *filename,struct adc *myADC); 
void InitADCStruct(struct adc *myADC);
void PrintADC(const struct adc myADC); 
void ReconfigADCStruct(double SignalLength,struct adc *myADC); 

int SISLoad(struct adc *myADC); 
int SISBaseInit(int vme_handle,struct adc *myADC);
int SISReInit(int vme_handle,struct adc *myADC,int event);
int SISInitGlobalVariables(const struct adc myADC,bool isFNAL);
int SISIOSpaceRead(int vme_handle,u_int32_t MOD_ID,const struct adc myADC);
int FIFORead(int vme_handle,int an_addr,u_int32_t NUM_SAMPLES);

// int SISInit(int vme_handle,struct adc *myADC,int event);

#endif
