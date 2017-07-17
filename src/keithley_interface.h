// functions for comms with the Keithley DMM

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "util.h" 

int keithley_interface_open_connection(void);
int keithley_interface_get_device_id(int portNo,char *response); 
int keithley_interface_get_mode(int portNo,char *response); 
int keithley_interface_set_to_remote_mode(int portNo); 
int keithley_interface_set_range(int portNo,double maxRange); 
int keithley_interface_check_errors(int portNo,char *err_msg); 
int keithley_interface_write_cmd(int portNo,const char *cmd); 
int keithley_interface_query(int portNo,const char *cmd,char *response); 
double keithley_interface_get_resistance(int portNo);

