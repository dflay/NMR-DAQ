//__________________________________________________________________________  
//                                    
//  Filename: SIS3316.h               
//  Adapted from SIS3302.h by D. Flay
//  Comments are from the SIS3316 manual. 
//__________________________________________________________________________  

#define SIS3316_CONTROL_STATUS                                       0x0     // read/write; D32 
#define SIS3316_MODID                                                0x4     // read only ; D32  
#define SIS3316_IRQ_CONFIG                                           0x8     // read/write; D32 
#define SIS3316_IRQ_CONTROL                                          0xC     // read/write; D32 
#define SIS3316_ACQUISTION_CONTROL                                  0x60     // read/write; D32 NOTE TYPO! Also, this value is different from the SIS3302
#define SIS3316_START_DELAY                                         0x14     // read/write; D32 
#define SIS3316_STOP_DELAY                                          0x18     // read/write; D32 

#define SIS3316_MAX_NOF_EVENT                                       0x20     // read/write; D32 
#define SIS3316_ACTUAL_EVENT_COUNTER                                0x24     // read only ; D32 

#define SIS3316_CBLT_BROADCAST_SETUP                                0x30     // read/write; D32 
#define SIS3316_ADC_MEMORY_PAGE_REGISTER                            0x34     // read/write; D32 

// This read/write register is used to set the Sample Clock Frequency via I2C.
#define SIS3316_ADC_CLK_OSC_I2C_REG                                 0x40     // read/write; D32 
// This read/write register is used to set the Link1 Clock Frequency via I2C (not used).
#define SIS3316_MGT1_OSC_I2C_REG                                    0x44     // read/write; D32 
// This read/write register is used to set the Link2 Clock Frequency via I2C (not used).
#define SIS3316_MGT2_OSC_I2C_REG                                    0x48     // read/write; D32 
// This read/write register is used to set the DDR3 Clock Frequency via I2C (not used)
#define SIS3316_DD3_OSC_I2C_REG                                     0x4C     // read/write; D32 

#define SIS3316_SAMPLE_CLOCK_DISTRIBUTION_CONTROL                   0x50     // read/write; D32 

// This read/write register below is used to multiply the External NIM 
// Clock (IN) via Serial Peripheral Interface (SPI).  Several parameters 
// of the Clock Multiplier SI5325 chip can be configured with the SPI.
#define SIS3316_NIM_CLK_MULTIPLIER_SPI_REG                          0x54     // read/write; D32 

#define SIS3316_NIM_INPUT_CONTROL_REG                               0x5C     // read/write; D32 

#define SIS3316_KEY_RESET                                          0x400     // write only; D32 
#define SIS3316_KEY_USER_FUNCTION                                  0x404     // write only; D32 
#define SIS3316_KEY_ARM                                            0x410     // write only; D32 
#define SIS3316_KEY_DISARM                                         0x414     // write only; D32 
#define SIS3316_KEY_START                                          0x418     // write only; D32; also known as KEY_TRIGGER
#define SIS3316_KEY_STOP                                           0x41C     // write only; D32 
#define SIS3316_KEY_DISARM                                         0x414     // write only; D32 
#define SIS3316_KEY_TIMESTAMP_CLR                                  0x41C     // version 010B and higher 
#define SIS3316_KEY_DISARM_AND_ARM_BANK1                           0x420     // write only; D32 
#define SIS3316_KEY_DISARM_AND_ARM_BANK2                           0x424     // write only; D32 
#define SIS3316_KEY_ENABLE_SAMPLE_BANK_SWAP_CONTROL_WITH_NIM_INPUT 0x428
#define SIS3316_KEY_DISABLE_PRESCALER_OUTPUT_PULSE_DIVIDER_LOGIC   0x42C
#define SIS3316_KEY_PPS_LATCH_BIT_CLEAR                            0x430     // write only; D32 
#define SIS3316_KEY_ADC_FPGA_RESET                                 0x434     // write only; D32 

// A write with arbitrary data to this register (key address) resets the DCM/PLL of all FPGAs.
// This command is necessary after a change in Sample Clock.
#define SIS3316_KEY_ADC_CLOCK_DCM_RESET                            0x438     // write only; D32

