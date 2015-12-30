#! /usr/bin/python 

import os
import time
import sys
from confmgr import ConfigManager
from timeit import default_timer

home = os.getcwd()
config_name = ""
myConfig = ConfigManager(home,config_name) 

# commands 
run_cmd      = "./run_nmr.sh"
list_configs = "ls ./input/configs/" 
config_dir   = home + "/input/configs"

# grab all file names in the configuration directory 
filenames    = next( os.walk(config_dir) )[2]

# some units 
sec    = 1
ms     = 1E-3*sec
us     = 1E-6*sec
minute = 60*sec; 
hr     = 60*minute 

max_time = 0

input_units = -1 

print "----------------------- Batch NMR DAQ script ----------------------- \n" 
# this reads inputs as integer, double, etc 
input_units = input("Enter time units (0 = min, 1 = sec): ")
max_time    = input("Enter maximum time to run:           ") 
wait_time   = input("Enter time between runs:             ") 

# get parameter configuration 
print "Choose input parameter configuration (enter -1 if choice not available): "
# os.system(list_configs)
# os.listdir(config_dir)
i = 0
N = len(filenames) 
for f in filenames:
   print("%d: %s" %(i,f) )
   i = i + 1  

choice = input("Choice: ") 


if(choice>=0 and choice<N): 
   array = filenames[choice].split(".") 
   config_name = array[0]
else:
   print "Configuration not present.  To create a configuration: "
   print "1. Run the GUI, fill in the entry fields"
   print "2. Click 'Apply Configuration' "
   print "3. Click 'Quit' "
   print "4. Re-run this script to enter batch mode. "
   sys.exit(0) 

# this reads input as a string 
# config_name = raw_input("Enter configuaration name (no suffix): ")  
print("Loading %s..." %(config_name) ) 

# set up the configuration
myConfig.config = config_name

myConfig.Print()  
myConfig.CreateSymLinks() 

# set time units 
unit = 0
if input_units==0:
   unit = minute
elif input_units==1:
   unit = sec 

max_time   = max_time*unit  
wait_time  = wait_time*unit
t_start    = default_timer()
t_duration = 0

print( "max time:  %.3f s" %(max_time)  ) 
print( "wait time: %.3f s" %(wait_time) )

# run the DAQ  
while t_duration <= max_time:
   print( "Time passed: %.3f s" %(t_duration) )
   # do stuff
   # os.system(run_cmd) 
   # wait specified time 
   time.sleep(wait_time)
   # get elapsed time to see if we keep taking data 
   t_duration = default_timer() - t_start
# end of loop 

print "End of runs.  Exiting..."
print "-------------------------------------------------------------------- \n" 

