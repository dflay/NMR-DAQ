README
======

Table of Contents
----------------- 

   1. Description 
   2. Compilation and Running
   3. Input Files
   4. Pin Tables 
   5. FPGA FLASH Programming 
   6. Hardware Details 
   7. Driver Installations 
   8. Authors and Contact Information 

1. Description
-------------- 

   When executed, the binary file muon_g2_nmr communicates with the Acromag IP EP-201 
   FPGA board, mounted into slot B of the AVME 9660 6U carrier board.  Signals specified
   in the input files are loaded onto the IP EP-201 board.  This results in TTL pulses 
   on the FPGA output pins.  These pulses are propagated through the hardware, controlling
   the mechanical and RF switches to deliver pulses to the NMR probe and eventually 
   receiving the NMR signal.  The NMR signal is sent through one stage of amplification
   before being passed to the Struck SIS3302/SIS3316 Analog to Digital Converter (ADC).  A list 
   of the C files (located in ./src/) that controls the operation of the hardware is given  
   below.

   "main.c":            The main program where all of the relevant high-level commands 
                        are carried out.
   "acromag_ipep201.c": Describes the importing and processing of pulse signals to be 
                        delivered to the FPGA.  Uses the data structure "fpga" to store 
                        the data.  Also has a corresponding "*.h" file.  
   "fpga.h":            Data structure used by "acromag_ipep201.c" and is also used in 
                        the "main" program.
   "sg382.c":           Controls the operation of the Stanford Research Systems SG-382 
                        function generator, which serves as the local oscillator (LO) 
                        of the system. Also has its own "*.h" file. 
   "FuncGen.h":         Data structure used by "sg382.c"; contains general function generator
                        data. 
   "struck_adc.c":      Controls the operation of the SIS3302 or SIS3316 Struck ADCs.  
                        It has a corresponding "*.h" file.
   "sis3302.c":         Functions used to operate the Struck SIS3302 ADC. 
                        It has a corresponding "*.h" file.
   "sis3316.c":         Functions used to operate the Struck SIS3316 ADC. 
                        It has a corresponding "*.h" file.
   "sis_util.c":        Utility functions used by the Struck ADCs.  It has a corresponding 
                        "*.h" file.  
   "adc.h":             Data structure used by "struck_adc.c"; generic data structure for 
                        ADCs. 
   "acquisition.c":     Describes the acquisition of data, passing a bit-pattern of 
                        the FPGA gates to the ADC; when the FPGA transmit gate is closed, 
                        a trigger signal is initiated on the ADC, and data is recorded
                        for the specified duration of the transmit gate.  There is a 
                        corresponding "*.h" file. 
   "diagnostics.c":     Miscellaneous diagnostic functions that print pertinent run 
                        information to files and functions that checks the hardware configuration.  
                        There is a corresponding "*.h" file. 
   "util.c":            Utility functions used by the other files.  It has a corresponding 
                        "*.h" file.  


   NOTE: Do NOT change the source code "before" contacting the author!  Changes made to the 
         source code could cause serious damage to the FPGA and the rest of the hardware. 
         See the Author and Contact Information section below. 

