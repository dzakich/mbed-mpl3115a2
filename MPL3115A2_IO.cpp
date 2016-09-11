#include "MPL3115A2_IO.h"
#include "MPL3115A2_REGISTER_MAP.h"


MPL3115A2::MPL3115A2(PinName sda, PinName scl) : _i2c(sda, scl)
{
    _i2c.frequency(frequency);   // Set I2C object frequency too 400 KHz.
    Bar_Mode = true;        // Default to Barometer mode @ startup. 
    is_Reset = false;         // Defaults to not-reset
}

void MPL3115A2::MPL_Init()  // Used to initialize proper settings: Oversampling, Device Mode, Interrupts etc. TBD
{
    // Contents to be deternined. Non-essential fucntion. 
}
    
char MPL3115A2::MPL_Get_Status()  // Reads the STATUS register and returns the contents. Can be used to find if new P,A,T data is available for retrieval.
{
    char temp[2];
    temp[0] = STATUS;
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,1);
    
    return temp[0];
}

char MPL3115A2::MPL_Who_Am_I_()
{
    char temp[2];
    temp[0] = WHO_AM_I;
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,1);
    
    return temp[0];
}

double MPL3115A2::MPL_Get_Pressure()     // Returns the Atmospheric Pressure reading.
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
    
    MPL_One_Shot_Measure();  // Initiate the measurement. 
 
    //wait_ms(700);  Alternative to polling DR_PDR bit to see if the data is available for reading.
    temp[0] = 0xFF;  // Prime the while() loop.
    
    while((temp[0] & CTRL_REG1_OST) != 0 )  // Poll the OST binary in CTRL_REG1 register to see when oversampling is completed and data is available. The bit is auto-clear @FIFO = 00. Poll while 1. 
    {    
        temp[0] = CTRL_REG1;                     
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,1);
    
    }
     
    if (Bar_Mode == true)   // Verify that the device is in Barometer mode so we do return Pressure from the register.
    {
        temp[0] = OUT_P_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
    
    }
    
    else
    {
        MPL_Barometer_Mode();   //Change to the Barometer Mode.
        
        temp[0] = OUT_P_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
        
    }
    
    // Now we will reassemble the whole.fractional return value from 3 8-bit portions.
    // Pressure data: 20-bit unsigned in Pa. First 18 bits {OUT_P_MSB[7:0],OUT_C_MSB[7:0],OUT_P_LSB[7:6]} is Whole and OUT_P_LSB[5:4] Fractional.
    
    double temp_Whole = (double)( (temp[0] << 10) | (temp[1] << 2) | (temp[2] >> 6) );  // Assemble 18-bit Whole segment
    
    double temp_Fraction = 0.25 * (double)((temp[2] >> 4) & 0x03);   // right-shift bits 5:4 and isolate. Fractional values are in increments of 0.25 Pa. i.e. 11 is 0.25*3 = 0.75 Pa. 10 is 0.25*2, and 01 is 0.25*1.
    
    return (temp_Whole + temp_Fraction);
}

double MPL3115A2::MPL_Get_Altitude()     // Returns the Altitude reading.
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
    
    MPL_One_Shot_Measure();  // Initiate the measurement. 
 
    //wait_ms(700);  Alternative to polling DR_PDR bit to see if the data is available for reading.
    
    temp[0] = 0xFF;  // Prime the while() loop.
    
    while((temp[0] & CTRL_REG1_OST) != 0 )  // Poll the OST binary in CTRL_REG1 register to see when oversampling is completed and data is available. The bit is auto-clear @FIFO = 00. Poll while 1. 
    {    
        temp[0] = CTRL_REG1;                     
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,1);
    
    }
    
    if (Bar_Mode == false)   // Verify that the device is in Altimeter mode so we do return Altitude from the register.
    {
        temp[0] = OUT_P_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
    
    }
    
    else
    {
        MPL_Altimeter_Mode();   //Change to the Altimeter Mode.
        
        temp[0] = OUT_P_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
        
    }
    
    // Now we will reassemble the whole.fractional return value from 3 8-bit portions.
    // Altitude data: 20-bit unsigned in m. First 16 bits {OUT_P_MSB[7:0],OUT_C_MSB[7:0]}(signed, 2's comp.) is Whole and OUT_P_LSB[7:4] is Fractional(unsigned)
    
    int temp_Whole = ( (temp[0] << 8) | temp[1] );  // Assemble 16-bit Whole segment to determine the sign next.
    
    int MSB = temp_Whole >> 15;                                 // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x00007FFF;                              // Mask to retain 15 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's.
      {
          temp_Whole = (temp_Whole | 0xFFFF8000 );   // 32-bit
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);
    
    double temp_Fraction = 0.0625 * (double)((temp[2] >> 4) & 0x0F);   // right-shift bits 7:4 and isolate. Fractional values are in increments of 0.0625 m. i.e. 1111 is 0.0625*15 = 0.9375 m.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }
}

