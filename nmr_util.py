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
     # year         = self.FindDir(path) 
     # month        = self.FindDir(path+year)  
     # day          = self.FindDir(path+year+"/"+month) 
     # top_data_dir = path + "/" + year + "/" + month + "/" + day + "/"
     now   = datetime.datetime.now()
     year  = str(now.year)
     month = str(now.month).zfill(2)
     day   = str(now.day).zfill(2)

     # top_data_dir = path + year + "/" + month + "_" + year[2] + year[3] + "/" + month + "_" + day + "_" + year[2] +year[3] + "/"
     top_data_dir = path + "/"

     last_run = self.FindDir(top_data_dir) 
     run_dir  = top_data_dir + last_run 

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
          myList[j] = last_run + "/" + str( entry ) 
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
                self.src_dir  = "/input/configs/files/"
                self.tgt_dir  = "/input/"
        #__________________________________________________________________
        def CreateSymLinks(self,fileList):  
                # create symbolic links to the current configuration
                cd_input  = "cd " + self.home + "/input/"
                cd_home   = "cd " + self.home
                symlink   = "ln -s "
                # loop over files and make symbolic links 
                for entry in fileList:
                   # construct source and target paths 
                   src_path = self.src_dir + entry + "_" + self.config + ".dat"
                   tgt_path = self.tgt_dir + entry + ".dat"
                   # delete current file at target location  
                   cmd      = "rm " + tgt_path
                   os.system(cmd)
                   # symlink source to target  
                   cmd      = symlink + src_path + " " + tgt_path
                   os.system(cd_input)
                   os.system(cmd)
                   # check if things look good 
                   if (os.path.isfile(tgt_path) ): print "[ConfigManager]: symbolic link from %s to %s created." % (src_path,tgt_path)
                # cd back to main dir
                os.system(cd_home)
        #__________________________________________________________________