2. Compilation and Running 
-------------------------- 

   - To compile the program, type "make" (no quotes).  
   - To run the program, the user may run the shell script "./run_nmr.sh" (no quotes), 
     or use the graphical user interface by running: "python nmr_daq.py" (no quotes). 
 
   Graphical User Interface 

   A Graphical User Interface (GUI) has been developed to run the system.  The user may input by hand
   the desired FPGA timing, function generator and ADC information, or load configurations previously saved.  
   The latter is done by clicking on the "Import Configuration..." button, which opens up a new window; 
   in this window, there are entry fields for all needed input files.  Here, the user may write in the "full"
   path to the file, or click the button next to the input field labeled "Choose file...".  Clicking this button 
   will open a third window, from which the user may make a selection.  Once the file has been specified, 
   clicking "Load Data" will load the data into the GUI.  Clicking "Set Configuration" will write the data 
   in the GUI to the appropriate locations (in "~/input/configs/files/").  To run the DAQ, click "Run"; this will 
   symbolically link all generated input files to the proper location ("~/input/") and run the C code via a 
   bash script ("./run_nmr.sh").    

   NOTE: Since the input files follow a specific format (see Chapter 3), the user should take care 
   to familiarize themselves with each category (FPGA, Function Generator, ADC and Utilities).
   The root file names for each category are as follows:

   FPGA:               "pulse-data.dat"
   Function Generator: "sg382.dat"
   ADC:                "struck_adc.dat"
   Utilities:          "utilities.dat" 

   See Chapter 3 for more details on the input files. 

3. Input Files 
--------------

   There are a number of input files, located in the ./input/ directory:  

   - pulse-data.dat: The pulse durations and offsets for the various components of the circuit.
                     These signals are sent to the FPGA. 
   - sg382.dat:      Stanford Research Systems SG382 function generator characteristics.  
   - struck_adc.dat: Struck ADC characteristics.    
   - utilities.dat:  Debug and test modes and other settings.  
   - comments.txt:   General comments that the user wants to keep with the output files. 
                     (This shows up in the output directory after the completion of a run, 
                     titled diagnostics.dat.  It has other important run information as well.) 

### Acromag IP-EP201 FPGA (pulse-data.dat) 
 
   The pulse characteristics are contained in the file ./input/pulse-data.dat.  It has 
   the following format:

   <ID>  <on/off> <time offset>  <pulse time> <units>  

   - ID:          A string identifying the component to which the signal is destined; 
                  see the Pin Tables below.  The strings must match the form shown in 
                  the Pin Tables. 
   - on/off:      Turns the signal on or off. 
   - time offset: How far after t = 0 should the pulse start.
   - pulse time:  The duration of the signal. 
   - units:       The units of the times indicated.  The valid units are seconds (s), 
                  milliseconds (ms), microseconds (us), nanoseconds (ns) or picoseconds (ps). 

   The input file must contain the global on/off switch (ID = global_on_off),
   and must placed in the first row.  Placing the string "on" in the on/off column 
   will allow the FPGA to output signals on the pins.  Using the string "off" turns 
   all output off.  The time offset and pulse times are irrelevant for the global switch, 
   so leave these as zero.  Also, the units may be declared as not defined ("ND").  Do 
   not use quotes in the input file.       

   The row-ordering of the input file does not matter after the first entry
   (corresponding to the global on/off switch). 

   See the file ./input/pulse-data.dat for example input. 
   
### Stanford Research Systems SG382  (sg382.dat) 

   This input file for the local oscillator (LO) contains the desired frequency and its units, 
   the BNC voltage, the N-Type voltage and their respective units. 

   The file format is as follows:

   # <ID>  <state>  <value>  <units> 

   - ID:    A string identifying a type
   - state: The state of the type: either on or off; valid only for BNC and N-Type fields
   - value: A number that determines how ID will function in the program.
   - units: The units for a given type.  Valid frequency units are MHz; 
            valid voltage units are Vpp (peak to peak), rms (root mean square)
            or dBm. 

   The row-ordering of the file does not matter. 

   See the file "./input/sg382.dat" for example input. 