double MPL3115A2::MPL_Get_Temperature()  // Returns Teperature reading.
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
    
    MPL_One_Shot_Measure();  // Initiate the measurement. 
 
    //wait_ms(700);  Alternative to polling DR_PDR bit to see if the data is available for reading.
    
    temp[0] = 0xFF;  // Prime the while() loop.
    
    while((temp[0] & CTRL_REG1_OST) != 0 )  // Poll the DR_TDR pin in DR_STATUS register to see when oversampling is completed and data is available. 
    {    
        temp[0] = CTRL_REG1;                     
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,1);
    
    }
    
        temp[0] = OUT_T_MSB;                        // Start the Temperature reading 
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,2);
        
        // Now we will reassemble the whole.fractional return value from 2 8-bit portions.
        // Temperature data: 12-bit signed in degrees C. First 8 bits {OUT_T_MSB[7:0]}(signed, 2's comp.) is Whole and OUT_T_LSB[7:4] is Fractional(unsigned)
    
    int temp_Whole = temp[0];  // Assemble 8-bit Whole segment to determine the sign next.
    
    int MSB = temp_Whole >> 7;                           // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x0000007F;                // Mask to retain 7 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's to turn into negative 2's comp rep.
      {
          temp_Whole = (temp_Whole | 0xFFFFFF80 );       // 32-bit. Pad with '1's all the way to MSB
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);
    
    double temp_Fraction = 0.0625 * (double)((temp[1] >> 4) & 0x0F);   // right-shift bits 7:4 and isolate. Fractional values are in increments of 0.0625 C. i.e. 1111 is 0.0625*15 = 0.9375 C.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }   
}

double MPL3115A2::MPL_Get_Pressure_Change()     // Returns the Atmospheric Pressure deifference from the last reading.
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
    
    temp[0] = 0x00;  // Prime the while() loop.
         
    if (Bar_Mode == true)   // Verify that the device is in Barometer mode so we do return Pressure from the register.
    {
        temp[0] = OUT_P_DELTA_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
    
    }
    
    else
    {
        MPL_Barometer_Mode();   //Change tto the Barometer Mode.
        
        temp[0] = OUT_P_DELTA_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
        
    }
    
    // Now we will reassemble the whole.fractional return value from 3 8-bit portions.
    // Pressure data: 20-bit signed in Pa. First 18 bits {OUT_P_DELTA_MSB[7:0],OUT_P_DELTA_CSB[7:0],OUT_P_DELTA_LSB[7:6]} is Whole and OUT_P_DELTA_LSB[5:4] Fractional.
    
    int temp_Whole = ( (temp[0] << 10) | (temp[1] << 2) | (temp[2] >> 6) );  // Assemble 18-bit Whole segment
    
    int MSB = temp_Whole >> 17;                                 // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x0001FFFF;                       // Mask to retain 17 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's.
      {
          temp_Whole = (temp_Whole | 0xFFFE0000 );       // 32-bit padding with '1's
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);        // typecast to double after all is set with signs
    
    double temp_Fraction = 0.25 * (double)((temp[2] >> 4) & 0x03);   // right-shift bits 5:4 and isolate. Fractional values are in increments of 0.25 Pa. i.e. 11 is 0.25*3 = 0.75 Pa. 10 is 0.25*2, and 01 is 0.25*1.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }
}

double MPL3115A2::MPL_Get_Altitude_Change()     // Returns the Altitude reading.
{
    char temp[3];  // Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
    
    if (Bar_Mode == false)   // Verify that the device is in Altimeter mode so we do return Altitude from the register.
    {
        temp[0] = OUT_P_DELTA_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
    
    }
    
    else
    {
        MPL_Altimeter_Mode();   // Change to the Altimeter Mode.
        
        temp[0] = OUT_P_DELTA_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
        
    }
    
    // Now we will reassemble the whole.fractional return value from 3 8-bit portions.
    // Altitude data: 20-bit unsigned in m. First 16 bits {OUT_P_DELTA_MSB[7:0],OUT_P_DELTA_CSB[7:0]}(signed, 2's comp.) is Whole and OUT_P_DELTA_LSB[7:4] is Fractional(unsigned)
    
    int temp_Whole = ( (temp[0] << 8) | temp[1] );  // Assemble 16-bit Whole segment to determine the sign next.
    
    int MSB = temp_Whole >> 15;                                 // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x00007FFF;                              // Mask to retain 15 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's.
      {
          temp_Whole = (temp_Whole | 0xFFFF8000 );   // 32-bit
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);
    
    double temp_Fraction = 0.0625 * (double)((temp[2] >> 4) & 0x0F);   // right-shift bits 7:4 and isolate. Fractional values are in increments of 0.0625 m. i.e. 1111 is 0.0625*15 = 0.9375 m.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }
}

