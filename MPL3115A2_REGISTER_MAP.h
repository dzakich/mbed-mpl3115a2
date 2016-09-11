/*!
 *   This Section defines MPL3115A2 register addresses and important masks related to
 *   most registers. 
 *
*/

#ifndef MPL3115A2_REGISTER_MAP_H_
#define MPL3115A2_REGISTER_MAP_H_

//=== Device Registers Addresses, Description and Definition ===

//                                      | Reset |     | Type |     | Auto-Increment Address |     | Description                |      | Comments |
   
#define STATUS          0x00        //    0x00           R           0x01                           Sensor Status Register              Alias for DR_STATUS or F_STATUS
#define OUT_P_MSB       0x01        //    0x00           R           0x02 | 0x01                    Pressure Data Out MSB               Bits 12-19 of 20-bit real-time pressure sample | Root pointer to Pr and Tp FIFO data
#define OUT_P_CSB       0x02        //    0x00           R           0x03                           Pressure Data Out MSB               Bits 4-11 of 20-bit real-time Pressure sample
#define OUT_P_LSB       0x03        //    0x00           R           0x04                           Pressure Data Out MSB               Bits 0-3  of 20-bit real-time Pressure sample
#define OUT_T_MSB       0x04        //    0x00           R           0x05                           Temperature Data Out MSB            Bits 4-11 of 12-bit real-time Temperature sample
#define OUT_T_LSB       0x05        //    0x00           R           0x00                           Temperature Data Out LSB            Bits 0-3  of 12-bit real-time Temperature sample
#define DR_STATUS       0x06        //    0x00           R           0x07                           Sensor Status Register              Data Ready status information
#define OUT_P_DELTA_MSB 0x07        //    0x00           R           0x08                           Pressure Data Out Delta MSB         Bits 12-19 of 20-bit Pressure change data
#define OUT_P_DELTA_CBS 0x08        //    0x00           R           0x09                           Pressure Data Out Delta CSB         Bits 4-11  of 20-bit Pressure change data
#define OUT_P_DELTA_LSB 0x09        //    0x00           R           0x0A                           Pressure Data Out Delta LSB         Bits 0-3   of 20-bit Pressure change data
#define OUT_T_DELTA_MSB 0x0A        //    0x00           R           0x0B                           Temperature Data Out Delta LSB      Bits 4-11  of 12-bit Temperature change data
#define OUT_T_DELTA_LSB 0x0B        //    0x00           R           0x06                           Temperature Data Out Delta LSB      Bits 0-3   of 12-bit Temperature change data

#define WHO_AM_I        0x0C        //    0xC4           R           0x0D                           Device Identification Register      Fixed Device ID Number
#define MPL3115A2_WRITE 0xC0        //    NA             NA          NA                             8-bit Write                         The standard 7-bit I2C slave address is 0x60 [_110 0000]. Left shifted by 1 and 0 LSB
#define MPL3115A2_READ  0xC1        //    NA             NA          NA                             8-bit Read                          The standard 7-bit I2C slave address is 0x60 [_110 0000]. Left shifted by 1 and 1 LSB

