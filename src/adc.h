#ifndef ADC_H
#define ADC_H 

// data structure for Struck ADCs 

struct adc{

   char *fName;                 // name of the ADC 
   char *fClockFreqUnits;       // input units of the clock frequency [kHz, MHz, GHz] 
   char *fSignalLengthUnits;    // input units of the signal length [us, ms, s]  

   int fID;                     // ID number; can be 3302 or 3316  
   int fChannelNumber;          // channel number utilized 
   int fNumberOfEvents;         // number of events 
   int fNumberOfSamples;        // number of samples; NOTE: this is different from the number of events!   
   int fClockType;              // clock type: 0 = internal, 1 = external; default is internal 
   int fMultiEventState;        // multi-event state: 0 = disabled; 1 = enabled; default is disabled  

   double fClockFrequency;      // ADC clock frequency                 [in Hz]  
   double fClockPeriod;         // ADC clock period                    [in sec] 
   double fSignalLength;        // time duration of anticipated signal [in sec] 

}; 

#endif 
