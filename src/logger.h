#ifndef LOGGER_H
#define LOGGER_H

// a logging data structure to keep track of the system status

#include <string>  

enum sysIndicator{
  kPreRun = 0,
  kInRun  = 1
}; 

typedef struct logger { 
  unsigned long long timeStamp; 
  std::string prerunOutpath; 
  std::string runOutpath; 
  int numPulsesRecorded; 
  int errCode;
  int runStatus;  
} logger_t; 

#endif 
