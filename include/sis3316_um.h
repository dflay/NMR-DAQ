#ifndef SIS3316_UM_H  
#define SIS3316_UM_H   

// Functions used to operate the Struck SIS3316 ADC.
// Some functions are taken from the Struck SIS3316 class.  
// Slight modifications have been made to them so that 
// they work with the rest of our software.
// The 'UM' tag stands for UMass.  

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include "sis3100_vme_calls.h"
#include "sis1100_var.h"
#include "sis3316_var.h"

#include "adc.h"
#include "util.h"
#include "sis_util.h"
#include "run.h"

int SIS3316IsEventAvailable(int vme_handle); 
int SIS3316Test(int vme_handle,const struct adc myADC); 
int SIS3316Init(int vme_handle,const struct adc myADC);
int SIS3316BaseInit(int vme_handle,const struct adc myADC);
int SIS3316ReInit(int vme_handle,const struct adc myADC);
int SIS3316BaseInitNew(int vme_handle,const struct adc myADC);
int SIS3316ReInitNew(int vme_handle,const struct adc myADC);
int SIS3316ConfigureClock(int vme_handle,const struct adc myADC,int use_ext_clock,int adc_125MHz_flag); 
int SIS3316SampleData(int vme_handle,const struct adc myADC,char *output_dir,int EventNum,int *armed_bank_flag); 
int SIS3316SampleDataTest(int vme_handle,const struct adc myADC); 

// below are the functions taken from Struck's classes. 
int WriteEventToFile(int EventNum,unsigned short* memory_data_array,
                     unsigned int nof_write_length_lwords,char *outdir);

int call_vme_A32MBLT64FIFO_read(int vme_handle, u_int32_t vme_adr, u_int32_t* vme_data,
                      u_int32_t req_num_of_lwords, u_int32_t* got_num_of_lwords);

int read_DMA_Channel_PreviousBankDataBuffer(int vme_handle,
                                            unsigned int bank2_read_flag,
                                            unsigned int channel_no,
                                            unsigned int max_read_nof_words,
                                            unsigned int *dma_got_no_of_words,
                                            u_int32_t *uint_adc_buffer,
                                            u_int32_t NumOfEvents);

int change_frequency_HSdiv_N1div(int vme_handle,int osc, unsigned hs_div_val, unsigned n1_div_val); 

int I2cStart(int vme_handle,int osc);
int I2cStop(int vme_handle,int osc);
int I2cWriteByte(int vme_handle,int osc, unsigned char data, char *ack);
int I2cReadByte(int vme_handle,int osc, unsigned char *data, char ack);
int Si570FreezeDCO(int vme_handle,int osc);
int Si570ReadDivider(int vme_handle,int osc, unsigned char *data);
int Si570Divider(int vme_handle,int osc, unsigned char *data);
int Si570UnfreezeDCO(int vme_handle,int osc);
int Si570NewFreq(int vme_handle,int osc);
int set_frequency(int vme_handle,int osc, unsigned char *values);
int adc_spi_setup(int vme_handle,int adc_125MHz_flag); 
int adc_spi_write(int vme_handle,unsigned int adc_fpga_group, unsigned int adc_chip, unsigned int spi_addr, unsigned int spi_data); 
int adc_spi_read(int vme_handle,unsigned int adc_fpga_group, unsigned int adc_chip, unsigned int spi_addr, unsigned int* spi_data); 

#endif 
