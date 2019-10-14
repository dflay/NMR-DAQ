#ifndef CONSTANTS_H
#define CONSTANTS_H 

// a data structure to hold useful string constants
// this makes doing string comparisons much easier

typedef struct constants { 
   // times 
   static const std::string second;
   static const std::string millisecond;
   static const std::string microsecond;
   static const std::string nanosecond;
   static const std::string picosecond;
   // voltage and power 
   static const std::string Vpp; 
   static const std::string Vp;
   static const std::string Watts;
   static const std::string dBm;
   // frequencies 
   static const std::string kHz;
   static const std::string MHz;
   static const std::string GHz;
   // miscellaneous  
   static const std::string eof_tag; 
   static const std::string notdef;
   static const std::string on;  
   static const std::string ON;  
   static const std::string off;  
   static const std::string OFF; 
   static const std::string MECH_SWITCH_NAME;
   static const std::string GLOBAL_ON_OFF_NAME; 
   static const std::string MECH_SWITCH_1_NAME;   
   static const std::string MECH_SWITCH_2_NAME;   
   static const std::string MECH_SWITCH_3_NAME;   
   static const std::string MECH_SWITCH_4_NAME;   
   static const std::string RF_TRANSMIT_NAME;
   static const std::string RF_RECEIVE_NAME;
   static const std::string RF_GATE_NAME;
   static const std::string TOMCO_NAME;
   static const std::string DATA_DIR; 
   // device paths 
   static const std::string SG382_LO_DEV_PATH;
   static const std::string SG382_PI2_DEV_PATH;
   // miscellaneous paths
   static const std::string SG382_LO_IDN; 
   static const std::string SG382_PI2_IDN; 
   static const std::string FNAL_LOG_DIR; 
   static const std::string FNAL_DATA_DIR; 
   static const std::string FNAL_CURR_DIR;
   static const std::string FNAL_PARENT_DIR; 
   static const std::string ANL_LOG_DIR; 
   static const std::string ANL_DATA_DIR; 
   static const std::string ANL_CURR_DIR;
   static const std::string ANL_PARENT_DIR; 
   // hardware limits 
   static const double PI2_POWER_LIMIT; 
} constants_t;

#endif  