// These read only registers hold the actual sampling address for the given channel.
#define SIS3316_ADC_CH1_ACTUAL_SAMPLE_ADDRESS_REG                  0x1110    // read only 
#define SIS3316_ADC_CH2_ACTUAL_SAMPLE_ADDRESS_REG                  0x1114    // read only 
#define SIS3316_ADC_CH3_ACTUAL_SAMPLE_ADDRESS_REG                  0x1118    // read only 
#define SIS3316_ADC_CH4_ACTUAL_SAMPLE_ADDRESS_REG                  0x111C    // read only 
#define SIS3316_ADC_CH5_ACTUAL_SAMPLE_ADDRESS_REG                  0x2110    // read only 
#define SIS3316_ADC_CH6_ACTUAL_SAMPLE_ADDRESS_REG                  0x2114    // read only 
#define SIS3316_ADC_CH7_ACTUAL_SAMPLE_ADDRESS_REG                  0x2118    // read only 
#define SIS3316_ADC_CH8_ACTUAL_SAMPLE_ADDRESS_REG                  0x211C    // read only 
#define SIS3316_ADC_CH9_ACTUAL_SAMPLE_ADDRESS_REG                  0x3110    // read only 
#define SIS3316_ADC_CH10_ACTUAL_SAMPLE_ADDRESS_REG                 0x3114    // read only 
#define SIS3316_ADC_CH11_ACTUAL_SAMPLE_ADDRESS_REG                 0x3118    // read only 
#define SIS3316_ADC_CH12_ACTUAL_SAMPLE_ADDRESS_REG                 0x311C    // read only 
#define SIS3316_ADC_CH13_ACTUAL_SAMPLE_ADDRESS_REG                 0x4110    // read only 
#define SIS3316_ADC_CH14_ACTUAL_SAMPLE_ADDRESS_REG                 0x4114    // read only 
#define SIS3316_ADC_CH15_ACTUAL_SAMPLE_ADDRESS_REG                 0x4118    // read only 
#define SIS3316_ADC_CH16_ACTUAL_SAMPLE_ADDRESS_REG                 0x411C    // read only 

// These read only registers hold the stored next sampling address of the previous bank. 
// It is the stop address + 1; the Previous Bank Sample Address points to 32-bit words.
#define SIS3316_ADC_CH1_PREVIOUS_BANK_SAMPLE_ADDRESS_REG           0x1120    // read only   
#define SIS3316_ADC_CH2_PREVIOUS_BANK_SAMPLE_ADDRESS_REG           0x1124    // read only 
#define SIS3316_ADC_CH3_PREVIOUS_BANK_SAMPLE_ADDRESS_REG           0x1128    // read only 
#define SIS3316_ADC_CH4_PREVIOUS_BANK_SAMPLE_ADDRESS_REG           0x112C    // read only 
#define SIS3316_ADC_CH5_PREVIOUS_BANK_SAMPLE_ADDRESS_REG           0x2120    // read only 
#define SIS3316_ADC_CH6_PREVIOUS_BANK_SAMPLE_ADDRESS_REG           0x2124    // read only 
#define SIS3316_ADC_CH7_PREVIOUS_BANK_SAMPLE_ADDRESS_REG           0x2128    // read only 
#define SIS3316_ADC_CH8_PREVIOUS_BANK_SAMPLE_ADDRESS_REG           0x212C    // read only 
#define SIS3316_ADC_CH9_PREVIOUS_BANK _SAMPLE_ADDRESS_REG          0x3120    // read only 
#define SIS3316_ADC_CH10_PREVIOUS_BANK_SAMPLE_ADDRESS_REG          0x3124    // read only 
#define SIS3316_ADC_CH11_PREVIOUS_BANK_SAMPLE_ADDRESS_REG          0x3128    // read only 
#define SIS3316_ADC_CH12_PREVIOUS_BANK_SAMPLE_ADDRESS_REG          0x312C    // read only 
#define SIS3316_ADC_CH13_PREVIOUS_BANK_SAMPLE_ADDRESS_REG          0x4120    // read only 
#define SIS3316_ADC_CH14_PREVIOUS_BANK_SAMPLE_ADDRESS_REG          0x4124    // read only 
#define SIS3316_ADC_CH15_PREVIOUS_BANK_SAMPLE_ADDRESS_REG          0x4128    // read only 
#define SIS3316_ADC_CH16_PREVIOUS_BANK_SAMPLE_ADDRESS_REG          0x412C    // read only 

