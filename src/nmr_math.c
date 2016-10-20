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
   double P      = GetPower(R,v_p); 
   double P_1mW  = 1E-3; 
   double G_dBm  = 10.*log(P/P_1mW); 
   return G_dBm;  
}
//______________________________________________________________________________
double ConvertVoltageFrom_Vp_to_dBm(double v_p){
   double R      = 50.;                 // 50-Ohm impedance  
   double P      = GetPower(R,v_p); 
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

