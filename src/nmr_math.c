#include "nmr_math.h"
//______________________________________________________________________________
double ConvertVoltageFrom_Vp_to_Vpp(double v_p){
   double v_pp = 2.*v_p;
   return v_pp; 
}
//______________________________________________________________________________
double ConvertVoltageFrom_Vpp_to_Vp(double v_pp){
   double v_p = v_pp/2.;
   return v_p; 
}
//______________________________________________________________________________
double ConvertVoltageFrom_Vrms_to_Vpp(double v_rms){
   double v_pp = 2.*sqrt(2.)*v_rms;
   return v_pp; 
}
//______________________________________________________________________________
double ConvertVoltageFrom_Vp_to_Vrms(double v_p){
   double v_rms = v_p/( sqrt(2.) );
   return v_rms; 
}
//______________________________________________________________________________
double ConvertVoltageFrom_Vpp_to_Vrms(double v_pp){
   double v_rms = v_pp/( 2.*sqrt(2.) );
   return v_rms; 
}
//______________________________________________________________________________
double GetPower(double R,double V_p){
   double P = V_p*V_p/(2.*R); 
   return P;
}
//______________________________________________________________________________
double ConvertVoltageFrom_Vrms_to_Vp(double v_rms){
   double v_pp = ConvertVoltageFrom_Vrms_to_Vpp(v_rms);
   double v_p  = ConvertVoltageFrom_Vpp_to_Vp(v_pp);
   return v_p; 
}
//______________________________________________________________________________
double ConvertVoltageFrom_dBm_to_Vp(double v_dbm){
   // convert voltage to Vrms
   double R       = 50.;                   // 50-Ohm impedance 
   double P_1mW   = 1E-3; 
   double G       = v_dbm;
   double T1      = 2.*R*P_1mW;
   double exp_arg = G/10.;
   double T2      = pow(10.,exp_arg);
   double arg     = T1*T2;
   double v_p     = sqrt(arg); 
   return v_p; 
}
//______________________________________________________________________________
double ConvertVoltageFrom_dBm_to_Vpp(double v_dbm){
   // convert voltage to Vp
   double v_p  = ConvertVoltageFrom_dBm_to_Vp(v_dbm); 
   // now get Vpp 
   double v_pp = 2.*v_p;   
   return v_pp; 
}
//______________________________________________________________________________
double ConvertVoltageFrom_Vpp_to_dBm(double v_pp){
   double v_p    = ConvertVoltageFrom_Vpp_to_Vp(v_pp); 
   double R      = 50.; 
   double P      = GetPower(v_p,R); 
   double P_1mW  = 1E-3; 
   double G_dBm  = 10.*log(P/P_1mW); 
   return G_dBm;  
}
//______________________________________________________________________________
double ConvertVoltageFrom_Vp_to_dBm(double v_p){
   double R      = 50.;                 // 50-Ohm impedance  
   double P      = GetPower(v_p,R); 
   double P_1mW  = 1E-3; 
   double G_dBm  = 10.*log(P/P_1mW); 
   return G_dBm;  
}
//______________________________________________________________________________
double ConvertVoltageFrom_dBm_to_Vrms(double v_dbm){
   double v_p   = ConvertVoltageFrom_dBm_to_Vp(v_dbm); 
   double v_rms = ConvertVoltageFrom_Vp_to_Vrms(v_p);
   return v_rms; 
}
//______________________________________________________________________________
double GetVoltageUsingPower(double P,double R){
   // voltage in Volts (amplitude) 
   double V = sqrt(2.*P*R);
   return V;
}
//______________________________________________________________________________
double GetVoltageUsingGain(double V_in,double G){
   // voltage in Volts 
   double arg   = G/20.;
   double V_out = V_in*pow(10.,arg);
   return V_out;
}
//______________________________________________________________________________
double GetGainUsingPower(double P_in,double P_out){
   // gain in dB 
   double num = P_out;
   double den = P_in;
   double T1  = 10.;
   double T2  = log10(num/den);
   double arg = T1*T2;
   return arg;
}
//______________________________________________________________________________
double GetGainUsingVoltage(double V_in,double V_out){
   // gain in dB 
   // assumes R_in and R_out are equal! 
   double num = V_out;
   double den = V_in;
   double T1  = 20.;
   double T2  = log10(num/den);
   double arg = T1*T2;
   return arg;
}
//______________________________________________________________________________
double GetGainUsingVoltage_dBm(double V_out,double R){
   // gain in dBm using Voltage as input 
   double P_out = GetPower(V_out,R);
   double mW    = 1E-3;    // 1 milliwatt
   double num   = P_out;   // power in W 
   double den   = mW;
   double T1    = 10.;
   double T2    = log10(num/den);
   double arg   = T1*T2;
   return arg;
}
//______________________________________________________________________________
double GetGainUsingPower_dBm(double P_out){
   // gain in dBm 
   double mW  = 1E-3;    // 1 milliwatt
   double num = P_out;   // power in mW 
   double den = mW;
   double T1  = 10.;
   double T2  = log10(num/den);
   double arg = T1*T2;
   return arg;
}
//______________________________________________________________________________
double CalculateVinForTOMCO(double P_out,double R){
   // compute the voltage input to the TOMCO 
   // resulting in an output power P_out. 
   // inputs:
   // P_out: desired output power (from TOMCO) 
   // R:     resistance 
   // P_in:  required input power to obtain P_out, given P_tomco  
   // V_in:  required input voltage to obtain P_out, given P_tomco  
   // V_out: required voltage to obtain P_out, given P_tomco  
   // Given: TOMCO power output, P_tomco, is 250 W.  
   double P_tomco = 250.;
   double G       = GetGainUsingPower_dBm(P_tomco);
   double arg     = G/10.;
   double num     = P_out;
   double den     = pow(10.,arg);
   double P_in    = num/den;
   // double V_out   = GetVoltageUsingPower(P_out,R);  // don't need this... 
   double V_in    = GetVoltageUsingPower(P_in ,R);
   return V_in; 
}
