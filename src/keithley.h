#ifndef KEITHLEY_STRUCT_H
#define KEITHLEY_STRUCT_H 

// a data structure for the Keithley 2100 DMM 

typedef struct keithley { 
   std::string temp_sensor_name; 
   double maxRange; // resistance range (depends on temperature sensor) 
   int portNo;      // for USBTMC connection 
   int enable;      // enable (1) or disable (0) readout 
} keithley_t; 

#endif 
