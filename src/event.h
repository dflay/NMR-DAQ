#ifndef EVENT_H
#define EVENT_H 

// an NMR event for a calibration probe 

typedef struct event { 
   unsigned long long timestamp;      // UTC timestamp [ns] 
   double temperature;                // from Keithley [Ohms]  
   double x;                          // radial coordinate (cm) 
   double y;                          // vertical coordinate (cm)
   double z;                          // azimuthal coordinate (cm) 
   int chNum;                         // channel number (mechanical switch port) 
   int pulseNum;                      // overall pulse number 
} event_t;  

#endif 