### Struck ADCs: SIS3302 or SIS3316 (struck_adc.dat) 

   This input file contains the number of events (i.e., pulses), the 
   internal and external clock frequencies, and the time duration of the incoming 
   signal. 

   The file format is as follows:

   # <ID>  <value>  <units> 

   - ID:    A string identifying a type
   - value: A number that determines how ID will function in the program.
   - units: The units for a given type.  Valid frequency units are MHz.
   
   The row-ordering of the file does not matter. 
 
   The ADC model may be toggled between the SIS3302 or the SIS3316.  To set
   the ADC model, set the adc_id field to either 3302 or 3316. 
 
   To use an external clock, set the "external_clock" field to a value greater than 
   zero; the source code will configure the ADC to use the external clock when this 
   entry has a frequency greater than zero.  At minimum, the frequency f should 
   be in the range of 1 < f < 100 MHz.   

   For the SIS3302 ADC, it may be set in "single-event" mode or "multi-event" mode.  
   In single-event mode, the ADC will record one pulse, and the source code will immediately 
   write the data to file, as the next pulse will overwrite the ADC buffer.  In multi-event mode, 
   multiple pulses may be stored in the ADC memory; here, the data will be written to file
   at the end of the run.  To set the ADC to single-event mode, set the variable
   multi_event_state to 0.  For multi-event mode, set it to 1. 

   For the SIS3316 ADC, one has to specify the channel being utilized.  Set the channel_number
   field to a value (1--16) as necessary.

   The fields that are not needed by a specific ADC are ignored, so the input for 
   such fields may be arbitary and will not affect the functionality of the hardware.  
   For instance, the value of multi_event_state is not utilized by the SIS3316 ADC.
   Additionally, the field channel_number is not currently used by the SIS3302 ADC. 
   Despite this, one should take care to use some value for all input fields.      

   See the file ./input/struck_adc.dat for example input. 

### Debug Mode, Test Mode and Verbosity  (utilities.dat) 

   A debug mode has been built into the program to assist in debugging the software
   if any bugs are discovered.  In debug mode, the program will run as it was usual, 
   but now will print out more detail concerning what it is doing, including the process
   of initializing and programming the FPGA and ADC, etc.  The input file ./input/utilities.dat
   contains information used to run the program in debug mode.  Its format is as follows: 

   # <ID>  <value> 

   - ID:    A string identifying a type
   - value: A number that determines how ID will function in the program. 

   To run the program in debug mode, change the debug flag in the ./input/utilities.dat 
   file to be true (i.e., set debug_mode to 1).  Additionally, the verbosity 
   of the debug mode may be set by using the verbosity flag, where 0 is the least 
   verbose, and 4 is the most verbose.  To turn debug mode off, set debug_mode to 0.   

   There is also a test mode built in.  This mode can be used for any test 
   the user may wish to carry out; for example, testing out hardware configurations 
   in the main system, or just testing a single component like an ADC.  Below is a 
   table listing all of the currently available test mode options:

   -----------------------------------------------------------------------
   Test Mode Flag |   Description 
   -----------------------------------------------------------------------
         1        | Running in test mode will run the NMR DAQ, but will NOT 
                  | utilize the ADC.  The system will run indefinitely until 
                  | the user is ready to quit.  This is useful for testing 
                  | out hardware configurations and looking for inconsistencies 
                  | during a run. 
   -----------------------------------------------------------------------
         2        | The software will read the Module ID of the specified 
                  | ADC and quit thereafter.  Nothing else occurs.  
   -----------------------------------------------------------------------
         3        | Generic test mode for the Struck SIS3316 ADC.  Various 
                  | commands for reading and writing to the 3316 will be 
                  | executed.  After these commands have been 
                  | attempted/completed, the system will quit.  Nothing 
                  | else occurs.      
   -----------------------------------------------------------------------

   To run the system in a given test mode, set the test_mode flag to the 
   desired value in the utilities input file.   
 
   The row-ordering of the file does not matter. 

   See the file ./input/utilities.dat for example input. 

