#ifndef KEITHLEY_INTERFACE_H
#define KEITHLEY_INTERFACE_H

// functions for comms with the Keithley DMM

#include <fstream>
#include <sstream> 
#include <string>
#include <cstdlib> 
#include <stdio.h> 
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

int keithley_interface_open_connection(void);
int keithley_interface_close_connection(int portNo);
int keithley_interface_get_device_id(int portNo,char *response);
int keithley_interface_get_mode(int portNo,char *response);
int keithley_interface_set_to_remote_mode(int portNo);
int keithley_interface_set_range(int portNo,double maxRange);
int keithley_interface_check_errors(int portNo,char *err_msg);
int keithley_interface_write_cmd(int portNo,const char *cmd);
int keithley_interface_query(int portNo,const char *cmd,char *response);
double keithley_interface_get_resistance(int portNo);

#endif 
