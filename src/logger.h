#ifndef LOGGER_H
#define LOGGER_H

// a logging data structure to keep track of the system status

#include <string>  

typedef struct logger { 
  unsigned long long timeStamp; 
  std::string outpath; 
  int numPulsesRecorded; 
  int errCode;
  int runStatus;  
} logger_t; 

#endif 