#define SIS3316_ADC_CH1_4_STATUS_REG                               0x1104    // read only 
#define SIS3316_ADC_CH5_8_STATUS_REG                               0x2104    // read only
#define SIS3316_ADC_CH9_12_STATUS_REG                              0x3104    // read only
#define SIS3316_ADC_CH13_16_STATUS_REG                             0x4104    // read only

// This register below reflects the ADC FPGA Firmware Version and Revision number.
// The Version level will be used to distinguish between substantial design differences and
// experiment specific designs, while the Revision level will be used to mark user specific
// adaptations.
#define SIS3316_ADC_CH1_4_FIRMWARE_REG                             0x1100    // read only 
#define SIS3316_ADC_CH5_8_FIRMWARE_REG                             0x2100    // read only  
#define SIS3316_ADC_CH9_12_FIRMWARE_REG                            0x3100    // read only  
#define SIS3316_ADC_CH13_16_FIRMWARE_REG                           0x4100    // read only  

// Histogramming energy registers
#define SIS3316_ADC_CH1_ENERGY_HISTOGRAM_CONFIG_REG                0x10D0
#define SIS3316_ADC_CH2_ENERGY_HISTOGRAM_CONFIG_REG                0x10D4
#define SIS3316_ADC_CH3_ENERGY_HISTOGRAM_CONFIG_REG                0x10D8
#define SIS3316_ADC_CH4_ENERGY_HISTOGRAM_CONFIG_REG                0x10DC
#define SIS3316_ADC_CH5_ENERGY_HISTOGRAM_CONFIG_REG                0x20D0
#define SIS3316_ADC_CH6_ENERGY_HISTOGRAM_CONFIG_REG                0x20D4
#define SIS3316_ADC_CH7_ENERGY_HISTOGRAM_CONFIG_REG                0x20D8
#define SIS3316_ADC_CH8_ENERGY_HISTOGRAM_CONFIG_REG                0x20DC
#define SIS3316_ADC_CH9_ENERGY_HISTOGRAM_CONFIG_REG                0x30D0
#define SIS3316_ADC_CH10_ENERGY_HISTOGRAM_CONFIG_REG               0x30D4
#define SIS3316_ADC_CH11_ENERGY_HISTOGRAM_CONFIG_REG               0x30D8
#define SIS3316_ADC_CH12_ENERGY_HISTOGRAM_CONFIG_REG               0x30DC
#define SIS3316_ADC_CH13_ENERGY_HISTOGRAM_CONFIG_REG               0x40D0
#define SIS3316_ADC_CH14_ENERGY_HISTOGRAM_CONFIG_REG               0x40D4
#define SIS3316_ADC_CH15_ENERGY_HISTOGRAM_CONFIG_REG               0x40D8
#define SIS3316_ADC_CH16_ENERGY_HISTOGRAM_CONFIG_REG               0x40DC

// These read/write registers hold the Peaking  Time, Gap Time and Tau Factor (decay time)
// values of the trapezoidal FIR Energy filter.
#define SIS3316_ADC_CH1_FIR_ENERGY_SETUP_REG                       0x10C0
#define SIS3316_ADC_CH2_FIR_ENERGY_SETUP_REG                       0x10C4
#define SIS3316_ADC_CH3_FIR_ENERGY_SETUP_REG                       0x10C8
#define SIS3316_ADC_CH4_FIR_ENERGY_SETUP_REG                       0x10CC
#define SIS3316_ADC_CH5_FIR_ENERGY_SETUP_REG                       0x20C0
#define SIS3316_ADC_CH6_FIR_ENERGY_SETUP_REG                       0x20C4
#define SIS3316_ADC_CH7_FIR_ENERGY_SETUP_REG                       0x20C8
#define SIS3316_ADC_CH8_FIR_ENERGY_SETUP_REG                       0x20CC
#define SIS3316_ADC_CH9_FIR_ENERGY_SETUP_REG                       0x30C0
#define SIS3316_ADC_CH10_FIR_ENERGY_SETUP_REG                      0x30C4
#define SIS3316_ADC_CH11_FIR_ENERGY_SETUP_REG                      0x30C8
#define SIS3316_ADC_CH12_FIR_ENERGY_SETUP_REG                      0x30CC
#define SIS3316_ADC_CH13_FIR_ENERGY_SETUP_REG                      0x40C0
#define SIS3316_ADC_CH14_FIR_ENERGY_SETUP_REG                      0x40C4
#define SIS3316_ADC_CH15_FIR_ENERGY_SETUP_REG                      0x40C8
#define SIS3316_ADC_CH16_FIR_ENERGY_SETUP_REG                      0x40CC