4. Pin Tables 
-------------

   The I/O pins on the FPGA are configured to deliver signals to 
   the RF electronics.  Below is a table of which pins on the FPGA 
   correspond to a given RF electronics component. 

   FPGA Pin Table:  
 
   Pin     ID           Description
   ---------------------------------------------------
   1       mech_sw_1    Mechanical switch 1 (connects J1 to input) 
   2       mech_sw_2    Mechanical switch 2 (connects J2 to input)
   3       mech_sw_3    Mechanical switch 3 (connects J3 to input)
   4       mech_sw_4    Mechanical switch 4 (connects J4 to input) 
   5       rf_sw_1      RF switch 1 (connects TOMCO gated signal to probe)
   6       rf_sw_2      RF switch 2 (connects probe to second RF switch)
   7       rf_sw_3      RF switch 3 (connects second RF switch to Pasternack and Mini-Circuits amplifiers) 
   8       rf_clear     RF clear signal  
   10      rf_gate      RF gate 

   NOTE: The user must use these strings ("ID") in the input file in the "ID" column.  

   Pin tables for internal electronics are displayed below, along with 
   their FPGA pin (corresponds to the 50-pin table above).  The column labeled 
   "Internal Pin" corresponds to the Dsub9 connector that passes the signals onto 
   the 24V pulser and Radant Mems RMDR card enclosure ("Power Enclosure").  
   In the Power Enclosure, the pin numbers change once more.  The "RF Enclosure" 
   houses the RF switches.  The pin-ordering changes in this enclosure relative
   to the internal pin-ordering.  Note that the "rf_gate" does not appear 
   in the tables below.  This is because it is taken from the FPGA breakout board
   to a BNC connection that sends the signal to the TOMCO amplifier. 

   Mechanical Switch Pin Table: 

   Switch     FPGA Pin  Internal Pin  Power Enclosure Pin    
   -------------------------------------------------------------------------------------------
   mech_sw_1     1           6              2                   
   mech_sw_2     2           7              3                   
   mech_sw_3     3           8              4                   
   mech_sw_4     4           9              5                   

   The physical layout of the mechanical switches on the RF box is shown below:

   ----------------
   - J1        J2 -
   -              -
   -      IN      -
   -              -
   - J4        J3 -
   ----------------

   where IN is the input port; the pulse from the TOMCO is delivered to this port, and 
   mech_sw_1,...,mech_sw_4 connect IN to ports J1,...,J4.  

   The pin table for the RF switches is below.  "RMDR Input Label" and "RMDR Output Label" are 
   the input and output signal names from the RMDR data sheet.  

   RF Switch Pin Table: 

   Switch     FPGA Pin  Internal Pin  RMDR Input Label  RMDR Output Label  RF Enclosure Pin     
   -------------------------------------------------------------------------------------------
   rf_sw_1       5           1             Ctrl0               Out1            2                   
   rf_sw_2       6           2             Ctrl1               Out2            3                   
   rf_sw_3       7           3             Ctrl2               Out4            7                   
   rf_clear      8           4             Clr                 ---            ---

   The physical layout of the RF switches is shown below.  The switch labeled "rf_sw_2"
   connects the path from the TOMCO to the probe: 

                      To Pasternack 

                      |                        
       RF Switch A    |
                      |
                      |
                        ------  unused       
                      | 
                      |
                      |
                      |
    ---------------------------------------------------------------
                      |
       RF Switch B    |
                      |
                      |

                        "------"  To Tomco  
                      "/"
                     "|"
                     "|"
                     "|"

                      To probe        


   The switch labeled "rf_sw_3" connects the path from the probe to the preamp (Pasternack): 

                     To Pasternack 

                     "|"
       RF Switch A   "|"
                     "|"
                     "|"
                     "|"  ------  unused   
                     "|" 
                     "|"
                     "|"
   ---------------------------------------------------------------
                     "|"
       RF Switch B   "|"
                     "|"
                     "|"
                     "|"  ------  To Tomco  
                     "|" 
                     "|" 
                     "|"
                     "|"

                      To probe         

   NOTE: rf_sw_1, rf_sw_2 and rf_sw_3 follow a certain truth table.  If the signals 
         overlap, an output signal on the RMDR card will be set to logic high which
         is NOT connected in our setup. 

   Alternate approach: One RF switch may be used, where "rf_sw_2" controls the 
   connection between the Tomco and the probe and "rf_sw_3" controls the connection
   between the probe and the Pasternack amplifier.  When "rf_sw_3" is closed, 
   we have: 

   ---------------------------------------------------------------
                     "|"
       RF Switch     "|"
                     "|"
                     "|"
                     "|"  ------  To Tomco  
                     "|" 
                     "|" 
                     "|"
                     "|"
   ---------------------------------------------------------------

   This method provides slightly worse isolation between the probe and the 
   Pasternack when "rf_sw_2" is activated, at about -67 dB.  Adding the second 
   switch gives an isolation of roughly -78 dB.  Currently, only one RF switch is used.     