double MPL3115A2::MPL_Get_Temperature_Change()  // Returns Teperature reading.
{
    char temp[3];  //Dummy array to store Outgoing and incomming bytes. Cleared after the function returns. 
    
    temp[0] = 0x00;  // Prime the while() loop.

        temp[0] = OUT_T_DELTA_MSB;                        // Start the Temperature reading 
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,2);
        
        // Now we will reassemble the whole.fractional return value from 2 8-bit portions.
        // Temperature data: 12-bit unsigned in degrees C. First 8 bits {OUT_T_DELTA_MSB[7:0]}(signed, 2's comp.) is Whole and OUT_T_DELTA_LSB[7:4] is Fractional(unsigned)
    
    int temp_Whole = temp[0];  // Assemble 8-bit Whole segment to determine the sign next.
    
    int MSB = temp_Whole >> 7;                           // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x0000007F;                // Mask to retain 7 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's to turn into negative 2's comp rep.
      {
          temp_Whole = (temp_Whole | 0xFFFFFF80 );       // 32-bit. Pad with '1's all the way to MSB
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);
    
    double temp_Fraction = 0.0625 * (double)((temp[1] >> 4) & 0x0F);   // right-shift bits 7:4 and isolate. Fractional values are in increments of 0.0625 C. i.e. 1111 is 0.0625*15 = 0.9375 C.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }
}

void MPL3115A2::MPL_Trim_Pressure(int16_t P_Trim)  // Pressure Trimming [-512,508] Pa. 4Pa per LSB
{   
    char temp[2];
    
    // Input Over-Range safety
    if (P_Trim > 508)
    {
        P_Trim = 508; 
    }
    
     if (P_Trim < -512)
    {
        P_Trim = -512; 
    }
    
    char P_Trim_In = ( P_Trim  - ( P_Trim % 4 )) / 4;  // This expression makes sure that all possible inputs are trimmed to 4 Pa per LSB input byte. [-128,127]
    
        temp[0] = OFF_P;        // OFF_P register address
        temp[1] = P_Trim_In;    // Value to be written for Pressure offset
        
        _i2c.write(MPL3115A2_WRITE, temp, 2);
    
}

void MPL3115A2::MPL_Trim_Altitude(int8_t A_Trim)  // Altitude Trimming [-128,127] meters. 1 m per LSB.
{
    char temp[2];
    
    // Input Over-Range safety
    if (A_Trim > 127)
    {
        A_Trim = 127; 
    }
    
     if (A_Trim < -128)
    {
        A_Trim = -128; 
    }
    
        temp[0] = OFF_H;        // OFF_H register address
        temp[1] = A_Trim;    // Value to be written for Altitude offset
        
        _i2c.write(MPL3115A2_WRITE, temp, 2);
}

void MPL3115A2::MPL_Trim_Temperature(double T_Trim)  // Temperature trim [-8, 7.9375] degrees C. 0.0625 C per LSB.
{
  char temp[2];
  
  if (T_Trim > 7.9375)
    {
        T_Trim = 7.9375; 
    }
    
     if (T_Trim < -8)
    {
        T_Trim = -8; 
    } 
  
  char Trim_T_In = (char)(T_Trim / 0.0625);
  
  temp[0] = OFF_T;        // OFF_T register address
  temp[1] = Trim_T_In;    // Value to be written for Temperature offset
        
  _i2c.write(MPL3115A2_WRITE, temp, 2);
  
}

bool MPL3115A2::MPL_is_Active()  // Returns the status whether the device in Active (True) or Standby (False) mode.
{
    char temp[2];
    
    temp[0] = CTRL_REG1;        // CTRL_REG1 register address
    
    _i2c.write(MPL3115A2_WRITE, temp,1,true);
    _i2c.read(MPL3115A2_READ, temp,1);
    
    if((temp[0] & CTRL_REG1_SBYB) != 0)
        {
            return true;
        }
    else
        {
            return false;   
        }
}

void MPL3115A2::MPL_Set_Barometric_Reference(uint32_t Bar_Reference)  // Atmospheric reference at current location for Altitude calculations. Input is equivalent to Sea Level pressure @ current location. Unit: 2 Pa
{                                                                     // Deafult is 101 326 Pa.
    char temp[3]; 
    if (Bar_Reference > 110000){Bar_Reference = 110000;}   //Set Max value per data sheet.
    if (Bar_Reference < 50000 ){Bar_Reference = 110000;}   //Set Min value per data sheet.
    
    uint16_t Bar_Reference_In = (uint16_t)(Bar_Reference / 2);         // Register reqires input units in 2 Pa increments: i.e. to write 100 000 Pa the register must contain a value of 50 000.
    
    char MSB = ((Bar_Reference_In & 0xFF00) >> 8);   // Isolate the MSB for transfer
    char LSB = ( Bar_Reference_In & 0x00FF);         // Isolate the LSB for transfer
    
    
    temp[0] = BAR_IN_MSB;        // BAR_IN_MSB register address
    temp[1] = MSB;
    temp[2] = LSB;
    
    _i2c.write(MPL3115A2_WRITE, temp,3);
    
}