// These registers define the length and the start index 
// of the accumulator gates
#define SIS3316_ADC_CH1_4_ACCUMULATOR_GATE1_CONFIG_REG             0x10A0
#define SIS3316_ADC_CH5_8_ACCUMULATOR_GATE1_CONFIG_REG             0x20A0
#define SIS3316_ADC_CH9_12_ACCUMULATOR_GATE1_CONFIG_REG            0x30A0
#define SIS3316_ADC_CH13_16_ACCUMULATOR_GATE1_CONFIG_REG           0x40A0
#define SIS3316_ADC_CH1_4_ACCUMULATOR_GATE2_CONFIG_REG             0x10A4
#define SIS3316_ADC_CH5_8_ACCUMULATOR_GATE2_CONFIG_REG             0x20A4
#define SIS3316_ADC_CH9_12_ACCUMULATOR_GATE2_CONFIG_REG            0x30A4
#define SIS3316_ADC_CH13_16_ACCUMULATOR_GATE2_CONFIG_REG           0x40A4
#define SIS3316_ADC_CH1_4_ACCUMULATOR_GATE8_CONFIG_REG             0x10BC
#define SIS3316_ADC_CH5_8_ACCUMULATOR_GATE8_CONFIG_REG             0x20BC
#define SIS3316_ADC_CH9_12_ACCUMULATOR_GATE8_CONFIG_REG            0x30BC
#define SIS3316_ADC_CH13_16_ACCUMULATOR_GATE8_CONFIG_REG           0x40BC

// The registers below define the length and the start index
// of the raw data buffer and the number of samples that will
// be copied to the memory. Data are stored to memory in packets 
// of 2 consecutive samples by the copy logic.
#define SIS3316_ADC_CH1_4_RAW_DATA_BUFFER_CONFIG_REG               0x1020 
#define SIS3316_ADC_CH5_8_RAW_DATA_BUFFER_CONFIG_REG               0x2020  
#define SIS3316_ADC_CH9_12_RAW_DATA_BUFFER_CONFIG_REG              0x3020 
#define SIS3316_ADC_CH13_16_RAW_DATA_BUFFER_CONFIG_REG             0x4020 

// With these registers, it is possible to define a greater sample length 
// than 64K, which can be defined with the Raw Data Buffer Configuration registers.
// The value of the ¿Raw Buffer Sample_Length¿ will be used for the sample 
// length if the value of the ¿Extended Raw Buffer Sample_Length¿ is zero!
// If the value of the ¿Extended Raw Buffer Sample_Length¿ is not zero then 
// this value will be used for the sample length!
// The maximum Raw Buffer Sample_Length is 32M  2 (33.554.430)
#define SIS3316_ADC_CH1_4_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG      0x1098
#define SIS3316_ADC_CH5_8_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG      0x2098
#define SIS3316_ADC_CH9_12_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG     0x3098
#define SIS3316_ADC_CH13_16_EXTENDED_RAW_DATA_BUFFER_CONFIG_REG    0x4098

// These registers enable and define the Peak/Charge mode
#define SIS3316_ADC_CH1_4_PEAK_CHARGE_CONFIGURATION_REG            0x1094
#define SIS3316_ADC_CH5_8_PEAK_CHARGE_CONFIGURATION_REG            0x2094
#define SIS3316_ADC_CH9_12_PEAK_CHARGE_CONFIGURATION_REG           0x3094
#define SIS3316_ADC_CH13_16_PEAK_CHARGE_CONFIGURATION_REG          0x4094

// These read/write register hold features of the Trigger Statistic Counters
#define SIS3316_ADC_CH1_4_TRIGGER_STATISTIC_COUNTER_MODE_REG       0x1090
#define SIS3316_ADC_CH5_8_TRIGGER_STATISTIC_COUNTER_MODE_REG       0x2090
#define SIS3316_ADC_CH9_12_TRIGGER_STATISTIC_COUNTER_MODE_REG      0x3090
#define SIS3316_ADC_CH13_16_TRIGGER_STATISTIC_COUNTER_MODE_REG     0x4090

