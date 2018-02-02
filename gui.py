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
 
      self.counter          = 0 
      self.MyHOME           = os.getcwd() + "/"  
      self.ConfVarSelection = Tkinter.StringVar()  
      self.IsDebug          = 1     # if 1, print file contents to screen 
      self.IsImported       = 0     # set to be true if the config is loaded via the Import Configuration button.
      self.IsGolden         = 0     # for golden FPGA configs  

      self.global_fn     = "global_on_off"
      self.delay_time_fn = "delay-time" 
      self.fpga_fn       = "pulse-data"
      self.fg_fn         = "sg382"
      self.fgpi2_fn      = "sg382_pi2"
      self.adc_fn        = "struck_adc"
      self.util_fn       = "utilities"
      self.com_fn        = "comments"
      self.temp_fn       = "keithley"

      self.HASH          = "#"
      self.EOF           = "end_of_file"
      self.ZERO          = "0" 
      self.NINETYNINE    = "99"
      self.MIN1          = "-1"
      self.blSTATE       = "--"
      self.ND            = "ND"
      
      self.unitWidth     = 5 

      self.HeaderFont    = "Helvetica 12 bold" 

      # variables and lists  
      TickBox            = "on"
      self.TimeChoices   = ['s','ms','us']
      self.DebugChoices  = ['off','on']
      self.VerbChoices   = ['0','1','2','3','4']
      self.TestChoices   = ['0','1','2','3','4','5']
      FreqValues         = ['1','10','25','50','100','125','250']
      FreqChoices        = ['units','kHz','MHz']
      FreqUnitChoices    = ['MHz']
      VoltChoices        = ['units','Vpp','rms','dBm','W']
      self.RowOffset     = 0 
      self.ColumnOffset  = 0

      self.ChName        = ['S1','S2','S3','S4']
      self.ChID          = ['1','2','3','4']

      self.ConfigChoices = ['OFF','Short Coil','Long Coil','Cyl. Probe','Sph. Probe','Other','3He']
      self.ConfigFN      = ['off','short-coil','long-coil','cyl-probe' ,'sph-probe' ,'other','3He'] 

      self.tempChoices   = ['PT1000','USP17464']

      # frame 
      # self = Tkinter.Frame(self,borderwidth=2,relief=Tkinter.GROOVE) 
      # self.pack()   

      # ----------------------------------------------------------------------------------
      # FPGA 
      # ID Label for pulse timing
      self.FPGATimingLabelVariable = Tkinter.StringVar()
      self.FPGATimingLabelVariable.set("FPGA TIMING")
      self.FPGATimingLabel = Tkinter.Label(self,width=10,textvariable=self.FPGATimingLabelVariable,anchor="w",font=self.HeaderFont)
      self.FPGATimingLabel.config(width=12) 
      self.FPGATimingLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+0) 
      # global on/off 
      self.GlobalOnOffLabelVariable = Tkinter.StringVar() 
      self.GlobalOnOffLabelVariable.set("Global On/Off") 
      GlobalOnOffLabel  = Tkinter.Label(self,width=10,textvariable=self.GlobalOnOffLabelVariable,anchor="w") 
      GlobalOnOffLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+1) 
      # check box 
      self.chk_global_on_off_var = Tkinter.IntVar() 
      self.chk_global_on_off     = Tkinter.Checkbutton(self,text="On",variable=self.chk_global_on_off_var) 
      self.chk_global_on_off.grid(column=self.ColumnOffset+1,row=self.RowOffset+1) 

      # channel label (header)  
      self.ChHeaderLabelVariable = Tkinter.StringVar() 
      self.ChHeaderLabelVariable.set("Channel") 
      ChHeaderLabel  = Tkinter.Label(self,width=10,textvariable=self.ChHeaderLabelVariable,anchor="center") 
      ChHeaderLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+2) 

      # configuration label 
      self.ConfigLabelVariable = Tkinter.StringVar() 
      self.ConfigLabelVariable.set("Configuration") 
      ConfigLabel  = Tkinter.Label(self,width=10,textvariable=self.ConfigLabelVariable,anchor="w") 
      ConfigLabel.grid(column=self.ColumnOffset+1,row=self.RowOffset+2) 

      self.UnitName = Tkinter.StringVar() 
      self.UnitName.set("Units") 

      # Channel Label (ports 1--4 on mechanical switch) 
      # 1 
      self.ChLabelVariable1 = Tkinter.StringVar() 
      self.ChLabelVariable1.set(self.ChName[0]) 
      ChLabel1  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable1,anchor="w") 
      ChLabel1.grid(column=self.ColumnOffset+0,row=self.RowOffset+4) 
      # 2 
      self.ChLabelVariable2 = Tkinter.StringVar() 
      self.ChLabelVariable2.set(self.ChName[1]) 
      ChLabel2  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable2,anchor="w") 
      ChLabel2.grid(column=self.ColumnOffset+0,row=self.RowOffset+5) 
      # 3 
      self.ChLabelVariable3 = Tkinter.StringVar() 
      self.ChLabelVariable3.set(self.ChName[2]) 
      ChLabel3  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable3,anchor="w") 
      ChLabel3.grid(column=self.ColumnOffset+0,row=self.RowOffset+6) 
      # 4 
      self.ChLabelVariable4 = Tkinter.StringVar() 
      self.ChLabelVariable4.set(self.ChName[3]) 
      ChLabel4  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable4,anchor="w") 
      ChLabel4.grid(column=self.ColumnOffset+0,row=self.RowOffset+7)

      OffTitle = "Offset" 
      DurTitle = "Duration" 

      # Mech Switch Offset Label  
      self.MechSwOffsetNameA = Tkinter.StringVar() 
      self.MechSwOffsetNameA.set("Mechanical Switch") 
      MechSwOffsetLabelA  = Tkinter.Label(self,textvariable=self.MechSwOffsetNameA,anchor="w") 
      MechSwOffsetLabelA.grid(column=self.ColumnOffset+2,row=self.RowOffset+2,columnspan=2) 
      self.MechSwOffsetNameB = Tkinter.StringVar() 
      self.MechSwOffsetNameB.set(OffTitle) 
      MechSwOffsetLabelB  = Tkinter.Label(self,textvariable=self.MechSwOffsetNameB,anchor="w") 
      MechSwOffsetLabelB.grid(column=self.ColumnOffset+2,row=self.RowOffset+3) 
      # Mech Switch Duration Label 
      self.MechSwDurationNameB = Tkinter.StringVar() 
      self.MechSwDurationNameB.set(DurTitle) 
      MechSwDurationLabelB  = Tkinter.Label(self,textvariable=self.MechSwDurationNameB,anchor="w") 
      MechSwDurationLabelB.grid(column=self.ColumnOffset+3,row=self.RowOffset+3) 
      # Mech Switch Unit Label 
      MechSwUnitLabel  = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      MechSwUnitLabel.grid(column=self.ColumnOffset+4,row=self.RowOffset+2) 

      # RF Transmit Offset Label  
      self.RFTransOffsetNameA = Tkinter.StringVar() 
      self.RFTransOffsetNameA.set("RF Transmit Switch") 
      RFTransOffsetLabelA  = Tkinter.Label(self,textvariable=self.RFTransOffsetNameA,anchor="w") 
      RFTransOffsetLabelA.grid(column=self.ColumnOffset+5,row=self.RowOffset+2,columnspan=2) 
      self.RFTransOffsetNameB = Tkinter.StringVar() 
      self.RFTransOffsetNameB.set(OffTitle) 
      RFTransOffsetLabelB  = Tkinter.Label(self,textvariable=self.RFTransOffsetNameB,anchor="w") 
      RFTransOffsetLabelB.grid(column=self.ColumnOffset+5,row=self.RowOffset+3) 
      # RF Transmit Duration Label 
      self.RFTransDurationNameB = Tkinter.StringVar() 
      self.RFTransDurationNameB.set(DurTitle) 
      RFTransDurationLabelB  = Tkinter.Label(self,textvariable=self.RFTransDurationNameB,anchor="w") 
      RFTransDurationLabelB.grid(column=self.ColumnOffset+6,row=self.RowOffset+3) 
      # RF Transmit Unit Label 
      RFTransUnitLabel  = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      RFTransUnitLabel.grid(column=self.ColumnOffset+7,row=self.RowOffset+2) 

      # Tomco Offset Label  
      self.TomcoOffsetNameA = Tkinter.StringVar() 
      self.TomcoOffsetNameA.set("Tomco Amplifier") 
      TomcoOffsetLabelA  = Tkinter.Label(self,textvariable=self.TomcoOffsetNameA,anchor="w") 
      TomcoOffsetLabelA.grid(column=self.ColumnOffset+8,row=self.RowOffset+2,columnspan=2) 
      self.TomcoOffsetNameB = Tkinter.StringVar() 
      self.TomcoOffsetNameB.set(OffTitle) 
      TomcoOffsetLabelB  = Tkinter.Label(self,textvariable=self.TomcoOffsetNameB,anchor="w") 
      TomcoOffsetLabelB.grid(column=self.ColumnOffset+8,row=self.RowOffset+3) 
      # Tomco Duration Label 
      self.TomcoDurationNameB = Tkinter.StringVar() 
      self.TomcoDurationNameB.set(DurTitle) 
      TomcoDurationLabelB  = Tkinter.Label(self,textvariable=self.TomcoDurationNameB,anchor="w") 
      TomcoDurationLabelB.grid(column=self.ColumnOffset+9,row=self.RowOffset+3) 
      # Tomco Unit Label 
      TomcoUnitLabel  = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      TomcoUnitLabel.grid(column=self.ColumnOffset+10,row=self.RowOffset+2) 
      # Tomco Enable Label
      self.TomcoEnableA = Tkinter.StringVar()
      self.TomcoEnableA.set("Tomco")  
      TomcoEnableLabelA  = Tkinter.Label(self,width=7,textvariable=self.TomcoEnableA,anchor="w") 
      TomcoEnableLabelA.grid(column=self.ColumnOffset+11,row=self.RowOffset+2) 
      self.TomcoEnableB = Tkinter.StringVar()
      self.TomcoEnableB.set("Enable")  
      TomcoEnableLabelB  = Tkinter.Label(self,width=7,textvariable=self.TomcoEnableB,anchor="w") 
      TomcoEnableLabelB.grid(column=self.ColumnOffset+11,row=self.RowOffset+3) 

      # pi/2 header Label   
      # self.Pi2NameA = Tkinter.StringVar() 
      # self.Pi2NameA.set("pi/2 Pulse") 
      # Pi2LabelA  = Tkinter.Label(self,textvariable=self.Pi2NameA,anchor="w",font=self.HeaderFont) 
      # Pi2LabelA.grid(column=self.ColumnOffset+0,row=self.RowOffset+10,columnspan=2) 

      # # 1 
      # Pi2ChLabel1  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable1,anchor="w") 
      # Pi2ChLabel1.grid(column=self.ColumnOffset+4,row=self.RowOffset+10) 
      # # 2 
      # Pi2ChLabel2  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable2,anchor="w") 
      # Pi2ChLabel2.grid(column=self.ColumnOffset+4,row=self.RowOffset+11) 
      # # 3 
      # Pi2ChLabel3  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable3,anchor="w") 
      # Pi2ChLabel3.grid(column=self.ColumnOffset+4,row=self.RowOffset+12) 
      # # 4 
      # Pi2ChLabel4  = Tkinter.Label(self,width=3,textvariable=self.ChLabelVariable4,anchor="w") 
      # Pi2ChLabel4.grid(column=self.ColumnOffset+4,row=self.RowOffset+13)

      # pi/2 frequency Label 
      # self.Pi2FreqName = Tkinter.StringVar() 
      # self.Pi2FreqName.set("Frequency") 
      # Pi2FreqLabel  = Tkinter.Label(self,textvariable=self.Pi2FreqName,anchor="w") 
      # Pi2FreqLabel.grid(column=self.ColumnOffset+5,row=self.RowOffset+9) 
      # # pi/2 amplitude Unit Label 
      # Pi2FreqUnitLabel = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      # Pi2FreqUnitLabel.grid(column=self.ColumnOffset+6,row=self.RowOffset+9) 
      # # pi/2 amplitude Label 
      # self.Pi2AmplName = Tkinter.StringVar() 
      # self.Pi2AmplName.set("Amplitude") 
      # Pi2AmplLabel     = Tkinter.Label(self,textvariable=self.Pi2AmplName,anchor="w") 
      # Pi2AmplLabel.grid(column=self.ColumnOffset+7,row=self.RowOffset+9) 
      # # pi/2 amplitude Unit Label 
      # Pi2AmplUnitLabel = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      # Pi2AmplUnitLabel.grid(column=self.ColumnOffset+8,row=self.RowOffset+9) 

      # RF Receive Offset Label  
      self.RFRecOffsetNameA = Tkinter.StringVar() 
      self.RFRecOffsetNameA.set("RF Receive Switch") 
      RFRecOffsetLabelA  = Tkinter.Label(self,textvariable=self.RFRecOffsetNameA,anchor="w") 
      RFRecOffsetLabelA.grid(column=self.ColumnOffset+12,row=self.RowOffset+2,columnspan=2) 
      self.RFRecOffsetNameB = Tkinter.StringVar() 
      self.RFRecOffsetNameB.set(OffTitle) 
      RFRecOffsetLabelB  = Tkinter.Label(self,textvariable=self.RFRecOffsetNameB,anchor="w") 
      RFRecOffsetLabelB.grid(column=self.ColumnOffset+12,row=self.RowOffset+3)
      # RF Receive Duration Label 
      self.RFRecDurationNameB = Tkinter.StringVar() 
      self.RFRecDurationNameB.set(DurTitle) 
      RFRecDurationLabelB  = Tkinter.Label(self,textvariable=self.RFRecDurationNameB,anchor="w") 
      RFRecDurationLabelB.grid(column=self.ColumnOffset+13,row=self.RowOffset+3) 
      # RF Receive Unit Label 
      RFRecUnitLabel  = Tkinter.Label(self,textvariable=self.UnitName,anchor="w") 
      RFRecUnitLabel.grid(column=self.ColumnOffset+14,row=self.RowOffset+2) 

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
      self.opt_conf_ch1.config(width=10)  
      self.opt_conf_ch1.grid(column=self.ColumnOffset+1,row=self.RowOffset+4,sticky="ew")  
      # 2 
      self.opt_conf_ch2 = Tkinter.OptionMenu(self,self.cnfCh[1],*self.ConfigChoices) 
      self.opt_conf_ch2.config(width=10)  
      self.opt_conf_ch2.grid(column=self.ColumnOffset+1,row=self.RowOffset+5,sticky="ew")  
      # 3 
      self.opt_conf_ch3 = Tkinter.OptionMenu(self,self.cnfCh[2],*self.ConfigChoices) 
      self.opt_conf_ch3.config(width=10)  
      self.opt_conf_ch3.grid(column=self.ColumnOffset+1,row=self.RowOffset+6,sticky="ew")  
      # 4 
      self.opt_conf_ch4 = Tkinter.OptionMenu(self,self.cnfCh[3],*self.ConfigChoices) 
      self.opt_conf_ch4.config(width=10)  
      self.opt_conf_ch4.grid(column=self.ColumnOffset+1,row=self.RowOffset+7,sticky="ew")  

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

      unit_pi2Freq1    = Tkinter.StringVar() 
      # unit_pi2Freq2    = Tkinter.StringVar() 
      # unit_pi2Freq3    = Tkinter.StringVar() 
      # unit_pi2Freq4    = Tkinter.StringVar() 

      # self.unitPi2Freq = [unit_pi2Freq1,unit_pi2Freq2,unit_pi2Freq3,unit_pi2Freq4] 
      # unit_pi2Ampl1    = Tkinter.StringVar() 
      # unit_pi2Ampl2    = Tkinter.StringVar() 
      # unit_pi2Ampl3    = Tkinter.StringVar() 
      # unit_pi2Ampl4    = Tkinter.StringVar() 
      # self.unitPi2Ampl = [unit_pi2Ampl1,unit_pi2Ampl2,unit_pi2Ampl3,unit_pi2Ampl4] 

      self.unitPi2Freq = Tkinter.StringVar() 
      self.unitPi2Ampl = Tkinter.StringVar() 

      self.unitPi2Freq.set("units")
      self.unitPi2Ampl.set("units")

      for element in self.unitMech: 
         element.set("s") 
      for element in self.unitRFTrans: 
         element.set("s") 
      for element in self.unitTomco: 
         element.set("s") 
      for element in self.unitRFRec: 
         element.set("s") 
      # for element in self.unitPi2Freq: 
      #    element.set("units") 
      # for element in self.unitPi2Ampl: 
      #    element.set("units") 
 
      # entry fields
      # 1 
      # offsets  
      self.entryMechOff1    = Tkinter.Entry(self,width=7,textvariable=self.entryMechOffVariable[0]) 
      self.entryMechOff1.grid(column=self.ColumnOffset+2,row=self.RowOffset+4,sticky='EW')
      self.entryRFTransOff1 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransOffVariable[0]) 
      self.entryRFTransOff1.grid(column=self.ColumnOffset+5,row=self.RowOffset+4,sticky='EW')
      self.entryTomcoOff1   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoOffVariable[0]) 
      self.entryTomcoOff1.grid(column=self.ColumnOffset+8,row=self.RowOffset+4,sticky='EW')
      self.entryRFRecOff1   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecOffVariable[0]) 
      self.entryRFRecOff1.grid(column=self.ColumnOffset+12,row=self.RowOffset+4,sticky='EW')
      # durations 
      self.entryMechDur1    = Tkinter.Entry(self,width=7,textvariable=self.entryMechDurVariable[0]) 
      self.entryMechDur1.grid(column=self.ColumnOffset+3,row=self.RowOffset+4,sticky='EW')
      self.entryRFTransDur1 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransDurVariable[0]) 
      self.entryRFTransDur1.grid(column=self.ColumnOffset+6,row=self.RowOffset+4,sticky='EW')
      self.entryTomcoDur1   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoDurVariable[0]) 
      self.entryTomcoDur1.grid(column=self.ColumnOffset+9,row=self.RowOffset+4,sticky='EW')
      self.entryRFRecDur1   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecDurVariable[0]) 
      self.entryRFRecDur1.grid(column=self.ColumnOffset+13,row=self.RowOffset+4,sticky='EW')
      # check box 
      self.chkTomcoEnable1     = Tkinter.Checkbutton(self,variable=self.chkTomcoEnableVariable[0]) 
      self.chkTomcoEnable1.grid(column=self.ColumnOffset+11,row=self.RowOffset+4) 
      # units 
      self.opt_mech1 = Tkinter.OptionMenu(self,self.unitMech[0],*self.TimeChoices) 
      self.opt_mech1.config(width=self.unitWidth)  
      self.opt_mech1.grid(column=self.ColumnOffset+4,row=self.RowOffset+4)  
      self.opt_rft1 = Tkinter.OptionMenu(self,self.unitRFTrans[0],*self.TimeChoices) 
      self.opt_rft1.config(width=self.unitWidth)  
      self.opt_rft1.grid(column=self.ColumnOffset+7,row=self.RowOffset+4)  
      self.opt_tom1 = Tkinter.OptionMenu(self,self.unitTomco[0],*self.TimeChoices) 
      self.opt_tom1.config(width=self.unitWidth)  
      self.opt_tom1.grid(column=self.ColumnOffset+10,row=self.RowOffset+4)  
      self.opt_rfr1 = Tkinter.OptionMenu(self,self.unitRFRec[0],*self.TimeChoices) 
      self.opt_rfr1.config(width=self.unitWidth)  
      self.opt_rfr1.grid(column=self.ColumnOffset+14,row=self.RowOffset+4)  
      # 2 
      # offsets  
      self.entryMechOff2    = Tkinter.Entry(self,width=7,textvariable=self.entryMechOffVariable[1]) 
      self.entryMechOff2.grid(column=self.ColumnOffset+2,row=self.RowOffset+5,sticky='EW')
      self.entryRFTransOff2 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransOffVariable[1]) 
      self.entryRFTransOff2.grid(column=self.ColumnOffset+5,row=self.RowOffset+5,sticky='EW')
      self.entryTomcoOff2   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoOffVariable[1]) 
      self.entryTomcoOff2.grid(column=self.ColumnOffset+8,row=self.RowOffset+5,sticky='EW')
      self.entryRFRecOff2   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecOffVariable[1]) 
      self.entryRFRecOff2.grid(column=self.ColumnOffset+12,row=self.RowOffset+5,sticky='EW')
      # durations 
      self.entryMechDur2    = Tkinter.Entry(self,width=7,textvariable=self.entryMechDurVariable[1]) 
      self.entryMechDur2.grid(column=self.ColumnOffset+3,row=self.RowOffset+5,sticky='EW')
      self.entryRFTransDur2 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransDurVariable[1]) 
      self.entryRFTransDur2.grid(column=self.ColumnOffset+6,row=self.RowOffset+5,sticky='EW')
      self.entryTomcoDur2   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoDurVariable[1]) 
      self.entryTomcoDur2.grid(column=self.ColumnOffset+9,row=self.RowOffset+5,sticky='EW')
      self.entryRFRecDur2   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecDurVariable[1]) 
      self.entryRFRecDur2.grid(column=self.ColumnOffset+13,row=self.RowOffset+5,sticky='EW')
      # check box 
      self.chkTomcoEnable2     = Tkinter.Checkbutton(self,variable=self.chkTomcoEnableVariable[1]) 
      self.chkTomcoEnable2.grid(column=self.ColumnOffset+11,row=self.RowOffset+5) 
      # units 
      self.opt_mech2 = Tkinter.OptionMenu(self,self.unitMech[1],*self.TimeChoices) 
      self.opt_mech2.config(width=self.unitWidth)  
      self.opt_mech2.grid(column=self.ColumnOffset+4,row=self.RowOffset+5)  
      self.opt_rft2 = Tkinter.OptionMenu(self,self.unitRFTrans[1],*self.TimeChoices) 
      self.opt_rft2.config(width=self.unitWidth)  
      self.opt_rft2.grid(column=self.ColumnOffset+7,row=self.RowOffset+5)  
      self.opt_tom2 = Tkinter.OptionMenu(self,self.unitTomco[1],*self.TimeChoices) 
      self.opt_tom2.config(width=self.unitWidth)  
      self.opt_tom2.grid(column=self.ColumnOffset+10,row=self.RowOffset+5)  
      self.opt_rfr2 = Tkinter.OptionMenu(self,self.unitRFRec[1],*self.TimeChoices) 
      self.opt_rfr2.config(width=self.unitWidth)  
      self.opt_rfr2.grid(column=self.ColumnOffset+14,row=self.RowOffset+5)  
      # 3
      # offsets  
      self.entryMechOff3    = Tkinter.Entry(self,width=7,textvariable=self.entryMechOffVariable[2]) 
      self.entryMechOff3.grid(column=self.ColumnOffset+2,row=self.RowOffset+6,sticky='EW')
      self.entryRFTransOff3 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransOffVariable[2]) 
      self.entryRFTransOff3.grid(column=self.ColumnOffset+5,row=self.RowOffset+6,sticky='EW')
      self.entryTomcoOff3   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoOffVariable[2]) 
      self.entryTomcoOff3.grid(column=self.ColumnOffset+8,row=self.RowOffset+6,sticky='EW')
      self.entryRFRecOff3   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecOffVariable[2]) 
      self.entryRFRecOff3.grid(column=self.ColumnOffset+12,row=self.RowOffset+6,sticky='EW')
      # durations 
      self.entryMechDur3    = Tkinter.Entry(self,width=7,textvariable=self.entryMechDurVariable[2]) 
      self.entryMechDur3.grid(column=self.ColumnOffset+3,row=self.RowOffset+6,sticky='EW')
      self.entryRFTransDur3 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransDurVariable[2]) 
      self.entryRFTransDur3.grid(column=self.ColumnOffset+6,row=self.RowOffset+6,sticky='EW')
      self.entryTomcoDur3   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoDurVariable[2]) 
      self.entryTomcoDur3.grid(column=self.ColumnOffset+9,row=self.RowOffset+6,sticky='EW')
      self.entryRFRecDur3   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecDurVariable[2]) 
      self.entryRFRecDur3.grid(column=self.ColumnOffset+13,row=self.RowOffset+6,sticky='EW')
      # check box 
      self.chkTomcoEnable3     = Tkinter.Checkbutton(self,variable=self.chkTomcoEnableVariable[2]) 
      self.chkTomcoEnable3.grid(column=self.ColumnOffset+11,row=self.RowOffset+6) 
      # units 
      self.opt_mech3 = Tkinter.OptionMenu(self,self.unitMech[2],*self.TimeChoices) 
      self.opt_mech3.config(width=self.unitWidth)  
      self.opt_mech3.grid(column=self.ColumnOffset+4,row=self.RowOffset+6)  
      self.opt_rft3 = Tkinter.OptionMenu(self,self.unitRFTrans[2],*self.TimeChoices) 
      self.opt_rft3.config(width=self.unitWidth)  
      self.opt_rft3.grid(column=self.ColumnOffset+7,row=self.RowOffset+6)  
      self.opt_tom3 = Tkinter.OptionMenu(self,self.unitTomco[2],*self.TimeChoices) 
      self.opt_tom3.config(width=self.unitWidth)  
      self.opt_tom3.grid(column=self.ColumnOffset+10,row=self.RowOffset+6)  
      self.opt_rfr3 = Tkinter.OptionMenu(self,self.unitRFRec[2],*self.TimeChoices) 
      self.opt_rfr3.config(width=self.unitWidth)  
      self.opt_rfr3.grid(column=self.ColumnOffset+14,row=self.RowOffset+6)  
      # 4
      # offsets  
      self.entryMechOff4    = Tkinter.Entry(self,width=7,textvariable=self.entryMechOffVariable[3]) 
      self.entryMechOff4.grid(column=self.ColumnOffset+2,row=self.RowOffset+7,sticky='EW')
      self.entryRFTransOff4 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransOffVariable[3]) 
      self.entryRFTransOff4.grid(column=self.ColumnOffset+5,row=self.RowOffset+7,sticky='EW')
      self.entryTomcoOff4   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoOffVariable[3]) 
      self.entryTomcoOff4.grid(column=self.ColumnOffset+8,row=self.RowOffset+7,sticky='EW')
      self.entryRFRecOff4   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecOffVariable[3]) 
      self.entryRFRecOff4.grid(column=self.ColumnOffset+12,row=self.RowOffset+7,sticky='EW')
      # durations 
      self.entryMechDur4    = Tkinter.Entry(self,width=7,textvariable=self.entryMechDurVariable[3]) 
      self.entryMechDur4.grid(column=self.ColumnOffset+3,row=self.RowOffset+7,sticky='EW')
      self.entryRFTransDur4 = Tkinter.Entry(self,width=7,textvariable=self.entryRFTransDurVariable[3]) 
      self.entryRFTransDur4.grid(column=self.ColumnOffset+6,row=self.RowOffset+7,sticky='EW')
      self.entryTomcoDur4   = Tkinter.Entry(self,width=7,textvariable=self.entryTomcoDurVariable[3]) 
      self.entryTomcoDur4.grid(column=self.ColumnOffset+9,row=self.RowOffset+7,sticky='EW')
      self.entryRFRecDur4   = Tkinter.Entry(self,width=7,textvariable=self.entryRFRecDurVariable[3]) 
      self.entryRFRecDur4.grid(column=self.ColumnOffset+13,row=self.RowOffset+7,sticky='EW')
      # check box 
      self.chkTomcoEnable4     = Tkinter.Checkbutton(self,variable=self.chkTomcoEnableVariable[3]) 
      self.chkTomcoEnable4.grid(column=self.ColumnOffset+11,row=self.RowOffset+7) 
      # units 
      self.opt_mech4 = Tkinter.OptionMenu(self,self.unitMech[3],*self.TimeChoices) 
      self.opt_mech4.config(width=self.unitWidth)  
      self.opt_mech4.grid(column=self.ColumnOffset+4,row=self.RowOffset+7)  
      self.opt_rft4 = Tkinter.OptionMenu(self,self.unitRFTrans[3],*self.TimeChoices) 
      self.opt_rft4.config(width=self.unitWidth)  
      self.opt_rft4.grid(column=self.ColumnOffset+7,row=self.RowOffset+7)  
      self.opt_tom4 = Tkinter.OptionMenu(self,self.unitTomco[3],*self.TimeChoices) 
      self.opt_tom4.config(width=self.unitWidth)  
      self.opt_tom4.grid(column=self.ColumnOffset+10,row=self.RowOffset+7)  
      self.opt_rfr4 = Tkinter.OptionMenu(self,self.unitRFRec[3],*self.TimeChoices) 
      self.opt_rfr4.config(width=self.unitWidth)  
      self.opt_rfr4.grid(column=self.ColumnOffset+14,row=self.RowOffset+7)  

      # ----------------------------------------------------------------------------------
      # function generator 
      # ID Label 
      self.FGLabelVariable = Tkinter.StringVar()
      self.FGLabelVariable.set("SIGNAL GENERATORS")
      self.FGLabel = Tkinter.Label(self,textvariable=self.FGLabelVariable,anchor="w",font=self.HeaderFont)
      self.FGLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+8) 
      # Labels  
      self.FGFreqLabelVariable = Tkinter.StringVar()
      self.FGFreqLabelVariable.set("LO Pulse")
      self.FGFreqLabel = Tkinter.Label(self,textvariable=self.FGFreqLabelVariable,anchor="w") 
      self.FGFreqLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+9) 
      # don't even let BNC be an option anymore 
      # self.FGBNCLabelVariable = Tkinter.StringVar()
      # self.FGBNCLabelVariable.set("BNC")
      # self.FGBNCLabel = Tkinter.Label(self,textvariable=self.FGBNCLabelVariable,anchor="w") 
      # self.FGBNCLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+10)  
      # self.FGNTypeLabelVariable = Tkinter.StringVar()
      # self.FGNTypeLabelVariable.set("N-Type")
      # self.FGNTypeLabel = Tkinter.Label(self,textvariable=self.FGNTypeLabelVariable,anchor="w") 
      # self.FGNTypeLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+11) 

      self.FGVoltLabelVariable = Tkinter.StringVar()
      self.FGVoltLabelVariable.set("Voltage")
      self.FGVoltLabel = Tkinter.Label(self,textvariable=self.FGVoltLabelVariable,anchor="w") 
      self.FGVoltLabel.grid(column=self.ColumnOffset+1,row=self.RowOffset+9) 

      # entry fields 
      # frequency 
      self.entryFGFreqVariable = Tkinter.StringVar() 
      self.entryFGFreqVariable.set(u"Frequency")
      self.entryFGFreq = Tkinter.Entry(self,width=7,textvariable=self.entryFGFreqVariable)
      self.entryFGFreq.grid(column=self.ColumnOffset+1,row=self.RowOffset+9,sticky='EW') 
      # BNC 
      # self.entryFGBNCVoltVariable = Tkinter.StringVar() 
      # self.entryFGBNCVoltVariable.set(u"Voltage")
      # self.entryFGBNCVolt = Tkinter.Entry(self,width=7,textvariable=self.entryFGBNCVoltVariable)
      # self.entryFGBNCVolt.grid(column=self.ColumnOffset+1,row=self.RowOffset+10,sticky='EW')  
      # N-Type
      self.entryFGNTypeVoltVariable = Tkinter.StringVar() 
      self.entryFGNTypeVoltVariable.set(u"Voltage")
      self.entryFGNTypeVolt = Tkinter.Entry(self,width=7,textvariable=self.entryFGNTypeVoltVariable)
      self.entryFGNTypeVolt.grid(column=self.ColumnOffset+3,row=self.RowOffset+9,sticky='EW')  

      # pull down menus 
      # frequency 
      self.unit_str_freq_fg = Tkinter.StringVar() 
      self.unit_str_freq_fg.set("units")
      self.opt_freq_fg = Tkinter.OptionMenu(self,self.unit_str_freq_fg,*FreqChoices) 
      self.opt_freq_fg.grid(column=self.ColumnOffset+2,row=self.RowOffset+9)  
      self.opt_freq_fg.config(width=self.unitWidth)  
      # BNC voltage 
      # self.unit_str_volt_fg_bnc = Tkinter.StringVar() 
      # self.unit_str_volt_fg_bnc.set("units")
      # self.opt_volt_fg_bnc = Tkinter.OptionMenu(self,self.unit_str_volt_fg_bnc,*VoltChoices) 
      # self.opt_volt_fg_bnc.grid(column=self.ColumnOffset+2,row=self.RowOffset+10)  
      # N-Type voltage
      self.unit_str_volt_fg_ntype = Tkinter.StringVar() 
      self.unit_str_volt_fg_ntype.set("units")
      self.opt_volt_fg_ntype = Tkinter.OptionMenu(self,self.unit_str_volt_fg_ntype,*VoltChoices) 
      self.opt_volt_fg_ntype.grid(column=self.ColumnOffset+4,row=self.RowOffset+9)  
      self.opt_volt_fg_ntype.config(width=self.unitWidth)  
      # check boxes 
      # self.chk_fg_bnc_var   = Tkinter.IntVar() 
      # self.chk_fg_bnc   = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_fg_bnc_var) 
      # self.chk_fg_bnc.grid(column=self.ColumnOffset+3,row=self.RowOffset+10) 
      self.chk_fg_ntype_var = Tkinter.IntVar() 
      self.chk_fg_ntype = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_fg_ntype_var) 
      self.chk_fg_ntype.grid(column=self.ColumnOffset+5,row=self.RowOffset+9)

      # pi/2 function generator [only use zeroth entry for now]  

      self.FGPi2LabelVariable = Tkinter.StringVar()
      self.FGPi2LabelVariable.set("pi/2 Pulse")
      self.FGPi2Label = Tkinter.Label(self,textvariable=self.FGPi2LabelVariable,anchor="w") 
      self.FGPi2Label.grid(column=self.ColumnOffset+0,row=self.RowOffset+10) 

      # pi2_freq_str_1 = Tkinter.StringVar()
      # pi2_freq_str_2 = Tkinter.StringVar()
      # pi2_freq_str_3 = Tkinter.StringVar()
      # pi2_freq_str_4 = Tkinter.StringVar()
      self.entryPi2FreqVariable = Tkinter.StringVar() # [pi2_freq_str_1,pi2_freq_str_2,pi2_freq_str_3,pi2_freq_str_4] 
      self.entryPi2FreqVariable.set(u"Frequency") 
      # for element in self.entryPi2FreqVariable: 
      #    element.set(u"Frequency") 

      # pi2_ampl_str_1 = Tkinter.StringVar()
      # pi2_ampl_str_2 = Tkinter.StringVar()
      # pi2_ampl_str_3 = Tkinter.StringVar()
      # pi2_ampl_str_4 = Tkinter.StringVar()
      self.entryPi2AmplVariable = Tkinter.StringVar() # [pi2_ampl_str_1,pi2_ampl_str_2,pi2_ampl_str_3,pi2_ampl_str_4] 
      self.entryPi2AmplVariable.set(u"Voltage")  
      # for element in self.entryPi2AmplVariable: 
      #    element.set(u"Voltage") 

      # pi2_state_1   = Tkinter.IntVar() 
      # pi2_state_2   = Tkinter.IntVar() 
      # pi2_state_3   = Tkinter.IntVar() 
      # pi2_state_4   = Tkinter.IntVar() 
      self.chk_fgpi2_var = Tkinter.IntVar() # [pi2_state_1,pi2_state_2,pi2_state_3,pi2_state_4]
      self.chk_fgpi2_var.set(0) 
      # for element in self.chk_fgpi2_var: 
      #    element.set(0) 

      # frequency 
      # entry fields 
      self.entryPi2Freq1    = Tkinter.Entry(self,width=7,textvariable=self.entryPi2FreqVariable) 
      self.entryPi2Freq1.grid(column=self.ColumnOffset+1,row=self.RowOffset+10,sticky='EW')
      # self.entryPi2Freq2    = Tkinter.Entry(self,width=7,textvariable=self.entryPi2FreqVariable[1]) 
      # self.entryPi2Freq2.grid(column=self.ColumnOffset+5,row=self.RowOffset+11,sticky='EW')
      # self.entryPi2Freq3    = Tkinter.Entry(self,width=7,textvariable=self.entryPi2FreqVariable[2]) 
      # self.entryPi2Freq3.grid(column=self.ColumnOffset+5,row=self.RowOffset+12,sticky='EW')
      # self.entryPi2Freq4    = Tkinter.Entry(self,width=7,textvariable=self.entryPi2FreqVariable[3]) 
      # self.entryPi2Freq4.grid(column=self.ColumnOffset+5,row=self.RowOffset+13,sticky='EW')

      # pull down menu (units)
      self.opt_pi2Freq1 = Tkinter.OptionMenu(self,self.unitPi2Freq,*FreqChoices) 
      self.opt_pi2Freq1.config(width=self.unitWidth)  
      self.opt_pi2Freq1.grid(column=self.ColumnOffset+2,row=self.RowOffset+10)  

      # self.opt_pi2Freq2 = Tkinter.OptionMenu(self,self.unitPi2Freq[1],*FreqUnitChoices) 
      # self.opt_pi2Freq2.config(width=self.unitWidth)  
      # self.opt_pi2Freq2.grid(column=self.ColumnOffset+6,row=self.RowOffset+11)  

      # self.opt_pi2Freq3 = Tkinter.OptionMenu(self,self.unitPi2Freq[2],*FreqUnitChoices) 
      # self.opt_pi2Freq3.config(width=self.unitWidth)  
      # self.opt_pi2Freq3.grid(column=self.ColumnOffset+6,row=self.RowOffset+12)  

      # self.opt_pi2Freq4 = Tkinter.OptionMenu(self,self.unitPi2Freq[3],*FreqUnitChoices) 
      # self.opt_pi2Freq4.config(width=self.unitWidth)  
      # self.opt_pi2Freq4.grid(column=self.ColumnOffset+6,row=self.RowOffset+13)  

      # amplitude 
      # entry fields 
      self.entryPi2Ampl1    = Tkinter.Entry(self,width=7,textvariable=self.entryPi2AmplVariable) 
      self.entryPi2Ampl1.grid(column=self.ColumnOffset+3,row=self.RowOffset+10,sticky='EW')
      # self.entryPi2Ampl2    = Tkinter.Entry(self,width=7,textvariable=self.entryPi2AmplVariable[1]) 
      # self.entryPi2Ampl2.grid(column=self.ColumnOffset+7,row=self.RowOffset+11,sticky='EW')
      # self.entryPi2Ampl3    = Tkinter.Entry(self,width=7,textvariable=self.entryPi2AmplVariable[2]) 
      # self.entryPi2Ampl3.grid(column=self.ColumnOffset+7,row=self.RowOffset+12,sticky='EW')
      # self.entryPi2Ampl4    = Tkinter.Entry(self,width=7,textvariable=self.entryPi2AmplVariable[3]) 
      # self.entryPi2Ampl4.grid(column=self.ColumnOffset+7,row=self.RowOffset+13,sticky='EW')

      # pull down menu (units)
      self.opt_pi2Ampl1 = Tkinter.OptionMenu(self,self.unitPi2Ampl,*VoltChoices) 
      self.opt_pi2Ampl1.config(width=self.unitWidth)  
      self.opt_pi2Ampl1.grid(column=self.ColumnOffset+4,row=self.RowOffset+10)  

      # self.opt_pi2Ampl2 = Tkinter.OptionMenu(self,self.unitPi2Ampl[1],*VoltChoices) 
      # self.opt_pi2Ampl2.config(width=self.unitWidth)  
      # self.opt_pi2Ampl2.grid(column=self.ColumnOffset+8,row=self.RowOffset+11)  

      # self.opt_pi2Ampl3 = Tkinter.OptionMenu(self,self.unitPi2Ampl[2],*VoltChoices) 
      # self.opt_pi2Ampl3.config(width=self.unitWidth)  
      # self.opt_pi2Ampl3.grid(column=self.ColumnOffset+8,row=self.RowOffset+12)  

      # self.opt_pi2Ampl4 = Tkinter.OptionMenu(self,self.unitPi2Ampl[3],*VoltChoices) 
      # self.opt_pi2Ampl4.config(width=self.unitWidth)  
      # self.opt_pi2Ampl4.grid(column=self.ColumnOffset+8,row=self.RowOffset+13)  

      self.chk_fgpi2 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_fgpi2_var) 
      self.chk_fgpi2.grid(column=self.ColumnOffset+5,row=self.RowOffset+10)

      # ----------------------------------------------------------------------------------
      # ADC 
      # ID Label for ADC
      self.ADCLabelVariable = Tkinter.StringVar()
      self.ADCLabelVariable.set("DIGITIZER")
      self.ADCLabel = Tkinter.Label(self,textvariable=self.ADCLabelVariable,anchor="w",font=self.HeaderFont)
      self.ADCLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+12) 
      # Struck ADC ID number 
      self.ADCNumLabelVariable = Tkinter.StringVar()
      self.ADCNumLabelVariable.set("Struck ID") 
      self.ADCNumLabel = Tkinter.Label(self,textvariable=self.ADCNumLabelVariable,anchor="w") 
      self.ADCNumLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+13) 
      # Struck ADC channel number 
      self.ADCchLabelVariable = Tkinter.StringVar()
      self.ADCchLabelVariable.set("Channel Number") 
      self.ADCchLabel = Tkinter.Label(self,textvariable=self.ADCchLabelVariable,anchor="w") 
      self.ADCchLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+14) 
      # number of pulses 
      self.ADCPulseLabelVariable = Tkinter.StringVar()
      self.ADCPulseLabelVariable.set("Number of Pulses") 
      self.ADCPulseLabel = Tkinter.Label(self,textvariable=self.ADCPulseLabelVariable,anchor="w") 
      self.ADCPulseLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+15) 
      # sampling frequency
      self.ADCFreqLabelVariable = Tkinter.StringVar()
      self.ADCFreqLabelVariable.set("Sampling Freq.") 
      self.ADCFreqLabel = Tkinter.Label(self,textvariable=self.ADCFreqLabelVariable,anchor="w") 
      self.ADCFreqLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+16) 

      # entry fields  
      # Struck ID number 
      self.entryADCNumVariable = Tkinter.StringVar() 
      self.entryADCNumVariable.set(u"Value") 
      self.entryADCNum = Tkinter.Entry(self,width=7,textvariable=self.entryADCNumVariable) 
      self.entryADCNum.grid(column=self.ColumnOffset+1,row=self.RowOffset+13,sticky='EW')     
      # Struck channel number 
      self.entryADCchVariable = Tkinter.StringVar() 
      self.entryADCchVariable.set(u"Value") 
      self.entryADCch = Tkinter.Entry(self,width=7,textvariable=self.entryADCchVariable) 
      self.entryADCch.grid(column=self.ColumnOffset+1,row=self.RowOffset+14,sticky='EW')     
      # number of pulses 
      self.entryADCPulseVariable = Tkinter.StringVar() 
      self.entryADCPulseVariable.set(u"Value") 
      self.entryADCPulse = Tkinter.Entry(self,width=7,textvariable=self.entryADCPulseVariable) 
      self.entryADCPulse.grid(column=self.ColumnOffset+1,row=self.RowOffset+15,sticky='EW') 

      # pull down menus
      # sampling frequency (value)  
      self.adc_freq_val = Tkinter.StringVar() 
      self.adc_freq_val.set('1')
      self.opt_adc_freq_val = Tkinter.OptionMenu(self,self.adc_freq_val,*FreqValues) 
      self.opt_adc_freq_val.grid(column=self.ColumnOffset+1,row=self.RowOffset+16)  
      # sampling frequency (unit)  
      self.unit_str_adc_freq = Tkinter.StringVar() 
      self.unit_str_adc_freq.set('MHz')
      self.opt_adc_freq = Tkinter.OptionMenu(self,self.unit_str_adc_freq,*FreqUnitChoices) 
      self.opt_adc_freq.grid(column=self.ColumnOffset+2,row=self.RowOffset+16)  
      # check boxes 
      self.chk_adc_clk_type_var = Tkinter.IntVar() 
      self.chk_adc_clk_type     = Tkinter.Checkbutton(self,text="External",variable=self.chk_adc_clk_type_var) 
      self.chk_adc_clk_type.grid(column=self.ColumnOffset+3,row=self.RowOffset+16) 

      # # ----------------------------------------------------------------------------------
      # Utilities 
      # header label  
      self.UtilLabelVariable = Tkinter.StringVar()
      self.UtilLabelVariable.set("UTILITIES")
      self.UtilLabel = Tkinter.Label(self,textvariable=self.UtilLabelVariable,anchor="w",font=self.HeaderFont)
      self.UtilLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+17) 
      # lab  
      self.LabLabelVariable = Tkinter.StringVar()
      self.LabLabelVariable.set("Laboratory")
      self.LabLabel = Tkinter.Label(self,textvariable=self.LabLabelVariable,anchor="w")
      self.LabLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+18) 
      self.chk_lab_var = Tkinter.IntVar() 
      self.chk_lab     = Tkinter.Checkbutton(self,text="Fermilab [Argonne is Default]",variable=self.chk_lab_var) 
      self.chk_lab.grid(column=self.ColumnOffset+1,row=self.RowOffset+18) 
      # temperature   
      self.TempLabelVariable = Tkinter.StringVar()
      self.TempLabelVariable.set("Temp. Sensor") 
      self.TempLabel = Tkinter.Label(self,textvariable=self.TempLabelVariable,anchor="w") 
      self.TempLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+19) 
      self.chk_temp_var = Tkinter.IntVar() 
      self.chk_temp     = Tkinter.Checkbutton(self,text="Enable",variable=self.chk_temp_var) 
      self.chk_temp.grid(column=self.ColumnOffset+2,row=self.RowOffset+19) 
      # debug  
      self.UtilDebugLabelVariable = Tkinter.StringVar()
      self.UtilDebugLabelVariable.set("Debug Mode") 
      self.UtilDebugLabel = Tkinter.Label(self,textvariable=self.UtilDebugLabelVariable,anchor="w") 
      self.UtilDebugLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+20) 
      # verbosity  
      self.UtilVerbLabelVariable = Tkinter.StringVar()
      self.UtilVerbLabelVariable.set("Verbosity") 
      self.UtilVerbLabel = Tkinter.Label(self,textvariable=self.UtilVerbLabelVariable,anchor="w") 
      self.UtilVerbLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+21) 
      # test mode  
      self.UtilTestLabelVariable = Tkinter.StringVar()
      self.UtilTestLabelVariable.set("Test Mode") 
      self.UtilTestLabel = Tkinter.Label(self,textvariable=self.UtilTestLabelVariable,anchor="w") 
      self.UtilTestLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+22) 
      # test mode  
      self.UtilDelayTimeLabelVariable = Tkinter.StringVar()
      self.UtilDelayTimeLabelVariable.set("Delay Time") 
      self.UtilDelayTimeLabel = Tkinter.Label(self,textvariable=self.UtilDelayTimeLabelVariable,anchor="w") 
      self.UtilDelayTimeLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+23) 
      # reference frequency 
      # self.UtilRefFreqLabelVariable = Tkinter.StringVar()
      # self.UtilRefFreqLabelVariable.set("PTS160 Frequency") 
      # self.UtilRefFreqLabel = Tkinter.Label(self,textvariable=self.UtilRefFreqLabelVariable,anchor="w") 
      # self.UtilRefFreqLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+22) 

      # entry fields  
      # delay time  
      self.entryDelayTimeVariable = Tkinter.StringVar() 
      self.entryDelayTimeVariable.set(u"Delay time") 
      self.entryDelayTime = Tkinter.Entry(self,width=10,textvariable=self.entryDelayTimeVariable) 
      self.entryDelayTime.grid(column=self.ColumnOffset+1,row=self.RowOffset+23,sticky='EW')     
      # file name 
      self.UtilFNLabelVariable = Tkinter.StringVar()
      self.UtilFNLabelVariable.set("Configuration Label") 
      self.UtilFNLabel = Tkinter.Label(self,textvariable=self.UtilFNLabelVariable,anchor="w") 
      self.UtilFNLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+25) 

      # reference frequency 
      # self.entryUtilRefFreqVariable = Tkinter.StringVar() 
      # self.entryUtilRefFreqVariable.set(u"Frequency") 
      # self.entryUtilRefFreq = Tkinter.Entry(self,width=10,textvariable=self.entryUtilRefFreqVariable) 
      # self.entryUtilRefFreq.grid(column=self.ColumnOffset+1,row=self.RowOffset+22,sticky='EW')     

      # pull down menus
      # temperature sensor 
      self.tempName  = Tkinter.StringVar() 
      self.tempName.set('PT1000')  
      self.opt_temp = Tkinter.OptionMenu(self,self.tempName,*self.tempChoices)
      self.opt_temp.grid(column=self.ColumnOffset+1,row=self.RowOffset+19)  
      # debug mode  
      self.util_debug_val = Tkinter.StringVar() 
      self.util_debug_val.set('off')
      self.opt_util_debug_val = Tkinter.OptionMenu(self,self.util_debug_val,*self.DebugChoices) 
      self.opt_util_debug_val.grid(column=self.ColumnOffset+1,row=self.RowOffset+20)  
      # verbosity    
      self.util_verb_val = Tkinter.StringVar() 
      self.util_verb_val.set('0')
      self.opt_util_verb_val = Tkinter.OptionMenu(self,self.util_verb_val,*self.VerbChoices) 
      self.opt_util_verb_val.grid(column=self.ColumnOffset+1,row=self.RowOffset+21)  
      # test mode    
      self.util_test_val = Tkinter.StringVar() 
      self.util_test_val.set('0')
      self.opt_test_val = Tkinter.OptionMenu(self,self.util_test_val,*self.TestChoices) 
      self.opt_test_val.grid(column=self.ColumnOffset+1,row=self.RowOffset+22)  
      # delay time units     
      self.util_delay_time_unit_val = Tkinter.StringVar() 
      self.util_delay_time_unit_val.set('units')
      self.opt_delay_time_val = Tkinter.OptionMenu(self,self.util_delay_time_unit_val,*self.TimeChoices) 
      self.opt_delay_time_val.grid(column=self.ColumnOffset+2,row=self.RowOffset+23)  
      # ref frequency units    
      # self.util_ref_freq_unit_val = Tkinter.StringVar() 
      # self.util_ref_freq_unit_val.set('units')
      # self.opt_ref_freq_val = Tkinter.OptionMenu(self,self.util_ref_freq_unit_val,*FreqChoices) 
      # self.opt_ref_freq_val.grid(column=self.ColumnOffset+2,row=self.RowOffset+22)  

      # comments field 
      self.CommentsLabelVariable = Tkinter.StringVar() 
      self.CommentsLabelVariable.set("Comments") 
      self.CommentsLabel = Tkinter.Label(self,textvariable=self.CommentsLabelVariable,anchor="w") 
      self.CommentsLabel.grid(column=self.ColumnOffset+0,row=self.RowOffset+24) 

      self.entryCommentsVar = Tkinter.StringVar() 
      self.entryCommentsVar.set(u"Run comments") 
      self.entryComments    = Tkinter.Entry(self,textvariable=self.entryCommentsVar) 
                                       # anchor="w",fg="black",bg="white") 
      self.entryComments.grid(column=self.ColumnOffset+1,row=self.RowOffset+24,columnspan=4,sticky='EW') 

      # file name  
      self.entryUtilFNVar = Tkinter.StringVar() 
      self.entryUtilFNVar.set(u"Label (no spaces)")
      self.entryUtilFN    = Tkinter.Entry(self,textvariable=self.entryUtilFNVar)
      self.entryUtilFN.grid(column=self.ColumnOffset+1,row=self.RowOffset+25,columnspan=4,sticky='EW') 

      # ----------------------------------------------------------------------------------
      # make buttons 

      # load config 
      self.buttonImportConfig = Tkinter.Button(self,text=u"Import Probe Configs",command=self.LoadDataGold) 
      self.buttonImportConfig.grid(column=self.ColumnOffset+11,row=self.RowOffset+22,columnspan=2,sticky='EW') 
      # print config to files  
      self.buttonConfig = Tkinter.Button(self,text=u"Save Probe Configs",command=self.PrintToFileGold) 
      self.buttonConfig.grid(column=self.ColumnOffset+13,row=self.RowOffset+22,columnspan=2,sticky='EW',) 

      # load config 
      self.buttonImportConfig = Tkinter.Button(self,text=u"Import Configuration...",command=self.ImportConfig) 
      self.buttonImportConfig.grid(column=self.ColumnOffset+11,row=self.RowOffset+23,columnspan=4,sticky='EW') 
      # print config to files  
      # self.buttonConfig = Tkinter.Button(self,text=u"Apply Configuration",command=self.PrintToFile) 
      # self.buttonConfig.grid(column=self.ColumnOffset+13,row=self.RowOffset+22,columnspan=2,sticky='EW',) 
      # run  
      self.buttonRun = Tkinter.Button(self,text=u"Run",command=self.CheckHardware) 
      self.buttonRun.grid(column=self.ColumnOffset+11,row=self.RowOffset+24,columnspan=4,sticky='EW') 

      # turn off FPGA and function generator  
      # self.buttonStop = Tkinter.Button(self,text=u"Stop") 
      # self.buttonStop.grid(column=self.ColumnOffset+5,row=self.RowOffset+19,columnspan=1,sticky='EW') 

      self.buttonPlot = Tkinter.Button(self,text=u"Plot Data",command=self.UpdatePlots) 
      self.buttonPlot.grid(column=self.ColumnOffset+11,row=self.RowOffset+25,columnspan=2,sticky='EW') 

      # quit the program 
      self.buttonQuit = Tkinter.Button(self,text=u"Quit",command=quit) 
      self.buttonQuit.grid(column=self.ColumnOffset+13,row=self.RowOffset+25,columnspan=2,sticky='EW') 
      # ----------------------------------------------------------------------------------
      # status field  
      description         = "MSW = Mechanical Switch Gate; RFT = RF Transmit Gate; RFR = RF Receive Gate; Tom = Tomco; Off = Offset; Dur = Duration"
      self.StatusVariable = Tkinter.StringVar() 
      self.StatusVariable.set(u"Fill out the fields above before clicking 'Run.'  You can also import a configuration from 'Import Configuration....'  Be sure your entries are correct.") 
      self.Status  = Tkinter.Label(self,textvariable=self.StatusVariable,
                                   anchor="w",fg="white",bg="blue") 
      self.Status.grid(column=self.ColumnOffset+0,row=self.RowOffset+26,columnspan=15,sticky='ew') 
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
       self.StatusVariable.set("Running...") 
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
   # ----------------------------------------------------------------------------------
   # def NoButton(self): 
   #     self.StatusVariable.set("Start-of-run cancelled.") 
   #     self.CHWindow.destroy 
   # ----------------------------------------------------------------------------------
   def ImportConfig(self): 
       self.counter   += 1
       self.IsImported = 1 
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
   #---------------------------------------------------------------------------- 
   def GetFileNameConf(self):
       initdir = self.MyHOME + "input/configs/" 
       fn = askopenfilename(initialdir=initdir) # show an "Open" dialog box and return the path to the file
       self.ConfVar.set(fn) 
   #---------------------------------------------------------------------------- 
   def LoadData(self): 
       self.StatusVariable.set("Loading configuration...") 
       config_fn = self.ConfVar.get() 
       self.LoadDataChConfig(config_fn) 
       # general vars 
       fn_prefix       = self.MyHOME + "input/configs/files/" 
       global_path     = fn_prefix   + self.global_fn     + "_" + self.ConfVarSelection.get() + ".dat"
       delay_time_path = fn_prefix   + self.delay_time_fn + "_" + self.ConfVarSelection.get() + ".dat"
       fpga_path       = fn_prefix   + self.fpga_fn       + "_" + self.ConfVarSelection.get() + ".dat"
       fg_path         = fn_prefix   + self.fg_fn         + "_" + self.ConfVarSelection.get() + ".dat" 
       fgpi2_path      = fn_prefix   + self.fgpi2_fn      + "_" + self.ConfVarSelection.get() + ".dat" 
       adc_path        = fn_prefix   + self.adc_fn        + "_" + self.ConfVarSelection.get() + ".dat" 
       util_path       = fn_prefix   + self.util_fn       + "_" + self.ConfVarSelection.get() + ".dat" 
       temp_path       = fn_prefix   + self.temp_fn       + "_" + self.ConfVarSelection.get() + ".dat" 
       self.LoadDataGlobal(global_path) 
       if self.IsGolden==0:
          # did we load the "golden" config files already? if not, load FPGA data  
          self.LoadDataFPGA(fpga_path)
          self.LoadDataFGPi2(fgpi2_path)
       self.LoadDataFG(fg_path)
       self.LoadDataADC(adc_path)
       self.LoadDataTemp(temp_path) 
       self.LoadDataUtil(util_path) 
       # update status bar 
       self.StatusVariable.set("Configuration loaded.") 
   #---------------------------------------------------------------------------- 
   def LoadDataGold(self): 
       self.StatusVariable.set("Loading probe configuration...") 
       self.IsGolden = 1 
       self.LoadDataFPGAGold()
       # self.LoadDataFGPi2Gold()
       # update status bar 
       self.StatusVariable.set("Configuration loaded.") 
   #---------------------------------------------------------------------------- 
   def LoadDataGlobal(self,fn):  
       # global on/off variable  
       fileG = open(fn, 'r')
       for line in fileG:
          entry = line.split() # puts every entry of a line in an array called entry
          if entry[0]!=self.HASH and entry[0]!=self.EOF: 
            self.chk_global_on_off_var.set( entry[1] )  
       fileG.close() 
   #---------------------------------------------------------------------------- 
   def LoadDataChConfig(self,fn):  
       lines       = [ line.rstrip('\n') for line in open(fn) ]
       cnfList     = [] 
       config_name = lines[0] 
       self.entryUtilFNVar.set(config_name) 
       self.ConfVarSelection.set(config_name) 
       N = len(lines) 
       # set the configuration name in the GUI for each channel 
       # first get the list of configs 
       if self.IsGolden==0:  
          for i in xrange(1,N):
             parsed = lines[i].split("_") 
             cnfList.append( parsed[0] )
          M = len(cnfList)
          for i in xrange(0,M):
             cnf_str = self.GetConfigVal( cnfList[i] )  
             self.cnfCh[i].set( cnf_str )  
           
       if self.IsDebug==1:
          print "Number of configs: %d" % (M)  
          for i in xrange(0,M): print "[LoadDataChConfig]: %s" %( cnfList[i] ) 
   #---------------------------------------------------------------------------- 
   def LoadDataFPGA(self,fn):  
       eof      = "99" 
       fileFPGA = open(fn, 'r')
       for line in fileFPGA:
          entry = line.split() # puts every entry of a line in an array called entry
          # print entry 
          if entry[0]!=self.HASH and entry[0]!=eof: 
             for i in xrange(0,4): 
                if entry[0]==self.ChID[i]:    
                   # mechanical switch  
                   self.entryMechOffVariable[i].set(entry[2])       
                   self.entryMechDurVariable[i].set(entry[3])      
                   self.unitMech[i].set(entry[4])
                   # rf transmit  
                   self.entryRFTransOffVariable[i].set(entry[5])       
                   self.entryRFTransDurVariable[i].set(entry[6])      
                   self.unitRFTrans[i].set(entry[7])
                   # tomco   
                   self.entryTomcoOffVariable[i].set(entry[8])       
                   self.entryTomcoDurVariable[i].set(entry[9])      
                   self.unitTomco[i].set(entry[10])
                   self.chkTomcoEnableVariable[i].set(entry[11])
                   # rf receive   
                   self.entryRFRecOffVariable[i].set(entry[12])       
                   self.entryRFRecDurVariable[i].set(entry[13])      
                   self.unitRFRec[i].set(entry[14])
       fileFPGA.close()
   #---------------------------------------------------------------------------- 
   def LoadDataFPGAGold(self): 
       # for when we want to load a "golden" config for each channel that's not OFF.
       j = 0 
       prefix = self.MyHOME + "input/configs/files/"
       for i in xrange(0,4):
         if self.cnfCh[i].get()!='OFF': 
            tag = self.GetConfigLabel(i) 
            fn  = prefix + tag + "_gold.dat"
            file = open(fn,"r") 
            for line in file: 
               entry = line.split() 
               # mechanical switch  
               self.entryMechOffVariable[i].set(entry[2])       
               self.entryMechDurVariable[i].set(entry[3])      
               self.unitMech[i].set(entry[4])
               # rf transmit  
               self.entryRFTransOffVariable[i].set(entry[5])       
               self.entryRFTransDurVariable[i].set(entry[6])      
               self.unitRFTrans[i].set(entry[7])
               # tomco   
               self.entryTomcoOffVariable[i].set(entry[8])       
               self.entryTomcoDurVariable[i].set(entry[9])      
               self.unitTomco[i].set(entry[10])
               self.chkTomcoEnableVariable[i].set(entry[11])
               # rf receive   
               self.entryRFRecOffVariable[i].set(entry[12])       
               self.entryRFRecDurVariable[i].set(entry[13])      
               self.unitRFRec[i].set(entry[14])
            file.close() 
   #---------------------------------------------------------------------------- 
   def LoadDataFG(self,fn):  
       # function generator
       bnc       = "bnc"
       ntype     = "ntype"
       freq      = "frequency"
       freq_val  = "0"
       freq_unit = "ND"
       volt_unit = "ND"
       fileFG = open(fn, 'r')
       for line in fileFG:
          entry = line.split() # puts every entry of a line in an array called entry
          if entry[0]!=self.HASH and entry[0]!=self.EOF: 
             # if entry[0]==bnc: 
             #    # self.entryFGBNCVoltVariable.set(entry[2]) 
             #    # self.unit_str_volt_fg_bnc.set(entry[3]) 
             #    # if entry[1]=="on":  self.chk_fg_bnc_var.set(1) 
             #    # if entry[1]=="off": self.chk_fg_bnc_var.set(0) 
             if entry[0]==ntype: 
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
   #---------------------------------------------------------------------------- 
   def LoadDataFGPi2(self,fn):  
       fileFGPi2 = open(fn, 'r')
       # fmin = 0 
       # fmax = 1 
       for line in fileFGPi2:
          entry = line.split() # puts every entry of a line in an array called entry
          # print entry 
          if entry[0]!=self.HASH and entry[0]!=self.NINETYNINE: 
             # for i in xrange(fmin,fmax): 
             # if entry[0]==self.ChID[i]:    
             self.chk_fgpi2_var.set(entry[1]) 
             self.entryPi2FreqVariable.set(entry[2])       
             self.unitPi2Freq.set(entry[3])
             self.entryPi2AmplVariable.set(entry[4])      
             iunit = entry[5]
             if(entry[5]=="Watts"): iunit = "W"
             self.unitPi2Ampl.set(iunit)
       fileFGPi2.close()
   #---------------------------------------------------------------------------- 
   # def LoadDataFGPi2Gold(self):  
   #     # for when we want to load a "golden" config for each channel that's not OFF.
   #     prefix = self.MyHOME + "input/configs/files/"
   #     fmin = 0 
   #     fmax = 1 
   #     for i in xrange(fmin,fmax): 
   #       if self.cnfCh[i].get()!='OFF': 
   #          tag = self.GetConfigLabel(i) 
   #          fn  = prefix + "sg382_pi2_" + tag + "_gold.dat"
   #          file = open(fn,"r") 
   #          for line in file: 
   #             entry = line.split() # puts every entry of a line in an array called entry
   #             # print entry 
   #             self.chk_fgpi2_var[i].set(entry[1]) 
   #             self.entryPi2FreqVariable[i].set(entry[2])       
   #             self.unitPi2Freq[i].set(entry[3])
   #             self.entryPi2AmplVariable[i].set(entry[4])      
   #             iunit = entry[5]
   #             if(entry[5]=="Watts"): iunit = "W"
   #             self.unitPi2Ampl[i].set(iunit)
   #          file.close()
   #---------------------------------------------------------------------------- 
   def LoadDataADC(self,fn):  
       # ADC 
       id_num        = "adc_id"
       ch_num        = "channel_number"
       nof           = "number_of_events"
       freq          = "frequency"
       ext_clk       = "external_clock"
       ext_freq_val  = 0
       ext_freq_unit = "ND"
       fileADC = open(fn, 'r')
       for line in fileADC:
          entry = line.split() # puts every entry of a line in an array called entry
          if entry[0]!=self.HASH and entry[0]!=self.EOF: 
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
   #---------------------------------------------------------------------------- 
   def LoadDataUtil(self,fn):  
       # utilities 
       debug          = "debug_mode"
       verb           = "verbosity"
       test           = "test_mode"
       dt             = "delay_time"
       ref_freq       = "rf_frequency"
       lab            = "lab"
       ref_freq_val   = 0
       ref_freq_str   = "0" 
       delay_time_val = 0 
       delay_time_str = "0"
       dt_unit        = "ND" 
       fileUtil = open(fn, 'r')
       for line in fileUtil:
          entry = line.split() # puts every entry of a line in an array called entry
          # print entry 
          if entry[0]!=self.HASH and entry[0]!=self.EOF: 
             if entry[0]==debug: 
                if entry[1]=="1": self.util_debug_val.set("on") 
                if entry[1]=="0": self.util_debug_val.set("off") 
             elif entry[0]==verb: 
                self.util_verb_val.set(entry[1]) 
             elif entry[0]==test: 
                self.util_test_val.set(entry[1]) 
             elif entry[0]==ref_freq: 
                ref_freq_val = float(entry[1]) 
             elif entry[0]==dt: 
                delay_time_val = entry[1]
             elif entry[0]==lab: 
                self.chk_lab_var.set(entry[1]) 
       # reference frequency convert to MHz (when would it ever not be?) 
       # ref_freq_str = ref_freq_val/1E+6 
       # self.entryUtilRefFreqVariable.set(ref_freq_str)  
       # self.util_ref_freq_unit_val.set("MHz")  
       # delay time 
       delay_time_str = delay_time_val  
       self.entryDelayTimeVariable.set(delay_time_str) 
       self.util_delay_time_unit_val.set("s") 
       fileUtil.close()