uint32_t MPL3115A2::MPL_Get_Barometric_Reference()
{
    char temp[3]; 
    
    temp[0] = BAR_IN_MSB;
    _i2c.write(MPL3115A2_WRITE, temp,1, true);
    _i2c.read(MPL3115A2_READ, temp,2);
    
   uint16_t Pressure_Reference = (( temp[0] << 8) | temp[1] );  // Assemble 16-bit unsigned value  from 2 8-bit register segments. 
   
   return (Pressure_Reference * 2);  // Return 2*value because register value is 2 times smaller of the actual. 
}

void MPL3115A2::MPL_Set_Pressure_Target(uint32_t P_Target)  //  Target Pressure for interrupts/alarms. Units: Pascals
{
    char temp[3]; 
    if (P_Target > 110000){P_Target = 110000;}   //Set Max value per data sheet.
    if (P_Target < 50000 ){P_Target = 110000;}   //Set Min value per data sheet.
    
    uint16_t P_Target_In = (uint16_t)(P_Target / 2);         // Register reqires input units in 2 Pa increments: i.e. to write 100 000 Pa the register must contain a value of 50 000.
    
    char MSB = ((P_Target_In & 0xFF00) >> 8);   // Isolate the MSB for transfer
    char LSB = ( P_Target_In & 0x00FF);         // Isolate the LSB for transfer
    
    
    temp[0] = P_TGT_MSB;        // P_TGT_MSB register address
    temp[1] = MSB;
    temp[2] = LSB;
    
    _i2c.write(MPL3115A2_WRITE, temp,3);
}

void MPL3115A2::MPL_Set_Altitude_Target(int16_t A_Target)   //  Target Altitude for interrupts/alarms. Units: meters
{
    char temp[3];

    if (A_Target > 5000){A_Target = 5000;}   //Set Max value per safety.
    if (A_Target < -550){A_Target = -550;}   //Set Min value per safety.
    
    char MSB = ((A_Target & 0xFF00) >> 8);   // Isolate the MSB for transfer
    char LSB = ( A_Target & 0x00FF);         // Isolate the LSB for transfer
    
    
    temp[0] = P_TGT_MSB;        // P_TGT_MSB register address
    temp[1] = MSB;
    temp[2] = LSB;
    
    _i2c.write(MPL3115A2_WRITE, temp,3);
}

void MPL3115A2::MPL_Set_Temperature_Target(int8_t T_Target) //  Target Temperature for interrupts/alarms. Units: Degrees C
{
    char temp[2];
    
    if (T_Target > 85){T_Target = 85;}     //Set Max value per data sheet.
    if (T_Target < -40 ){T_Target = -40;}  //Set Min value per data sheet.
    
    temp[0] = T_TGT;        // P_TGT_MSB register address
    temp[1] = T_Target;
     
    _i2c.write(MPL3115A2_WRITE, temp,2);
    
}

void MPL3115A2::MPL_Set_Pressure_Window(uint32_t P_Window)  //  Window for Pressure for interrupts/alarms. Units: 2 Pascals
{
    // Obtain the current value from the Pressure Target Register. Will be used to set safe operational windows not to exceed min/max operational values. 
    char temp[3]; 
    temp[0] = P_TGT_MSB;        // P_TGT_MSB register address
    
    _i2c.write(MPL3115A2_WRITE, temp,1, true);
    _i2c.read(MPL3115A2_READ, temp,2);
    
    uint32_t Current_Target = ( (temp[0] << 8 | temp[1]) & 0x0000FFFF ) * 2; // Reconstruct the Target value. Note: Target register contains value in 2Pa increments -> post-multiply by 2 to geta actual value. 
    
    // Max/Min Checks and corrections: The min Barometric pressure for the sensor is 50kPa. The Max is 110kPa. 
    // Interrupts will occur @ Target +/- Window: Target - Window, Target, and Target + Window. i.e. Target of 100000 and Window of 1000 will trigget @ 99000, 100000, and 101000 Pa. 
    if( ((Current_Target - P_Window) < 50000) || ((Current_Target + P_Window) > 110000) || (Current_Target == P_Window) )
    {
            P_Window = 0;  // Eliminate the window because the desired interrupts will be out-of-range for a pressure sensor.      
    }
    
    uint16_t P_Window_In = (uint16_t)(P_Window / 2); // Register reqires input units in 2 Pa increments: i.e. to write 100 000 Pa the register must contain a value of 50 000.
    
    char MSB = ((P_Window_In & 0xFF00) >> 8);   // Isolate the MSB for transfer
    char LSB = ( P_Window_In & 0x00FF);         // Isolate the LSB for transfer
    
    
    temp[0] = P_WND_MSB;        // P_WND_MSB register address
    temp[1] = MSB;
    temp[2] = LSB;
    
    _i2c.write(MPL3115A2_WRITE, temp,3);

}

