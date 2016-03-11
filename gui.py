#! /usr/bin/python 

import Tkinter  
import os
from decimal      import Decimal 
from tkFileDialog import askopenfilename
from nmr_util     import DataManager 
from subprocess   import call

#--------------------------------------------------------------------------------
class daq(Tkinter.Tk): 

   def __init__(self,parent): 
      Tkinter.Tk.__init__(self,parent)
      self.parent = parent # reference to the parent 
      self.initialize() 

   def initialize(self): 
      self.grid() 
 
      self.counter = 0 
      self.MyHOME = os.getcwd() + "/"  
      self.ConfVarLabel = Tkinter.StringVar()  
      self.IsDebug = 1   # if 1, print file contents to screen  

      # variables and lists  
      TickBox            = "on"
      self.TimeChoices   = ['s','ms','us']
      self.DebugChoices  = ['off','on']
      self.VerbChoices   = ['0','1','2','3','4']
      self.TestChoices   = ['0','1','2','3','4','5']
      self.ConfigChoices = ['OFF','Short Coil','Long Coil','Cyl. Probe','Sph. Probe','Other']
      FreqValues         = ['1','10','25','50','100','125','250']
      FreqChoices        = ['units','kHz','MHz']
      FreqUnitChoices    = ['MHz']
      VoltChoices        = ['units','Vpp','rms','dBm']
      RowOffset          = 0 
      ColumnOffset       = 0

      self.ChName        = ['S1','S2','S3','S4']

      self.ConfigFN      = ['off','short-coil','long-coil','cyl-probe','sph-probe','other']  

      # frame 
      # self = Tkinter.Frame(self,borderwidth=2,relief=Tkinter.GROOVE) 
      # self.pack()   

      # ----------------------------------------------------------------------------------
      # FPGA 
      # ID Label for pulse timing
      self.FPGATimingLabelVariable = Tkinter.StringVar()
      self.FPGATimingLabelVariable.set("FPGA Pulse Timing")
      self.FPGATimingLabel = Tkinter.Label(self,textvariable=self.FPGATimingLabelVariable,anchor="w",font="Helvetica 11 bold")
      self.FPGATimingLabel.grid(column=ColumnOffset+0,row=RowOffset+0) 
      # global on/off 
      self.GlobalOnOffLabelVariable = Tkinter.StringVar() 
      self.GlobalOnOffLabelVariable.set("Global On/Off") 
      GlobalOnOffLabel  = Tkinter.Label(self,textvariable=self.GlobalOnOffLabelVariable,anchor="w") 
      GlobalOnOffLabel.grid(column=ColumnOffset+0,row=RowOffset+1) 
      # check box 
      self.chk_global_on_off_var = Tkinter.IntVar() 
      self.chk_global_on_off     = Tkinter.Checkbutton(self,text="On",variable=self.chk_global_on_off_var) 
      self.chk_global_on_off.grid(column=ColumnOffset+1,row=RowOffset+1) 

      # channel label (header)  
      self.ChHeaderLabelVariable = Tkinter.StringVar() 
      self.ChHeaderLabelVariable.set("Channel") 
      ChHeaderLabel  = Tkinter.Label(self,textvariable=self.ChHeaderLabelVariable,anchor="w") 
      ChHeaderLabel.grid(column=ColumnOffset+0,row=RowOffset+2) 

      # configuration label 
      self.ConfigLabelVariable = Tkinter.StringVar() 
      self.ConfigLabelVariable.set("Configuration") 
      ConfigLabel  = Tkinter.Label(self,textvariable=self.ConfigLabelVariable,anchor="w") 
      ConfigLabel.grid(column=ColumnOffset+1,row=RowOffset+2) 

      self.UnitName = Tkinter.StringVar() 
      self.UnitName.set("Units") 

      # Channel Label (ports 1--4 on mechanical switch) 
      # 1 
      self.ChLabelVariable1 = Tkinter.StringVar() 
      self.ChLabelVariable1.set(self.ChName[0]) 
      ChLabel1  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable1,anchor="w") 
      ChLabel1.grid(column=ColumnOffset+0,row=RowOffset+3) 
      # 2 
      self.ChLabelVariable2 = Tkinter.StringVar() 
      self.ChLabelVariable2.set(self.ChName[1]) 
      ChLabel2  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable2,anchor="w") 
      ChLabel2.grid(column=ColumnOffset+0,row=RowOffset+4) 
      # 3 
      self.ChLabelVariable3 = Tkinter.StringVar() 
      self.ChLabelVariable3.set(self.ChName[2]) 
      ChLabel3  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable3,anchor="w") 
      ChLabel3.grid(column=ColumnOffset+0,row=RowOffset+5) 
      # 4 
      self.ChLabelVariable4 = Tkinter.StringVar() 
      self.ChLabelVariable4.set(self.ChName[3]) 
      ChLabel4  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable4,anchor="w") 
      ChLabel4.grid(column=ColumnOffset+0,row=RowOffset+6)

      # Mech Switch Offset Label  
      self.MechSwOffsetName = Tkinter.StringVar() 
      self.MechSwOffsetName.set("MSW Off.") 
      MechSwOffsetLabel  = Tkinter.Label(self,textvariable=self.MechSwOffsetName,anchor="w") 
      MechSwOffsetLabel.grid(column=ColumnOffset+2,row=RowOffset+2) 
      # Mech Switch Duration Label 
      self.MechSwDurationName = Tkinter.StringVar() 
      self.MechSwDurationName.set("MSW Dur.") 
      MechSwDurationLabel  = Tkinter.Label(self,textvariable=self.MechSwDurationName,anchor="w") 
      MechSwDurationLabel.grid(column=ColumnOffset+3,row=RowOffset+2) 
      # Mech Switch Unit Label 
      MechSwUnitLabel  = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      MechSwUnitLabel.grid(column=ColumnOffset+4,row=RowOffset+2) 

      # RF Transmit Offset Label  
      self.RFTransOffsetName = Tkinter.StringVar() 
      self.RFTransOffsetName.set("RFT Off.") 
      RFTransOffsetLabel  = Tkinter.Label(self,textvariable=self.RFTransOffsetName,anchor="w") 
      RFTransOffsetLabel.grid(column=ColumnOffset+5,row=RowOffset+2) 
      # RF Transmit Duration Label 
      self.RFTransDurationName = Tkinter.StringVar() 
      self.RFTransDurationName.set("RFT Dur.") 
      RFTransDurationLabel  = Tkinter.Label(self,textvariable=self.RFTransDurationName,anchor="w") 
      RFTransDurationLabel.grid(column=ColumnOffset+6,row=RowOffset+2) 
      # RF Transmit Unit Label 
      RFTransUnitLabel  = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      RFTransUnitLabel.grid(column=ColumnOffset+7,row=RowOffset+2) 

      # Tomco Offset Label  
      self.TomcoOffsetName = Tkinter.StringVar() 
      self.TomcoOffsetName.set("Tom Off.") 
      TomcoOffsetLabel  = Tkinter.Label(self,textvariable=self.TomcoOffsetName,anchor="w") 
      TomcoOffsetLabel.grid(column=ColumnOffset+8,row=RowOffset+2) 
      # Tomco Duration Label 
      self.TomcoDurationName = Tkinter.StringVar() 
      self.TomcoDurationName.set("Tom Dur.") 
      TomcoDurationLabel  = Tkinter.Label(self,textvariable=self.TomcoDurationName,anchor="w") 
      TomcoDurationLabel.grid(column=ColumnOffset+9,row=RowOffset+2) 
      # Tomco Unit Label 
      TomcoUnitLabel  = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      TomcoUnitLabel.grid(column=ColumnOffset+10,row=RowOffset+2) 
      # Tomco Enable Label
      self.TomcoEnable = Tkinter.StringVar()
      self.TomcoEnable.set("Tom Enbl")  
      TomcoEnableLabel  = Tkinter.Label(self,width=7,textvariable=self.TomcoEnable,anchor="w") 
      TomcoEnableLabel.grid(column=ColumnOffset+11,row=RowOffset+2) 

      # RF Receive Offset Label  
      self.RFRecOffsetName = Tkinter.StringVar() 
      self.RFRecOffsetName.set("RFR Off.") 
      RFRecOffsetLabel  = Tkinter.Label(self,textvariable=self.RFRecOffsetName,anchor="w") 
      RFRecOffsetLabel.grid(column=ColumnOffset+12,row=RowOffset+2) 
      # RF Receive Duration Label 
      self.RFRecDurationName = Tkinter.StringVar() 
      self.RFRecDurationName.set("RFR Dur.") 
      RFRecDurationLabel  = Tkinter.Label(self,textvariable=self.RFRecDurationName,anchor="w") 
      RFRecDurationLabel.grid(column=ColumnOffset+13,row=RowOffset+2) 
      # RF Receive Unit Label 
      RFRecUnitLabel  = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      RFRecUnitLabel.grid(column=ColumnOffset+14,row=RowOffset+2) 

      cnf_1 = Tkinter.StringVar() 
      cnf_2 = Tkinter.StringVar() 
      cnf_3 = Tkinter.StringVar() 
      cnf_4 = Tkinter.StringVar()
      self.cnfCh = [cnf_1,cnf_2,cnf_3,cnf_4]

      for element in self.cnfCh: 
         element.set('OFF') 
 
      # pull down menu: probe configs 
      # 1 
      self.opt_conf_ch1 = Tkinter.OptionMenu(self,self.cnfCh[0],*self.ConfigChoices) 
      self.opt_conf_ch1.grid(column=ColumnOffset+1,row=RowOffset+3)  
      # 2 
      self.opt_conf_ch2 = Tkinter.OptionMenu(self,self.cnfCh[1],*self.ConfigChoices) 
      self.opt_conf_ch2.grid(column=ColumnOffset+1,row=RowOffset+4)  
      # 3 
      self.opt_conf_ch3 = Tkinter.OptionMenu(self,self.cnfCh[2],*self.ConfigChoices) 
      self.opt_conf_ch3.grid(column=ColumnOffset+1,row=RowOffset+5)  
      # 4 
      self.opt_conf_ch4 = Tkinter.OptionMenu(self,self.cnfCh[3],*self.ConfigChoices) 
      self.opt_conf_ch4.grid(column=ColumnOffset+1,row=RowOffset+6)  

      # entry fields (offset)  
      # Mech 1 
      mech_off_str_1 = Tkinter.StringVar()
      mech_off_str_2 = Tkinter.StringVar()
      mech_off_str_3 = Tkinter.StringVar()
      mech_off_str_4 = Tkinter.StringVar()
      self.entryMechOffVariable   = [mech_off_str_1,mech_off_str_2,mech_off_str_3,mech_off_str_4] 
      rf_trans_off_str_1 = Tkinter.StringVar()
      rf_trans_off_str_2 = Tkinter.StringVar()
      rf_trans_off_str_3 = Tkinter.StringVar()
      rf_trans_off_str_4 = Tkinter.StringVar()
      self.entryRFTransOffVariable   = [rf_trans_off_str_1,rf_trans_off_str_2,rf_trans_off_str_3,rf_trans_off_str_4] 
      rf_rec_off_str_1 = Tkinter.StringVar()
      rf_rec_off_str_2 = Tkinter.StringVar()
      rf_rec_off_str_3 = Tkinter.StringVar()
      rf_rec_off_str_4 = Tkinter.StringVar()
      self.entryRFRecOffVariable   = [rf_rec_off_str_1,rf_rec_off_str_2,rf_rec_off_str_3,rf_rec_off_str_4] 
      tomco_off_str_1 = Tkinter.StringVar()
      tomco_off_str_2 = Tkinter.StringVar()
      tomco_off_str_3 = Tkinter.StringVar()
      tomco_off_str_4 = Tkinter.StringVar()
      self.entryTomcoOffVariable   = [tomco_off_str_1,tomco_off_str_2,tomco_off_str_3,tomco_off_str_4] 
      mech_dur_str_1 = Tkinter.StringVar()
      mech_dur_str_2 = Tkinter.StringVar()
      mech_dur_str_3 = Tkinter.StringVar()
      mech_dur_str_4 = Tkinter.StringVar()
      self.entryMechDurVariable   = [mech_dur_str_1,mech_dur_str_2,mech_dur_str_3,mech_dur_str_4] 
      rf_trans_dur_str_1 = Tkinter.StringVar()
      rf_trans_dur_str_2 = Tkinter.StringVar()
      rf_trans_dur_str_3 = Tkinter.StringVar()
      rf_trans_dur_str_4 = Tkinter.StringVar()
      self.entryRFTransDurVariable   = [rf_trans_dur_str_1,rf_trans_dur_str_2,rf_trans_dur_str_3,rf_trans_dur_str_4] 
      rf_rec_dur_str_1 = Tkinter.StringVar()
      rf_rec_dur_str_2 = Tkinter.StringVar()
      rf_rec_dur_str_3 = Tkinter.StringVar()
      rf_rec_dur_str_4 = Tkinter.StringVar()
      self.entryRFRecDurVariable   = [rf_rec_dur_str_1,rf_rec_dur_str_2,rf_rec_dur_str_3,rf_rec_dur_str_4] 
      tomco_dur_str_1 = Tkinter.StringVar()
      tomco_dur_str_2 = Tkinter.StringVar()
      tomco_dur_str_3 = Tkinter.StringVar()
      tomco_dur_str_4 = Tkinter.StringVar()
      self.entryTomcoDurVariable   = [tomco_dur_str_1,tomco_dur_str_2,tomco_dur_str_3,tomco_dur_str_4] 

      tomco_en_1 = Tkinter.IntVar()
      tomco_en_2 = Tkinter.IntVar()
      tomco_en_3 = Tkinter.IntVar()
      tomco_en_4 = Tkinter.IntVar()
      self.chkTomcoEnableVariable = [tomco_en_1,tomco_en_2,tomco_en_3,tomco_en_4] 

      for element in self.chkTomcoEnableVariable:
         element.set(0)
      for element in self.entryMechOffVariable: 
         element.set(u"Offset") 
      for element in self.entryMechDurVariable: 
         element.set(u"Duration") 
      for element in self.entryRFTransOffVariable: 
         element.set(u"Offset") 
      for element in self.entryRFTransDurVariable: 
         element.set(u"Duration") 
      for element in self.entryTomcoOffVariable: 
         element.set(u"Offset") 
      for element in self.entryTomcoDurVariable: 
         element.set(u"Duration") 
      for element in self.entryRFRecOffVariable: 
         element.set(u"Offset") 
      for element in self.entryRFRecDurVariable: 
         element.set(u"Duration") 

      # units     
      unit_mech1 = Tkinter.StringVar() 
      unit_mech2 = Tkinter.StringVar() 
      unit_mech3 = Tkinter.StringVar() 
      unit_mech4 = Tkinter.StringVar() 
      self.unitMech = [unit_mech1,unit_mech2,unit_mech3,unit_mech4] 
      unit_rft1 = Tkinter.StringVar() 
      unit_rft2 = Tkinter.StringVar() 
      unit_rft3 = Tkinter.StringVar() 
      unit_rft4 = Tkinter.StringVar() 
      self.unitRFTrans = [unit_rft1,unit_rft2,unit_rft3,unit_rft4] 
      unit_tom1 = Tkinter.StringVar() 
      unit_tom2 = Tkinter.StringVar() 
      unit_tom3 = Tkinter.StringVar() 
      unit_tom4 = Tkinter.StringVar() 
      self.unitTomco = [unit_tom1,unit_tom2,unit_tom3,unit_tom4] 
      unit_rfr1 = Tkinter.StringVar() 
      unit_rfr2 = Tkinter.StringVar() 
      unit_rfr3 = Tkinter.StringVar() 
      unit_rfr4 = Tkinter.StringVar() 
      self.unitRFRec = [unit_rfr1,unit_rfr2,unit_rfr3,unit_rfr4]

      for element in self.unitMech: 
         element.set("s") 
      for element in self.unitRFTrans: 
         element.set("s") 
      for element in self.unitTomco: 
         element.set("s") 
      for element in self.unitRFRec: 
         element.set("s") 
 
      # entry fields
      # 1 
      # offsets  
      self.entryMechOff1    = Tkinter.Entry(self,width=7,textvariable=self.entryMechOffVariable[0]) 
      self.entryMechOff1.grid(column=ColumnOffset+2,row=RowOffset+3,sticky='EW')
      self.entryRFTransOff1 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransOffVariable[0]) 
      self.entryRFTransOff1.grid(column=ColumnOffset+5,row=RowOffset+3,sticky='EW')
      self.entryTomcoOff1   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoOffVariable[0]) 
      self.entryTomcoOff1.grid(column=ColumnOffset+8,row=RowOffset+3,sticky='EW')
      self.entryRFRecOff1   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecOffVariable[0]) 
      self.entryRFRecOff1.grid(column=ColumnOffset+12,row=RowOffset+3,sticky='EW')
      # durations 
      self.entryMechDur1    = Tkinter.Entry(self,width=7,textvariable=self.entryMechDurVariable[0]) 
      self.entryMechDur1.grid(column=ColumnOffset+3,row=RowOffset+3,sticky='EW')
      self.entryRFTransDur1 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransDurVariable[0]) 
      self.entryRFTransDur1.grid(column=ColumnOffset+6,row=RowOffset+3,sticky='EW')
      self.entryTomcoDur1   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoDurVariable[0]) 
      self.entryTomcoDur1.grid(column=ColumnOffset+9,row=RowOffset+3,sticky='EW')
      self.entryRFRecDur1   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecDurVariable[0]) 
      self.entryRFRecDur1.grid(column=ColumnOffset+13,row=RowOffset+3,sticky='EW')
      # check box 
      self.chkTomcoEnable1     = Tkinter.Checkbutton(self,variable=self.chkTomcoEnableVariable[0]) 
      self.chkTomcoEnable1.grid(column=ColumnOffset+11,row=RowOffset+3) 
      # units 
      self.opt_mech1 = Tkinter.OptionMenu(self,self.unitMech[0],*self.TimeChoices) 
      self.opt_mech1.grid(column=ColumnOffset+4,row=RowOffset+3)  
      self.opt_rft1 = Tkinter.OptionMenu(self,self.unitRFTrans[0],*self.TimeChoices) 
      self.opt_rft1.grid(column=ColumnOffset+7,row=RowOffset+3)  
      self.opt_tom1 = Tkinter.OptionMenu(self,self.unitTomco[0],*self.TimeChoices) 
      self.opt_tom1.grid(column=ColumnOffset+10,row=RowOffset+3)  
      self.opt_rfr1 = Tkinter.OptionMenu(self,self.unitRFRec[0],*self.TimeChoices) 
      self.opt_rfr1.grid(column=ColumnOffset+14,row=RowOffset+3)  
      # 2 
      # offsets  
      self.entryMechOff2    = Tkinter.Entry(self,width=7,textvariable=self.entryMechOffVariable[1]) 
      self.entryMechOff2.grid(column=ColumnOffset+2,row=RowOffset+4,sticky='EW')
      self.entryRFTransOff2 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransOffVariable[1]) 
      self.entryRFTransOff2.grid(column=ColumnOffset+5,row=RowOffset+4,sticky='EW')
      self.entryTomcoOff2   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoOffVariable[1]) 
      self.entryTomcoOff2.grid(column=ColumnOffset+8,row=RowOffset+4,sticky='EW')
      self.entryRFRecOff2   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecOffVariable[1]) 
      self.entryRFRecOff2.grid(column=ColumnOffset+12,row=RowOffset+4,sticky='EW')
      # durations 
      self.entryMechDur2    = Tkinter.Entry(self,width=7,textvariable=self.entryMechDurVariable[1]) 
      self.entryMechDur2.grid(column=ColumnOffset+3,row=RowOffset+4,sticky='EW')
      self.entryRFTransDur2 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransDurVariable[1]) 
      self.entryRFTransDur2.grid(column=ColumnOffset+6,row=RowOffset+4,sticky='EW')
      self.entryTomcoDur2   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoDurVariable[1]) 
      self.entryTomcoDur2.grid(column=ColumnOffset+9,row=RowOffset+4,sticky='EW')
      self.entryRFRecDur2   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecDurVariable[1]) 
      self.entryRFRecDur2.grid(column=ColumnOffset+13,row=RowOffset+4,sticky='EW')
      # check box 
      self.chkTomcoEnable2     = Tkinter.Checkbutton(self,variable=self.chkTomcoEnableVariable[1]) 
      self.chkTomcoEnable2.grid(column=ColumnOffset+11,row=RowOffset+4) 
      # units 
      self.opt_mech2 = Tkinter.OptionMenu(self,self.unitMech[1],*self.TimeChoices) 
      self.opt_mech2.grid(column=ColumnOffset+4,row=RowOffset+4)  
      self.opt_rft2 = Tkinter.OptionMenu(self,self.unitRFTrans[1],*self.TimeChoices) 
      self.opt_rft2.grid(column=ColumnOffset+7,row=RowOffset+4)  
      self.opt_tom2 = Tkinter.OptionMenu(self,self.unitTomco[1],*self.TimeChoices) 
      self.opt_tom2.grid(column=ColumnOffset+10,row=RowOffset+4)  
      self.opt_rfr2 = Tkinter.OptionMenu(self,self.unitRFRec[1],*self.TimeChoices) 
      self.opt_rfr2.grid(column=ColumnOffset+14,row=RowOffset+4)  
      # 3
      # offsets  
      self.entryMechOff3    = Tkinter.Entry(self,width=7,textvariable=self.entryMechOffVariable[2]) 
      self.entryMechOff3.grid(column=ColumnOffset+2,row=RowOffset+5,sticky='EW')
      self.entryRFTransOff3 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransOffVariable[2]) 
      self.entryRFTransOff3.grid(column=ColumnOffset+5,row=RowOffset+5,sticky='EW')
      self.entryTomcoOff3   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoOffVariable[2]) 
      self.entryTomcoOff3.grid(column=ColumnOffset+8,row=RowOffset+5,sticky='EW')
      self.entryRFRecOff3   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecOffVariable[2]) 
      self.entryRFRecOff3.grid(column=ColumnOffset+12,row=RowOffset+5,sticky='EW')
      # durations 
      self.entryMechDur3    = Tkinter.Entry(self,width=7,textvariable=self.entryMechDurVariable[2]) 
      self.entryMechDur3.grid(column=ColumnOffset+3,row=RowOffset+5,sticky='EW')
      self.entryRFTransDur3 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransDurVariable[2]) 
      self.entryRFTransDur3.grid(column=ColumnOffset+6,row=RowOffset+5,sticky='EW')
      self.entryTomcoDur3   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoDurVariable[2]) 
      self.entryTomcoDur3.grid(column=ColumnOffset+9,row=RowOffset+5,sticky='EW')
      self.entryRFRecDur3   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecDurVariable[2]) 
      self.entryRFRecDur3.grid(column=ColumnOffset+13,row=RowOffset+5,sticky='EW')
      # check box 
      self.chkTomcoEnable3     = Tkinter.Checkbutton(self,variable=self.chkTomcoEnableVariable[2]) 
      self.chkTomcoEnable3.grid(column=ColumnOffset+11,row=RowOffset+5) 
      # units 
      self.opt_mech3 = Tkinter.OptionMenu(self,self.unitMech[2],*self.TimeChoices) 
      self.opt_mech3.grid(column=ColumnOffset+4,row=RowOffset+5)  
      self.opt_rft3 = Tkinter.OptionMenu(self,self.unitRFTrans[2],*self.TimeChoices) 
      self.opt_rft3.grid(column=ColumnOffset+7,row=RowOffset+5)  
      self.opt_tom3 = Tkinter.OptionMenu(self,self.unitTomco[2],*self.TimeChoices) 
      self.opt_tom3.grid(column=ColumnOffset+10,row=RowOffset+5)  
      self.opt_rfr3 = Tkinter.OptionMenu(self,self.unitRFRec[2],*self.TimeChoices) 
      self.opt_rfr3.grid(column=ColumnOffset+14,row=RowOffset+5)  
      # 4
      # offsets  
      self.entryMechOff4    = Tkinter.Entry(self,width=7,textvariable=self.entryMechOffVariable[3]) 
      self.entryMechOff4.grid(column=ColumnOffset+2,row=RowOffset+6,sticky='EW')
      self.entryRFTransOff4 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransOffVariable[3]) 
      self.entryRFTransOff4.grid(column=ColumnOffset+5,row=RowOffset+6,sticky='EW')
      self.entryTomcoOff4   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoOffVariable[3]) 
      self.entryTomcoOff4.grid(column=ColumnOffset+8,row=RowOffset+6,sticky='EW')
      self.entryRFRecOff4   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecOffVariable[3]) 
      self.entryRFRecOff4.grid(column=ColumnOffset+12,row=RowOffset+6,sticky='EW')
      # durations 
      self.entryMechDur4    = Tkinter.Entry(self,width=7,textvariable=self.entryMechDurVariable[3]) 
      self.entryMechDur4.grid(column=ColumnOffset+3,row=RowOffset+6,sticky='EW')
      self.entryRFTransDur4 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransDurVariable[3]) 
      self.entryRFTransDur4.grid(column=ColumnOffset+6,row=RowOffset+6,sticky='EW')
      self.entryTomcoDur4   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoDurVariable[3]) 
      self.entryTomcoDur4.grid(column=ColumnOffset+9,row=RowOffset+6,sticky='EW')
      self.entryRFRecDur4   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecDurVariable[3]) 
      self.entryRFRecDur4.grid(column=ColumnOffset+13,row=RowOffset+6,sticky='EW')
      # check box 
      self.chkTomcoEnable4     = Tkinter.Checkbutton(self,variable=self.chkTomcoEnableVariable[3]) 
      self.chkTomcoEnable4.grid(column=ColumnOffset+11,row=RowOffset+6) 
      # units 
      self.opt_mech4 = Tkinter.OptionMenu(self,self.unitMech[3],*self.TimeChoices) 
      self.opt_mech4.grid(column=ColumnOffset+4,row=RowOffset+6)  
      self.opt_rft4 = Tkinter.OptionMenu(self,self.unitRFTrans[3],*self.TimeChoices) 
      self.opt_rft4.grid(column=ColumnOffset+7,row=RowOffset+6)  
      self.opt_tom4 = Tkinter.OptionMenu(self,self.unitTomco[3],*self.TimeChoices) 
      self.opt_tom4.grid(column=ColumnOffset+10,row=RowOffset+6)  
      self.opt_rfr4 = Tkinter.OptionMenu(self,self.unitRFRec[3],*self.TimeChoices) 
      self.opt_rfr4.grid(column=ColumnOffset+14,row=RowOffset+6)  

      # ----------------------------------------------------------------------------------
      # function generator 
      # ID Label 
      self.FGLabelVariable = Tkinter.StringVar()
      self.FGLabelVariable.set("LO Settings")
      self.FGLabel = Tkinter.Label(self,textvariable=self.FGLabelVariable,anchor="w",font = "Helvetica 11 bold")
      self.FGLabel.grid(column=ColumnOffset+0,row=RowOffset+8) 
      # Labels  
      self.FGFreqLabelVariable = Tkinter.StringVar()
      self.FGFreqLabelVariable.set("Frequency")
      self.FGFreqLabel = Tkinter.Label(self,textvariable=self.FGFreqLabelVariable,anchor="w") 
      self.FGFreqLabel.grid(column=ColumnOffset+0,row=RowOffset+9) 
      self.FGBNCLabelVariable = Tkinter.StringVar()
      self.FGBNCLabelVariable.set("BNC")
      self.FGBNCLabel = Tkinter.Label(self,textvariable=self.FGBNCLabelVariable,anchor="w") 
      self.FGBNCLabel.grid(column=ColumnOffset+0,row=RowOffset+10)  
      self.FGNTypeLabelVariable = Tkinter.StringVar()
      self.FGNTypeLabelVariable.set("N-Type")
      self.FGNTypeLabel = Tkinter.Label(self,textvariable=self.FGNTypeLabelVariable,anchor="w") 
      self.FGNTypeLabel.grid(column=ColumnOffset+0,row=RowOffset+11) 

      # entry fields 
      # frequency 
      self.entryFGFreqVariable = Tkinter.StringVar() 
      self.entryFGFreqVariable.set(u"Frequency")
      self.entryFGFreq = Tkinter.Entry(self,width=7,textvariable=self.entryFGFreqVariable)
      self.entryFGFreq.grid(column=ColumnOffset+1,row=RowOffset+9,sticky='EW') 
      # BNC 
      self.entryFGBNCVoltVariable = Tkinter.StringVar() 
      self.entryFGBNCVoltVariable.set(u"Voltage")
      self.entryFGBNCVolt = Tkinter.Entry(self,width=7,textvariable=self.entryFGBNCVoltVariable)
      self.entryFGBNCVolt.grid(column=ColumnOffset+1,row=RowOffset+10,sticky='EW')  
      # N-Type
      self.entryFGNTypeVoltVariable = Tkinter.StringVar() 
      self.entryFGNTypeVoltVariable.set(u"Voltage")
      self.entryFGNTypeVolt = Tkinter.Entry(self,width=7,textvariable=self.entryFGNTypeVoltVariable)
      self.entryFGNTypeVolt.grid(column=ColumnOffset+1,row=RowOffset+11,sticky='EW')  

      # pull down menus 
      # frequency 
      self.unit_str_freq_fg = Tkinter.StringVar() 
      self.unit_str_freq_fg.set("units")
      self.opt_freq_fg = Tkinter.OptionMenu(self,self.unit_str_freq_fg,*FreqChoices) 
      self.opt_freq_fg.grid(column=ColumnOffset+2,row=RowOffset+9)  
      # BNC voltage 
      self.unit_str_volt_fg_bnc = Tkinter.StringVar() 
      self.unit_str_volt_fg_bnc.set("units")
      self.opt_volt_fg_bnc = Tkinter.OptionMenu(self,self.unit_str_volt_fg_bnc,*VoltChoices) 
      self.opt_volt_fg_bnc.grid(column=ColumnOffset+2,row=RowOffset+10)  
      # N-Type voltage
      self.unit_str_volt_fg_ntype = Tkinter.StringVar() 
      self.unit_str_volt_fg_ntype.set("units")
      self.opt_volt_fg_ntype = Tkinter.OptionMenu(self,self.unit_str_volt_fg_ntype,*VoltChoices) 
      self.opt_volt_fg_ntype.grid(column=ColumnOffset+2,row=RowOffset+11)  
      # check boxes 
      self.chk_fg_bnc_var   = Tkinter.IntVar() 
      self.chk_fg_bnc   = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_fg_bnc_var) 
      self.chk_fg_bnc.grid(column=ColumnOffset+3,row=RowOffset+10) 
      self.chk_fg_ntype_var = Tkinter.IntVar() 
      self.chk_fg_ntype = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_fg_ntype_var) 
      self.chk_fg_ntype.grid(column=ColumnOffset+3,row=RowOffset+11)

      # ----------------------------------------------------------------------------------
      # ADC 
      # ID Label for ADC
      self.ADCLabelVariable = Tkinter.StringVar()
      self.ADCLabelVariable.set("Digitizer Settings")
      self.ADCLabel = Tkinter.Label(self,textvariable=self.ADCLabelVariable,anchor="w",font="Helvetica 11 bold")
      self.ADCLabel.grid(column=ColumnOffset+0,row=RowOffset+12) 
      # Struck ADC ID number 
      self.ADCNumLabelVariable = Tkinter.StringVar()
      self.ADCNumLabelVariable.set("Struck ID") 
      self.ADCNumLabel = Tkinter.Label(self,textvariable=self.ADCNumLabelVariable,anchor="w") 
      self.ADCNumLabel.grid(column=ColumnOffset+0,row=RowOffset+13) 
      # Struck ADC channel number 
      self.ADCchLabelVariable = Tkinter.StringVar()
      self.ADCchLabelVariable.set("Channel Number") 
      self.ADCchLabel = Tkinter.Label(self,textvariable=self.ADCchLabelVariable,anchor="w") 
      self.ADCchLabel.grid(column=ColumnOffset+0,row=RowOffset+14) 
      # number of pulses 
      self.ADCPulseLabelVariable = Tkinter.StringVar()
      self.ADCPulseLabelVariable.set("Number of Pulses") 
      self.ADCPulseLabel = Tkinter.Label(self,textvariable=self.ADCPulseLabelVariable,anchor="w") 
      self.ADCPulseLabel.grid(column=ColumnOffset+0,row=RowOffset+15) 
      # sampling frequency
      self.ADCFreqLabelVariable = Tkinter.StringVar()
      self.ADCFreqLabelVariable.set("Sampling Freq.") 
      self.ADCFreqLabel = Tkinter.Label(self,textvariable=self.ADCFreqLabelVariable,anchor="w") 
      self.ADCFreqLabel.grid(column=ColumnOffset+0,row=RowOffset+16) 

      # entry fields  
      # Struck ID number 
      self.entryADCNumVariable = Tkinter.StringVar() 
      self.entryADCNumVariable.set(u"Enter value") 
      self.entryADCNum = Tkinter.Entry(self,width=7,textvariable=self.entryADCNumVariable) 
      self.entryADCNum.grid(column=ColumnOffset+1,row=RowOffset+13,sticky='EW')     
      # Struck channel number 
      self.entryADCchVariable = Tkinter.StringVar() 
      self.entryADCchVariable.set(u"Enter value") 
      self.entryADCch = Tkinter.Entry(self,width=7,textvariable=self.entryADCchVariable) 
      self.entryADCch.grid(column=ColumnOffset+1,row=RowOffset+14,sticky='EW')     
      # number of pulses 
      self.entryADCPulseVariable = Tkinter.StringVar() 
      self.entryADCPulseVariable.set(u"Enter value") 
      self.entryADCPulse = Tkinter.Entry(self,width=7,textvariable=self.entryADCPulseVariable) 
      self.entryADCPulse.grid(column=ColumnOffset+1,row=RowOffset+15,sticky='EW') 

      # pull down menus
      # sampling frequency (value)  
      self.adc_freq_val = Tkinter.StringVar() 
      self.adc_freq_val.set('1')
      self.opt_adc_freq_val = Tkinter.OptionMenu(self,self.adc_freq_val,*FreqValues) 
      self.opt_adc_freq_val.grid(column=ColumnOffset+1,row=RowOffset+16)  
      # sampling frequency (unit)  
      self.unit_str_adc_freq = Tkinter.StringVar() 
      self.unit_str_adc_freq.set('MHz')
      self.opt_adc_freq = Tkinter.OptionMenu(self,self.unit_str_adc_freq,*FreqUnitChoices) 
      self.opt_adc_freq.grid(column=ColumnOffset+2,row=RowOffset+16)  
      # check boxes 
      self.chk_adc_clk_type_var = Tkinter.IntVar() 
      self.chk_adc_clk_type     = Tkinter.Checkbutton(self,text="External",variable=self.chk_adc_clk_type_var) 
      self.chk_adc_clk_type.grid(column=ColumnOffset+3,row=RowOffset+16) 

      # # ----------------------------------------------------------------------------------
      # Utilities 
      # header label  
      self.UtilLabelVariable = Tkinter.StringVar()
      self.UtilLabelVariable.set("Utilities")
      self.UtilLabel = Tkinter.Label(self,textvariable=self.UtilLabelVariable,anchor="w",font="Helvetica 11 bold")
      self.UtilLabel.grid(column=ColumnOffset+0,row=RowOffset+17) 
      # debug  
      self.UtilDebugLabelVariable = Tkinter.StringVar()
      self.UtilDebugLabelVariable.set("Debug Mode") 
      self.UtilDebugLabel = Tkinter.Label(self,textvariable=self.UtilDebugLabelVariable,anchor="w") 
      self.UtilDebugLabel.grid(column=ColumnOffset+0,row=RowOffset+18) 
      # verbosity  
      self.UtilVerbLabelVariable = Tkinter.StringVar()
      self.UtilVerbLabelVariable.set("Verbosity") 
      self.UtilVerbLabel = Tkinter.Label(self,textvariable=self.UtilVerbLabelVariable,anchor="w") 
      self.UtilVerbLabel.grid(column=ColumnOffset+0,row=RowOffset+19) 
      # test mode  
      self.UtilTestLabelVariable = Tkinter.StringVar()
      self.UtilTestLabelVariable.set("Test Mode") 
      self.UtilTestLabel = Tkinter.Label(self,textvariable=self.UtilTestLabelVariable,anchor="w") 
      self.UtilTestLabel.grid(column=ColumnOffset+0,row=RowOffset+20) 
      # reference frequency 
      self.UtilRefFreqLabelVariable = Tkinter.StringVar()
      self.UtilRefFreqLabelVariable.set("PTS160 Frequency") 
      self.UtilRefFreqLabel = Tkinter.Label(self,textvariable=self.UtilRefFreqLabelVariable,anchor="w") 
      self.UtilRefFreqLabel.grid(column=ColumnOffset+0,row=RowOffset+21) 
      # file name 
      self.UtilFNLabelVariable = Tkinter.StringVar()
      self.UtilFNLabelVariable.set("Configuration Label") 
      self.UtilFNLabel = Tkinter.Label(self,textvariable=self.UtilFNLabelVariable,anchor="w") 
      self.UtilFNLabel.grid(column=ColumnOffset+0,row=RowOffset+23) 

      # entry fields  
      # reference frequency 
      self.entryUtilRefFreqVariable = Tkinter.StringVar() 
      self.entryUtilRefFreqVariable.set(u"Frequency") 
      self.entryUtilRefFreq = Tkinter.Entry(self,width=10,textvariable=self.entryUtilRefFreqVariable) 
      self.entryUtilRefFreq.grid(column=ColumnOffset+1,row=RowOffset+21,sticky='EW')     

      # comments field 
      self.CommentsLabelVariable = Tkinter.StringVar() 
      self.CommentsLabelVariable.set("Comments") 
      self.CommentsLabel = Tkinter.Label(self,textvariable=self.CommentsLabelVariable,anchor="w") 
      self.CommentsLabel.grid(column=ColumnOffset+0,row=RowOffset+22) 

      self.entryCommentsVar = Tkinter.StringVar() 
      self.entryCommentsVar.set(u"Run comments") 
      self.entryComments    = Tkinter.Entry(self,textvariable=self.entryCommentsVar) 
                                       # anchor="w",fg="black",bg="white") 
      self.entryComments.grid(column=ColumnOffset+1,row=RowOffset+22,columnspan=4,sticky='EW') 

      # file name  
      self.entryUtilFNVar = Tkinter.StringVar() 
      self.entryUtilFNVar.set(u"Label (no spaces)")
      self.entryUtilFN    = Tkinter.Entry(self,textvariable=self.entryUtilFNVar)
      self.entryUtilFN.grid(column=ColumnOffset+1,row=RowOffset+23,columnspan=4,sticky='EW') 

      # pull down menus
      # debug mode  
      self.util_debug_val = Tkinter.StringVar() 
      self.util_debug_val.set('off')
      self.opt_util_debug_val = Tkinter.OptionMenu(self,self.util_debug_val,*self.DebugChoices) 
      self.opt_util_debug_val.grid(column=ColumnOffset+1,row=RowOffset+18)  
      # verbosity    
      self.util_verb_val = Tkinter.StringVar() 
      self.util_verb_val.set('0')
      self.opt_util_verb_val = Tkinter.OptionMenu(self,self.util_verb_val,*self.VerbChoices) 
      self.opt_util_verb_val.grid(column=ColumnOffset+1,row=RowOffset+19)  
      # test mode    
      self.util_test_val = Tkinter.StringVar() 
      self.util_test_val.set('0')
      self.opt_test_val = Tkinter.OptionMenu(self,self.util_test_val,*self.TestChoices) 
      self.opt_test_val.grid(column=ColumnOffset+1,row=RowOffset+20)  
      # ref frequency units    
      self.util_ref_freq_unit_val = Tkinter.StringVar() 
      self.util_ref_freq_unit_val.set('units')
      self.opt_ref_freq_val = Tkinter.OptionMenu(self,self.util_ref_freq_unit_val,*FreqChoices) 
      self.opt_ref_freq_val.grid(column=ColumnOffset+2,row=RowOffset+21)  

      # ----------------------------------------------------------------------------------
      # make buttons 

      # load config 
      self.buttonImportConfig = Tkinter.Button(self,text=u"Import Configuration...",command=self.ImportConfig) 
      self.buttonImportConfig.grid(column=ColumnOffset+11,row=RowOffset+21,columnspan=2,sticky='EW') 
      # print config to files  
      self.buttonConfig = Tkinter.Button(self,text=u"Apply Configuration",command=self.PrintToFile) 
      self.buttonConfig.grid(column=ColumnOffset+13,row=RowOffset+21,columnspan=2,sticky='EW',) 
      # run  
      self.buttonRun = Tkinter.Button(self,text=u"Run",command=self.CheckHardware) 
      self.buttonRun.grid(column=ColumnOffset+11,row=RowOffset+22,columnspan=4,sticky='EW') 

      # turn off FPGA and function generator  
      # self.buttonStop = Tkinter.Button(self,text=u"Stop") 
      # self.buttonStop.grid(column=ColumnOffset+5,row=RowOffset+19,columnspan=1,sticky='EW') 

      self.buttonPlot = Tkinter.Button(self,text=u"Plot Data",command=self.UpdatePlots) 
      self.buttonPlot.grid(column=ColumnOffset+11,row=RowOffset+23,columnspan=2,sticky='EW') 

      # quit the program 
      self.buttonQuit = Tkinter.Button(self,text=u"Quit",command=quit) 
      self.buttonQuit.grid(column=ColumnOffset+13,row=RowOffset+23,columnspan=2,sticky='EW') 
      # ----------------------------------------------------------------------------------
      # status field  
      description        = "MSW = Mechanical Switch Gate; RFT = RF Transmit Gate; RFR = RF Receive Gate; Tom = Tomco; Off = Offset; Dur = Duration"
      self.StatusVariable = Tkinter.StringVar() 
      self.StatusVariable.set(u"Fill out the fields above before clicking 'Apply Configuration' and then 'Run.'  Be sure your entries are correct.") 
      self.Status  = Tkinter.Label(self,textvariable=self.StatusVariable,
                                   anchor="w",fg="white",bg="blue") 
      self.Status.grid(column=ColumnOffset+0,row=RowOffset+24,columnspan=15,sticky='ew') 
      # ----------------------------------------------------------------------------------
      # sizing details 
      self.grid_columnconfigure(0,weight=1)  
      # set so that you can only resize horizontally 
      # self.resizable(True,False)  # horizontal resizing 
      # self.resizable(False,True)  # vertical resizing 
      # self.resizable(True,True)   # horizontal and vertical resizing 
      self.resizable(False,False)   # no resizing  
      # restrict window size to one size 
      self.update()
      self.geometry(self.geometry()) 
      # auto-select the text field upon an event 
      # self.entry.focus_set()
      # self.entry.selection_range(0,Tkinter.END) 
      # ----------------------------------------------------------------------------------

      # ----------------------------------------------------------------------------------
      # event actions
   def CheckHardware(self): 
       self.CHWindow = Tkinter.Toplevel(self)
       Message       = Tkinter.Label(self.CHWindow, text="Is everything powered on correctly?\nClick Yes to start the run.")
       Message.grid(column=0,row=0,sticky='ew') 
       # some buttons 
       yesButton     = Tkinter.Button(self.CHWindow, text='Yes',command=self.RunDAQ)
       yesButton.grid(column=0,row=2,columnspan=1) 
       # quit child window and return to root window
       # the button is optional here, simply use the corner x of the child window
       noButton = Tkinter.Button(self.CHWindow, text='No', command=self.CHWindow.destroy) 
       noButton.grid(column=0,row=3,columnspan=1) 
   def ImportConfig(self): 
       self.counter += 1
       self.LCWindow = Tkinter.Toplevel(self)
       self.LCWindow.wm_title("Import Configuration")
       Label = Tkinter.Label(self.LCWindow,text="Configuration",anchor="w") 
       Label.grid(column=0,row=1) 

       self.ConfVar = Tkinter.StringVar() 
       self.ConfVar.set("a-config.cnf") 
       entryConf = Tkinter.Entry(self.LCWindow,textvariable=self.ConfVar); 
       entryConf.grid(column=1,row=1)  

       # button to go get a file  
       confButton = Tkinter.Button(self.LCWindow, text='Choose file...', command=self.GetFileNameConf)
       confButton.grid(column=2,row=1,columnspan=1) 

       # some buttons 
       loadButton = Tkinter.Button(self.LCWindow, text='Load Data', command=self.LoadData)
       loadButton.grid(column=0,row=5,columnspan=3) 
       # quit child window and return to root window
       # the button is optional here, simply use the corner x of the child window
       closeButton = Tkinter.Button(self.LCWindow, text='Close Window', command=self.LCWindow.destroy) 
       closeButton.grid(column=0,row=6,columnspan=3) 
       # # update status banner 
       # self.StatusVariable.set("Importing configuration...") 
   def GetFileNameConf(self):
       initdir = self.MyHOME + "input/configs/" 
       fn = askopenfilename(initialdir=initdir) # show an "Open" dialog box and return the path to the file
       self.ConfVar.set(fn) 
   def GetFileNameFPGA(self):
       initdir = self.MyHOME + "input/configs/" 
       fn = askopenfilename(initialdir=initdir) # show an "Open" dialog box and return the path to the file
       self.fpgaFNVar.set(fn) 
   def GetFileNameFG(self):
       initdir = self.MyHOME + "input/configs/" 
       fn = askopenfilename(initialdir=initdir) # show an "Open" dialog box and return the path to the file
       self.fgFNVar.set(fn) 
   def GetFileNameADC(self):
       initdir = self.MyHOME + "input/configs/" 
       fn = askopenfilename(initialdir=initdir) # show an "Open" dialog box and return the path to the file
       self.adcFNVar.set(fn) 
   def GetFileNameUtil(self):
       initdir = self.MyHOME + "input/configs/" 
       fn = askopenfilename(initialdir=initdir) # show an "Open" dialog box and return the path to the file
       self.utilFNVar.set(fn) 
   def LoadData(self): 
       self.StatusVariable.set("Loading configuration...") 
       fn = self.ConfVar.get() 
       fileConf = open(fn,'r') 
       config_name = fileConf.readline().strip() 
       # print "%s" %(config_name) 
       fileConf.close() 
       self.ConfVarLabel.set(config_name) 
       self.StatusVariable.set("Loading configuration...") 
       # general vars 
       eof   = "end_of_file" 
       start = "#"
       global_on_off = "global_on_off"
       fn_prefix = self.MyHOME + "input/configs/files/" 

       # fpga  
       mech_sw_1  = "mech_sw_1"  
       mech_sw_2  = "mech_sw_2"  
       mech_sw_3  = "mech_sw_3"  
       mech_sw_4  = "mech_sw_4" 
       rf_sw_1    = "rf_sw_1" 
       rf_sw_2    = "rf_sw_2" 
       rf_sw_3    = "rf_sw_3" 
       rf_gate    = "rf_gate"
       # fn = self.fpgaFNVar.get() 
       fn = fn_prefix + "pulse-data_" + self.ConfVarLabel.get() + ".dat" 
       fileFPGA = open(fn, 'r')
       for line in fileFPGA:
          entry = line.split() # puts every entry of a line in an array called entry
          # print entry 
          if entry[0]!=start and entry[0]!=eof: 
             if entry[0]==global_on_off: 
                if entry[1]=="on":  self.chk_global_var.set(1) 
                if entry[1]=="off": self.chk_global_var.set(0) 
             elif entry[0]==mech_sw_1: 
                self.entryMechOffsetVariable[0].set(entry[2])       
                self.entryMechDurationVariable[0].set(entry[3])      
                self.unit_str_mech[0].set(entry[4])
                if entry[1]=="on":  self.chk_var_mech[0].set(1)
                if entry[1]=="off": self.chk_var_mech[0].set(0)
             elif entry[0]==mech_sw_2: 
                self.entryMechOffsetVariable[1].set(entry[2])       
                self.entryMechDurationVariable[1].set(entry[3])      
                self.unit_str_mech[1].set(entry[4])
                self.chk_var_mech[1].set(entry[1])
                if entry[1]=="on":  self.chk_var_mech[1].set(1)
                if entry[1]=="off": self.chk_var_mech[1].set(0)
             elif entry[0]==mech_sw_3: 
                self.entryMechOffsetVariable[2].set(entry[2])       
                self.entryMechDurationVariable[2].set(entry[3])      
                self.unit_str_mech[2].set(entry[4])
                if entry[1]=="on":  self.chk_var_mech[2].set(1)
                if entry[1]=="off": self.chk_var_mech[2].set(0)
             elif entry[0]==mech_sw_4: 
                self.entryMechOffsetVariable[3].set(entry[2])       
                self.entryMechDurationVariable[3].set(entry[3])      
                self.unit_str_mech[3].set(entry[4])
                if entry[1]=="on":  self.chk_var_mech[3].set(1)
                if entry[1]=="off": self.chk_var_mech[3].set(0)
             if entry[0]==rf_sw_1: 
                self.entryRFOffsetVariable[0].set(entry[2])       
                self.entryRFDurationVariable[0].set(entry[3])      
                self.unit_str_rf[0].set(entry[4])
                if entry[1]=="on":  self.chk_var_rf[0].set(1)
                if entry[1]=="off": self.chk_var_rf[0].set(0)
             elif entry[0]==rf_sw_2: 
                self.entryRFOffsetVariable[1].set(entry[2])       
                self.entryRFDurationVariable[1].set(entry[3])      
                self.unit_str_rf[1].set(entry[4])
                if entry[1]=="on":  self.chk_var_rf[1].set(1)
                if entry[1]=="off": self.chk_var_rf[1].set(0)
             elif entry[0]==rf_sw_3: 
                self.entryRFOffsetVariable[2].set(entry[2])       
                self.entryRFDurationVariable[2].set(entry[3])      
                self.unit_str_rf[2].set(entry[4])
                if entry[1]=="on":  self.chk_var_rf[2].set(1)
                if entry[1]=="off": self.chk_var_rf[2].set(0)
             elif entry[0]==rf_gate:
                self.entryRFOffsetVariableGate.set(entry[2]) 
                self.entryRFDurationVariableGate.set(entry[3])  
                self.unit_str_rf_gate.set(entry[4])
                if entry[1]=="on":  self.chk_var_rf_gate.set(1)
                if entry[1]=="off": self.chk_var_rf_gate.set(0)
       fileFPGA.close()
       # function generator 
       bnc       = "bnc"
       ntype     = "ntype"
       freq      = "frequency"
       freq_val  = "0"
       freq_unit = "ND"
       volt_unit = "ND"
       # fn = self.fgFNVar.get() 
       fn = fn_prefix + "sg382_" + self.ConfVarLabel.get() + ".dat" 
       fileFG = open(fn, 'r')
       for line in fileFG:
          entry = line.split() # puts every entry of a line in an array called entry
          if entry[0]!=start and entry[0]!=eof: 
             if entry[0]==bnc: 
                self.entryFGBNCVoltVariable.set(entry[2]) 
                self.unit_str_volt_fg_bnc.set(entry[3]) 
                if entry[1]=="on":  self.chk_fg_bnc_var.set(1) 
                if entry[1]=="off": self.chk_fg_bnc_var.set(0) 
             elif entry[0]==ntype: 
                self.entryFGNTypeVoltVariable.set(entry[2]) 
                self.unit_str_volt_fg_ntype.set(entry[3]) 
                if entry[1]=="on":  self.chk_fg_ntype_var.set(1) 
                if entry[1]=="off": self.chk_fg_ntype_var.set(0) 
             elif entry[0]==freq: 
                freq_val  = entry[2]; 
                freq_unit = entry[3];  
       self.entryFGFreqVariable.set(freq_val)  
       self.unit_str_freq_fg.set(freq_unit) 
       fileFG.close() 
       # ADC 
       id_num        = "adc_id"
       ch_num        = "channel_number"
       nof           = "number_of_events"
       freq          = "frequency"
       ext_clk       = "external_clock"
       ext_freq_val  = 0
       ext_freq_unit = "ND"
       # fn = self.adcFNVar.get() 
       fn = fn_prefix + "struck_adc_" + self.ConfVarLabel.get() + ".dat" 
       fileADC = open(fn, 'r')
       for line in fileADC:
          entry = line.split() # puts every entry of a line in an array called entry
          if entry[0]!=start and entry[0]!=eof: 
             if entry[0]==id_num:  
                self.entryADCNumVariable.set(entry[1]) 
             elif entry[0]==ch_num: 
                self.entryADCchVariable.set(entry[1])
             elif entry[0]==nof: 
                self.entryADCPulseVariable.set(entry[1]) 
             elif entry[0]==freq: 
                freq_val      = entry[1]; 
                freq_unit     = entry[2]; 
             elif entry[0]==ext_clk: 
                ext_freq_val  = entry[1]; 
                ext_freq_unit = entry[2];
       if ext_freq_val>0: 
          self.adc_freq_val.set(ext_freq_val)
          self.unit_str_adc_freq.set(ext_freq_unit) 
          self.chk_adc_clk_type_var.set(1)
       elif ext_freq_val<=0: 
          self.adc_freq_val.set(freq_val)
          self.unit_str_adc_freq.set(freq_unit) 
          self.chk_adc_clk_type_var.set(0)
       fileADC.close() 
       # utilities 
       debug    = "debug_mode"
       verb     = "verbosity"
       test     = "test_mode"
       ref_freq = "rf_frequency"
       ref_freq_val = 0
       ref_freq_str = "0" 
       # fn = self.utilFNVar.get() 
       fn = fn_prefix + "utilities_" + self.ConfVarLabel.get() + ".dat" 
       fileUtil = open(fn, 'r')
       for line in fileUtil:
          entry = line.split() # puts every entry of a line in an array called entry
          # print entry 
          if entry[0]!=start and entry[0]!=eof: 
             if entry[0]==debug: 
                if entry[1]=="1": self.util_debug_val.set("on") 
                if entry[1]=="0": self.util_debug_val.set("off") 
             elif entry[0]==verb: 
                self.util_verb_val.set(entry[1]) 
             elif entry[0]==test: 
                self.util_test_val.set(entry[1]) 
             elif entry[0]==ref_freq: 
                ref_freq_val = float(entry[1]) 
       # convert to MHz (when would it ever not be?) 
       ref_freq_str = ref_freq_val/1E+6 
       self.entryUtilRefFreqVariable.set(ref_freq_str)  
       self.util_ref_freq_unit_val.set("MHz")   
       fileUtil.close()
       # update status bar 
       self.StatusVariable.set("Configuration loaded.") 
   def PrintToFile(self):   
       # init vars 
       fpga_header_1  = "# ID     on/off"
       fpga_header_2  = " mech-sw offset     mech-sw duration     units"
       fpga_header_3  = " rf-trans offset    rf-trans duration    units"
       fpga_header_4  = " tomco offset       tomco duration       units    tomco enable"
       fpga_header_5  = " rf-rec offset      rf-rec duration      units"
       fpga_header    = fpga_header_1 + fpga_header_2 + fpga_header_3 + fpga_header_4 + fpga_header_5 
       fg_header      = "# type   state      value           units"
       adc_header     = "# ID     value      units"
       util_header    = "# ID     value"
       EOF            = "end_of_file"
       ZERO           = "0" 
       NINETYNINE     = "99"
       MIN1           = "-1"
       blSTATE        = "--"
       ND             = "ND"
       global_str     = "none"  
       eof_fpga_str   = "%-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s  " %(NINETYNINE,blSTATE,ZERO,ZERO,ND,ZERO,ZERO,ND,ZERO,ZERO,ND,ZERO,ZERO,ZERO,ND) 
       eof_fg_str     = "%-20s %-20s %-20s %-20s" %(EOF,blSTATE,ZERO,ND)
       eof_adc_str    = "%-20s %-20s %-20s" %(EOF,ZERO,ND)
       eof_util_str   = "%-20s %-20s " %(EOF,NINETYNINE)

       # function generator 
       bnc_str         = self.GetFuncGenString(1)  
       ntype_str       = self.GetFuncGenString(2)  
       freq_str        = self.GetFuncGenString(3) 
       # ADC 
       adc_id_str      = self.GetADCString(1) 
       ch_num_str      = self.GetADCString(2) 
       pulse_num_str   = self.GetADCString(3) 
       sample_freq_str = self.GetADCString(4) 
       extern_clk_str  = self.GetADCString(6) 
       # utilities
       debug_str       = self.GetUtilString(1)  
       verb_str        = self.GetUtilString(2)  
       test_str        = self.GetUtilString(3)  
       rf_str          = self.GetUtilString(4)  
       # comments
       com_str          = self.entryCommentsVar.get() 
       # configurations 
       ChList = []
       util_filename = str( self.entryUtilFNVar.get() )  
       for i in xrange(1,4): 
          if self.cnfCh[i]!='OFF': 
              my_cnf_str = str( self.cnfCh[i].get() )
              ch_str = my_cnf_str + "_" + util_filename  
              ChList.append(ch_str) 
       # get file names ready  
       self.prefix    = "input/configs/files/"
       self.conf_fn   = self.entryUtilFNVar.get() + ".cnf"
       self.fpga_fn   = "pulse-data_" + self.entryUtilFNVar.get() + ".dat" # self.entryFPGAFNVar.get()
       self.fg_fn     = "sg382_"      + self.entryUtilFNVar.get() + ".dat" # self.entryFuncGenFNVar.get() 
       self.adc_fn    = "struck_adc_" + self.entryUtilFNVar.get() + ".dat" # self.entryADCFNVar.get()
       self.util_fn   = "utilities_"  + self.entryUtilFNVar.get() + ".dat" # self.entryADCFNVar.get()
       self.com_fn    = "comments_"   + self.entryUtilFNVar.get() + ".txt" 
       self.conf_path = self.MyHOME + "input/configs/" + self.conf_fn 
       self.fpga_path = "./" + self.prefix + self.fpga_fn 
       self.fg_path   = "./" + self.prefix + self.fg_fn 
       self.adc_path  = "./" + self.prefix + self.adc_fn 
       self.util_path = "./" + self.prefix + self.util_fn 
       self.com_path  = "./" + self.prefix + self.com_fn 
       # write to file 
       # configuration file
       if self.IsDebug==0:  
          confFile       = open(self.conf_path,"w") 
          conf_label = self.entryUtilFNVar.get() 
          confFile.write(conf_label +"\n")
          for i in xrange(1,4): 
             ch_str = self.ConfigFN[i].get() + "_" + self.entryUtilFNVar.get()  
             confFile.write(ch_str) 
          confFile.close() 
       elif self.IsDebug==1: 
          print self.conf_path 
          conf_label = self.entryUtilFNVar.get() 
          print conf_label 
          for i in xrange(1,4): 
               ch_str = self.ConfigFN[i].get() + "_" + self.entryUtilFNVar.get()  
               print ch_str 
       # fpga file 
       if self.IsDebug==0: 
          # writing to the file  
          fpgaFile = open(self.fpga_path,"w")
          fpgaFile.write(fpga_header+"\n")
          fpgaFile.write(global_str +"\n")
          for i in xrange(1,4):
             if( self.cnfCh[i].get()!='OFF' ):
                my_str = self.GetFPGAString(i) 
                fpgaList.append(my_str) 
          fpgaFile.write(eof_fpga_str+"\n") 
          fpgaFile.close() 
       elif self.IsDebug==1:
          # print to screen  
          print self.fpga_path 
          print fpga_header
          print global_str 
          for i in xrange(1,4):
             if( self.cnfCh[i].get()!='OFF' ):
                my_str = self.GetFPGAString(i) 
                print my_str 
          print eof_fpga_str
       # function generator file 
       if self.IsDebug==0: 
          # write to file 
          fgFile = open(self.fg_path  ,"w")
          fgFile.write(fg_header +"\n") 
          fgFile.write(freq_str  +"\n")
          fgFile.write(bnc_str   +"\n")
          fgFile.write(ntype_str +"\n")
          fgFile.write(eof_fg_str+"\n") 
          fgFile.close() 
       elif self.IsDebug==1: 
          print self.fg_path 
          print fg_header
          print freq_str
          print bnc_str
          print ntype_str 
          print eof_fg_str 
       # adc file
       if self.IsDebug==0: 
          # write to file  
          adcFile = open(self.adc_path ,"w")
          adcFile.write(adc_header      +"\n")
          adcFile.write(adc_id_str      +"\n")
          adcFile.write(ch_num_str      +"\n")
          adcFile.write(pulse_num_str   +"\n")
          adcFile.write(sample_freq_str +"\n")
          adcFile.write(extern_clk_str  +"\n")
          adcFile.write(eof_adc_str     +"\n") 
          adcFile.close() 
       elif self.IsDebug==1:  
          print self.adc_path 
          print adc_header     
          print adc_id_str     
          print ch_num_str     
          print pulse_num_str  
          print sample_freq_str
          print extern_clk_str 
          print eof_adc_str    
       # utilities
       if self.IsDebug==0: 
          # write to file  
          utilFile  = open(self.util_path,"w")
          utilFile.write(util_header  + "\n")        
          utilFile.write(debug_str    + "\n")        
          utilFile.write(verb_str     + "\n")        
          utilFile.write(test_str     + "\n")        
          utilFile.write(rf_str       + "\n")        
          utilFile.write(eof_util_str + "\n")        
          utilFile.close()
       elif self.IsDebug==1:
          print self.util_path  
          print util_header  
          print debug_str    
          print verb_str     
          print test_str     
          print rf_str       
          print eof_util_str 
       # comments
       if self.IsDebug==0: 
          # write to file 
          commentFile = open(self.com_path ,"w") 
          commentFile.write(com_str+"\n")
          commentFile.close()  
       elif self.IsDebug==1: 
          print com_path 
          print com_str
       # update status banner 
       self.StatusVariable.set("Configuration printed to file.") 
   #----------------------------------------------------------------------------
   def GetFPGAString(self,ch):
      # gather all values and put into a string  
      state    = "off"
      if( self.cnfCh[ch].get()=="OFF" ): 
         state = "off" 
      else:  
         state = "on"
 
      msw_off  = self.entryMechOffVariable[ch].get()  
      msw_dur  = self.entryMechDurVariable[ch].get()  
      msw_unit = self.unitMech[ch].get()  
      rft_off  = self.entryRFTransOffVariable[ch].get()  
      rft_dur  = self.entryRFTransDurVariable[ch].get()  
      rft_unit = self.unitRFTrans[ch].get()  
      tom_off  = self.entryTomcoOffVariable[ch].get()  
      tom_dur  = self.entryTomcoDurVariable[ch].get()  
      tom_unit = self.unitTomco[ch].get()  
      tom_enbl = self.chkTomcoEnableVariable[ch].get()  
      rfr_off  = self.entryRFRecOffVariable[ch].get()  
      rfr_dur  = self.entryRFRecDurVariable[ch].get()  
      rfr_unit = self.unitRFRec[ch].get()
      beg_str  = "%s %s"          % (self.ChName[ch],state)   
      msw_str  = "%s %s %s"       % (msw_off,msw_dur,msw_unit) 
      rft_str  = "%s %s %s"       % (rft_off,rft_dur,rft_unit) 
      tom_str  = "%s %s %s %s"    % (tom_off,tom_dur,tom_unit,tom_enbl) 
      rfr_str  = "%s %s %s"       % (rfr_off,rfr_dur,rfr_unit) 
      my_str   = "%s %s %s %s %s" % (beg_str,msw_str,rft_str,tom_str,rfr_str) 
      return my_str  
   #----------------------------------------------------------------------------
   def GetFPGAState(self,type,index): 
      # get the state of the FPGA variable: is it on (1) or off (0)   
      # adjust array index  
      i = index 
      # our return variable  
      state = 0  
      # get values
      if type==0:   # global on/off 
         state = self.chk_global_var.get() 
      elif type==1: # mechanical switch
         state = self.chk_var_mech[i].get()
      elif type==2: # RF 
         state = self.chk_var_rf[i].get()
      elif type==3: # TOMCO gate 
         state = self.chk_var_rf_gate.get()
      return state 
   #----------------------------------------------------------------------------
   def GetFuncGenString(self,type):
      bnc_state       = "off"
      ntype_state     = "off"
      label           = "none"
      value           = "none"
      state           = "none"
      units           = "none"
      if self.chk_fg_bnc_var.get():   bnc_state   = "on"
      if self.chk_fg_ntype_var.get(): ntype_state = "on"
      freq            = self.entryFGFreqVariable.get()  
      freq_unit       = self.unit_str_freq_fg.get() 
      bnc_volt        = self.entryFGBNCVoltVariable.get()  
      bnc_volt_unit   = self.unit_str_volt_fg_bnc.get() 
      ntype_volt      = self.entryFGNTypeVoltVariable.get()  
      ntype_volt_unit = self.unit_str_volt_fg_ntype.get() 
      # bnc_freq        = self.entryFGBNCFreqVariable.get()  
      # ntype_freq      = self.entryFGNTypeFreqVariable.get()  
      # bnc_freq_unit   = self.unit_str_freq_fg_bnc.get() 
      # ntype_freq_unit = self.unit_str_freq_fg_ntype.get() 
      if type==1: 
         label = "bnc"
         value = bnc_volt 
         state = bnc_state 
         units = bnc_volt_unit 
      elif type==2: 
         label = "ntype"
         value = ntype_volt 
         state = ntype_state 
         units = ntype_volt_unit
      elif type==3:
         label = "frequency"
         value = freq 
         state = "--" 
         units = freq_unit 
      my_str = "%-20s %-20s %-20s %-20s" %(label,state,value,units)
      return my_str  
   #----------------------------------------------------------------------------
   def GetADCString(self,type): 
      adc_id_val = self.entryADCNumVariable.get() 
      ch_num     = self.entryADCchVariable.get()
      pulse_num  = self.entryADCPulseVariable.get() 
      freq       = self.adc_freq_val.get()
      # event_len  = self.entryADCLengthVariable.get()
      freq_unit  = "MHz"
      # event_unit = self.unit_str_adc_length.get() 
      label      = "none"
      value      = "none"
      unit       = "ND" 
      extern_clk = self.chk_adc_clk_type_var.get() 
      if type==1: 
         label = "adc_id"
         value = adc_id_val  
         unit  = "ND"
      elif type==2: 
         label = "channel_number"
         value = ch_num 
         unit  = "ND"
      elif type==3: 
         label = "number_of_events"
         value = pulse_num 
         unit  = "ND"
      elif type==4: 
         label = "frequency"
         value = freq
         unit  = freq_unit
      # elif type==5: 
      #    label = "signal_length"
      #    value = event_len 
      #    unit  = event_unit
      elif type==6:
         label = "external_clock"
         if extern_clk:     value = freq 
         if not extern_clk: value = "-" + freq 
         unit  = freq_unit 
      my_str   = "%-20s %-20s %-20s" %(label,value,unit)
      return my_str 
   #----------------------------------------------------------------------------
   def GetUtilString(self,type): 
      debug_str = self.util_debug_val.get() 
      debug_val = "0"
      verb_val  = self.util_verb_val.get() 
      test_val  = self.util_test_val.get()
      freq      = self.entryUtilRefFreqVariable.get()
      freq_unit = self.util_ref_freq_unit_val.get()
      freq_dbl  = float(freq) 
          
      if debug_str=="off": debug_val = "0"
      if debug_str=="on":  debug_val = "1"
     
      if freq_unit=="kHz": freq_dbl = freq_dbl*1.E+3  
      if freq_unit=="MHz": freq_dbl = freq_dbl*1.E+6  
      if freq_unit=="GHz": freq_dbl = freq_dbl*1.E+9  

      freq = freq_dbl

      if type==1:  
         label = "debug_mode"
         value = debug_val  
      elif type==2: 
         label = "verbosity"
         value = verb_val 
      elif type==3: 
         label = "test_mode"
         value = test_val 
      elif type==4: 
         label = "rf_frequency"
         value = freq
      my_str   = "%-20s %-20s" %(label,value)
      return my_str 
   #----------------------------------------------------------------------------
   def RunDAQ(self):  
      # run the DAQ: first create symbolic links to the current configuration,
      # then run the system (bash script that starts the C code)  
      HOME      = self.MyHOME 
      cd_input  = "cd " + HOME + "input/"
      cd_home   = "cd " + HOME
      symlink   = "ln -s " 
      # define the source files 
      fpga_src  = HOME + self.prefix + self.fpga_fn
      fg_src    = HOME + self.prefix + self.fg_fn 
      adc_src   = HOME + self.prefix + self.adc_fn 
      util_src  = HOME + self.prefix + self.util_fn 
      com_src   = HOME + self.prefix + self.com_fn 
      # define targets: note here that the names are hard-coded.  
      # this is done because these file names NEVER change
      fpga_tgt  = HOME + "input/" + "pulse-data.dat" 
      fg_tgt    = HOME + "input/" + "sg382.dat"
      adc_tgt   = HOME + "input/" + "struck_adc.dat"
      util_tgt  = HOME + "input/" + "utilities.dat"
      com_tgt   = HOME + "input/" + "comments.txt"
      # remove commands
      rm_fpga   = "rm " + fpga_tgt 
      rm_fg     = "rm " + fg_tgt 
      rm_adc    = "rm " + adc_tgt 
      rm_util   = "rm " + util_tgt 
      rm_com    = "rm " + com_tgt 
      # check for existing files 
      fpga_is_alive = os.path.isfile(fpga_tgt)
      fg_is_alive   = os.path.isfile(fg_tgt)
      adc_is_alive  = os.path.isfile(adc_tgt)
      util_is_alive = os.path.isfile(util_tgt)
      com_is_alive  = os.path.isfile(com_tgt)
      # delete current symbolic links if necessary 
      if fpga_is_alive: os.system( rm_fpga ) 
      if fg_is_alive:   os.system( rm_fg   ) 
      if adc_is_alive:  os.system( rm_adc  ) 
      if util_is_alive: os.system( rm_util ) 
      if com_is_alive:  os.system( rm_com  ) 
      # define the commands 
      fpga_cmd  = symlink + fpga_src + " " + fpga_tgt   # symbolic link for FPGA 
      fg_cmd    = symlink + fg_src   + " " + fg_tgt     # symbolic link for function generator  
      adc_cmd   = symlink + adc_src  + " " + adc_tgt    # symbolic link for ADC 
      util_cmd  = symlink + util_src + " " + util_tgt   # symbolic link for utilities 
      com_cmd   = symlink + com_src  + " " + com_tgt    # symbolic link for comments
      run_cmd   = "./run_nmr.sh"                        # runs the C code 
      # print fpga_cmd 
      # print fg_cmd 
      # print adc_cmd 
      # print run_cmd 
      # symbolically link files
      os.system(cd_input) 
      os.system(fpga_cmd)
      os.system(fg_cmd)
      os.system(adc_cmd)
      os.system(util_cmd)
      os.system(com_cmd)
      # check to see if commands succeeded 
      fpga_is_alive = os.path.isfile(fpga_tgt)
      fg_is_alive   = os.path.isfile(fg_tgt)
      adc_is_alive  = os.path.isfile(adc_tgt)
      util_is_alive = os.path.isfile(util_tgt)
      com_is_alive  = os.path.isfile(com_tgt)
      if fpga_is_alive: print "[NMRDAQ]: symbolic link from %s to %s created." % (fpga_src,fpga_tgt) 
      if fg_is_alive:   print "[NMRDAQ]: symbolic link from %s to %s created." % (fg_src  ,fg_tgt  )  
      if adc_is_alive:  print "[NMRDAQ]: symbolic link from %s to %s created." % (adc_src ,adc_tgt )  
      if util_is_alive: print "[NMRDAQ]: symbolic link from %s to %s created." % (util_src,util_tgt)   
      if com_is_alive:  print "[NMRDAQ]: symbolic link from %s to %s created." % (com_src ,com_tgt )  
      # cd back to main dir
      os.system(cd_home)
      # start the C code 
      self.StatusVariable.set("DAQ is running") 
      os.system(run_cmd) 
      self.StatusVariable.set("Acquisition complete.") 
   #----------------------------------------------------------------------------
   def UpdatePlots(self):
      # paths 
      data_path  = self.MyHOME + "data/" 
      c_code_dir = self.MyHOME + "plotting/"
      # find the most recent data files 
      DataMgr   = DataManager() 
      gFile     = DataMgr.FindData(data_path)  
      # call the C code 
      call( [c_code_dir+"./quickplot3.exe",gFile[0],gFile[1],gFile[2],gFile[3]] )
      os.chdir(self.MyHOME) 
#--------------------------------------------------------------------------------

if __name__ == "__main__": 
   app = daq(None) 
   app.title('NMR DAQ')
   app.mainloop()