5. FPGA FLASH Programming
-------------------------
 
   The directions to program the VHDL code to flash memory are described below
   (these instructions are from the Acromag programming manual).  All tasks are 
   carried out on the Windows machine, except where specified. 

   1.  Power-down the IP-EP2 module and connect the 10-pin Altera JTAG
       cable (not included) to the board. This cable is available from Altera.
   2.  Set the Configuration Jumper to "FLASH" as shown in JTAG
       Interface/Jumper Location drawing located in the IP-EP2 Series User’s
       Manual. Failure to set this jumper correctly will cause programming to
       fail.
   3.  Power-up the IP-EP2 module and start the Quartus II software.
   4.  Generate the .sof programming file. This file is automatically generated
       by Quartus II upon successful compilation. The file is located in your
       Quartus II project directory.
   5.  Select Convert Programming Files from the File menu.
   6.  In the Convert Programming Files dialog box, select JTAG Indirect
       Configuration File (.jic) from the Programming file type pull-down menu.
   7.  In the Configuration Device pull-down menu select EPCS4.
   8.  In the File name field, set the output file name and directory.
   9.  Click on “SOF Data” in the Input Files to convert section.
   10. Click Add File and select the sof file generated in step 1. Click OK
   11. Highlight FlashLoader and click Add Device.
   12. Select the Cyclone II EP2C20F256 device. Click OK.
   13. Click the Generate process to create the *.jic file. 
   14. Start the Quartus II Programmer. The Programmer can be started by
       first starting the Quartus II software and then selecting Programmer
       under the Tools menu.
   15. In the Programming Window, click Hardware Setup. Under the
       Currently Selected Hardware pull-down menu, select the device that
       connects to the IP-EP2 Series board (i.e. ByteBlaster II). Click close to
       return to the Programming Window.
   16. From the Mode pull-down window, select JTAG.
   17. In the left pane, click the Add File button. Then select the *.jic file
       generated in step 5. Click Open. Now the programming file and the
       Cyclone II device should be listed in the window.
   18. Check both boxes under the Program/Configure column.
   19. Then click on the Start button to download the file to the FLASH via
       JTAG
   20. Once complete the IP-EP2 module will still be in configuration
       mode. To trigger a configuration cycle to load the program from
       Flash, either write a “1” to bit 0 of the Configuration Control/Status
       Register (base_addr + 0x0) or power down and then power the
       board back up. NOTE: This step is programmed into the *.c code.  
       Turn on the global variable gIsFLASH to activate it. 

6. Hardware Details
-------------------

   The following hardware addresses are in the source code, but it is useful to 
   repeat this information here. 

   - VME base address: 0x5500ffff

   - Carrier board Short I/O base address: 0000 (hex) 
    
   - Short I/O Memory Maps (base address + hex)
     - IP A ID space: 0080 
     - IP B ID space: 0180 
     - IP C ID space: 0280 
     - IP D ID space: 0380 

