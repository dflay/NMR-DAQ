#ifndef FPGA_H
#define FPGA_H 

// A structure for the FPGA data. 

#define FPGAArraySize 20  

struct fpga{

   char *fName;                                                              // name of the FPGA  
   char **fSignalName;                                                       // names of the signals 
   char **fSignalUnits;                                                      // time units of the signals  

   int fCarrierAddr;                                                         // carrier board address
   int fIOSpaceAddr;                                                         // I/O space address
   int fIDSpaceAddr;                                                         // ID space address
   int fNSignals;                                                            // number of signals programmed to board 
   int fFlag[FPGAArraySize];                                                 // flags to turn pins on and off 
   int fSignalAddrStartLo[FPGAArraySize],fSignalAddrStartHi[FPGAArraySize];  // addresses for start times, low bytes and high bytes 
   int fSignalAddrEndLo[FPGAArraySize]  ,fSignalAddrEndHi[FPGAArraySize];    // addresses for end times  , low bytes and high bytes
   int fSignalStartTimeLo[FPGAArraySize],fSignalStartTimeHi[FPGAArraySize];  // start times, low bytes and high bytes; in counts 
   int fSignalEndTimeLo[FPGAArraySize]  ,fSignalEndTimeHi[FPGAArraySize];    // end times  , low bytes and high bytes; in counts  
   int fSignalPulseTimeLo[FPGAArraySize],fSignalPulseTimeHi[FPGAArraySize];  // pulse times, low bytes and high bytes; in counts
   u_int16_t fBitPatternFlag;                                                // representation of flags in a single decimal form

   double fClockFrequency;                                                   // FPGA clock frequency 

}; 

#endif 