#define F_STATUS        0x0D        //    0x00           R           0x0E                           FIFO Status Register                FIFO Status: No FIFO event detected
#define F_DATA          0x0E        //    0x00           R           0x0E                           FIFO 8-bit Data Access              FIFO 8-bit data access
#define F_SETUP         0x0F        //    0x00           R/W         0x10                           FIFO Setup Register                 FIFO setup
#define TIME_DLY        0x10        //    0x00           R           0x11                           Time Delay                          Time since FIFO overflow
#define SYSMOD          0x11        //    0x00           R           0x12                           System Mode Register                Current System mode
#define INT_SOURCE      0x12        //    0x00           R           0x13                           Interrupt Source Register           Interrupt Status
#define PT_DATA_CFG     0x13        //    0x00           R/W         0x14                           PT Data Configuration Register      Data Event Flag configuration
#define BAR_IN_MSB      0x14        //    0xC5           R/W         0x15                           BAR input in MSB                    Barometric Input for Altitude calculation bith 8-15
#define BAR_IN_LSB      0x15        //    0xE7           R/W         0x16                           BAR input in LSB                    Barometric Input for Altitude calculation bith 0-7
#define P_TGT_MSB       0x16        //    0x00           R/W         0x17                           Pressure Target MSB                 Pressure/Altitude Target Value bits 8-15
#define P_TGT_LSB       0x17        //    0x00           R/W         0x18                           Pressure Target MSB                 Pressure/Altitude Target Value bits 0-7
#define T_TGT           0x18        //    0x00           R/W         0x19                           Temperature Target                  Temperature Target Value
#define P_WND_MSB       0x19        //    0x00           R/W         0x1A                           Pressure/Altitude Window MSB        Pressure/Altitude window value bits 8-15
#define P_WND_LSB       0x1A        //    0x00           R/W         0x1B                           Pressure/Altitude Window LSB        Pressure/Altitude window value bits 0-7
#define T_WND           0x1B        //    0x00           R/W         0x1C                           Temperature Window                  Temperature Winndow Value
#define P_MIN_MSB       0x1C        //    0x00           R/W         0x1D                           Minimum Pressure Data Out MSB       Minimum Pressure/Altitude bits 12-19
#define P_MIN_CSB       0x1D        //    0x00           R/W         0x1E                           Minimum Pressure Data Out CSB       Minimum Pressure/Altitude bits 4-11
#define P_MIN_LSB       0x1E        //    0x00           R/W         0x1F                           Minimum Pressure Data Out LSB       Minimum Pressure/Altitude bits 0-3
#define T_MIN_MSB       0x1F        //    0x00           R/W         0x20                           Minimum Temperature Data Out MSB    Minimum Temperature bits 8-15
#define T_MIN_LSB       0x20        //    0x00           R/W         0x21                           Minimum Temperature Data Out LSB    Minimum Temperature bits 0-7
#define P_MAX_MSB       0x21        //    0x00           R/W         0x22                           Maximum Pressure Data Out MSB       Maximum Pressure/Altitude bits 12-19
#define P_MAX_CSB       0x22        //    0x00           R/W         0x23                           Maximum Pressure Data Out CSB       Maximum Pressure/Altitude bits 4-11
#define P_MAX_LSB       0x23        //    0x00           R/W         0x24                           Maximum Pressure Data Out LSB       Maximum Pressure/Altitude bits 0-3
#define T_MAX_MSB       0x24        //    0x00           R/W         0x25                           Maximum Temperature Data Out MSB    Maximum Temperature bits 8-15
#define T_MAX_LSB       0x25        //    0x00           R/W         0x26                           Maximum Temperature Data Out LSB    Maximum Temperature bits 0-7
#define CTRL_REG1       0x26        //    0x00           R/W         0x27                           Control Register 1                  Modes, Oversampling
#define CTRL_REG2       0x27        //    0x00           R/W         0x28                           Control Register 2                  Aquisition Time Step
#define CTRL_REG3       0x28        //    0x00           R/W         0x29                           Control Register 3                  Interrup Pin Configuration
#define CTRL_REG4       0x29        //    0x00           R/W         0x2A                           Control Register 4                  Interrupt Enables
#define CTRL_REG5       0x2A        //    0x00           R/W         0x2B                           Control Register 5                  Interrupt Output Pin Assignment 
#define OFF_P           0x2B        //    0x00           R/W         0x2C                           Pressure Data User Offset           Pressure Data Offset     
#define OFF_T           0x2C        //    0x00           R/W         0x2D                           Temperature Data User Offset        Temperature Data Offset 
#define OFF_H           0x2D        //    0x00           R/W         0x0C                           Altitude Data User Offset           Altitude Data Offset  

//=== End of the Section ===




//=== Register Masks ===
/*
    To use the Mask, the Register value must be read and logically ANDed with the desired Mask (or ORed compination of Masks ). 
    The value is then compared with the Mask to see if the register bit is set to 1 or 0.
*/

//--- DR_STATUS Register [Indicates Real-Time status of the sample data]---