void MPL3115A2::MPL_Set_Altitude_Window(uint16_t A_Window)   //  Window for Altitude for interrupts/alarms. Units: meters
{
    char temp[3]; 
    temp[0] = P_TGT_MSB;        // P_TGT_MSB register address
    
    _i2c.write(MPL3115A2_WRITE, temp,1, true);
    _i2c.read(MPL3115A2_READ, temp,2);
    
    int16_t Current_Target = ((temp[0] << 8) | temp[1]); // Reconstruct the Target value. Note: Target register contains value in 1 m increments. 
    
    // Max/Min Checks and corrections: The min Altitude for the Target implementation  is -550 m. The Max is 5000 m. 
    // Interrupts will occur @ Target +/- Window: Target - Window, Target, and Target + Window.
    if( ((Current_Target - A_Window) < -550) || ((Current_Target + A_Window) > 5000) || (Current_Target == A_Window) )
    {
            A_Window = 0;  // Eliminate the window because the desired interrupts will be out-of-range for a pressure sensor.      
    }
    
    char MSB = ((A_Window & 0xFF00) >> 8);   // Isolate the MSB for transfer
    char LSB = ( A_Window & 0x00FF);         // Isolate the LSB for transfer
    
    
    temp[0] = P_WND_MSB;        // P_WND_MSB register address
    temp[1] = MSB;
    temp[2] = LSB;
    
    _i2c.write(MPL3115A2_WRITE, temp,3);
}

void MPL3115A2::MPL_Set_Temperature_Window(uint8_t T_Window) //  Window for Temperature for interrupts/alarms. Units: Degrees C
{
    char temp[2]; 
    temp[0] = T_TGT;        // T_TGT register address
    
    _i2c.write(MPL3115A2_WRITE, temp,1, true);
    _i2c.read(MPL3115A2_READ, temp,1);
    
    int8_t Current_Target = temp[0]; // Note: Target register contains value in 1 C increments. 
    
    // Max/Min Checks and corrections: The min Temperatur for the Target implementation  is -40 m. The Max +85 C. 
    // Interrupts will occur @ Target +/- Window: Target - Window, Target, and Target + Window.
    if( ((Current_Target - T_Window) < -40) || ((Current_Target + T_Window) > 85) || (Current_Target == T_Window) )
    {
            T_Window = 0;  // Eliminate the window because the desired interrupts will be out-of-range for a Temperature sensor.      
    }
   
    temp[0] = T_WND;        // T_WND register address
    temp[1] = T_Window;
    
    _i2c.write(MPL3115A2_WRITE,temp,2);
}

double MPL3115A2::MPL_Get_Min_Pressure()  // Obtain the lowest recorded Pressure since the last reset
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
     
    if (Bar_Mode == true)   // Verify that the device is in Barometer mode so we do return Pressure from the register.
    {
        temp[0] = P_MIN_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
    }
    
    else
    {
        MPL_Barometer_Mode();   //Change to the Barometer Mode.
        
        temp[0] = P_MIN_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);     
    }
    
    // Now we will reassemble the whole.fractional return value from 3 8-bit portions.
    // Pressure data: 20-bit unsigned in Pa. First 18 bits {OUT_P_MSB[7:0],OUT_C_MSB[7:0],OUT_P_LSB[7:6]} is Whole and OUT_P_LSB[5:4] Fractional.
    
    double temp_Whole = (double)( (temp[0] << 10) | (temp[1] << 2) | (temp[2] >> 6) );  // Assemble 18-bit Whole segment
    
    double temp_Fraction = 0.25 * (double)((temp[2] >> 4) & 0x03);   // right-shift bits 5:4 and isolate. Fractional values are in increments of 0.25 Pa. i.e. 11 is 0.25*3 = 0.75 Pa. 10 is 0.25*2, and 01 is 0.25*1.
    
    return (temp_Whole + temp_Fraction);
}