// These read/write registers hold the High Energy threshold values for the ADC channels
#define SIS3316_ADC_CH1_FIR_HIGH_ENERGY_THRESHOLD_REG              0x1048
#define SIS3316_ADC_CH2_FIR_HIGH_ENERGY_THRESHOLD_REG              0x1058
#define SIS3316_ADC_CH3_FIR_HIGH_ENERGY_THRESHOLD_REG              0x1068
#define SIS3316_ADC_CH4_FIR_HIGH_ENERGY_THRESHOLD_REG              0x1078
#define SIS3316_ADC_CH1_4_SUM_FIR_HIGH_ENERGY_THRESHOLD_REG        0x1088
#define SIS3316_ADC_CH5_FIR_HIGH_ENERGY_THRESHOLD_REG              0x2048
#define SIS3316_ADC_CH6_FIR_HIGH_ENERGY_THRESHOLD_REG              0x2058
#define SIS3316_ADC_CH7_FIR_HIGH_ENERGY_THRESHOLD_REG              0x2068
#define SIS3316_ADC_CH8_FIR_HIGH_ENERGY_THRESHOLD_REG              0x2078
#define SIS3316_ADC_CH5_8_SUM_FIR_HIGH_ENERGY_THRESHOLD_REG        0x2088
#define SIS3316_ADC_CH9_FIR_HIGH_ENERGY_THRESHOLD_REG              0x3048
#define SIS3316_ADC_CH10_FIR_HIGH_ENERGY_THRESHOLD_REG             0x3058
#define SIS3316_ADC_CH11_FIR_HIGH_ENERGY_THRESHOLD_REG             0x3068
#define SIS3316_ADC_CH12_FIR_HIGH_ENERGY_THRESHOLD_REG             0x3078
#define SIS3316_ADC_CH9_12_SUM_FIR_HIGH_ENERGY_THRESHOLD_REG       0x3088
#define SIS3316_ADC_CH13_FIR_HIGH_ENERGY_THRESHOLD_REG             0x4048
#define SIS3316_ADC_CH14_FIR_HIGH_ENERGY_THRESHOLD_REG             0x4058
#define SIS3316_ADC_CH15_FIR_HIGH_ENERGY_THRESHOLD_REG             0x4068
#define SIS3316_ADC_CH16_FIR_HIGH_ENERGY_THRESHOLD_REG             0x4078
#define SIS3316_ADC_CH13_16_SUM_FIR_HIGH_ENERGY_THRESHOLD_REG      0x4088

// These read/write registers hold the trigger threshold values for the ADC channels
#define SIS3316_ADC_CH1_FIR_TRIGGER_THRESHOLD_REG                  0x1044
#define SIS3316_ADC_CH2_FIR_TRIGGER_THRESHOLD_REG                  0x1054
#define SIS3316_ADC_CH3_FIR_TRIGGER_THRESHOLD_REG                  0x1064
#define SIS3316_ADC_CH4_FIR_TRIGGER_THRESHOLD_REG                  0x1074
#define SIS3316_ADC_CH1_4_SUM_FIR_TRIGGER_THRESHOLD_REG            0x1084
#define SIS3316_ADC_CH5_FIR_TRIGGER_THRESHOLD_REG                  0x2044
#define SIS3316_ADC_CH6_FIR_TRIGGER_THRESHOLD_REG                  0x2054
#define SIS3316_ADC_CH7_FIR_TRIGGER_THRESHOLD_REG                  0x2064
#define SIS3316_ADC_CH8_FIR_TRIGGER_THRESHOLD_REG                  0x2074
#define SIS3316_ADC_CH5_8_SUM_FIR_TRIGGER_THRESHOLD_REG            0x2084
#define SIS3316_ADC_CH9_FIR_TRIGGER_THRESHOLD_REG                  0x3044
#define SIS3316_ADC_CH10_FIR_TRIGGER_THRESHOLD_REG                 0x3054
#define SIS3316_ADC_CH11_FIR_TRIGGER_THRESHOLD_REG                 0x3064
#define SIS3316_ADC_CH12_FIR_TRIGGER_THRESHOLD_REG                 0x3074
#define SIS3316_ADC_CH9_12_SUM_FIR_TRIGGER_THRESHOLD_REG           0x3084
#define SIS3316_ADC_CH13_FIR_TRIGGER_THRESHOLD_REG                 0x4044
#define SIS3316_ADC_CH14_FIR_TRIGGER_THRESHOLD_REG                 0x4054
#define SIS3316_ADC_CH15_FIR_TRIGGER_THRESHOLD_REG                 0x4064
#define SIS3316_ADC_CH16_FIR_TRIGGER_THRESHOLD_REG                 0x4074
#define SIS3316_ADC_CH13_16_SUM_FIR_TRIGGER_THRESHOLD_REG          0x4084

