#ifndef FPGAPULSESEQUENCE_H
#define FPGAPULSESEQUENCE_H 

// A structure for the FPGA data.
// Contains pulse timing for a given mechanical switch and the necessary RF switch timing 
// Note: no addresses are needed here, since the pin output of the FPGA is controlled by a 
//       bit pattern based on whether fEnableFlag==1 or not.  The addresses are stored in the acromag_ipep201 code.  

struct fpgaPulseSequence{

   char *fName;                                                              // name of the pulse sequence 
   char *fSignalUnits;                                                       // time units of the signals  

   int fEnableFlag;                                                          // enable flag (1 = enabled)  
   int fMechSwStartTimeLo ,fMechSwStartTimeHi;                               // mechanical switch    start times, low bytes and high bytes; in counts 
   int fMechSwEndTimeLo   ,fMechSwEndTimeHi;                                 // mechanical switch    end   times, low bytes and high bytes; in counts 
   int fRFTransStartTimeLo,fRFTransStartTimeHi;                              // rf switch (transmit) start times, low bytes and high bytes; in counts 
   int fRFTransEndTimeLo  ,fRFTransEndTimeHi;                                // rf switch (transmit) end   times, low bytes and high bytes; in counts 
   int fRFRecStartTimeLo  ,fRFRecStartTimeHi;                                // rf switch (receive)  start times, low bytes and high bytes; in counts 
   int fRFRecEndTimeLo    ,fRFRecEndTimeHi;                                  // rf switch (receive)  end   times, low bytes and high bytes; in counts 
   int fTomcoStartTimeLo  ,fTomcoStartTimeHi;                                // tomco                start times, low bytes and high bytes; in counts 
   int fTomcoEndTimeLo    ,fTomcoEndTimeHi;                                  // tomco                end   times, low bytes and high bytes; in counts 

}; 

#endif 