double MPL3115A2::MPL_Get_Max_Pressure()  // Obtain the highest recorded Pressure since the last reset
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
     
    if (Bar_Mode == true)   // Verify that the device is in Barometer mode so we do return Pressure from the register.
    {
        temp[0] = P_MAX_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
    }
    
    else
    {
        MPL_Barometer_Mode();   //Change to the Barometer Mode.
        
        temp[0] = P_MAX_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);     
    }
    
    // Now we will reassemble the whole.fractional return value from 3 8-bit portions.
    // Pressure data: 20-bit unsigned in Pa. First 18 bits {OUT_P_MSB[7:0],OUT_C_MSB[7:0],OUT_P_LSB[7:6]} is Whole and OUT_P_LSB[5:4] Fractional.
    
    double temp_Whole = (double)( (temp[0] << 10) | (temp[1] << 2) | (temp[2] >> 6) );  // Assemble 18-bit Whole segment
    
    double temp_Fraction = 0.25 * (double)((temp[2] >> 4) & 0x03);   // right-shift bits 5:4 and isolate. Fractional values are in increments of 0.25 Pa. i.e. 11 is 0.25*3 = 0.75 Pa. 10 is 0.25*2, and 01 is 0.25*1.
    
    return (temp_Whole + temp_Fraction);
}

double MPL3115A2::MPL_Get_Min_Altitude()  // Obtain the lowest recorded Altitude since thelast reset
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
    
    if (Bar_Mode == false)   // Verify that the device is in Altimeter mode so we do return Altitude from the register.
    {
        temp[0] = P_MIN_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3); 
    }
    
    else
    {
        MPL_Altimeter_Mode();   //Change to the Barometer Mode.
        
        temp[0] = P_MIN_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
        
    }
    
    // Now we will reassemble the whole.fractional return value from 3 8-bit portions.
    // Altitude data: 20-bit unsigned in m. First 16 bits {OUT_P_MSB[7:0],OUT_C_MSB[7:0]}(signed, 2's comp.) is Whole and OUT_P_LSB[7:4] is Fractional(unsigned)
    
    int temp_Whole = ( (temp[0] << 8) | temp[1] );  // Assemble 16-bit Whole segment to determine the sign next.
    
    int MSB = temp_Whole >> 15;                                 // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x00007FFF;                              // Mask to retain 15 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's.
      {
          temp_Whole = (temp_Whole | 0xFFFF8000 );   // 32-bit
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);
    
    double temp_Fraction = 0.0625 * (double)((temp[2] >> 4) & 0x0F);   // right-shift bits 7:4 and isolate. Fractional values are in increments of 0.0625 m. i.e. 1111 is 0.0625*15 = 0.9375 m.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }
}

double MPL3115A2::MPL_Get_Max_Altitude()  // Obtain the highest recorded Altitude since thelast reset
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 
    
    if (Bar_Mode == false)   // Verify that the device is in Altimeter mode so we do return Altitude from the register.
    {
        temp[0] = P_MAX_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3); 
    }
    
    else
    {
        MPL_Altimeter_Mode();   //Change to the Barometer Mode.
        
        temp[0] = P_MAX_MSB;
        _i2c.write(MPL3115A2_WRITE,temp,1,true);
        _i2c.read(MPL3115A2_READ,temp,3);
        
    }
    
    // Now we will reassemble the whole.fractional return value from 3 8-bit portions.
    // Altitude data: 20-bit unsigned in m. First 16 bits {OUT_P_MSB[7:0],OUT_C_MSB[7:0]}(signed, 2's comp.) is Whole and OUT_P_LSB[7:4] is Fractional(unsigned)
    
    int temp_Whole = ( (temp[0] << 8) | temp[1] );  // Assemble 16-bit Whole segment to determine the sign next.
    
    int MSB = temp_Whole >> 15;                                 // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x00007FFF;                              // Mask to retain 15 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's.
      {
          temp_Whole = (temp_Whole | 0xFFFF8000 );   // 32-bit
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);
    
    double temp_Fraction = 0.0625 * (double)((temp[2] >> 4) & 0x0F);   // right-shift bits 7:4 and isolate. Fractional values are in increments of 0.0625 m. i.e. 1111 is 0.0625*15 = 0.9375 m.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }
}

double MPL3115A2::MPL_Get_Min_Temperature()  // Obtain the lowest recorded Temperature since thelast reset
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 


    temp[0] = T_MIN_MSB;                        // Start the Min Temperature reading 
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,2);
        
        // Now we will reassemble the whole.fractional return value from 2 8-bit portions.
        // Temperature data: 12-bit signed in degrees C. First 8 bits {OUT_T_MSB[7:0]}(signed, 2's comp.) is Whole and OUT_T_LSB[7:4] is Fractional(unsigned)
    
    int temp_Whole = temp[0];  // Assemble 8-bit Whole segment to determine the sign next.
    
    int MSB = temp_Whole >> 7;                           // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x0000007F;                // Mask to retain 7 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's to turn into negative 2's comp rep.
      {
          temp_Whole = (temp_Whole | 0xFFFFFF80 );       // 32-bit. Pad with '1's all the way to MSB
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);
    
    double temp_Fraction = 0.0625 * (double)((temp[1] >> 4) & 0x0F);   // right-shift bits 7:4 and isolate. Fractional values are in increments of 0.0625 C. i.e. 1111 is 0.0625*15 = 0.9375 C.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }   
}

