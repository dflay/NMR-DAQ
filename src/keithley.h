#ifndef KEITHLEY_STRUCT_H
#define KEITHLEY_STRUCT_H 

// a data structure for the Keithley 2100 DMM 

typedef struct keithley { 
   int portNo;      // for USBTMC connection 
   double maxRange; // resistance range (depends on temperature sensor) 
} keithley_t; 

#endif 