7. Driver Installations
-----------------------

   All needed driver files are located in the "./drivers/" folder.
   Directions for specific hardware follow below.  

   NOTE: Unless specified otherwise, such messages shown below from the dmesg command 
   are examples only.  Your device naming scheme may differ. 

   > Stanford Research Systems SG382 Function Generator 

     - First plug in the USB-Serial Port adaptor to one of your USB ports. 
       Wait for a couple of seconds, then run “dmesg”. You should see these 
       messages at the end of dmesg output.  
      
       usb 1-1: new full speed USB device using uhci_and address 2
       usb 1-1: configuration #1 chosen from 1 choice
      
     - After that, unplug the device and type lsusb. You will see a list of output similar to this
       generic example: 
      
       Bus 003 Device 001: ID 0000:0000
       Bus 002 Device 007: ID 03f0:4f11 Hewlett-Packard
       Bus 002 Device 006: ID 05e3:1205 Genesys Logic, Inc. Afilias Optical Mouse H3003
       Bus 002 Device 004: ID 15d9:0a33
      
     - Plug in the USB-Serial Port converter back, and run lsusb again, and you shall 
       see an additional line, like this: 
      
       Bus 003 Device 001: ID 0000:0000
       Bus 002 Device 007: ID 03f0:4f11 Hewlett-Packard
      "Bus 001 Device 002: ID 4348:5523"  <-------------------- note the additional line 
       Bus 002 Device 006: ID 05e3:1205 Genesys Logic, Inc. Afilias Optical Mouse H3003
       Bus 002 Device 004: ID 15d9:0a33
     
     - Now we know the vendor ID and the product ID of the USB-Serial Port converter, 
       this will enable us to load the linux kernel module usbserial to activate the device, like this :
      
       sudo modprobe usbserial vendor=0x4348 product=0x5523
      
     - Run dmesg again and you shall see lines similar to this:
       
       usbserial_generic 1-1:1.0: generic converter detected
       usb 1-1: generic converter now attached to ttyUSB0
       usbcore: registered new interface driver usbserial_generic

     - The last line may be buried in the dmesg command -- not necessarily near the end. 
       This is because it appears that the machine may do these steps for you (that is, automatically)
       upon connecting the device, and hence such messages appear somewhere in the middle 
       of the output.  In any case, for this USB-Serial Port Converter used for the Stanford 
       Research Systems SG382 Function Generator, we are looking for the lines:

       "usb 3-1: new full speed USB device number 6 using xhci_hcd       "       
       "usb 3-1: New USB device found, idVendor=067b, idProduct=2303     "             
       "usb 3-1: New USB device strings: Mfr=1, Product=2, SerialNumber=0"               
       "usb 3-1: Product: USB-Serial Controller D                        "           
       "usb 3-1: Manufacturer: Prolific Technology Inc.                  "    
       "usb 3-1: configuration #1 chosen from 1 choice                   "     
       "pl2303 3-1:1.0: pl2303 converter detected                        "  
       "usb 3-1: pl2303 converter now attached to ttyUSB0                " 
 
       Note that the usb tag number (3-1 here) may be different in your setup. Above, 
       we see the vendor number is 0x067b (in hex).  The product 
       number is 0x2303 (in hex).  The result of the sudo command above should show 
       new lines corresponding to:   

       "usbcore: registered new interface driver pl2303     "
       "pl2303: Prolific PL2303 USB to serial adaptor driver"
 
       Again, this might have been done automatically.  The easiest approach is to 
       run dmesg and pipe the output into a text file and search for the usbcore output.  
       To put the dmesg output in a file, run in a Terminal:  

       dmesg > my_dmesg 

       From here open the file my_dmesg and search for the usbcore output. 

       After the above has been verified, run in a Terminal:
     
       "sudo chmod 666 /dev/ttyUSB0" (no quotes) 

       This device may be attached to a different point than ttyUSB0.  Replace that 
       string as necessary. 

       After this has been done, the function generator is now recognizable by the system
       and can be accessed.

       "NOTE: Do this after every restart of the system."

   > Struck SIS3302 ADC Driver 

     - cd into the Struck ADC driver directory, and into ~/dev/pci. Then run (in a Terminal):
       make clean
       make 
   
     - Once that is completed, run:
       sudo make install 
   
     - Check the installation:
       dmesg
   
       At the end of this printout, you should see something like: 
   
       SIS1100 driver V2.13 (c) 2012-01-19 Forschungszentrum Juelich
       sis1100: found SIS1100 PCIe single link at 0000:05:04.0
       sis1100 0000:05:04.0: PCI INT A -> GSI 16 (level, low) -> IRQ 16
       sis1100[0]: hardwired device id: 0x9056
       sis1100[0]: 64 Bit DMA not available
       sis1100[0]: local ident: 0x06220201
       sis1100[0]: HW version 2; FW code 34; FW version 6
       sis1100[0]: 2GBit link!
       sis1100[0]: Driver not tested with firmware versions higher than 2.
       sis1100[0]: size of remote space: 0x10000000 (256 MByte)
       sis1100[0]: board type: PCIe_single_link, serial=177
       sis1100[0]: remote ident: 0x03020202
       sis1100[0]: remote is VME
       sis1100[0]: remote HW_ver 2 FW_code 2 FW_ver 3
       sis1100[0]: 310x: Driver not tested with remote firmware versions higher than 2.
       sis1100[0]: 3100: berr_time=6250 ns
       sis1100[0]: 3100: long_time=10 ms
       sis1100[0]: remote device is SIS3104 with 1G/2G autobaud
       sis1100[0]: 3100: remote stat=0x00015108
       sis1100 driver exit
       sis1100 0000:05:04.0: PCI INT A disabled
       SIS1100 driver V2.13 (c) 2012-01-19 Forschungszentrum Juelich
       sis1100: found SIS1100 PCIe single link at 0000:05:04.0
       sis1100 0000:05:04.0: PCI INT A -> GSI 16 (level, low) -> IRQ 16
       sis1100[0]: hardwired device id: 0x9056
       sis1100[0]: 64 Bit DMA not available
       sis1100[0]: local ident: 0x06220201
       sis1100[0]: HW version 2; FW code 34; FW version 6
       sis1100[0]: 2GBit link!
       sis1100[0]: Driver not tested with firmware versions higher than 2.
       sis1100[0]: size of remote space: 0x10000000 (256 MByte)
       sis1100[0]: board type: PCIe_single_link, serial=177
       sis1100[0]: remote ident: 0x03020202
       sis1100[0]: remote is VME
       sis1100[0]: remote HW_ver 2 FW_code 2 FW_ver 3
       sis1100[0]: 310x: Driver not tested with remote firmware versions higher than 2.
       sis1100[0]: 3100: berr_time=6250 ns
       sis1100[0]: 3100: long_time=10 ms
       sis1100[0]: remote device is SIS3104 with 1G/2G autobaud
       sis1100[0]: 3100: remote stat=0x00015108
   
     - Additionally, check the installation by listing the contents of /dev/:
   
       ls /dev/sis* 
   
       /dev/sis1100_00ctrl  /dev/sis1100_00remote  /dev/sis1100_01ram     /dev/sis1100_02dsp     /dev/sis1100_03ctrl  /dev/sis1100_03remote
       /dev/sis1100_00dsp   /dev/sis1100_01ctrl    /dev/sis1100_01remote  /dev/sis1100_02ram     /dev/sis1100_03dsp
       /dev/sis1100_00ram   /dev/sis1100_01dsp     /dev/sis1100_02ctrl    /dev/sis1100_02remote  /dev/sis1100_03ram

     - Once the above has been verified, the Struck ADC has been successfully installed onto the system. 

8. Authors and Contact Information 
---------------------------------- 

   If there are any questions, concerns or issues, please contact the authors:   

   authors:     David Flay and Zac Meadows  
   affiliation: UMass Amherst 
   contact:     David (flay@umass.edu)   

