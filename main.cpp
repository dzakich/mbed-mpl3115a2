#include "mbed.h"
#include "MPL3115A2_IO.h"
#include "MPL3115A2_REGISTER_MAP.h"
 
MPL3115A2 MPL(p9, p10);

//const int addr_write = 0xC0;
//const int addr_read = 0xC1;

int main() 
{
  
  //char outgoing[6];
  //char incomming[6];
    
    while (1)
     { 
     bool Activity  = MPL.MPL_is_Active();
     
     char stat = MPL.MPL_Get_Status();
     
     char ink = MPL.MPL_Who_Am_I_();
     
     double my_pressude  = MPL.MPL_Get_Pressure();
     
     double my_temper = MPL.MPL_Get_Temperature();
     
     wait_ms(300);

        /*          
        outgoing[0] = 0x26;   // CTRL_REG1 address
        outgoing[1] = 0x02;  // Set ALT mode and OTS Pin
        
        i2c.write(addr_write, outgoing, 2);  //do not generate repeated start.
        
        wait(1);  //wait at least 700 ms for the Oversampling to complete
        
        outgoing[0] = 0x01;  // OUT_P_MSB address
      
        i2c.write(addr_write, outgoing, 1, true);  // generate repeated START and... 
        
        i2c.read(addr_read, incomming, 5); //... use auto-indexing to receive 5 bytes: OUT_P_MSB, OUT_P_CSB, OUT_P_LMSB, OUT_T_MSB,OUT_T_LSB
 
        wait(0.3);
   
        */
    }
    
}