double MPL3115A2::MPL_Get_Max_Temperature()  // Obtain the highest recorded Temperature since the last reset
{
    char temp[3];  //Dummy array to store Outgoiing and incomming bytes. Cleared after the function returns. 


    temp[0] = T_MAX_MSB;                     // Start the Min Temperature reading 
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,2);
        
        // Now we will reassemble the whole.fractional return value from 2 8-bit portions.
        // Temperature data: 12-bit signed in degrees C. First 8 bits {OUT_T_MSB[7:0]}(signed, 2's comp.) is Whole and OUT_T_LSB[7:4] is Fractional(unsigned)
    
    int temp_Whole = temp[0];  // Assemble 8-bit Whole segment to determine the sign next.
    
    int MSB = temp_Whole >> 7;                           // Extract the sign of of 2's complement value
      
    temp_Whole = temp_Whole & 0x0000007F;                // Mask to retain 7 non-sign bits
      
    if(MSB == 1)                                         // If extracted sign is negative pad 15-bit with 1's to turn into negative 2's comp rep.
      {
          temp_Whole = (temp_Whole | 0xFFFFFF80 );       // 32-bit. Pad with '1's all the way to MSB
      }
      
    double temp_Whole_dbl = (double)(temp_Whole);
    
    double temp_Fraction = 0.0625 * (double)((temp[1] >> 4) & 0x0F);   // right-shift bits 7:4 and isolate. Fractional values are in increments of 0.0625 C. i.e. 1111 is 0.0625*15 = 0.9375 C.
    
    if (temp_Whole_dbl < 0)   // if whole is negative, we substract the fractional part: i.e. -100.25 is -100 - 0.25 
        {
            return (temp_Whole_dbl - temp_Fraction);
        }
    
    else                      // if whole is positive, we add the fractional part as usual.      
        {
            return (temp_Whole_dbl + temp_Fraction);
        }   
}

void MPL3115A2::MPL_Reset_Min_P_A()  // Reset the Lowest recorded Pressure/Altitude
{   
    // To clear we write '0' to the register.
    char temp[4];
    
    temp[0] = P_MIN_MSB;
    temp[1] = 0x00;
    temp[2] = 0x00;
    temp[3] = 0x00;
    
    _i2c.write(MPL3115A2_WRITE,temp,4);
}

void MPL3115A2::MPL_Reset_Max_P_A()  // Reset the Highest recorded Pressure/Altitude
{
       // To clear we write '0' to the register.
    char temp[4];
    
    temp[0] = P_MAX_MSB;
    temp[1] = 0x00;
    temp[2] = 0x00;
    temp[3] = 0x00;
    
    _i2c.write(MPL3115A2_WRITE,temp,4);
}

void MPL3115A2::MPL_Reset_Min_T()  // Reset the Lowest recorded Temperature
{
       // To clear we write '0' to the register.
    char temp[3];
    
    temp[0] = T_MIN_MSB;
    temp[1] = 0x00;
    temp[2] = 0x00;
    
    _i2c.write(MPL3115A2_WRITE,temp,3);
}

void MPL3115A2::MPL_Reset_Max_T()  // Reset the Highest recorded Temperature
{
    // To clear we write '0' to the register.
    char temp[3];
    
    temp[0] = T_MAX_MSB;
    temp[1] = 0x00;
    temp[2] = 0x00;
    
    _i2c.write(MPL3115A2_WRITE,temp,3);
}

void MPL3115A2::MPL_One_Shot_Measure()  //Initiate one-shot acquisition of Pressure/Altitude and Temperature. Retrieve the data with MPL_Get_...() functions.
{
    char temp[2];
    
    temp[0] = CTRL_REG1;                        // Read contents of CTRL_REG1 so we can OR our command bit while preserving the previous contents
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,1);
    
    char temp_Reg1 = temp[0] | CTRL_REG1_OST;  // Makes sure we preserve the previous contents intact. 
    
    temp[0] = CTRL_REG1;
    temp[1] = temp_Reg1;
    _i2c.write(MPL3115A2_WRITE,temp,2);
}

