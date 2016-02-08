#! /usr/bin/python 

import Tkinter  
import os
from decimal import Decimal 
from tkFileDialog import askopenfilename

#--------------------------------------------------------------------------------
class daq(Tkinter.Tk): 

   def __init__(self,parent): 
      Tkinter.Tk.__init__(self,parent)
      self.parent = parent # reference to the parent 
      self.initialize() 

   def initialize(self): 
      self.grid() 
 
      self.counter = 0 
      # self.MyHOME = "/home/gm2cal/Desktop/dflay/gui/"
      self.MyHOME = os.getcwd() + "/"  
      self.ConfVarLabel = Tkinter.StringVar()   

      # variables and lists  
      TickBox      = "on"
      self.TimeChoices  = ['units','s'  ,'ms','us']
      self.DebugChoices = ['off','on']
      self.VerbChoices  = ['0','1','2','3','4']
      self.TestChoices  = ['0','1','2','3','4','5']
      FreqValues        = ['1','10','25','50','100','125','250']
      FreqChoices       = ['units','kHz','MHz']
      FreqUnitChoices   = ['MHz']
      VoltChoices       = ['units','Vpp','rms','dBm']
      RowOffset         = 0 
      ColumnOffset      = 0

      MechSwName   = ['Mechanical Switch 1','Mechanical Switch 2','Mechanical Switch 3','Mechanical Switch 4']

      # frame 
      # self = Tkinter.Frame(self,borderwidth=2,relief=Tkinter.GROOVE) 
      # self.pack()   

      # ----------------------------------------------------------------------------------
      # FPGA 
      # ID Label for pulse timing
      self.FPGATimingLabelVariable = Tkinter.StringVar()
      self.FPGATimingLabelVariable.set("Pulse Timing")
      self.FPGATimingLabel = Tkinter.Label(self,textvariable=self.FPGATimingLabelVariable,anchor="w",font="Helvetica 11 bold")
      self.FPGATimingLabel.grid(column=ColumnOffset+0,row=RowOffset+0) 
      # global on/off 
      self.GlobalOnOffLabelVariable = Tkinter.StringVar() 
      self.GlobalOnOffLabelVariable.set("Global On/Off") 
      GlobalOnOffLabel  = Tkinter.Label(self,textvariable=self.GlobalOnOffLabelVariable,anchor="w") 
      GlobalOnOffLabel.grid(column=ColumnOffset+0,row=RowOffset+1) 
      # ID Label
      # 1 
      self.MechLabelVariable1 = Tkinter.StringVar() 
      self.MechLabelVariable1.set(MechSwName[0]) 
      MechLabel1  = Tkinter.Label(self,textvariable=self.MechLabelVariable1,anchor="w") 
      MechLabel1.grid(column=ColumnOffset+0,row=RowOffset+2) 
      # 2 
      self.MechLabelVariable2 = Tkinter.StringVar() 
      self.MechLabelVariable2.set(MechSwName[1]) 
      MechLabel2  = Tkinter.Label(self,textvariable=self.MechLabelVariable2,anchor="w") 
      MechLabel2.grid(column=ColumnOffset+0,row=RowOffset+3) 
      # 3 
      self.MechLabelVariable3 = Tkinter.StringVar() 
      self.MechLabelVariable3.set(MechSwName[2]) 
      MechLabel3  = Tkinter.Label(self,textvariable=self.MechLabelVariable3,anchor="w") 
      MechLabel3.grid(column=ColumnOffset+0,row=RowOffset+4) 
      # 4 
      self.MechLabelVariable4 = Tkinter.StringVar() 
      self.MechLabelVariable4.set(MechSwName[3]) 
      MechLabel4  = Tkinter.Label(self,textvariable=self.MechLabelVariable4,anchor="w") 
      MechLabel4.grid(column=ColumnOffset+0,row=RowOffset+5) 
      # RF 
      # 1 
      self.RFLabelVariable1 = Tkinter.StringVar() 
      self.RFLabelVariable1.set("RF Switch 1") 
      RFLabel1  = Tkinter.Label(self,textvariable=self.RFLabelVariable1,anchor="w") 
      RFLabel1.grid(column=ColumnOffset+0,row=RowOffset+6) 
      # 2 
      self.RFLabelVariable2 = Tkinter.StringVar() 
      self.RFLabelVariable2.set("RF Switch 2 (Transmit Gate)") 
      RFLabel2  = Tkinter.Label(self,textvariable=self.RFLabelVariable2,anchor="w") 
      RFLabel2.grid(column=ColumnOffset+0,row=RowOffset+7) 
      # 3 
      self.RFLabelVariable3 = Tkinter.StringVar() 
      self.RFLabelVariable3.set("RF Switch 3 (Receive Gate)") 
      RFLabel3  = Tkinter.Label(self,textvariable=self.RFLabelVariable3,anchor="w") 
      RFLabel3.grid(column=ColumnOffset+0,row=RowOffset+8) 
      # 4 
      self.RFLabelVariable4 = Tkinter.StringVar() 
      self.RFLabelVariable4.set("RF Clear") 
      RFLabel4  = Tkinter.Label(self,textvariable=self.RFLabelVariable4,anchor="w") 
      RFLabel4.grid(column=ColumnOffset+0,row=RowOffset+9) 
      # RF gate 
      self.RFLabelVariableGate = Tkinter.StringVar() 
      self.RFLabelVariableGate.set("TOMCO Gate") 
      self.RFLabelGate  = Tkinter.Label(self,textvariable=self.RFLabelVariableGate,anchor="w") 
      self.RFLabelGate.grid(column=ColumnOffset+0,row=RowOffset+10) 

      # entry fields (offset)  
      # Mech 1 
      TimeOffsetStart   = "Enter time offset"
      TimeDurationStart = "Enter time duration"
      mo_str_1 = Tkinter.StringVar()
      mo_str_2 = Tkinter.StringVar()
      mo_str_3 = Tkinter.StringVar()
      mo_str_4 = Tkinter.StringVar()
      self.entryMechOffsetVariable   = [mo_str_1,mo_str_2,mo_str_3,mo_str_4] 
      for element in self.entryMechOffsetVariable: 
         element.set(u"Enter time offset") 
      # 1 
      self.entryMechOffset1 = Tkinter.Entry(self,textvariable=self.entryMechOffsetVariable[0]) 
      self.entryMechOffset1.grid(column=ColumnOffset+1,row=RowOffset+2,sticky='EW')
      # 2 
      self.entryMechOffset2 = Tkinter.Entry(self,textvariable=self.entryMechOffsetVariable[1]) 
      self.entryMechOffset2.grid(column=ColumnOffset+1,row=RowOffset+3,sticky='EW')
      # 3 
      self.entryMechOffset3 = Tkinter.Entry(self,textvariable=self.entryMechOffsetVariable[2]) 
      self.entryMechOffset3.grid(column=ColumnOffset+1,row=RowOffset+4,sticky='EW')
      # 4 
      self.entryMechOffset4 = Tkinter.Entry(self,textvariable=self.entryMechOffsetVariable[3]) 
      self.entryMechOffset4.grid(column=ColumnOffset+1,row=RowOffset+5,sticky='EW')
      # RF  
      ro_str_1 = Tkinter.StringVar()
      ro_str_2 = Tkinter.StringVar()
      ro_str_3 = Tkinter.StringVar()
      ro_str_4 = Tkinter.StringVar()
      self.entryRFOffsetVariable   = [ro_str_1,ro_str_2,ro_str_3,ro_str_4] 
      for element in self.entryRFOffsetVariable: 
         element.set(u"Enter time offset") 
      # 1 
      self.entryRFOffset1 = Tkinter.Entry(self,textvariable=self.entryRFOffsetVariable[0]) 
      self.entryRFOffset1.grid(column=ColumnOffset+1,row=RowOffset+6,sticky='EW')
      # 2 
      self.entryRFOffset2 = Tkinter.Entry(self,textvariable=self.entryRFOffsetVariable[1]) 
      self.entryRFOffset2.grid(column=ColumnOffset+1,row=RowOffset+7,sticky='EW')
      # 3 
      self.entryRFOffset3 = Tkinter.Entry(self,textvariable=self.entryRFOffsetVariable[2]) 
      self.entryRFOffset3.grid(column=ColumnOffset+1,row=RowOffset+8,sticky='EW')
      # 4 
      self.entryRFOffset4 = Tkinter.Entry(self,textvariable=self.entryRFOffsetVariable[3]) 
      self.entryRFOffset4.grid(column=ColumnOffset+1,row=RowOffset+9,sticky='EW')
      # RF gate 
      self.entryRFOffsetVariableGate = Tkinter.StringVar()
      self.entryRFOffsetVariableGate.set(u"Enter time offset") 
      self.entryRFOffsetGate = Tkinter.Entry(self,textvariable=self.entryRFOffsetVariableGate) 
      self.entryRFOffsetGate.grid(column=ColumnOffset+1,row=RowOffset+10,sticky='EW')
      # entry fields (duration)  
      md_str_1 = Tkinter.StringVar()
      md_str_2 = Tkinter.StringVar()
      md_str_3 = Tkinter.StringVar()
      md_str_4 = Tkinter.StringVar()
      self.entryMechDurationVariable   = [md_str_1,md_str_2,md_str_3,md_str_4] 
      for element in self.entryMechDurationVariable: 
         element.set(u"Enter time duration") 
      # mech 1
      self.entryMechDuration1 = Tkinter.Entry(self,textvariable=self.entryMechDurationVariable[0]) 
      self.entryMechDuration1.grid(column=ColumnOffset+2,row=RowOffset+2,sticky='EW')
      # 2 
      self.entryMechDuration2 = Tkinter.Entry(self,textvariable=self.entryMechDurationVariable[1]) 
      self.entryMechDuration2.grid(column=ColumnOffset+2,row=RowOffset+3,sticky='EW')
      # 3 
      self.entryMechDuration3 = Tkinter.Entry(self,textvariable=self.entryMechDurationVariable[2]) 
      self.entryMechDuration3.grid(column=ColumnOffset+2,row=RowOffset+4,sticky='EW')
      # 4 
      self.entryMechDuration4 = Tkinter.Entry(self,textvariable=self.entryMechDurationVariable[3]) 
      self.entryMechDuration4.grid(column=ColumnOffset+2,row=RowOffset+5,sticky='EW')
      # rf  
      rd_str_1 = Tkinter.StringVar()
      rd_str_2 = Tkinter.StringVar()
      rd_str_3 = Tkinter.StringVar()
      rd_str_4 = Tkinter.StringVar()
      self.entryRFDurationVariable   = [rd_str_1,rd_str_2,rd_str_3,rd_str_4] 
      for element in self.entryRFDurationVariable: 
         element.set(u"Enter time duration") 
      # 1 
      self.entryRFDuration1 = Tkinter.Entry(self,textvariable=self.entryRFDurationVariable[0]) 
      self.entryRFDuration1.grid(column=ColumnOffset+2,row=RowOffset+6,sticky='EW')
      # 2 
      self.entryRFDuration2 = Tkinter.Entry(self,textvariable=self.entryRFDurationVariable[1]) 
      self.entryRFDuration2.grid(column=ColumnOffset+2,row=RowOffset+7,sticky='EW')
      # 3 
      self.entryRFDuration3 = Tkinter.Entry(self,textvariable=self.entryRFDurationVariable[2]) 
      self.entryRFDuration3.grid(column=ColumnOffset+2,row=RowOffset+8,sticky='EW')
      # 4 
      self.entryRFDuration4 = Tkinter.Entry(self,textvariable=self.entryRFDurationVariable[3]) 
      self.entryRFDuration4.grid(column=ColumnOffset+2,row=RowOffset+9,sticky='EW')
      # rf gate 
      self.entryRFDurationVariableGate = Tkinter.StringVar() 
      self.entryRFDurationVariableGate.set(u"Enter time duration") 
      self.entryRFDurationGate = Tkinter.Entry(self,textvariable=self.entryRFDurationVariableGate) 
      self.entryRFDurationGate.grid(column=ColumnOffset+2,row=RowOffset+10,sticky='EW')

      # pull down menu
      # mechanical switch 
      mu_str_1 = Tkinter.StringVar() 
      mu_str_2 = Tkinter.StringVar() 
      mu_str_3 = Tkinter.StringVar() 
      mu_str_4 = Tkinter.StringVar() 
      self.unit_str_mech = [mu_str_1,mu_str_2,mu_str_3,mu_str_4]
      for element in self.unit_str_mech: 
         element.set("units")
      # 1
      self.opt_mech_1 = Tkinter.OptionMenu(self,self.unit_str_mech[0],*self.TimeChoices)
      self.opt_mech_1.grid(column=ColumnOffset+3,row=RowOffset+2) 
      # 2
      self.opt_mech_2 = Tkinter.OptionMenu(self,self.unit_str_mech[1],*self.TimeChoices)
      self.opt_mech_2.grid(column=ColumnOffset+3,row=RowOffset+3) 
      # 3 
      self.opt_mech_3 = Tkinter.OptionMenu(self,self.unit_str_mech[2],*self.TimeChoices)
      self.opt_mech_3.grid(column=ColumnOffset+3,row=RowOffset+4) 
      # 4 
      self.opt_mech_4 = Tkinter.OptionMenu(self,self.unit_str_mech[3],*self.TimeChoices)
      self.opt_mech_4.grid(column=ColumnOffset+3,row=RowOffset+5) 
      # RF   
      ru_str_1 = Tkinter.StringVar() 
      ru_str_2 = Tkinter.StringVar() 
      ru_str_3 = Tkinter.StringVar() 
      ru_str_4 = Tkinter.StringVar() 
      self.unit_str_rf = [ru_str_1,ru_str_2,ru_str_3,ru_str_4]
      for element in self.unit_str_rf: 
         element.set("units")
      # RF 1 
      self.opt_rf_1 = Tkinter.OptionMenu(self,self.unit_str_rf[0],*self.TimeChoices)
      self.opt_rf_1.grid(column=ColumnOffset+3,row=RowOffset+6) 
      # RF 2
      self.opt_rf_2 = Tkinter.OptionMenu(self,self.unit_str_rf[1],*self.TimeChoices)
      self.opt_rf_2.grid(column=ColumnOffset+3,row=RowOffset+7) 
      # RF 3 
      self.opt_rf_3 = Tkinter.OptionMenu(self,self.unit_str_rf[2],*self.TimeChoices)
      self.opt_rf_3.grid(column=ColumnOffset+3,row=RowOffset+8) 
      # RF 4 
      self.opt_rf_4 = Tkinter.OptionMenu(self,self.unit_str_rf[3],*self.TimeChoices)
      self.opt_rf_4.grid(column=ColumnOffset+3,row=RowOffset+9) 
      # RF gate 
      self.unit_str_rf_gate = Tkinter.StringVar() 
      self.unit_str_rf_gate.set("units") # initial value 
      self.opt_rf_gate = Tkinter.OptionMenu(self,self.unit_str_rf_gate,*self.TimeChoices)
      self.opt_rf_gate.grid(column=ColumnOffset+3,row=RowOffset+10) 

      # check boxes
      # initialize  
      self.chk_global_var = Tkinter.IntVar() 
      self.chk_global     = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_global_var) 
      self.chk_global.grid(column=ColumnOffset+5,row=RowOffset+1)
      mc_int_1 = Tkinter.IntVar()
      mc_int_2 = Tkinter.IntVar()
      mc_int_3 = Tkinter.IntVar()
      mc_int_4 = Tkinter.IntVar()
      # mech 
      self.chk_var_mech = [mc_int_1,mc_int_2,mc_int_3,mc_int_4] 
      # 1 
      self.chk_mech_1 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_mech[0]) 
      self.chk_mech_1.grid(column=ColumnOffset+5,row=RowOffset+2)
      # 2 
      self.chk_mech_2 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_mech[1]) 
      self.chk_mech_2.grid(column=ColumnOffset+5,row=RowOffset+3)
      # 3 
      self.chk_mech_3 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_mech[2]) 
      self.chk_mech_3.grid(column=ColumnOffset+5,row=RowOffset+4)
      # 4 
      self.chk_mech_4 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_mech[3]) 
      self.chk_mech_4.grid(column=ColumnOffset+5,row=RowOffset+5)
      # rf
      # initialize 
      rc_int_1   = Tkinter.IntVar()
      rc_int_2   = Tkinter.IntVar()
      rc_int_3   = Tkinter.IntVar()
      rc_int_4   = Tkinter.IntVar()
      self.chk_var_rf_gate= Tkinter.IntVar()
      self.chk_var_rf = [rc_int_1,rc_int_2,rc_int_3,rc_int_4]
      # 1  
      self.chk_rf_1 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_rf[0]) 
      self.chk_rf_1.grid(column=ColumnOffset+5,row=RowOffset+6)
      # 2 
      self.chk_rf_2 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_rf[1]) 
      self.chk_rf_2.grid(column=ColumnOffset+5,row=RowOffset+7)
      # 3 
      self.chk_rf_3 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_rf[2]) 
      self.chk_rf_3.grid(column=ColumnOffset+5,row=RowOffset+8)
      # 4 
      self.chk_rf_4 = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_rf[3]) 
      self.chk_rf_4.grid(column=ColumnOffset+5,row=RowOffset+9)
      # tomco gate  
      self.chk_rf_gate = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_var_rf_gate) 
      self.chk_rf_gate.grid(column=ColumnOffset+5,row=RowOffset+10)

      # ----------------------------------------------------------------------------------
      # function generator 
      # ID Label 
      self.FGLabelVariable = Tkinter.StringVar()
      self.FGLabelVariable.set("Local Oscillator Settings")
      self.FGLabel = Tkinter.Label(self,textvariable=self.FGLabelVariable,anchor="w",font = "Helvetica 11 bold")
      self.FGLabel.grid(column=ColumnOffset+0,row=RowOffset+12) 
      # Labels  
      self.FGFreqLabelVariable = Tkinter.StringVar()
      self.FGFreqLabelVariable.set("Frequency")
      self.FGFreqLabel = Tkinter.Label(self,textvariable=self.FGFreqLabelVariable,anchor="w") 
      self.FGFreqLabel.grid(column=ColumnOffset+0,row=RowOffset+13) 
      self.FGBNCLabelVariable = Tkinter.StringVar()
      self.FGBNCLabelVariable.set("BNC")
      self.FGBNCLabel = Tkinter.Label(self,textvariable=self.FGBNCLabelVariable,anchor="w") 
      self.FGBNCLabel.grid(column=ColumnOffset+0,row=RowOffset+14)  
      self.FGNTypeLabelVariable = Tkinter.StringVar()
      self.FGNTypeLabelVariable.set("N-Type")
      self.FGNTypeLabel = Tkinter.Label(self,textvariable=self.FGNTypeLabelVariable,anchor="w") 
      self.FGNTypeLabel.grid(column=ColumnOffset+0,row=RowOffset+15) 

      # entry fields 
      # frequency 
      self.entryFGFreqVariable = Tkinter.StringVar() 
      self.entryFGFreqVariable.set(u"Enter frequency")
      self.entryFGFreq = Tkinter.Entry(self,textvariable=self.entryFGFreqVariable)
      self.entryFGFreq.grid(column=ColumnOffset+1,row=RowOffset+13,sticky='EW') 
      # BNC 
      self.entryFGBNCVoltVariable = Tkinter.StringVar() 
      self.entryFGBNCVoltVariable.set(u"Enter voltage")
      self.entryFGBNCVolt = Tkinter.Entry(self,textvariable=self.entryFGBNCVoltVariable)
      self.entryFGBNCVolt.grid(column=ColumnOffset+1,row=RowOffset+14,sticky='EW')  
      # N-Type
      self.entryFGNTypeVoltVariable = Tkinter.StringVar() 
      self.entryFGNTypeVoltVariable.set(u"Enter voltage")
      self.entryFGNTypeVolt = Tkinter.Entry(self,textvariable=self.entryFGNTypeVoltVariable)
      self.entryFGNTypeVolt.grid(column=ColumnOffset+1,row=RowOffset+15,sticky='EW')  

      # pull down menus 
      # frequency 
      self.unit_str_freq_fg = Tkinter.StringVar() 
      self.unit_str_freq_fg.set("units")
      self.opt_freq_fg = Tkinter.OptionMenu(self,self.unit_str_freq_fg,*FreqChoices) 
      self.opt_freq_fg.grid(column=ColumnOffset+2,row=RowOffset+13)  
      # BNC voltage 
      self.unit_str_volt_fg_bnc = Tkinter.StringVar() 
      self.unit_str_volt_fg_bnc.set("units")
      self.opt_volt_fg_bnc = Tkinter.OptionMenu(self,self.unit_str_volt_fg_bnc,*VoltChoices) 
      self.opt_volt_fg_bnc.grid(column=ColumnOffset+2,row=RowOffset+14)  
      # N-Type voltage
      self.unit_str_volt_fg_ntype = Tkinter.StringVar() 
      self.unit_str_volt_fg_ntype.set("units")
      self.opt_volt_fg_ntype = Tkinter.OptionMenu(self,self.unit_str_volt_fg_ntype,*VoltChoices) 
      self.opt_volt_fg_ntype.grid(column=ColumnOffset+2,row=RowOffset+15)  
      # check boxes 
      self.chk_fg_bnc_var   = Tkinter.IntVar() 
      self.chk_fg_bnc   = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_fg_bnc_var) 
      self.chk_fg_bnc.grid(column=ColumnOffset+5,row=RowOffset+14) 
      self.chk_fg_ntype_var = Tkinter.IntVar() 
      self.chk_fg_ntype = Tkinter.Checkbutton(self,text=TickBox,variable=self.chk_fg_ntype_var) 
      self.chk_fg_ntype.grid(column=ColumnOffset+5,row=RowOffset+15)

      # ----------------------------------------------------------------------------------
      # ADC 
      # ID Label for ADC
      self.ADCLabelVariable = Tkinter.StringVar()
      self.ADCLabelVariable.set("Digitizer Settings")
      self.ADCLabel = Tkinter.Label(self,textvariable=self.ADCLabelVariable,anchor="w",font="Helvetica 11 bold")
      self.ADCLabel.grid(column=ColumnOffset+0,row=RowOffset+16) 
      # Struck ADC ID number 
      self.ADCNumLabelVariable = Tkinter.StringVar()
      self.ADCNumLabelVariable.set("Struck ID") 
      self.ADCNumLabel = Tkinter.Label(self,textvariable=self.ADCNumLabelVariable,anchor="w") 
      self.ADCNumLabel.grid(column=ColumnOffset+0,row=RowOffset+17) 
      # Struck ADC channel number 
      self.ADCchLabelVariable = Tkinter.StringVar()
      self.ADCchLabelVariable.set("Channel Number") 
      self.ADCchLabel = Tkinter.Label(self,textvariable=self.ADCchLabelVariable,anchor="w") 
      self.ADCchLabel.grid(column=ColumnOffset+0,row=RowOffset+18) 
      # number of pulses 
      self.ADCPulseLabelVariable = Tkinter.StringVar()
      self.ADCPulseLabelVariable.set("Number of Pulses") 
      self.ADCPulseLabel = Tkinter.Label(self,textvariable=self.ADCPulseLabelVariable,anchor="w") 
      self.ADCPulseLabel.grid(column=ColumnOffset+0,row=RowOffset+19) 
      # sampling frequency
      self.ADCFreqLabelVariable = Tkinter.StringVar()
      self.ADCFreqLabelVariable.set("Sampling Frequency") 
      self.ADCFreqLabel = Tkinter.Label(self,textvariable=self.ADCFreqLabelVariable,anchor="w") 
      self.ADCFreqLabel.grid(column=ColumnOffset+0,row=RowOffset+20) 

      # entry fields  
      # Struck ID number 
      self.entryADCNumVariable = Tkinter.StringVar() 
      self.entryADCNumVariable.set(u"Enter value") 
      self.entryADCNum = Tkinter.Entry(self,textvariable=self.entryADCNumVariable) 
      self.entryADCNum.grid(column=ColumnOffset+1,row=RowOffset+17,sticky='EW')     
      # Struck channel number 
      self.entryADCchVariable = Tkinter.StringVar() 
      self.entryADCchVariable.set(u"Enter value") 
      self.entryADCch = Tkinter.Entry(self,textvariable=self.entryADCchVariable) 
      self.entryADCch.grid(column=ColumnOffset+1,row=RowOffset+18,sticky='EW')     
      # number of pulses 
      self.entryADCPulseVariable = Tkinter.StringVar() 
      self.entryADCPulseVariable.set(u"Enter value") 
      self.entryADCPulse = Tkinter.Entry(self,textvariable=self.entryADCPulseVariable) 
      self.entryADCPulse.grid(column=ColumnOffset+1,row=RowOffset+19,sticky='EW') 

      # pull down menus
      # sampling frequency (value)  
      self.adc_freq_val = Tkinter.StringVar() 
      self.adc_freq_val.set('1')
      self.opt_adc_freq_val = Tkinter.OptionMenu(self,self.adc_freq_val,*FreqValues) 
      self.opt_adc_freq_val.grid(column=ColumnOffset+1,row=RowOffset+20)  
      # sampling frequency (unit)  
      self.unit_str_adc_freq = Tkinter.StringVar() 
      self.unit_str_adc_freq.set('MHz')
      self.opt_adc_freq = Tkinter.OptionMenu(self,self.unit_str_adc_freq,*FreqUnitChoices) 
      self.opt_adc_freq.grid(column=ColumnOffset+2,row=RowOffset+20)  
      # check boxes 
      self.chk_adc_clk_type_var = Tkinter.IntVar() 
      self.chk_adc_clk_type     = Tkinter.Checkbutton(self,text="External",variable=self.chk_adc_clk_type_var) 
      self.chk_adc_clk_type.grid(column=ColumnOffset+3,row=RowOffset+20) 

      # ----------------------------------------------------------------------------------
      # Utilities 
      # header label  
      self.UtilLabelVariable = Tkinter.StringVar()
      self.UtilLabelVariable.set("Utilities")
      self.UtilLabel = Tkinter.Label(self,textvariable=self.UtilLabelVariable,anchor="w",font="Helvetica 11 bold")
      self.UtilLabel.grid(column=ColumnOffset+7,row=RowOffset+0) 
      # debug  
      self.UtilDebugLabelVariable = Tkinter.StringVar()
      self.UtilDebugLabelVariable.set("Debug Mode") 
      self.UtilDebugLabel = Tkinter.Label(self,textvariable=self.UtilDebugLabelVariable,anchor="w") 
      self.UtilDebugLabel.grid(column=ColumnOffset+7,row=RowOffset+1) 
      # verbosity  
      self.UtilVerbLabelVariable = Tkinter.StringVar()
      self.UtilVerbLabelVariable.set("Debug Verbosity") 
      self.UtilVerbLabel = Tkinter.Label(self,textvariable=self.UtilVerbLabelVariable,anchor="w") 
      self.UtilVerbLabel.grid(column=ColumnOffset+7,row=RowOffset+2) 
      # test mode  
      self.UtilTestLabelVariable = Tkinter.StringVar()
      self.UtilTestLabelVariable.set("Test Mode") 
      self.UtilTestLabel = Tkinter.Label(self,textvariable=self.UtilTestLabelVariable,anchor="w") 
      self.UtilTestLabel.grid(column=ColumnOffset+7,row=RowOffset+3) 
      # reference frequency 
      self.UtilRefFreqLabelVariable = Tkinter.StringVar()
      self.UtilRefFreqLabelVariable.set("Field Frequency (RF)") 
      self.UtilRefFreqLabel = Tkinter.Label(self,textvariable=self.UtilRefFreqLabelVariable,anchor="w") 
      self.UtilRefFreqLabel.grid(column=ColumnOffset+7,row=RowOffset+4) 
      # file name 
      self.UtilFNLabelVariable = Tkinter.StringVar()
      self.UtilFNLabelVariable.set("Configuration Label") 
      self.UtilFNLabel = Tkinter.Label(self,textvariable=self.UtilFNLabelVariable,anchor="w") 
      self.UtilFNLabel.grid(column=ColumnOffset+7,row=RowOffset+5) 

      # entry fields  
      # reference frequency 
      self.entryUtilRefFreqVariable = Tkinter.StringVar() 
      self.entryUtilRefFreqVariable.set(u"Enter frequency") 
      self.entryUtilRefFreq = Tkinter.Entry(self,textvariable=self.entryUtilRefFreqVariable) 
      self.entryUtilRefFreq.grid(column=ColumnOffset+7+1,row=RowOffset+4,sticky='EW')     
      # file name  
      self.entryUtilFNVar = Tkinter.StringVar() 
      self.entryUtilFNVar.set(u"Enter label (no spaces)")
      self.entryUtilFN    = Tkinter.Entry(self,textvariable=self.entryUtilFNVar)
      self.entryUtilFN.grid(column=ColumnOffset+7+1,row=RowOffset+5,columnspan=2,sticky='EW') 

      # pull down menus
      # debug mode  
      self.util_debug_val = Tkinter.StringVar() 
      self.util_debug_val.set('off')
      self.opt_util_debug_val = Tkinter.OptionMenu(self,self.util_debug_val,*self.DebugChoices) 
      self.opt_util_debug_val.grid(column=ColumnOffset+7+1,row=RowOffset+1)  
      # verbosity    
      self.util_verb_val = Tkinter.StringVar() 
      self.util_verb_val.set('0')
      self.opt_util_verb_val = Tkinter.OptionMenu(self,self.util_verb_val,*self.VerbChoices) 
      self.opt_util_verb_val.grid(column=ColumnOffset+7+1,row=RowOffset+2)  
      # test mode    
      self.util_test_val = Tkinter.StringVar() 
      self.util_test_val.set('0')
      self.opt_test_val = Tkinter.OptionMenu(self,self.util_test_val,*self.TestChoices) 
      self.opt_test_val.grid(column=ColumnOffset+7+1,row=RowOffset+3)  
      # ref frequency units    
      self.util_ref_freq_unit_val = Tkinter.StringVar() 
      self.util_ref_freq_unit_val.set('units')
      self.opt_ref_freq_val = Tkinter.OptionMenu(self,self.util_ref_freq_unit_val,*FreqChoices) 
      self.opt_ref_freq_val.grid(column=ColumnOffset+7+2,row=RowOffset+4)  

      # ----------------------------------------------------------------------------------
      # make buttons 

      # load config 
      self.buttonImportConfig = Tkinter.Button(self,text=u"Import Configuration...",command=self.ImportConfig) 
      self.buttonImportConfig.grid(column=ColumnOffset+6,row=RowOffset+17,columnspan=4,sticky='EW') 
      # print config to files  
      self.buttonConfig = Tkinter.Button(self,text=u"Apply Configuration",command=self.PrintToFile) 
      self.buttonConfig.grid(column=ColumnOffset+6,row=RowOffset+18,columnspan=4,sticky='EW',) 
      # run  
      # self.buttonRun = Tkinter.Button(self,text=u"Run",command=self.RunDAQ) 
      self.buttonRun = Tkinter.Button(self,text=u"Run",command=self.CheckHardware) 
      self.buttonRun.grid(column=ColumnOffset+6,row=RowOffset+19,columnspan=4,sticky='EW') 
      # turn off FPGA and function generator  
      # self.buttonStop = Tkinter.Button(self,text=u"Stop") 
      # self.buttonStop.grid(column=ColumnOffset+5,row=RowOffset+19,columnspan=1,sticky='EW') 
      # quit the program 
      self.buttonQuit = Tkinter.Button(self,text=u"Quit",command=quit) 
      self.buttonQuit.grid(column=ColumnOffset+6,row=RowOffset+20,columnspan=4,sticky='EW') 

      # ----------------------------------------------------------------------------------
      # comments field  
      self.entryCommentsVar = Tkinter.StringVar() 
      self.entryCommentsVar.set(u"Enter run comments") 
      self.entryComments    = Tkinter.Entry(self,textvariable=self.entryCommentsVar) 
                                       # anchor="w",fg="black",bg="white") 
      self.entryComments.grid(column=ColumnOffset+7,row=RowOffset+6,columnspan=3,rowspan=2,sticky='EW') 

      # ----------------------------------------------------------------------------------
      # status field  
      self.StatusVariable = Tkinter.StringVar() 
      self.StatusVariable.set(u"Fill out the fields above before clicking 'Apply Configuration' and then 'Run.'  Be sure your entries are correct.") 
      self.Status  = Tkinter.Label(self,textvariable=self.StatusVariable,
                                   anchor="w",fg="white",bg="blue") 
      self.Status.grid(column=ColumnOffset+0,row=RowOffset+24,columnspan=10,sticky='ew') 
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
       # Label      = Tkinter.Label(self.LCWindow, text="This is window #%s" % self.counter)
       # Label.grid(column=0,row=0,sticky='ew') 
       Label = Tkinter.Label(self.LCWindow,text="Configuration",anchor="w") 
       Label.grid(column=0,row=1) 
       # # fpga 
       # fpgaLabel  = Tkinter.Label(self.LCWindow, text="FPGA",anchor="w")
       # fpgaLabel.grid(column=0,row=1)
       # fgLabel    = Tkinter.Label(self.LCWindow, text="Function Generator",anchor="w")
       # fgLabel.grid(column=0,row=2)
       # # adc 
       # adcLabel    = Tkinter.Label(self.LCWindow, text="ADC",anchor="w")
       # adcLabel.grid(column=0,row=3)
       # # utilities
       # utilLabel   = Tkinter.Label(self.LCWindow, text="Utilities",anchor="w")
       # utilLabel.grid(column=0,row=4)

       self.ConfVar = Tkinter.StringVar() 
       self.ConfVar.set("a-configuration.conf") 
       entryConf = Tkinter.Entry(self.LCWindow,textvariable=self.ConfVar); 
       entryConf.grid(column=1,row=1)  

       # # FPGA data 
       # self.fpgaFNVar = Tkinter.StringVar() 
       # self.fpgaFNVar.set("pulse-data.dat")  
       # entryFPGA = Tkinter.Entry(self.LCWindow,textvariable=self.fpgaFNVar)
       # entryFPGA.grid(column=1,row=1)
       # # Function generator data 
       # self.fgFNVar = Tkinter.StringVar() 
       # self.fgFNVar.set("sg382.dat") 
       # entryFuncGen = Tkinter.Entry(self.LCWindow,textvariable=self.fgFNVar)
       # entryFuncGen.grid(column=1,row=2)
       # # ADC data 
       # self.adcFNVar = Tkinter.StringVar() 
       # self.adcFNVar.set("struck_adc.dat") 
       # entryADC = Tkinter.Entry(self.LCWindow,textvariable=self.adcFNVar)
       # entryADC.grid(column=1,row=3)
       # # utility data 
       # self.utilFNVar = Tkinter.StringVar() 
       # self.utilFNVar.set("utilities.dat") 
       # entryUtil = Tkinter.Entry(self.LCWindow,textvariable=self.utilFNVar)
       # entryUtil.grid(column=1,row=4)
       # button to go get a file  
       confButton = Tkinter.Button(self.LCWindow, text='Choose file...', command=self.GetFileNameConf)
       confButton.grid(column=2,row=1,columnspan=1) 

       # fpgaButton = Tkinter.Button(self.LCWindow, text='Choose file...', command=self.GetFileNameFPGA)
       # fpgaButton.grid(column=2,row=1,columnspan=1) 
       # fgButton   = Tkinter.Button(self.LCWindow, text='Choose file...', command=self.GetFileNameFG)
       # fgButton.grid(column=2,row=2,columnspan=1) 
       # adcButton  = Tkinter.Button(self.LCWindow, text='Choose file...', command=self.GetFileNameADC)
       # adcButton.grid(column=2,row=3,columnspan=1) 
       # utilButton  = Tkinter.Button(self.LCWindow, text='Choose file...', command=self.GetFileNameUtil)
       # utilButton.grid(column=2,row=4,columnspan=1) 

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
       fpga_header  = "# ID     on/off     time offset     pulse time     units"
       fg_header    = "# type   state      value           units"
       adc_header   = "# ID     value      units"
       util_header  = "# ID     value"
       EOF          = "end_of_file"
       ZERO         = "0" 
       NINETYNINE   = "99"
       MIN1         = "-1"
       blSTATE      = "--"
       ND           = "ND"
       mech_state   = [0,0,0,0] 
       rf_state     = [0,0,0,0] 
       global_str   = "none"  
       mech_list    = ["none","none","none","none"] 
       rf_list      = ["none","none","none","none"]
       eof_fpga_str = "%-20s %-20s %-20s %-20s %-20s" %(EOF,blSTATE,ZERO,MIN1,ND)
       eof_fg_str   = "%-20s %-20s %-20s %-20s" %(EOF,blSTATE,ZERO,ND)
       eof_adc_str  = "%-20s %-20s %-20s" %(EOF,ZERO,ND)
       eof_util_str = "%-20s %-20s " %(EOF,NINETYNINE)
       # grab values 
       # FPGA 
       global_str        = self.GetFPGAString(0,0)
       for i in xrange(0,4):  
           mech_list[i]  = self.GetFPGAString(1,i) 
           mech_state[i] = self.GetFPGAState(1,i) 
       for i in xrange(0,3):  
           rf_list[i]    = self.GetFPGAString(2,i) 
           rf_state[i]   = self.GetFPGAState(2,i) 
       tomco           = self.GetFPGAString(3,4)
       tomco_state     = self.GetFPGAState(3,0)
       # function generator 
       bnc_str         = self.GetFuncGenString(1)  
       ntype_str       = self.GetFuncGenString(2)  
       freq_str        = self.GetFuncGenString(3) 
       # ADC 
       adc_id_str      = self.GetADCString(1) 
       ch_num_str      = self.GetADCString(2) 
       pulse_num_str   = self.GetADCString(3) 
       sample_freq_str = self.GetADCString(4) 
       # signal_len_str  = self.GetADCString(5)
       extern_clk_str  = self.GetADCString(6) 
       # utilities
       debug_str       = self.GetUtilString(1)  
       verb_str        = self.GetUtilString(2)  
       test_str        = self.GetUtilString(3)  
       rf_str          = self.GetUtilString(4)  
       # comments
       com_str        = self.entryCommentsVar.get()  
       # get file names ready  
       self.prefix    = "input/configs/files/"
       # configuration file 
       self.conf_fn   = self.entryUtilFNVar.get() + ".conf"
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
       confFile       = open(self.conf_path,"w") 
       fpgaFile       = open(self.fpga_path,"w")
       fgFile         = open(self.fg_path  ,"w")
       adcFile        = open(self.adc_path ,"w")
       utilFile       = open(self.util_path,"w")
       commentFile    = open(self.com_path ,"w") 
       # print to screen 
       # print "-----------------------------------------------------"
       # print self.fpga_fn 
       # print fpga_header
       # print global_str 
       # for i in xrange(0,3):
       #    if mech_state[i]: print mech_list[i] 
       # for i in xrange(0,3):   
       #    if rf_state[i]: print rf_list[i] 
       # if tomco_state: print tomco 
       # print eof_fpga_str
       # writing to the file  
       conf_label = self.entryUtilFNVar.get() 
       confFile.write(conf_label +"\n")
       fpgaFile.write(fpga_header+"\n")
       fpgaFile.write(global_str +"\n")
       for i in xrange(0,4):
          if mech_state[i]: fpgaFile.write( mech_list[i] +"\n") 
       for i in xrange(0,3):   
          if rf_state[i]: fpgaFile.write( rf_list[i] +"\n")  
       # if tomco_state: fpgaFile.write(tomco+"\n") 
       fpgaFile.write(tomco+"\n")   # always print the TOMCO information to file 
       fpgaFile.write(eof_fpga_str+"\n")  
       # print "-----------------------------------------------------"
       # print self.fg_fn 
       # print fg_header
       # print freq_str
       # print bnc_str
       # print ntype_str 
       # print eof_fg_str 
       # write to file 
       fgFile.write(fg_header +"\n") 
       fgFile.write(freq_str  +"\n")
       fgFile.write(bnc_str   +"\n")
       fgFile.write(ntype_str +"\n")
       fgFile.write(eof_fg_str+"\n") 
       # print "-----------------------------------------------------"
       # print self.adc_fn 
       # print adc_header
       # print pulse_num_str
       # print sample_freq_str
       # print signal_len_str 
       # print eof_adc_str 
       # write to file  
       adcFile.write(adc_header      +"\n")
       adcFile.write(adc_id_str      +"\n")
       adcFile.write(ch_num_str      +"\n")
       adcFile.write(pulse_num_str   +"\n")
       adcFile.write(sample_freq_str +"\n")
       # adcFile.write(signal_len_str  +"\n")
       adcFile.write(extern_clk_str  +"\n")
       adcFile.write(eof_adc_str     +"\n") 
       # utilities 
       utilFile.write(util_header  + "\n")        
       utilFile.write(debug_str    + "\n")        
       utilFile.write(verb_str     + "\n")        
       utilFile.write(test_str     + "\n")        
       utilFile.write(rf_str       + "\n")        
       utilFile.write(eof_util_str + "\n")        
       # comments
       commentFile.write(com_str+"\n") 
       # close the files 
       confFile.close() 
       fpgaFile.close() 
       fgFile.close() 
       adcFile.close()
       commentFile.close()  
       # update status banner 
       self.StatusVariable.set("Configuration printed to file.") 
   #----------------------------------------------------------------------------
   def GetFPGAString(self,type,index):
      # gather all values and put into a string  
      MechSwLabel = ['mech_sw_1','mech_sw_2','mech_sw_3','mech_sw_4']
      RFSwLabel   = ['rf_sw_1'  ,'rf_sw_2'  ,'rf_sw_3'  ,'rf_clear']
      state       = "off"
      units       = "ND" 
      t_offset    = "none" 
      t_duration  = "none" 
      # adjust array index  
      i = index   
      # get values
      if type==0:   # global on/off 
         if self.chk_global_var.get(): state = "on"
         label      = "global_on_off" 
         t_offset   = "0" 
         t_duration = "0"
      elif type==1:  # mechanical switch
         if self.chk_var_mech[i].get(): state = "on"
         label      = MechSwLabel[i]
         t_offset   = self.entryMechOffsetVariable[i].get()  
         t_duration = self.entryMechDurationVariable[i].get()  
         units      = self.unit_str_mech[i].get()
      elif type==2: # RF 
         if self.chk_var_rf[i].get(): state = "on"
         label      = RFSwLabel[i]
         t_offset   = self.entryRFOffsetVariable[i].get()  
         t_duration = self.entryRFDurationVariable[i].get()  
         units      = self.unit_str_rf[i].get()
      elif type==3: # TOMCO gate 
         if self.chk_var_rf_gate.get(): state = "on"
         label      = "rf_gate"
         t_offset   = self.entryRFOffsetVariableGate.get() 
         t_duration = self.entryRFDurationVariableGate.get() 
         units      = self.unit_str_rf_gate.get()
      # build string 
      if(units == "units"): units = "ND" 
      my_str = "%-20s %-20s %-20s %-20s %-20s" %(label,state,t_offset,t_duration,units) 
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
#--------------------------------------------------------------------------------

if __name__ == "__main__": 
   app = daq(None) 
   app.title('NMR DAQ')
   app.mainloop()
