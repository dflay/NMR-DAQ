#ifndef NMR_MATH 
#define NMR_MATH 

#include <math.h> 

// general math functions 
double ConvertVoltageFrom_Vp_to_Vpp(double v_p); 
double ConvertVoltageFrom_Vpp_to_Vp(double v_pp);
double ConvertVoltageFrom_Vrms_to_Vpp(double v_rms);
double ConvertVoltageFrom_Vp_to_Vrms(double v_p);
double ConvertVoltageFrom_Vpp_to_Vrms(double v_pp);
double GetPower(double R,double V_p);
double ConvertVoltageFrom_Vrms_to_Vp(double v_rms);
double ConvertVoltageFrom_dBm_to_Vp(double v_dbm);
double ConvertVoltageFrom_dBm_to_Vpp(double v_dbm);
double ConvertVoltageFrom_Vpp_to_dBm(double v_pp);
double ConvertVoltageFrom_Vp_to_dBm(double v_p);
double ConvertVoltageFrom_dBm_to_Vrms(double v_dbm);

#endif 
