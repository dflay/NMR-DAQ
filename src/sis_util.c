#include "sis_util.h"
//______________________________________________________________________________
int SISMODID(int vme_handle,u_int32_t MOD_ID){

   // from Matthias Smith's code (UW) 

   int rc = 0;
   u_int32_t data32 = 0; 
   rc = SISRead32(vme_handle,MOD_ID,&data32);

   if(gIsDebug) printf("Module ID:      %04x \n",data32 >> 16        ); 
   if(gIsDebug) printf("Major revision: %02x \n",(data32 >> 8) & 0xff);
   if(gIsDebug) printf("Minor revision: %02x \n",data32 & 0xff       ); 

   return rc; 

}
//______________________________________________________________________________
int SISWrite32(int vme_handle, u_int32_t sis_addr, u_int32_t data32){

   int return_code;
   u_int32_t addr = MOD_BASE + sis_addr; 
   return_code    = vme_A32D32_write(vme_handle,addr,data32);

   if(gIsDebug && (return_code == 0) ){
      // printf("[StruckADC]: WRITE SUCCESSFUL: address = 0x%08x, data = 0x%08x\n",addr,data32); 
   }else if (gIsDebug || (return_code != 0)) {
      printf("[StruckADC]: ERROR! WRITE FAILED: return code = 0x%08x, address = 0x%08x, data = 0x%08x\n", return_code,addr,data32); 
   }

   return return_code;
}
//______________________________________________________________________________
int SISRead32(int vme_handle, u_int32_t sis_addr,u_int32_t *data32){

   int return_code;
   u_int32_t addr   = MOD_BASE + sis_addr; 
   return_code      = vme_A32D32_read(vme_handle,addr,data32);

   if( gIsDebug && (return_code == 0) ){
      // printf("[StruckADC]: READ SUCCESSFUL: address = 0x%08x, data = 0x%08x\n", addr, *data32); 
   }else if( gIsDebug || (return_code != 0) ){
      printf("[StruckADC]: ERROR! READ FAILED: return code = 0x%08x, address = 0x%08x, data = 0x%08x\n", 
            return_code,addr,*data32); 
   }

   return return_code;
}
//_____________________________________________________________________________
int SI5325Write(int vme_handle, u_int32_t si5325_addr, u_int32_t data32){

   int return_code, poll_counter;
   u_int32_t write_data, read_data;
   int POLL_COUNTER_MAX = 100;

   // tell SI5325 what address we want to write to
   write_data = 0x0000 + (si5325_addr & 0xff);
   return_code = SISWrite32(vme_handle, SIS3316_NIM_CLK_MULTIPLIER_SPI_REG, write_data);
   if (return_code != 0) { return -1; }
   usleep(10000);

   // confirm that the SI5325 understood/processed our request to set the write address
   poll_counter = 0;
   do{
      poll_counter++;
      SISRead32(vme_handle, SIS3316_NIM_CLK_MULTIPLIER_SPI_REG, &read_data);
   }while ( ( (read_data & 0x80000000) == 0x80000000) && (poll_counter < POLL_COUNTER_MAX) );
   if (poll_counter == POLL_COUNTER_MAX) {      return -2 ;     }

   // actually write data to the specificed SI5325 address
   write_data = 0x4000 + (data32 & 0xff);
   return_code = SISWrite32(vme_handle, SIS3316_NIM_CLK_MULTIPLIER_SPI_REG, write_data);
   if (return_code != 0) { return -1; }
   usleep(10000);

   // again confirm that the SI5325 understood/processed our requested write
   poll_counter = 0;
   do{
      poll_counter++;
      SISRead32(vme_handle, SIS3316_NIM_CLK_MULTIPLIER_SPI_REG, &read_data);
   }while ( ( (read_data & 0x80000000) == 0x80000000) && (poll_counter < POLL_COUNTER_MAX) );
   if (poll_counter == POLL_COUNTER_MAX) {      return -2 ;     }

   return 0;
}
//______________________________________________________________________________
int SISClose(int vme_handle){
   // just closes the handle for now
   return close(vme_handle);
}
//______________________________________________________________________________
void ClearOutputArrays(int NUM_SAMPLES){
   int i=0;
   for(i=0;i<NUM_SAMPLES;i++){
      gDATA[i]    = 0;
      gDATA_us[i] = 0;
   }
}