#define DR_PTOW 0x80  // Pressure/Altituda OR Temperature data overwrite. Cleared when OUT_P_MSB or OUT_T_MSB are read.
#define DR_POW  0x40  // Pressure/Altitude overwrite. Cleared when OUT_P_MSB is read.
#define DR_TOW  0x20  // Temperature data overwrite. Cleared when OUT_T_MSB is read.
#define DR_PTDR 0x08  // Pressure/Altitude OR Temperature data ready. New aquisition is availble for reading. Cleared anytime OUT_P_MSB or OUT_T_MSB are read.
#define DR_PDR  0x04  // Pressure/Altitude data ready. New aquisition is availble for reading. Cleared anytime OUT_P_MSB is read.
#define DR_TDR  0x02  // Temperature data ready. New aquisition is availble for reading. Cleared anytime OUT_T_MSB is read.

//--- SYSMOD Register [Current Operating Mode] ---

#define SYSMOD_ACTIVE  0x01  // If not ACTIVE then the device is in STANDBY mode

//--- INT_SOURCE Register [System Interrupt Status] ---

#define SRC_DRDY 0x80  // Data interrupt status. '1' indicates that Pres/Alt or Temp data ready interrupt is active; indicates the presence of a new data and/or data overwrite. Cleared by R the STATUS and Pres/Temp register
#define SRC_FIFO 0x40  // FIFO interrupt status. '1' indicates that a FIFO interrupt even such as an overflow occured. Cleared by R the F_STATUS register
#define SRC_PW   0x20  // Altitude/Pressure alerter status bit near or equal to target Pressure/Altitude (near is within target value +/- window value). Window must be non-zero for the interrupt to occur
#define SRC_TW   0x10  // Temperature alerter status bit near or equal to target Temperature (near is within target value +/- window value). Window must be non-zero for the interrupt to occur
#define SRC_PTH  0x08  // Altitude/Pressure Threshold Interrupt. With non-zero Window, the trigger will occur @ upper, center, and lower targets. If Window is zero, the trigger only occurs @ center target.
#define SRC_TTH  0x04  // Temperature Threshold Interrupt. With non-zero Window, the trigger will occur @ upper, center, and lower targets. If Window is zero, the trigger only occurs @ center target.
#define SRC_PCNG 0x02  // Delta P interrupt status
#define SRC_TCNG 0x01  // Delta T interrupt status

//--- PT_DATA_CFG Register [configures the Pressure data, Temperature data and event flag generator] ---

#define DREM 0x04   // Data ready event mode. If DREM'1' and and one or more of rdy event flags (PDEFE, TDEFE) are enabled, then SRC_DRDY event flag will be raised in INT_SOURCE register upon change in state of the data. 
                    // If DREM is '0', and one of one of more of rdy event flags are enabled, event SRC_DRDY will be raised when the system acquires a new set of data.
#define PDEFE 0x02  // Data Event flag enable on new Pressure/Altitude data. '1' Raise event flag on new Pres/Alt and '0' disabled
#define TDEFE 0x01  // Data Event flag enable on new Temperature data. '1' Raise event flag on new Temperature and '0' disabled

//--- Control Registers Section ---

//--- CTRL_REG1 Register ---

#define CTRL_REG1_SBYB   0x01  // '1' for ACTIVE where the system makes periodic measurements based off ST bits in CTRL_REG2. '0' - the system is in STANDBY (on-demand measurements)
#define CTRL_REG1_OST    0x02  // OST bit will iniiate the measurement immediately! On-demand calling must obide dead-time based on oversampling OS[3:5] With STANDBY enabled OST is auto-clear. Read OST bit prior to writing to it again. 
#define CTRL_REG1_RST    0x04  // Software reset. '1' Device reseat enabled. The bit is auto-clear after reset.define
#define CTRL_REG1_OS_2   0x08  // Oversample Ratio of 2. Minimum time btween Data Samples is 10 ms
#define CTRL_REG1_OS_4   0x10  // Oversample Ratio of 4. Minimum time btween Data Samples is 18 ms 
#define CTRL_REG1_OS_8   0x18  // Oversample Ratio of 8. Minimum time btween Data Samples is 34 ms
#define CTRL_REG1_OS_16  0x20  // Oversample Ratio of 16. Minimum time btween Data Samples is 66 ms
#define CTRL_REG1_OS_32  0x28  // Oversample Ratio of 32. Minimum time btween Data Samples is 130 ms
#define CTRL_REG1_OS_64  0x30  // Oversample Ratio of 64. Minimum time btween Data Samples is 258 ms
#define CTRL_REG1_OS_128 0x38  // Oversample Ratio of 128. Minimum time btween Data Samples is 512 ms
#define CTRL_REG1_RAW    0x40  // RAW output mode. Direct ADC readings. Uncompensated.
#define CTRL_REG1_ALT    0x80  // Altimeter/Barometer Mode. '1'- Altimeter Mode and '0' - Barometer Mode. 

