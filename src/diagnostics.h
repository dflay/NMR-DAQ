#ifndef DIAGNOSTICS_H 
#define DIAGNOSTICS_H 

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "logger.h"
#include "event.h"
#include "util.h"
#include "nmr_math.h"
#include "run.h"
#include "adc.h"
#include "fpga.h"
#include "fpgaPulseSequence.h"
#include "FuncGen.h"
#include "acromag_ipep201.h"

void CheckHardware(void);
void PrintTimeStamp(char *outpath,const struct adc myADC,unsigned long **timestamp);
void PrintTimeStampMicroSec(char *outpath,const struct adc myADC,unsigned long **timestamp);
void PrintMechSwIndex(char *prefix,const struct run myRun,const struct adc myADC,int *MECH); 

void PrintDiagnostics(char *outdir,int NumComments,char **comment,
                      const struct FuncGen myFuncGen,
                      const struct fpga    myFPGA,
                      const struct adc     myADC);

void PrintDiagnosticsNew(char *outdir,int NumComments,char **comment,
                         const struct run     myRun, 
                         const struct FuncGen myFuncGen,
                         const struct FuncGen *myFuncGenPi2,
                         const struct fpgaPulseSequence myPulseSequence,
                         const struct adc     myADC);

void PrintRunSummary(char *outdir,
                     int NCH,
                     const struct run     myRun, 
                     const struct FuncGen myFuncGen,
                     const struct FuncGen *myFuncGenPi2,
                     const struct adc     myADC);

void PrintEventData(char *prefix,int NEvents,event_t *myEvent); 

int WriteStatus(int run_status);
int WriteLog(int instance,logger_t myLogger);  

#endif 
