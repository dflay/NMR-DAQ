#ifndef FPGAPULSESEQUENCE_H
#define FPGAPULSESEQUENCE_H 

// A structure for the FPGA data.
// Contains pulse timing for a given mechanical switch and the necessary RF switch timing 
// Note: no addresses are needed here, since the pin output of the FPGA is controlled by a 
//       bit pattern based on whether fEnableFlag==1 or not.  The addresses are stored in the acromag_ipep201 code.  

#define FPS4 4  

struct fpgaPulseSequence{

   char *fBoardName;                                            // name of the FPGA board 
   char **fSeqName;                                             // name of the pulse sequence 
   char **fMechSwUnits;                                         // time units of the signals  
   char **fRFTransUnits;                                        // time units of the signals  
   char **fRFRecUnits;                                          // time units of the signals  
   char **fTomcoUnits;                                          // time units of the signals  

   int fCarrierAddr;                                            // carrier board address
   int fIOSpaceAddr;                                            // I/O space address
   int fIDSpaceAddr;                                            // ID space address

   int fNSequences; 
   int fGlobalEnable;                                           // global enable flag
   int fIsNoise[FPS4];                                          // noise flag  
   int fMechSwID[FPS4];                                         // which mechanical switch (1, 2, 3 or 4) 
   int fEnableFlag[FPS4];                                       // enable flag (1 = enabled) 
   // low and high bytes for pulse timing  
   int fMechSwStartTimeLo[FPS4] ,fMechSwStartTimeHi[FPS4];      // mechanical switch    start times in counts 
   int fMechSwEndTimeLo[FPS4]   ,fMechSwEndTimeHi[FPS4];        // mechanical switch    end   times in counts 
   int fRFTransStartTimeLo[FPS4],fRFTransStartTimeHi[FPS4];     // rf switch (transmit) start times in counts 
   int fRFTransEndTimeLo[FPS4]  ,fRFTransEndTimeHi[FPS4];       // rf switch (transmit) end   times in counts 
   int fRFRecStartTimeLo[FPS4]  ,fRFRecStartTimeHi[FPS4];       // rf switch (receive)  start times in counts 
   int fRFRecEndTimeLo[FPS4]    ,fRFRecEndTimeHi[FPS4];         // rf switch (receive)  end   times in counts 
   int fTomcoStartTimeLo[FPS4]  ,fTomcoStartTimeHi[FPS4];       // tomco                start times in counts 
   int fTomcoEndTimeLo[FPS4]    ,fTomcoEndTimeHi[FPS4];         // tomco                end   times in counts 

}; 

#endif 