// If "Select Sample Clock" is set, then the Sample Clock is assigned 
// to the LEMO Clock Out ("CO") connector.  If "Select Sample Clock" 
// is not set, then the other selected conditions are ored to the LEMO
// Clock Out ("CO") connector
#define SIS3316_LEMO_OUT_CO_SELECT_REG                               0x70      // read/write; D32 */

// Event configuration registers
#define SIS3316_ADC_CH1_4_EVENT_CONFIG_REG                         0x1010
#define SIS3316_ADC_CH5_8_EVENT_CONFIG_REG                         0x2010
#define SIS3316_ADC_CH9_12_EVENT_CONFIG_REG                        0x3010
#define SIS3316_ADC_CH13_16_EVENT_CONFIG_REG                       0x4010

// This register is implemented for each channel group. Its 12-bit 
// content will be used in the event header. The four registers can 
// be preset with a unique value to obtain channel and module 
// (in a multi module setup) identification.
#define SIS3316_ADC_CH1_4_CHANNEL_HEADER_REG                       0x1014  
#define SIS3316_ADC_CH5_8_CHANNEL_HEADER_REG                       0x2014 
#define SIS3316_ADC_CH9_12_CHANNEL_HEADER_REG                      0x3014  
#define SIS3316_ADC_CH13_16_CHANNEL_HEADER_REG                     0x4014   

// These registers define the "End Address Threshold" values for 
// the ADC channel groups.  The value of the "Actual Sample address 
// counter" (Bankx) will be compared with value of the "End Address 
// Threshold" register.  If the value of the "Actual Sample address 
// counter" goes above the value of the "End Address Threshold value"
// then a "Memory Address Threshold Flag Ch x" will set to indicate a "watermark".
#define SIS3316_ADC_CH1_4_ADDRESS_THRESHOLD_REG                    0x1018     
#define SIS3316_ADC_CH5_8_ADDRESS_THRESHOLD_REG                    0x2018   
#define SIS3316_ADC_CH9_12_ADDRESS_THRESHOLD_REG                   0x3018  
#define SIS3316_ADC_CH13_16_ADDRESS_THRESHOLD_REG                  0x4018    

// These registers define the length of the Active Trigger Gate Window (2, 4, to 65536) .
#define SIS3316_ADC_CH1_4_TRIGGER_GATE_WINDOW_LENGTH_REG           0x101C 
#define SIS3316_ADC_CH5_8_TRIGGER_GATE_WINDOW_LENGTH_REG           0x201C  
#define SIS3316_ADC_CH9_12_TRIGGER_GATE_WINDOW_LENGTH_REG          0x301C   
#define SIS3316_ADC_CH13_16_TRIGGER_GATE_WINDOW_LENGTH_REG         0x401C   


// #define SIS3316_DAC_CONTROL_STATUS                                  0x50     // read/write; D32 
// #define SIS3316_DAC_DATA                                            0x54     // read/write; D32 
 
