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
#define mlMAX 16             // global on/off + number of signals
#define SIZE4 4

#define FPGA_CLOCK_FREQ    8E+6     // 8 MHz clock 

#define MODULE_BASE_ADDR   0x00000000

#define IP_A_IO_SPACE_ADDR 0x00000000
#define IP_B_IO_SPACE_ADDR 0x00000100
#define IP_C_IO_SPACE_ADDR 0x00000200
#define IP_D_IO_SPACE_ADDR 0x00000300

#define IO_SPACE_OFFSET    0x80 

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
u_int16_t gModBase;
// I/O Space
u_int16_t gIPAIOSpace;
u_int16_t gIPBIOSpace;
u_int16_t gIPCIOSpace;
u_int16_t gIPDIOSpace;
// ID Space 
u_int16_t gIPAIDSpace;
u_int16_t gIPBIDSpace;
u_int16_t gIPCIDSpace;
u_int16_t gIPDIDSpace;

u_int16_t gOffset;

// FPGA (virtual) addresses: For the memory address register
char *gMasterList[mlMAX];          // make sure this array size matches mlMAX 
u_int16_t gMasterAddrList[mlMAX];  // make sure this array size matches mlMAX 
u_int16_t gMechSwitchAddr[SIZE4];
u_int16_t gRFSwitchAddr[SIZE4];
u_int16_t gRFPulseAddr;
u_int16_t gRFGateAddr;
u_int16_t gDigitizerAddr;          // this is not included in the master list 
u_int16_t gDigitizerAddr2;         // this is not included in the master list 

int RECEIVE_GATE_COUNTS;

double gFPGAClockFreq; 
double RECEIVE_GATE_TIME_SEC; 

char *RECEIVE_GATE_INPUT_TIME_UNITS;

int TimingCheck(const struct fpga myFPGA); 
int InitFPGA(int p,struct fpga *myFPGA);
int CheckMode(int p,u_int16_t carrier_addr,u_int16_t fpga_addr);
int ProgramSignalsToFPGA(int p,const struct fpga); 
int GetAddress(char *module);
int IsFPGATimingSet(int p,u_int16_t carrier_addr,u_int16_t daughter_addr);
int IsReturnGateClosed(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t *fpga_data);
int WriteMemoryDataReg(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t my_mem_addr,u_int16_t bit_pattern);

u_int16_t ReadMemoryDataReg(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t my_mem_addr);
u_int16_t ReadFPGAMemory(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t my_mem_addr);
u_int16_t GetBitPattern(int N,char **module_list,int *flag);

void InitFPGAGlobalVariables(void); 
void InitFPGAAddresses(void);
void InitFPGAStruct(struct fpga *myFPGA); 
void PrintFPGA(const struct fpga myFPGA); 
void Print(char *function,char *daughter_type,u_int16_t addr,u_int16_t data,int code);
void PrintBits(u_int16_t data16);
void PrintIOSpace(int p,u_int16_t carrier_addr,u_int16_t daughter_addr);
void PrintIDProm(int p,u_int16_t carrier_addr,u_int16_t fpga_addr);
void PrintSummary(const struct fpga myFPGA); 
void CheckStatus(u_int16_t addr,u_int16_t data,int ret_code);
void SetIOBitsAlt(int p,u_int16_t carrier_addr,u_int16_t fpga_addr,u_int16_t daughter_segment,u_int16_t bit_pattern);
void SetIOBits(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t bit_pattern);
void SetDirCtrlReg(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t bit_pattern);
void SetControlRegister(int p,u_int16_t carrier_addr,u_int16_t fpga_addr,u_int16_t state);
void SetMode(int p,u_int16_t carrier_addr,u_int16_t fpga_addr,int choice);
void QueryIOSpace(int p,u_int16_t carrier_addr,u_int16_t fpga_addr);
void BoardReset(int p,u_int16_t carrier_addr,u_int16_t daughter_addr);
void SoftwareReset(int p,u_int16_t carrier_addr,u_int16_t daughter_addr);
void SetCtrlRegIDBits(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t value);
void SetClockSpeed(int p,u_int16_t carrier_addr,u_int16_t daughter_addr,u_int16_t choice);
void ComputeLowAndHighBytes(int counts,int *v);
void ImportPulseData(char *filename,struct fpga *myFPGA);
void BlankFPGA(int p,struct fpga *myFPGA); 

#endif