bool MPL3115A2::MPL_System_Reset()  // Software reset of the MPL3115A2 unit. All registers defaulted. I2C is frozen to prevent data corruption.
{
    char temp[2];
    bool Status_Return;
    
    if(is_Reset == false)
    {
        // Initiate Software Reset
        temp[0] = CTRL_REG1;
        temp[1] = CTRL_REG1_RST;
        _i2c.write(MPL3115A2_WRITE,temp,2);
    
        is_Reset = true;  // Indicates that the device just undergo the reset. 
    }
    
    // Check if the device reset -> boot sequesce is complete and device is ready. 
    if (MPL_Who_Am_I_() != 0xC4)
    {
        Status_Return = false;
    }
    
    if (MPL_Who_Am_I_() == 0xC4)
    {   
         is_Reset = false;
         Status_Return = true;
    }
    
    return Status_Return;
}

void MPL3115A2::MPL_Altimeter_Mode()  // Set the device to operate as an Altimeter. Get_Altitude() must be used.
{
    char temp[2];
    
    temp[0] = CTRL_REG1;                        // Read contents of CTRL_REG1 so we can OR our command bit while preserving the previous contents
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,1);
    
    char temp_Reg1 = temp[0] | CTRL_REG1_ALT;  // Makes sure we preserve the previous contents intact while setting only the required control bit. . 
    
    temp[0] = CTRL_REG1;
    temp[1] = temp_Reg1;
    _i2c.write(MPL3115A2_WRITE,temp,2);

    Bar_Mode = false;  // Indicate that the device is set to Altimeter mode
}

void MPL3115A2::MPL_Barometer_Mode()  // Set the device to operate an a Barometer. Get_Pressure() must be used.
{
    char temp[2];
    
    temp[0] = CTRL_REG1;                        // Read contents of CTRL_REG1 so we can OR our command bit while preserving the previous contents
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,1);
    
    char temp_Reg1 = temp[0] & (~CTRL_REG1_ALT);  // Makes sure we preserve the previous contents intact while setting only the required control bit. This ANDs temp[0] with (0x7F) where 0x7F = ~0x80 (CTRL_REG1_ALT)
    
    temp[0] = CTRL_REG1;
    temp[1] = temp_Reg1;
    _i2c.write(MPL3115A2_WRITE,temp,2);
    
    Bar_Mode = true;  //Indicate that the device is set to Barometer mode
}

void MPL3115A2::MPL_Set_Oversampling(char Oversampling)  // Sets the oversampling according to user input: 1,2,4,8...128 samples averaging.
{   
    char o_s;
    char temp[2];
    
    switch(Oversampling)
    {
        case 1: o_s = 0x00; break;
        
        case 2: o_s = CTRL_REG1_OS_2; break;
        
        case 4: o_s = CTRL_REG1_OS_4; break;
        
        case 8: o_s = CTRL_REG1_OS_8; break;
        
        case 16: o_s = CTRL_REG1_OS_16; break;
        
        case 32: o_s = CTRL_REG1_OS_32; break;
        
        case 64: o_s = CTRL_REG1_OS_64; break;
        
        case 128: o_s = CTRL_REG1_OS_128; break;
        
        default: o_s = 0x00; break;
    }
 
    temp[0] = CTRL_REG1;                        // Read contents of CTRL_REG1 so we can OR our command bit while preserving the previous contents
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,1);
    
    char temp_Reg1 = (temp[0] & 0xC7) | o_s;   // The contents of the register preserved except bits [5:3] which are reset with 0xC7 {0011 1000}. After, the desired oversampling is ORed.
    
    temp[0] = CTRL_REG1;
    temp[1] = temp_Reg1;
    _i2c.write(MPL3115A2_WRITE,temp,2);
    
}


void MPL3115A2::MPL_Set_Interupt_Pins_and_Action(char Pin_Action, char Enable_Interrupts, char Interrupt_Route)  // Specify what pins generate the interrupts and how the interrupt is enerated.
{
    char temp[4];
    
    temp[0] = CTRL_REG3;            // Starts @CTRL_REG3. Each write auto-increments the register address to CTRL_REG4 and finally to CTRL_REG5
    temp[1] = Pin_Action;           // Define Polarity: Active Low/High and Internal Pullup or Open-Drain Configuration for INT1, INT2 pins. 
    temp[2] = Enable_Interrupts;    // Enable interrupts for various events: Data Ready, FIFO interrupt, Pressure Window, T Window, Pressure Threshold, T Threshold, Pressure Change, and T Change.
    temp[3] = Interrupt_Route;      // Defines to which pin the event (or group of events) is routed. INT1 or INT2. Default: All go to INT2. 
    
    _i2c.write(MPL3115A2_WRITE,temp,4);
    
}

char MPL3115A2::MPL_Get_Interrupt_Source()  // Since all interrupts are internaly ORed to the interrupt pins, this is needed to see what is causing the interrupt.
{   
    char temp[1];
    
    temp[0] = INT_SOURCE;                        // Read contents of INT_SOURCE
    _i2c.write(MPL3115A2_WRITE,temp,1,true);
    _i2c.read(MPL3115A2_READ,temp,1);
    
    return (temp[0]);
}
