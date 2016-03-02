# a collection of useful classes 
import os 
import datetime
import operator 
from decimal import Decimal
from stat    import * 

#__________________________________________________________________________
# a class to find data in the newest directory 
class DataManager:
   #_______________________________________________________________________
   def __init__(self): 
      self.fVerbosity = 0 
      self.fHomeDir   = os.getcwd() + "/" 
      self.fFile      = [0,0,0,0]   # init to size 4; dummy values  
      self.fModDir    = "UNKNOWN"
   #_______________________________________________________________________
   def FindDir(self,path): 
     os.chdir(path) 
     dirs = sorted(os.listdir(os.getcwd()),key=os.path.getmtime)
     for entry in dirs:
       if ( os.path.isdir(entry) ): last_dir_mod = entry   # gets last directory modified
     os.chdir("..") 
     return last_dir_mod  
   #_______________________________________________________________________
   def FindData(self,path):
     # find directories 
     year         = self.FindDir(path) 
     month        = self.FindDir(path+year)  
     day          = self.FindDir(path+year+"/"+month) 
     top_data_dir = path + "/" + year + "/" + month + "/" + day + "/"
     last_run     = self.FindDir(top_data_dir) 
     run_dir      = top_data_dir + last_run 

     # print top_data_dir
     # print run_dir  
 
     # get all files in the run directory  
     os.chdir(run_dir) 
     files = sorted(os.listdir(os.getcwd()),key=os.path.getmtime)
     files.reverse()

     # go back to initial data directory
     os.chdir(top_data_dir)

     # find last four runs 
     j      = 0
     NMAX   = 4
     myPulse = [0,0,0,0]
     for entry in files:
       length = len(entry)
       suffix = entry[length-3] + entry[length-2] + entry[length-1]
       if( (suffix == "bin")&(j < NMAX) ):
          myPulse[j] = entry  
          j=j+1

     j=0
     myPulse.reverse()  
     myList = [0,0,0,0] 
     for entry in myPulse: 
          myList[j] = last_run + "/" + entry
          j = j + 1
     # for entry in myList: print entry
     return myList  

#__________________________________________________________________________
# a class to set an NMR run configuration
class ConfigManager:
        #__________________________________________________________________
        def __init__(self,home_path,config_name):
                self.home     = home_path
                self.config   = config_name
                self.fpga     = "pulse-data" 
                self.func_gen = "sg382" 
                self.adc      = "struck_adc" 
                self.util     = "utilities" 
                self.com      = "comments" 
                self.src_dir  = "/input/configs/files/"
                self.tgt_dir  = "/input/"
        #__________________________________________________________________
        def Print(self):
                print( "[ConfigManager]: home               = %s" %(self.home)     )
                print( "[ConfigManager]: config             = %s" %(self.config)   )
                print( "[ConfigManager]: fpga file name     = %s" %(self.fpga)     ) 
                print( "[ConfigManager]: func gen file name = %s" %(self.func_gen) ) 
                print( "[ConfigManager]: adc file name      = %s" %(self.adc)      ) 
                print( "[ConfigManager]: util file name     = %s" %(self.util)     ) 
                print( "[ConfigManager]: comments file name = %s" %(self.com)      ) 
                print( "[ConfigManager]: src directory      = %s" %(self.src_dir)  ) 
                print( "[ConfigManager]: tgt directory      = %s" %(self.tgt_dir)  ) 
        #__________________________________________________________________
        def CreateSymLinks(self):  
                # create symbolic links to the current configuration
                cd_input  = "cd " + self.home + "/input/"
                cd_home   = "cd " + self.home
                symlink   = "ln -s "
                # define the source files 
                fpga_src  = self.home + self.src_dir + self.fpga     + "_" + self.config + ".dat" 
                fg_src    = self.home + self.src_dir + self.func_gen + "_" + self.config + ".dat"  
                adc_src   = self.home + self.src_dir + self.adc      + "_" + self.config + ".dat"  
                util_src  = self.home + self.src_dir + self.util     + "_" + self.config + ".dat"   
                com_src   = self.home + self.src_dir + self.com      + "_" + self.config + ".txt"  
                # define targets  
                fpga_tgt  = self.home + self.tgt_dir + self.fpga     + ".dat"
                fg_tgt    = self.home + self.tgt_dir + self.func_gen + ".dat"
                adc_tgt   = self.home + self.tgt_dir + self.adc      + ".dat"
                util_tgt  = self.home + self.tgt_dir + self.util     + ".dat"
                com_tgt   = self.home + self.tgt_dir + self.com      + ".txt"
                # remove commands
                rm_fpga   = "rm " + fpga_tgt
                rm_fg     = "rm " + fg_tgt
                rm_adc    = "rm " + adc_tgt
                rm_util   = "rm " + util_tgt
                rm_com    = "rm " + com_tgt
                # check for existing files 
                fpga_is_alive = os.path.islink(fpga_tgt)
                fg_is_alive   = os.path.islink(fg_tgt)
                adc_is_alive  = os.path.islink(adc_tgt)
                util_is_alive = os.path.islink(util_tgt)
                com_is_alive  = os.path.islink(com_tgt)
                # delete current symbolic links if necessary 
                if fpga_is_alive: os.remove( fpga_tgt )
                if fg_is_alive:   os.remove( fg_tgt   )
                if adc_is_alive:  os.remove( adc_tgt  )
                if util_is_alive: os.remove( util_tgt )
                if com_is_alive:  os.remove( com_tgt  )
                # define the commands 
                fpga_cmd  = symlink + fpga_src + " " + fpga_tgt   # symbolic link for FPGA 
                fg_cmd    = symlink + fg_src   + " " + fg_tgt     # symbolic link for function generator  
                adc_cmd   = symlink + adc_src  + " " + adc_tgt    # symbolic link for ADC 
                util_cmd  = symlink + util_src + " " + util_tgt   # symbolic link for utilities 
                com_cmd   = symlink + com_src  + " " + com_tgt    # symbolic link for comments
                # print fpga_cmd 
                # print fg_cmd 
                # print adc_cmd 
                # print util_cmd 
                # print com_cmd 
                # symbolically link files
                os.system(cd_input)
                os.system(fpga_cmd)
                os.system(fg_cmd)
                os.system(adc_cmd)
                os.system(util_cmd)
                os.system(com_cmd)
                # check to see if commands succeeded 
                fpga_is_alive = os.path.islink(fpga_tgt)
                fg_is_alive   = os.path.islink(fg_tgt)
                adc_is_alive  = os.path.islink(adc_tgt)
                util_is_alive = os.path.islink(util_tgt)
                com_is_alive  = os.path.islink(com_tgt)
                if fpga_is_alive: print "[ConfigManager]: symbolic link from %s to %s created." % (fpga_src,fpga_tgt)
                if fg_is_alive:   print "[ConfigManager]: symbolic link from %s to %s created." % (fg_src  ,fg_tgt  )
                if adc_is_alive:  print "[ConfigManager]: symbolic link from %s to %s created." % (adc_src ,adc_tgt )
                if util_is_alive: print "[ConfigManager]: symbolic link from %s to %s created." % (util_src,util_tgt)
                if com_is_alive:  print "[ConfigManager]: symbolic link from %s to %s created." % (com_src ,com_tgt )
                # cd back to main dir
                os.system(cd_home)
        #__________________________________________________________________

