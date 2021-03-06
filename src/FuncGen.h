#ifndef FUNCGEN_H 
#define FUNCGEN_H 

#include <stdlib.h> 

struct FuncGen{

   char *fName;                // name of the function generator 
   char *fFreqUnits;           // frequency units 
   char *fBNCVoltageUnits;     // BNC units
   char *fNTypeVoltageUnits;   // N-Type units
   char *fBNCState;            // BNC enable state (on or off)  
   char *fNTypeState;          // N-Type enable state (on or off) 
   char *fFreqCommand;         // frequency command to send to the hardware          
   char *fBNCCommand;          // BNC command to send to the hardware         
   char *fNTypeCommand;        // N-Type command to send to the hardware        

   int fIntBNCState;           // BNC enable state (0 = off, 1 = on) 
   int fIntNTypeState;         // N-Type enable state (0 = off, 1 = on) 

   double fFrequency;          // frequency       
   double fBNCVoltage;         // BNC voltage   
   double fNTypeVoltage;       // NType voltage  

}; 

#endif 
