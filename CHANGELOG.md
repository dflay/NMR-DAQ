======================================== CHANGELOG ========================================

>> 12/2/15, David Flay
   
   - Moved all hard-coded FPGA addresses to the ./src/acromag_ipep201.h file, and are now 
     defined values.  

>> 8/12/15, David Flay

   - Added a check on all function generator settings.  If either the frequency or voltage
     settings are out of range for a given output port (BNC or N-Type), then an error 
     is returned and the program quits. 

>> 8/10/15, David Flay

   - Implemented a new graphical user interface (GUI).  See the README for usage details.

>> 8/5/15, David Flay
  
   - Added a timing check for the FPGA signals.  The program quits without programming any 
     signals onto the FPGA and an error code is returned if the timing is not correct. 

>> 7/31/15, David Flay

   - Moved the SIS3302 functions out of the `struck_adc` files and into their own files, 
     named sis3302_um.c and sis3302_um.h, making the format consistent with the SIS3316 code.  
     Now the struck_adc.c and struck_adc.h files serve to control the ADC operation in terms 
     of initialization and other arbitrary needs, referring to either the 3302 or 3316 as 
     necessary.

  -  The `AcquireData` functions have been moved out of the main function and into their 
     own files, named acquisiton.c and acquisition.h; similarly, functions that print 
     run information to file have been moved to the diagnostics.c and diagnostics.h files.
     The CheckHardware() function is also located in the diagnostics files.  

>> 7/29/15, David Flay

   - Integrated the SIS3316 into the rest of the system -- now the ADC can be triggered by 
     the FPGA logic.  See the AcquireDataSIS3316 function in the main function.  Also some 
     cleaning up of print statements, where they have been relegated to debug or test modes.     

>> 7/28/15, David Flay
 
   - Added the `adc_spi_setup` (and related) functions from the SIS3316 class (from Struck).  
     This allows for proper initialization and turning on the ADC outputs!  See the 
     SIS3316Init() function in sis3316_um.c file.  

>> 7/27/15, David Flay

   - Added the `sis_util.c` and `sis_util.h` files; these hold generic read/write functions
     needed for VME and SIS accesses. 

>> 7/24/15 

   - Afternoon, David Flay 
     The read_DMA_... function (sis3316_um files) returns a data array of type unsigned int, 
     but that is technically *at least* 16 bits; casting to unsigned long changes the value,
     indicating that we have 32-bit words in the array.  Casting to unsigned long long (64 bits) 
     does not change the value further, as expected. Added the functionality to set the 
     *extended* raw data buffer for (standard) case where we want to record for longer than
     the baseline of 64k.  The input for the trigger gate window is now set as a time, and converted
     to an unsigned int according to which clock is chosen -- on-board or external.  Additionally, 
     added a function to print the data to a file.  The extended raw data buffer now receives a value 
     identical to the number of desired samples; previously using the number of 32-bit words, which 
     is half as large as the original number of samples, ultimately yielding half as much data.  
     Changing this to the sample length, we get the desired amount of data upon plotting the output.        
     This seems curious, but I suppose the fix is correct since there is no mention of these 
     registers expecting the number of 32-bit words upon writing to them.

   - Morning, David Flay and Zac Meadows
     Got the SIS3316 ADC working; can see data using 50-Ohm termination.  The
     order of the intialization function (SIS3316Test in struck_adc.c) was changed toi
     match that of the Struck code; also, the external trigger bit(s) were not set properly, 
     along with the wrong base address in the read_DMA_... function in the sis3316_um file.     

>> 7/23/15, David Flay 

   - Added the file `sis3316_um.c` and `sis3316_um.h` to the ./src/ directory.  This 
     contains the Struck SIS3316 C++ functions from their class, modified to work with our 
     code.  We should eventually develop one for the 3302 so that our struck_adc files
     utilize both as necessary.  

>> 6/25/15, David Flay and Zac Meadows

   - Implemented a block read of the ADC memory that operates in multi-event mode; 
     after every 5 pulses, the ADC memory will be printed to files. 
     This allows for long runs for a given hardware configuration.  Data printed 
     to files is in binary format.    

>> 6/24/15, David Flay

   - Generalized the sis3302 source code to struck_adc.  This way, we can use various 
     Struck ADC models with one code base. 

   - Added an IO space read function to the struck_adc source code.  This will read
     the ADCs module ID and compare it against the input ID value (adc_id in the input file)
     to verify things are consistent.   

>> 6/22/15, David Flay

   - Changed the maximum counter on the FPGA board to 275 ms.  This way, we can get 3--4 
     pulses per second. 

>> 6/8/15, David Flay

   - Added the option to run the ADC in single-event mode; here, the ADC will record a 
     single NMR pulse and immediately write it to file.  This will help when we run 
     for an arbitrarily large number of pulses, that would otherwise max-out the memory
     of the ADC in multi-event mode.  See the README and the SIS3302 input file for details.   

     - This needs debugging; the block-read of the ADC memory seems to be introducing noise
       into our signal; additionally, we see a funny offset near t = 5 ms. 

>> 6/5/15, David Flay

   - Added functions to get the time corresponding to the start of each pulse and 
     records it to file. 

>> 6/3/15, David Flay

   - Added a function that prompts the user to make sure the hardware is powered properly

>> 5/27/15, David Flay

   - Implemented functionality for using an external clock on the ADC.  See README concerning
     directions for usage.   

>> 5/15/15, David Flay 
   
   - CHANGELOG created.  

   - Changed the output of the SIS3302 class to be integers and unsigned long integers 
     for the time and voltage of the signal, respectively.  Previously, the time was 
     recorded in seconds and volts. 
 