//--- CTRL_REG2 Register ---

#define CTRL_REG2_ALARM_SEL   0x10  // Selects target value for SRC_PW/SRC_TW and SRC_PTH/SRC_TTH. '0' - values in P_TGT_MSB, P_TGT_LSB and T_TGT are used. 
                                    // '1' -  values in OUT_P/OUT_T are used for calculation interrupts SRC_PW/SRC_TW and SRC_PTH/SRC_TTH.

#define CTRL_REG2_LOAD_OUTPUT 0x20  // To load the target values for SRC_PW/SRC_TW and SRC_PTH/SRC_TTH. '0' - do not load OUT_P/OUT_T as target values.
                                    // '1' - The next values of OUT_P/OUT_T are used to set the target values for the interrupts.

//--- CTRL_REG3 Register [Interrupt Control Register] ---

#define CTRL_REG3_IPOL1  0x20  // Selects the polarity of the interrupt signal. '0' - active low and '1' - active high. 
#define CTRL_REG3_PP_OD1 0x10  // Configures the interrupt pin to Push-Pull or in Open-Drain on interrupt pad INT1
#define CTRL_REG3_IPOL2  0x02  // Selects the polarity of the interrupt signal. '0' - active low and '1' - active high. 
#define CTRL_REG3_PP_OD2 0x01  // Configures the interrupt pin to Push-Pull or in Open-Drain on interrupt pad INT2

//--- CTRL_REG4 Register [Interrupt Enable Register] ---

#define CTRL_REG4_INT_EN_DRDY 0x80  // Data Ready Interrupt '1' - Data Ready Enable and '0' - Disable
#define CTRL_REG4_INT_EN_FIFO 0x40  // FIFO Interrupt '1' - Enabled, '0' - Disabled
#define CTRL_REG4_INT_EN_PW   0x20  // Pressure Window Interrupt '1' - Enabled, '0' - Disabled
#define CTRL_REG4_INT_EN_TW   0x10  // Temperature Window Interrupt '1' - Enabled, '0' - Disabled
#define CTRL_REG4_INT_EN_PTH  0x08  // Pressure Threshold Interrupt '1' - Enabled, '0' - Disabled
#define CTRL_REG4_INT_EN_TTH  0x04  // Temperature Threshold Interrupt '1' - Enabled, '0' - Disabled  
#define CTRL_REG4_INT_EN_PCHG 0x02  // Pressure Change Interrupt '1' - Enabled, '0' - Disabled 
#define CTRL_REG4_INT_EN_TCHG 0x01  // Temperature Change Interrupt '1' - Enabled, '0' - Disabled

//--- CTRL_REG5 Register [Interrupt Configuration Register] ---

#define CTRL_REG5_INT_CFG_DRDY 0x80  // Data Ready  '1' - Interrupt is routed to INT1, '0' - Interrupt is routed to INT2 [common to all]
#define CTRL_REG5_INT_CFG_FIFO 0x40  // FIFO
#define CTRL_REG5_INT_CFG_PW   0x20  // Pressure Window
#define CTRL_REG5_INT_CFG_TW   0x10  // Temperature Window
#define CTRL_REG5_INT_CFG_PTH  0x08  // Pressure Threshold
#define CTRL_REG5_INT_CFG_TTH  0x04  // Temperature Threshold
#define CTRL_REG5_INT_CFG_PCHG 0x02  // Pressure Change
#define CTRL_REG5_INT_CFG_TCHG 0x01  // Temperature Change

// NOTE: All interrupts are logically ORed thus more than one functional block can assrt the interrupt silmutaneously. 
// Host is responsible for reading INT_SOURCE register to determine the source of the interrupt. 


#endif