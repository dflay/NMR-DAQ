#ifndef NMR_MATH 
#define NMR_MATH 

#include <math.h> 

#define _50_OHMS 50.000 

double ConvertVoltageFrom_Vp_to_Vpp(double v_p); 
double ConvertVoltageFrom_Vpp_to_Vp(double v_pp);
double ConvertVoltageFrom_Vrms_to_Vpp(double v_rms);
double ConvertVoltageFrom_Vp_to_Vrms(double v_p);
double ConvertVoltageFrom_Vpp_to_Vrms(double v_pp);
double ConvertVoltageFrom_Vrms_to_Vp(double v_rms);
double ConvertVoltageFrom_dBm_to_Vp(double v_dbm);
double ConvertVoltageFrom_dBm_to_Vpp(double v_dbm);
double ConvertVoltageFrom_Vpp_to_dBm(double v_pp);
double ConvertVoltageFrom_Vp_to_dBm(double v_p);
double ConvertVoltageFrom_dBm_to_Vrms(double v_dbm);

double GetPower(double V_p,double R);

double GetGainUsingPower(double P_in,double P_out); 
double GetGainUsingPower_dBm(double P_out);
double GetGainUsingVoltage(double V_in,double V_out); 
double GetGainUsingVoltage_dBm(double V_out,double R);

double GetVoltageUsingPower(double P,double R); 
double GetVoltageUsingGain(double V_in,double G); 

double CalculateVinForTOMCO(double P_out,double R); 
 

#endif 