#---------------------------------------------------------------------------- 
   def LoadDataTemp(self,fn):  
       # temperature data 
       sensor_id = "sensor_id"
       enable    = "enable"
       fileT = open(fn, 'r')
       for line in fileT:
          entry = line.split() # puts every entry of a line in an array called entry
          if entry[0]!=self.HASH and entry[0]!=self.EOF: 
            if entry[0]==sensor_id: 
               if entry[1]=='1000':  self.tempName.set( 'PT1000' )  
               if entry[1]=='17464': self.tempName.set( 'USP17464' )  
            if entry[0]==enable:   self.chk_temp_var.set( entry[1] )  
       fileT.close() 
   #----------------------------------------------------------------------------
   def PrintToFile(self):   
       # get file names ready 
       config_tag      = self.entryUtilFNVar.get() 
       conf_path       = self.MyHOME + "input/configs/" + config_tag + ".cnf"
       prefix          = "./input/configs/files/"
       global_path     = prefix + self.global_fn + "_" + config_tag + ".dat" 
       fpga_path       = prefix + self.fpga_fn   + "_" + config_tag + ".dat"  
       fg_path         = prefix + self.fg_fn     + "_" + config_tag + ".dat" 
       fgpi2_path      = prefix + self.fgpi2_fn  + "_" + config_tag + ".dat" 
       adc_path        = prefix + self.adc_fn    + "_" + config_tag + ".dat" 
       util_path       = prefix + self.util_fn   + "_" + config_tag + ".dat" 
       temp_path       = prefix + self.temp_fn   + "_" + config_tag + ".txt" 
       com_path        = prefix + self.com_fn    + "_" + config_tag + ".txt" 
       self.PrintToFileGlobal(global_path) 
       self.PrintToFileFPGA(fpga_path)
       self.PrintToFileFG(fg_path) 
       self.PrintToFileFGPi2(fgpi2_path) 
       self.PrintToFileADC(adc_path) 
       self.PrintToFileUtil(util_path) 
       self.PrintToFileTemp(temp_path)
       self.PrintToFileCom(com_path)
       self.PrintToFileConfig(conf_path) 
   #----------------------------------------------------------------------------
   def PrintToFileGlobal(self,fn): 
      global_tag   = "global_on_off"
      header       = "%-1s    ID    value" % (self.HASH) 
      global_state = self.chk_global_on_off_var.get()  
      global_str   = "%-20s %-5d" %(global_tag,global_state )   
      eof_str      = "%-20s %-5s" %(self.EOF,self.NINETYNINE )   
      if self.IsDebug==0: 
         globalFile = open(fn,"w")
         globalFile.write(header+"\n") 
         globalFile.write(global_str +"\n")
         globalFile.write(eof_str+"\n") 
         globalFile.close() 
      elif self.IsDebug==1: 
         print fn 
         print header 
         print global_str
         print eof_str  
   #----------------------------------------------------------------------------
   def PrintToFileConfig(self,fn):  
      prefix     = "input/configs/files/"
      config_tag = str( self.entryUtilFNVar.get() )  
      if self.IsDebug==0:  
         confFile   = open(fn,"w") 
         confFile.write(config_tag+"\n")
         for i in xrange(0,4): 
               ch_str = self.GetConfigLabel(i) + "_" + config_tag  
               confFile.write(ch_str+"\n") 
         confFile.close() 
      elif self.IsDebug==1: 
         print fn 
         print config_tag 
         for i in xrange(0,4): 
              ch_str = self.GetConfigLabel(i) + "_" + config_tag  
              print ch_str 
      # probe files: data for a given probe configuration 
      # allows the user to load from the Config pull-down menu 
      if self.IsDebug==0:  
         for i in xrange(0,4): 
            if( self.cnfCh[i].get()!='OFF' ): 
               pr_path = self.MyHOME + prefix + self.GetConfigLabel(i) + "_" + config_tag + ".dat" 
               my_str  = self.GetFPGAString(i) 
               aFile = open(pr_path,"w") 
               aFile.write(my_str+"\n") 
               aFile.close() 
      elif self.IsDebug==1: 
         for i in xrange(0,4): 
            if( self.cnfCh[i].get()!='OFF' ): 
               pr_path = prefix + self.GetConfigLabel(i) + "_" + config_tag + ".dat" 
               my_str  = self.GetFPGAString(i) 
               print pr_path + ": " + my_str  
   #----------------------------------------------------------------------------
   def PrintToFileGold(self):  
      prefix     = "input/configs/files/"
      config_tag = "gold"
      # probe files: data for a given probe configuration 
      # allows the user to load from the Config pull-down menu 
      if self.IsDebug==0:  
         for i in xrange(0,4): 
            if( self.cnfCh[i].get()!='OFF' ): 
               # FPGA data 
               fpga_path = self.MyHOME + prefix + self.GetConfigLabel(i) + "_" + config_tag + ".dat" 
               fpga_str  = self.GetFPGAString(i) 
               fpgaFile  = open(fpga_path,"w") 
               fpgaFile.write(fpga_str+"\n") 
               fpgaFile.close() 
         # pi/2 pulse data 
         # fgpi2_path = self.MyHOME + prefix + "sg382_pi2_" + "_" + config_tag + ".dat" 
         # fgpi2_str  = self.GetFGPi2String() 
         # fgpi2File  = open(fgpi2_path,"w") 
         # fgpi2File.write(fgpi2_str+"\n") 
         # fgpi2File.close() 
      elif self.IsDebug==1: 
         # for i in xrange(0,4): 
         if( self.cnfCh[i].get()!='OFF' ): 
            fpga_path  = prefix + self.GetConfigLabel(i) + "_" + config_tag + ".dat" 
            fpga_str   = self.GetFPGAString(i) 
            # fgpi2_path = self.MyHOME + prefix + "sg382_pi2_" + self.GetConfigLabel(i) + "_" + config_tag + ".dat" 
            # fgpi2_str  = self.GetFGPi2String() 
            print fpga_path  + ": " + fpga_str  
            # print fgpi2_path + ": " + fgpi2_str  
   #----------------------------------------------------------------------------
   def PrintToFileFG(self,fn):  
       # function generator 
       fg_header  = "# type   state      value           units"
       freq_str   = self.GetFuncGenString(3) 
       bnc_str    = self.GetFuncGenString(1)  
       ntype_str  = self.GetFuncGenString(2)  
       eof_fg_str = "%-20s %-20s %-20s %-20s" %(self.EOF,self.blSTATE,self.ZERO,self.ND)
       if self.IsDebug==0: 
          # write to file 
          fgFile = open(fn  ,"w")
          fgFile.write(fg_header +"\n") 
          fgFile.write(freq_str  +"\n")
          fgFile.write(bnc_str   +"\n")
          fgFile.write(ntype_str +"\n")
          fgFile.write(eof_fg_str+"\n") 
          fgFile.close() 
       elif self.IsDebug==1: 
          print fn 
          print fg_header
          print freq_str
          print bnc_str
          print ntype_str 
          print eof_fg_str 
   #----------------------------------------------------------------------------
   def PrintToFileFGPi2(self,fn):
       # init vars 
       fgpi2_header  = "# ID    state freq   units    ampl    units"
       eof_fgpi2_str = "%-5s %-5s %-5s %-5s %-5s %-5s" % (self.NINETYNINE,self.ZERO,self.ZERO,self.ND,self.ZERO,self.ND) 
       fmin = 0 
       fmax = 4  
       # fg file 
       if self.IsDebug==0: 
          # writing to the file  
          fgpi2File = open(fn,"w")
          fgpi2File.write(fgpi2_header+"\n")
          # for i in xrange(fmin,fmax): 
          #    if( self.cnfCh[i].get()!='OFF' ):
          my_str = self.GetFGPi2String() 
          fgpi2File.write(my_str+"\n") 
          fgpi2File.write(eof_fgpi2_str+"\n") 
          fgpi2File.close() 
       elif self.IsDebug==1:
          # print to screen  
          print fn 
          print fgpi2_header
          # for i in xrange(fmin,fmax): 
          #    if( self.cnfCh[i].get()!='OFF' ):
          my_str = self.GetFGPi2String() 
          print my_str 
          print eof_fgpi2_str
   #----------------------------------------------------------------------------
   def PrintToFileADC(self,fn):  
       # adc file
       adc_header      = "# ID     value      units"
       adc_id_str      = self.GetADCString(1) 
       ch_num_str      = self.GetADCString(2) 
       pulse_num_str   = self.GetADCString(3) 
       sample_freq_str = self.GetADCString(4) 
       extern_clk_str  = self.GetADCString(6) 
       eof_adc_str     = "%-20s %-20s %-20s" %(self.EOF,self.ZERO,self.ND)
       if self.IsDebug==0: 
          # write to file  
          adcFile = open(fn,"w")
          adcFile.write(adc_header      +"\n")
          adcFile.write(adc_id_str      +"\n")
          adcFile.write(ch_num_str      +"\n")
          adcFile.write(pulse_num_str   +"\n")
          adcFile.write(sample_freq_str +"\n")
          adcFile.write(extern_clk_str  +"\n")
          adcFile.write(eof_adc_str     +"\n") 
          adcFile.close() 
       elif self.IsDebug==1:  
          print fn 
          print adc_header     
          print adc_id_str     
          print ch_num_str     
          print pulse_num_str  
          print sample_freq_str
          print extern_clk_str 
          print eof_adc_str    
   #----------------------------------------------------------------------------
   def PrintToFileUtil(self,fn):  
       # utilities
       util_header  = "# ID     value"
       debug_str    = self.GetUtilString(1)  
       verb_str     = self.GetUtilString(2)  
       test_str     = self.GetUtilString(3)  
       # rf_str       = self.GetUtilString(4)  
       dt_str       = self.GetUtilString(5) 
       lab_str      = self.GetUtilString(6) 
       eof_util_str = "%-20s %-20s " %(self.EOF,self.NINETYNINE)
       if self.IsDebug==0: 
          # write to file  
          utilFile = open(fn,"w")
          utilFile.write(util_header  + "\n")        
          utilFile.write(debug_str    + "\n")        
          utilFile.write(verb_str     + "\n")        
          utilFile.write(test_str     + "\n")        
          utilFile.write(dt_str       + "\n")        
          # utilFile.write(rf_str       + "\n")       
          utilFile.write(lab_str      + "\n") 
          utilFile.write(eof_util_str + "\n")        
          utilFile.close()
       elif self.IsDebug==1:
          print fn  
          print util_header  
          print debug_str    
          print verb_str     
          print test_str     
          print dt_str       
          # print rf_str       
          print eof_util_str 
   #----------------------------------------------------------------------------
   def PrintToFileTemp(self,fn): 
      sensor_tag   = "sensor_id"
      enable_tag   = "enable"
      header       = "%-1s    type    value" % (self.HASH) 
      sensor_num   = 0 
      if self.tempName.get()=='PT1000':   sensor_num = 1000 
      if self.tempName.get()=='USP17464': sensor_num = 17464 
      enable_state = self.chk_temp_var.get()  
      sensor_str   = "%-20s %-5d" %(sensor_tag,sensor_num )   
      enable_str   = "%-20s %-5d" %(enable_tag,enable_state )   
      eof_str      = "%-20s %-5d" %(self.EOF, int(self.ZERO) )   
      if self.IsDebug==0: 
         theFile = open(fn,"w")
         theFile.write(header+"\n") 
         theFile.write(sensor_str +"\n")
         theFile.write(enabl_str +"\n")
         theFile.write(eof_str+"\n") 
         theFile.close() 
      elif self.IsDebug==1: 
         print fn 
         print header 
         print sensor_str
         print enable_str
         print eof_str  
   #----------------------------------------------------------------------------
   def PrintToFileCom(self,fn):  
       # comments
       com_str = self.entryCommentsVar.get() 
       if self.IsDebug==0: 
          # write to file 
          commentFile = open(fn,"w") 
          commentFile.write(com_str+"\n")
          commentFile.close()  
       elif self.IsDebug==1: 
          print fn 
          print com_str
       # update status banner 
       self.StatusVariable.set("Configuration printed to file.") 
   #----------------------------------------------------------------------------
   def PrintToFileFPGA(self,fn):
       # init vars 
       fpga_header_1  = "# ID     on/off"
       fpga_header_2  = " mech-sw offset     mech-sw duration     units"
       fpga_header_3  = " rf-trans offset    rf-trans duration    units"
       fpga_header_4  = " tomco offset       tomco duration       units    tomco enable"
       fpga_header_5  = " rf-rec offset      rf-rec duration      units"
       fpga_header    = fpga_header_1 + fpga_header_2 + fpga_header_3 + fpga_header_4 + fpga_header_5 
       eof_1          = "%-5s %-5s"           % (self.NINETYNINE,self.blSTATE)  
       eof_2          = "%-5s %-5s %-5s"      % (self.ZERO,self.ZERO,self.ND)   
       eof_3          = "%-5s %-5s %-5s %-5s" % (self.ZERO,self.ZERO,self.ND,self.ZERO)  
       eof_fpga_str   = eof_1 + eof_2 + eof_2 + eof_3 + eof_2  
       # fpga file 
       if self.IsDebug==0: 
          # writing to the file  
          fpgaFile = open(fn,"w")
          fpgaFile.write(fpga_header+"\n")
          for i in xrange(0,4): 
             if( self.cnfCh[i].get()!='OFF' ):
                my_str = self.GetFPGAString(i) 
                fpgaFile.write(my_str+"\n") 
          fpgaFile.write(eof_fpga_str+"\n") 
          fpgaFile.close() 
       elif self.IsDebug==1:
          # print to screen  
          print fn 
          print fpga_header
          for i in xrange(0,4):
             if( self.cnfCh[i].get()!='OFF' ):
                my_str = self.GetFPGAString(i) 
                print my_str 
          print eof_fpga_str
   #----------------------------------------------------------------------------
   def GetConfigLabel(self,i):
      j = -1
      if self.cnfCh[i].get()=='OFF':        j = 0   
      if self.cnfCh[i].get()=='Short Coil': j = 1   
      if self.cnfCh[i].get()=='Long Coil':  j = 2   
      if self.cnfCh[i].get()=='Cyl. Probe': j = 3   
      if self.cnfCh[i].get()=='Sph. Probe': j = 4   
      if self.cnfCh[i].get()=='Other':      j = 5  
      if self.cnfCh[i].get()=='3He':        j = 6  
      my_str = self.ConfigFN[j]  
      return my_str   
   #----------------------------------------------------------------------------
   def GetConfigVal(self,in_str):
      j = -1
      if in_str=='off':        j = 0   
      if in_str=='short-coil': j = 1   
      if in_str=='long-coil':  j = 2   
      if in_str=='cyl-probe':  j = 3   
      if in_str=='sph-probe':  j = 4   
      if in_str=='other':      j = 5  
      if in_str=='3He':        j = 6  
      my_str = self.ConfigChoices[j]  
      return my_str   
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
      beg_str  = "%d %s"          % (ch+1,state)   
      msw_str  = "%s %s %s"       % (msw_off,msw_dur,msw_unit) 
      rft_str  = "%s %s %s"       % (rft_off,rft_dur,rft_unit) 
      tom_str  = "%s %s %s %s"    % (tom_off,tom_dur,tom_unit,tom_enbl) 
      rfr_str  = "%s %s %s"       % (rfr_off,rfr_dur,rfr_unit) 
      my_str   = "%s %s %s %s %s" % (beg_str,msw_str,rft_str,tom_str,rfr_str) 
      return my_str  
   #----------------------------------------------------------------------------
   def GetFuncGenString(self,type):
      bnc_state       = "off"
      ntype_state     = "off"
      label           = "none"
      value           = "none"
      state           = "none"
      units           = "none"
      # if self.chk_fg_bnc_var.get():   bnc_state   = "on"
      if self.chk_fg_ntype_var.get(): ntype_state = "on"
      freq            = self.entryFGFreqVariable.get()  
      freq_unit       = self.unit_str_freq_fg.get() 
      # bnc_volt        = self.entryFGBNCVoltVariable.get()  
      # bnc_volt_unit   = self.unit_str_volt_fg_bnc.get() 
      ntype_volt      = self.entryFGNTypeVoltVariable.get()  
      ntype_volt_unit = self.unit_str_volt_fg_ntype.get() 
      if type==1: 
         # always disabled 
         label = "bnc"
         value = 1E-3 
         state = "off"  
         units = "Vpp" 
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
   def GetFGPi2String(self):
      # gather all values and put into a string  
      ch        = 1 
      units     = "ND"
      freq      = self.entryPi2FreqVariable.get() 
      ampl      = self.entryPi2AmplVariable.get() 
      freq_unit = self.unitPi2Freq.get() 
      ampl_unit = self.unitPi2Ampl.get() 
      if ( self.unitPi2Ampl.get()=="W"): 
         ampl_unit = "Watts" 
      state     = self.chk_fgpi2_var.get() 
      my_str    = "%s %s %s %s %s %s" % (ch,state,freq,freq_unit,ampl,ampl_unit)   
      return my_str  
   #----------------------------------------------------------------------------
   def GetADCString(self,type): 
      adc_id_val = self.entryADCNumVariable.get() 
      ch_num     = self.entryADCchVariable.get()
      pulse_num  = self.entryADCPulseVariable.get() 
      freq       = self.adc_freq_val.get()
      freq_unit  = "MHz"
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
      lab_val   = self.chk_lab_var.get() 
      # freq      = self.entryUtilRefFreqVariable.get()
      # freq_unit = self.util_ref_freq_unit_val.get()
      # freq_dbl  = float(freq) 
      dt_val    = self.entryDelayTimeVariable.get()
      dt_unit   = self.util_delay_time_unit_val.get()
      dt_dbl    = float(dt_val) 
 
      if debug_str=="off": debug_val = "0"
      if debug_str=="on":  debug_val = "1"
     
      # if freq_unit=="kHz": freq_dbl = freq_dbl*1.E+3  
      # if freq_unit=="MHz": freq_dbl = freq_dbl*1.E+6  
      # if freq_unit=="GHz": freq_dbl = freq_dbl*1.E+9  
      # freq = freq_dbl

      if dt_unit=="ms": dt_dbl = dt_dbl*1E-3; 
      if dt_unit=="us": dt_dbl = dt_dbl*1E-6; 
      dt_val = dt_dbl

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
         value = 0 
      elif type==5: 
         label = "delay_time"
         value = dt_val
      elif type==6: 
         label = "lab"
         value = lab_val

      my_str   = "%-20s %-20s" %(label,value)
      return my_str 
   #----------------------------------------------------------------------------
   def RunDAQ(self): 
      self.StatusVariable.set("Running...") 
      self.PrintToFile()  
      # run the DAQ: first create symbolic links to the current configuration,
      # then run the system (bash script that starts the C code)  
      HOME        = self.MyHOME 
      cd_input    = "cd " + HOME + "input/"
      cd_home     = "cd " + HOME
      run_cmd     = "./run_nmr.sh"          # runs the C code 
      symlink     = "ln -s "
      config_path = "input/configs/files/" 
      prefix_1    = HOME + config_path 
      prefix_2    = HOME + "input/" 
      fileList    = [self.global_fn,self.fpga_fn,self.fg_fn,self.fgpi2_fn,self.adc_fn,self.util_fn,self.temp_fn,self.com_fn]
      suffix      = ".dat" 
      N           = len(fileList) 
      cntr        = 0 
      # loop over files and make symbolic links 
      for entry in fileList: 
         # construct source and target paths 
         if (cntr==N-1): 
            suffix = ".txt"
         else: 
            suffix = ".dat"
         src_path = prefix_1 + entry + "_" + self.entryUtilFNVar.get() + suffix  
         tgt_path = prefix_2 + entry + suffix  
         # delete current file at target location  
         cmd      = "rm " + tgt_path 
         os.system(cmd)  
         # symlink source to target  
         cmd      = symlink + src_path + " " + tgt_path 
         os.system(cd_input) 
         os.system(cmd) 
         # check if things look good 
         # if (os.path.isfile(tgt_path) ): 
            # things look good, do nothing 
            # print "[NMRDAQ]: symbolic link from %s to %s created." % (src_path,tgt_path) 
         if ( (os.path.isfile(tgt_path))!=True ): 
            print "[NMRDAQ]: Symbolic link from %s to %s FAILED!" % (src_path,tgt_path)
         cntr = cntr+1 
      # cd back to main dir
      os.system(cd_home)
      # start the C code
      if(self.IsDebug==0): os.system(run_cmd) 
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
