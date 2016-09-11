#include "mbed.h"
#ifndef MPL3115A2_IO_H_
#define MPL3115A2_IO_H_

#include <stdint.h>    // to handle uintN_t and intN_t integer types

class MPL3115A2
{

public:

    MPL3115A2(PinName sda, PinName scl);
    
    void MPL_Init();               // Use to initialize proper settings: Oversampling, Device Mode, Interrupts etc.
    
    void MPL_Set_Oversampling(char Oversampling);   // Set the oversample ration of the data aquisition. 1 to 128 in 2^n intervals. NOTE: Consult REGISTER_MAP.h for minimum timing intervals

    char MPL_Who_Am_I_();          // Reads and returns the device ID. By default MPL3115A2 return 0xC4.
    
    char MPL_Get_Status();         // Reads the STATUS register and returns the contents. Can find if new P,A,T data is available for retrieval.

    double MPL_Get_Pressure();     // Returns the Atmospheric Pressure reading.

    double MPL_Get_Altitude();     // Returns the Altitude reading.

    double MPL_Get_Temperature();  // Returns Teperature reading.

    double MPL_Get_Pressure_Change();     // Returns the Atmospheric Pressure reading.

    double MPL_Get_Altitude_Change();     // Returns the Altitude reading.

    double MPL_Get_Temperature_Change();  // Returns Teperature reading.

    void MPL_Trim_Pressure(int16_t P_Trim);  // Pressure Trimming [-512,508] Pa. 4Pa per LSB

    void MPL_Trim_Altitude(int8_t A_Trim);  // Altitude Trimming [-128,127] meters. 1 m per LSB.

    void MPL_Trim_Temperature(double T_Trim);  // Temperature trim [-8, 7.9375] degrees C. 0.0625 C per LSB.

    bool MPL_is_Active();  // Returns the status whether the device in Active (True) or Standby (False) mode.

    void MPL_Set_Barometric_Reference(uint32_t Bar_Reference);  // Atmospheric reference at current location for Altitude calculations. Input is equivalent to Sea Level pressure @ current location. Unit: Pascals
    
    uint32_t MPL_Get_Barometric_Reference();  // Returns current Atmospheric reference at current location for Altitude calculations. 

    void MPL_Set_Pressure_Target(uint32_t P_Target);  //  Target Pressure for interrupts/alarms. Units: Pascals  [50kPa to 110kPa is 2Pa increments]

    void MPL_Set_Altitude_Target(int16_t A_Target);   //  Target Altitude for interrupts/alarms. Units: meters   [0 to 5000 meters. 1m increments]

    void MPL_Set_Temperature_Target(int8_t T_Target); //  Target Temperature for interrupts/alarms. Units: Degrees C

    void MPL_Set_Pressure_Window(uint32_t P_Window);  //  Window for Pressure for interrupts/alarms. Units: Pascals

    void MPL_Set_Altitude_Window(uint16_t A_Window);   //  Window for Altitude for interrupts/alarms. Units: meters

    void MPL_Set_Temperature_Window(uint8_t T_Window); //  Window for Temperature for interrupts/alarms. Units: Degrees C

    double MPL_Get_Min_Pressure();  // Obtain the lowest recorded Pressure since last the reset

    double MPL_Get_Max_Pressure();  // Obtain the highest recorded Pressure since the last reset

    double MPL_Get_Min_Altitude();  // Obtain the lowest recorded Altitude since last the reset

    double MPL_Get_Max_Altitude();  // Obtain the highest recorded Altitude since last the reset

    double MPL_Get_Min_Temperature();  // Obtain the lowest recorded Temperature since last the reset

    double MPL_Get_Max_Temperature();  // Obtain the highest recorded Temperature since last the reset

    void MPL_Reset_Min_P_A();  // Reset the Lowest recorded Pressure/Altitude

    void MPL_Reset_Max_P_A();  // Reset the Highest recorded Pressure/Altitude

    void MPL_Reset_Min_T();  // Reset the Lowest recorded Temperature

    void MPL_Reset_Max_T();  // Reset the Highest recorded Temperature

    void MPL_One_Shot_Measure();  //Initiate one-shot acquisition of Pressure/Altitude and Temperature. Retrieve the data with MPL_Get_...() functions.

    bool MPL_System_Reset();  // Software reset of the MPL3115A5 unit. All registers defaulted. I2C is frozen to prevent data corruption. Returns 'true' if the device is succesfully reset and is ready after boot. '0' - otherwise.

    void MPL_Altimeter_Mode();  // Set the device to operate as an Altimeter. Get_Altitude() must be used.

    void MPL_Barometer_Mode();  // Set the device to operate an a Barometer. Get_Pressure() must be used.

    void MPL_Set_Interupt_Pins_and_Action(char Pin_Action, char Enable_Interrupts, char Interrupt_Route);  // Specify what pins generate the interrupts.

    char MPL_Get_Interrupt_Source();  // Since all interrupts are internaly ORed to the interrupt pins, this is needed to see what is causing the interrupt.



private:

    I2C _i2c;

    bool Bar_Mode;
    bool is_Reset;
    
    static const uint32_t frequency  = 400000;

};

#endif