// #define SIS3316_TIMESTAMP_DIRECTORY                               0x10000   // read only; D32, BLT32; size: 1024Lwords 0x1000 Bytes 
// 
// #define SIS3316_EVENT_CONFIG_ALL_ADC                              0x01000000	  
// #define SIS3316_SAMPLE_LENGTH_ALL_ADC                             0x01000004	  
// #define SIS3316_SAMPLE_START_ADDRESS_ALL_ADC                      0x01000008	  
// #define SIS3316_ADC_INPUT_MODE_ALL_ADC                            0x0100000C	  
// 
// #define SIS3316_TRIGGER_FLAG_CLR_CNT_ALL_ADC                      0x0100002C      
// 
// #define SIS3316_ACTUAL_SAMPLE_ADDRESS_ADC1                        0x02000010	  
// #define SIS3316_ACTUAL_SAMPLE_ADDRESS_ADC2                        0x02000014	  
// #define SIS3316_ACTUAL_SAMPLE_ADDRESS_ADC3                        0x02800010	  
// #define SIS3316_ACTUAL_SAMPLE_ADDRESS_ADC4                        0x02800014	  
// #define SIS3316_ACTUAL_SAMPLE_ADDRESS_ADC5                        0x03000010	  
// #define SIS3316_ACTUAL_SAMPLE_ADDRESS_ADC6                        0x03000014	  
// #define SIS3316_ACTUAL_SAMPLE_ADDRESS_ADC7                        0x03800010	  
// #define SIS3316_ACTUAL_SAMPLE_ADDRESS_ADC8                        0x03800014	  
// 
// #define SIS3316_ACTUAL_SAMPLE_VALUE_ADC12                         0x02000020	  
// #define SIS3316_ACTUAL_SAMPLE_VALUE_ADC12                         0x02000020	  
// #define SIS3316_ACTUAL_SAMPLE_VALUE_ADC34                         0x02800020	  
// #define SIS3316_ACTUAL_SAMPLE_VALUE_ADC56                         0x03000020	  
// #define SIS3316_ACTUAL_SAMPLE_VALUE_ADC78                         0x03800020	  
// 
// #define SIS3316_TRIGGER_FLAG_CLR_CNT_ADC12                        0x0200002C      
// #define SIS3316_TRIGGER_FLAG_CLR_CNT_ADC34                        0x0280002C      
// #define SIS3316_TRIGGER_FLAG_CLR_CNT_ADC56                        0x0300002C      
// #define SIS3316_TRIGGER_FLAG_CLR_CNT_ADC78                        0x0380002C      
// 
// #define SIS3316_TRIGGER_SETUP_ADC1                                0x02000030	  
// #define SIS3316_TRIGGER_SETUP_ADC2                                0x02000038	  
// #define SIS3316_TRIGGER_SETUP_ADC3                                0x02800030	  
// #define SIS3316_TRIGGER_SETUP_ADC4                                0x02800038	  
// #define SIS3316_TRIGGER_SETUP_ADC5                                0x03000030	  
// #define SIS3316_TRIGGER_SETUP_ADC6                                0x03000038	  
// #define SIS3316_TRIGGER_SETUP_ADC7                                0x03800030	  
// #define SIS3316_TRIGGER_SETUP_ADC8                                0x03800038	  
// 
// #define SIS3316_TRIGGER_THRESHOLD_ADC1                            0x02000034	  
// #define SIS3316_TRIGGER_THRESHOLD_ADC2                            0x0200003C	  
// #define SIS3316_TRIGGER_THRESHOLD_ADC3                            0x02800034	  
// #define SIS3316_TRIGGER_THRESHOLD_ADC4                            0x0280003C	  
// #define SIS3316_TRIGGER_THRESHOLD_ADC5                            0x03000034	  
// #define SIS3316_TRIGGER_THRESHOLD_ADC6                            0x0300003C	  
// #define SIS3316_TRIGGER_THRESHOLD_ADC7                            0x03800034	  
// #define SIS3316_TRIGGER_THRESHOLD_ADC8                            0x0380003C	  
// 
// #define SIS3316_EVENT_DIRECTORY_ADC1                              0x02010000	  /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
// #define SIS3316_EVENT_DIRECTORY_ADC2                              0x02018000	  /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
// #define SIS3316_EVENT_DIRECTORY_ADC3                              0x02810000	  /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
// #define SIS3316_EVENT_DIRECTORY_ADC4                              0x02818000	  /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
// #define SIS3316_EVENT_DIRECTORY_ADC5                              0x03010000	  /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
// #define SIS3316_EVENT_DIRECTORY_ADC6                              0x03018000	  /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
// #define SIS3316_EVENT_DIRECTORY_ADC7                              0x03810000	  /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
// #define SIS3316_EVENT_DIRECTORY_ADC8                              0x03818000	  /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
// 
// #define SIS3316_ADC_INPUT_MODE_ADC12                              0x0200000C	  
// #define SIS3316_ADC_INPUT_MODE_ADC34                              0x0280000C	  
// #define SIS3316_ADC_INPUT_MODE_ADC56                              0x0300000C	  
// #define SIS3316_ADC_INPUT_MODE_ADC78                              0x0380000C	  
// 
// #define SIS3316_DDR2_TEST_REGISTER_ADC12                          0x02000028      
// #define SIS3316_DDR2_TEST_REGISTER_ADC34                          0x02800028      
// #define SIS3316_DDR2_TEST_REGISTER_ADC56                          0x03000028      
// #define SIS3316_DDR2_TEST_REGISTER_ADC78                          0x03800028      
// 
// #define SIS3316_ADC1_OFFSET                                       0x04000000	  
// #define SIS3316_ADC2_OFFSET                                       0x04800000	  
// #define SIS3316_ADC3_OFFSET                                       0x05000000	  
// #define SIS3316_ADC4_OFFSET                                       0x05800000	  
// #define SIS3316_ADC5_OFFSET                                       0x06000000	  
// #define SIS3316_ADC6_OFFSET                                       0x06800000	  
// #define SIS3316_ADC7_OFFSET                                       0x07000000	  
// #define SIS3316_ADC8_OFFSET                                       0x07800000	  
// 
// #define SIS3316_NEXT_ADC_OFFSET                                   0x00800000	  
// 
// /* define sample clock */
// #define SIS3316_ACQ_SET_CLOCK_TO_100MHZ                           0x70000000  /* default after Reset  */
// #define SIS3316_ACQ_SET_CLOCK_TO_50MHZ                            0x60001000
// #define SIS3316_ACQ_SET_CLOCK_TO_25MHZ                            0x50002000
// #define SIS3316_ACQ_SET_CLOCK_TO_10MHZ                            0x40003000   //   32 31 30 29...              15  14  13 ....                
// #define SIS3316_ACQ_SET_CLOCK_TO_1MHZ                             0x30004000   //    0  0  1  1 0000 0000 00000  1   0   0  0  000 0000 0000
// #define SIS3316_ACQ_SET_CLOCK_TO_LEMO_RANDOM_CLOCK_IN             0x20005000
// #define SIS3316_ACQ_SET_CLOCK_TO_LEMO_CLOCK_IN                    0x10006000
// #define SIS3316_ACQ_SET_CLOCK_TO_P2_CLOCK_IN                      0x00007000
// 
// #define SIS3316_ACQ_DISABLE_LEMO_START_STOP                       0x01000000
// #define SIS3316_ACQ_ENABLE_LEMO_START_STOP                        0x00000100
// 
// #define SIS3316_ACQ_DISABLE_LEMO1_TIMESTAMP_CLR                   0x02000000
// #define SIS3316_ACQ_ENABLE_LEMO1_TIMESTAMP_CLR                    0x00000200
// 
// #define SIS3316_ACQ_DISABLE_INTERNAL_TRIGGER                      0x00400000
// #define SIS3316_ACQ_ENABLE_INTERNAL_TRIGGER                       0x00000040
// 
// #define SIS3316_ACQ_DISABLE_MULTIEVENT                            0x00200000
// #define SIS3316_ACQ_ENABLE_MULTIEVENT                             0x00000020
// 
// #define SIS3316_ACQ_DISABLE_AUTOSTART                             0x00100000
// #define SIS3316_ACQ_ENABLE_AUTOSTART                              0x00000010
// 
// /* bits of SIS3316_Event Configuration register ; D-register*/
// #define EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP	                           0x20
// #define EVENT_CONF_ENABLE_WRAP_PAGE_MODE	                           0x10
// 
// #define EVENT_CONF_PAGE_SIZE_16M_WRAP	                            0x0
// #define EVENT_CONF_PAGE_SIZE_4M_WRAP		                            0x1
// #define EVENT_CONF_PAGE_SIZE_1M_WRAP		                            0x2
// #define EVENT_CONF_PAGE_SIZE_256K_WRAP	                            0x3
// 
// #define EVENT_CONF_PAGE_SIZE_64K_WRAP	                            0x4
// #define EVENT_CONF_PAGE_SIZE_16K_WRAP	                            0x5
// #define EVENT_CONF_PAGE_SIZE_4K_WRAP		                            0x6
// #define EVENT_CONF_PAGE_SIZE_1K_WRAP		                            0x7
// 
// #define EVENT_CONF_PAGE_SIZE_512_WRAP	                            0x8
// #define EVENT_CONF_PAGE_SIZE_256_WRAP	                            0x9
// #define EVENT_CONF_PAGE_SIZE_128_WRAP	                            0xA
// #define EVENT_CONF_PAGE_SIZE_64_WRAP		                            0xB
