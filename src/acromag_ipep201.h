#ifndef ACROMAGIPEP201_H
#define ACROMAGIPEP201_H

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "libs/sis1100_var.h"
#include "libs/sis3100_vme_calls.h"

#include "fpga.h"
#include "util.h"

#define _GNU_SOURCE
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var,pos) ((var) |= (1 <<(pos)))
#define CLEAR_BIT(var,pos) ((var) &= ~(1 <<(pos)))
#define mlMAX 11             // global on/off + number of signals
#define SIZE4 4

#define MECHANICAL_SWITCH_1_ADDR 0x0002
#define MECHANICAL_SWITCH_2_ADDR 0x000a
#define MECHANICAL_SWITCH_3_ADDR 0x0012
#define MECHANICAL_SWITCH_4_ADDR 0x001a
#define RF_SWITCH_1_ADDR         0x0022
#define RF_SWITCH_2_ADDR         0x002a
#define RF_SWITCH_3_ADDR         0x0032
#define RF_CLEAR_ADDR            0x003a
#define RF_PULSE_ADDR            0x0042
#define RF_GATE_ADDR             0x004a
#define DIGITIZER_ADDR_1         0x0020
#define DIGITIZER_ADDR_2         0x0024

// short I/O memory maps 
int gModBase;
// I/O Space
int gIPAIOSpace;
int gIPBIOSpace;
int gIPCIOSpace;
int gIPDIOSpace;
// ID Space 
int gIPAIDSpace;
int gIPBIDSpace;
int gIPCIDSpace;
int gIPDIDSpace;

int gOffset;

// FPGA (virtual) addresses: For the memory address register
char *gMasterList[mlMAX];    // make sure this array size matches mlMAX 
int gMasterAddrList[mlMAX];  // make sure this array size matches mlMAX 
int gMechSwitchAddr[SIZE4];
int gRFSwitchAddr[SIZE4];
int gRFPulseAddr;
int gRFGateAddr;
int gDigitizerAddr;          // this is not included in the master list 
int gDigitizerAddr2;         // this is not included in the master list 

int RECEIVE_GATE_COUNTS;

double gFPGAClockFreq; 
double RECEIVE_GATE_TIME_SEC; 

char *RECEIVE_GATE_INPUT_TIME_UNITS;

int TimingCheck(const struct fpga myFPGA); 
int InitFPGA(int p,struct fpga *myFPGA);
int CheckMode(int p,int carrier_addr,int fpga_addr);
int ProgramSignalsToFPGA(int p,const struct fpga); 
int ReadMemoryDataReg(int p,int carrier_addr,int daughter_addr,int my_mem_addr);
int ReadFPGAMemory(int p,int carrier_addr,int daughter_addr,int my_mem_addr);
int GetAddress(char *module);
int GetBitPattern(int N,char **module_list,int *flag);
int IsFPGATimingSet(int p,int carrier_addr,int daughter_addr);
int IsReturnGateClosed(int p,int carrier_addr,int daughter_addr,int *fpga_data);

void InitFPGAGlobalVariables(void); 
void InitFPGAAddresses(void);
void InitFPGAStruct(struct fpga *myFPGA); 
void PrintFPGA(const struct fpga myFPGA); 
void Print(char *function,char *daughter_type,int addr,int data,int code);
void PrintBits(u_int16_t data16);
void PrintIOSpace(int p,int carrier_addr,int daughter_addr);
void PrintIDProm(int p,int carrier_addr,int fpga_addr);
void PrintSummary(const struct fpga myFPGA); 
void CheckStatus(int addr,int data,int ret_code);
void SetIOBitsAlt(int p,int carrier_addr,int fpga_addr,int daughter_segment,int bit_pattern);
void SetIOBits(int p,int carrier_addr,int daughter_addr,int bit_pattern);
void SetDirCtrlReg(int p,int carrier_addr,int daughter_addr,int bit_pattern);
void SetControlRegister(int p,int carrier_addr,int fpga_addr,int state);
void SetMode(int p,int carrier_addr,int fpga_addr,int choice);
void QueryIOSpace(int p,int carrier_addr,int fpga_addr);
void BoardReset(int p,int carrier_addr,int daughter_addr);
void SoftwareReset(int p,int carrier_addr,int daughter_addr);
void SetCtrlRegIDBits(int p,int carrier_addr,int daughter_addr,int value);
void SetClockSpeed(int p,int carrier_addr,int daughter_addr,int choice);
void WriteMemoryDataReg(int p,int carrier_addr,int daughter_addr,int my_mem_addr,int bit_pattern);
void ComputeLowAndHighBytes(int counts,int *v);
void ImportPulseData(char *filename,struct fpga *myFPGA);
void BlankFPGA(int p,struct fpga *myFPGA); 

#endif
