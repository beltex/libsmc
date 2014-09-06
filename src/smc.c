/*
 * Apple System Management Controller (SMC) API from user space for Intel based
 * Macs. Works by talking to the AppleSMC.kext (kernel extension), the driver
 * for the SMC.
 *
 * smc.c
 * c-smc
 *
 * Copyright (C) 2014  beltex <https://github.com/beltex>
 *
 * Based off of fork from:
 * osx-cpu-temp <https://github.com/lavoiesl/osx-cpu-temp>
 *
 * With credits to:
 *
 * Copyright (C) 2006 devnull 
 * Apple System Management Control (SMC) Tool 
 *
 * Copyright (C) 2006 Hendrik Holtmann
 * smcFanControl <https://github.com/hholtmann/smcFanControl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <IOKit/IOKitLib.h>

#include "smc.h"

static io_connect_t conn;


//--------------------------------------------------------------------------
// MARK: HELPERS - TYPE CONVERSION
//--------------------------------------------------------------------------


/**
Convert data from SMC of fpe2 type to human readable.
    
:param: data Data from the SMC to be converted. Assumed data size of 2.
:returns: Converted data
*/
static UInt from_fpe2(uint8_t data[32])
{
    UInt ans = 0;
    
    // Data type for fan calls - fpe2
    // This is assumend to mean floating point, with 2 exponent bits
    // http://stackoverflow.com/questions/22160746/fpe2-and-sp78-data-types
    ans += data[0] << 6;
    ans += data[1] << 2;

    return ans;
}


/**
Convert SMC key to uint32_t. This must be done to pass it to the SMC.
    
:param: key The SMC key to convert
:returns: uint32_t translation.
          Returns zero if key is not 4 characters in length.
*/
static uint32_t to_uint32_t(char *key)
{
    uint32_t ans   = 0;
    uint32_t shift = 24;

    // SMC key is expected to be 4 bytes - thus 4 chars
    if (strlen(key) != SMC_KEY_SIZE) {
        return 0;
    }

    for (int i = 0; i < SMC_KEY_SIZE; i++) {
        ans += key[i] << shift;
        shift -= 8;
    }

    return ans;
}


/**
For converting the dataType return from the SMC to human readable 4 byte
multi-character constant.
*/
static void to_string(char *str, UInt32 val)
{
    str[0] = '\0';
    sprintf(str, "%c%c%c%c", 
            (unsigned int) val >> 24,
            (unsigned int) val >> 16,
            (unsigned int) val >> 8,
            (unsigned int) val);
}


//--------------------------------------------------------------------------
// MARK: HELPERS - TMP CONVERSION
//--------------------------------------------------------------------------


/**
Celsius to Fahrenheit
*/
static double to_fahrenheit(double tmp)
{
    // http://en.wikipedia.org/wiki/Fahrenheit#Definition_and_conversions
    return (tmp * 1.8) + 32;
}


/**
Celsius to Kelvin
*/
static double to_kelvin(double tmp)
{
    // http://en.wikipedia.org/wiki/Kelvin
    return tmp + 273.15;
}


//--------------------------------------------------------------------------
// MARK: "PRIVATE" FUNCTIONS
//--------------------------------------------------------------------------


/**
Make a call to the SMC
    
:param: inputStruct Struct that holds data telling the SMC what you want
:param: outputStruct Struct holding the SMC's response
:returns: I/O Kit return code
*/
static kern_return_t call_smc(SMCParamStruct *inputStruct,
                              SMCParamStruct *outputStruct)
{
    kern_return_t result;
    size_t inputStructCnt;
    size_t outputStructCnt;

    inputStructCnt  = sizeof(SMCParamStruct);
    outputStructCnt = sizeof(SMCParamStruct);

    result = IOConnectCallStructMethod(conn, kSMCHandleYPCEvent,
                                             inputStruct,
                                             inputStructCnt,
                                             outputStruct,
                                             &outputStructCnt);
    
    if (result != kIOReturnSuccess) {
        // IOReturn error code lookup
        // See "Accessing Hardware From Applications -> Handling Errors" Apple doc
        result = err_get_code(result);
    }

    return result;
}


/**
Read data from the SMC
    
:param: key The SMC key
*/
static kern_return_t read_smc(char *key, SMCVal_t *val)
{
    kern_return_t result;
    SMCParamStruct inputStructure;
    SMCParamStruct outputStructure;

    memset(&inputStructure,  0, sizeof(SMCParamStruct));
    memset(&outputStructure, 0, sizeof(SMCParamStruct));
    memset(val, 0, sizeof(SMCVal_t));

    inputStructure.key = to_uint32_t(key);
    inputStructure.data8 = kSMCGetKeyInfo;

    result = call_smc(&inputStructure, &outputStructure);
    if (result != kIOReturnSuccess) {
        return result;
    }

    val->dataSize = outputStructure.keyInfo.dataSize;
    to_string(val->dataType, outputStructure.keyInfo.dataType);
    inputStructure.keyInfo.dataSize = val->dataSize;
    inputStructure.data8 = kSMCReadKey;

    result = call_smc(&inputStructure, &outputStructure);
    if (result != kIOReturnSuccess) {
        return result;
    }

    memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));

    return result;
}


/**
Write data to the SMC.

:returns: IOReturn IOKit return code
*/
static kern_return_t write_smc(char *key, SMCVal_t *val)
{
    return kIOReturnSuccess;
}


//--------------------------------------------------------------------------
// MARK: "PUBLIC" FUNCTIONS
//--------------------------------------------------------------------------


/**
Open a connection to the SMC
    
:returns: kIOReturnSuccess on successful connection to the SMC.
*/
kern_return_t open_smc(void)
{
    kern_return_t result;
    io_service_t service;

    service = IOServiceGetMatchingService(kIOMasterPortDefault,
                                          IOServiceMatching(IOSERVICE_SMC));
   
    if (service == 0) {
        // NOTE: IOServiceMatching documents 0 on failure
        printf("ERROR: %s NOT FOUND\n", IOSERVICE_SMC);
        return kIOReturnError;
    }

    result = IOServiceOpen(service, mach_task_self(), 0, &conn);
    IOObjectRelease(service);

    return result;
}


/**
Close connection to the SMC
    
:returns: kIOReturnSuccess on successful close of connection to the SMC.
*/
kern_return_t close_smc(void)
{
    return IOServiceClose(conn);
}


/**
Check if an SMC key is valid. Useful for determining if a certain machine has
particular sensor or fan for example.
    
:param: key The SMC key to check. 4 byte multi-character constant. Must be 4
            characters in length.
:returns: True if the key is found, false otherwise
*/
bool is_key_valid(char *key)
{
    return true;
}


/**
Get the current temperature from a sensor
    
:param: key The temperature sensor to read from
:param: unit The unit for the temperature value.
:returns: Temperature of sensor. If the sensor is not found, or an error
          occurs, return will be zero
*/
double get_tmp(char *key, tmp_unit_t unit)
{
    SMCVal_t val;
    kern_return_t result;

    result = read_smc(key, &val);
    // read succeeded - check returned value
    if (result == kIOReturnSuccess && val.dataSize > 0
                                   && strcmp(val.dataType, DATATYPE_SP78) == 0) {
        // convert sp78 value to temperature
        double tmp = val.bytes[0];      
  
        switch (unit) {
            case CELSIUS:
                break;
            case FAHRENHEIT:
                tmp = to_fahrenheit(tmp);
                break;
            case KELVIN:
                tmp = to_kelvin(tmp);
                break;
        }

        return tmp;
    }

    // read failed
    return 0.0;
}


//--------------------------------------------------------------------------
// MARK: FAN FUNCTIONS
//--------------------------------------------------------------------------


/**
Get the current speed (RPM - revolutions per minute) of a fan.
    
:param: num The number of the fan to check
:returns: The fan RPM. If the fan is not found, or an error occurs, return
          will be zero
*/
UInt get_fan_rpm(UInt num)
{
    return 0;
}


/**
Get the number of fans on this machine.

:returns: The number of fans
*/
UInt get_num_fans(void)
{
    return 0;
}


/**
Set the speed (RPM - revolutions per minute) of a fan. This method requires root
privlages.

WARNING: You are playing with hardware here, BE CAREFUL.

:param: num The number of the fan to set
:param: rpm The speed you would like to set the fan to.
:param: auth Should the function do authentication?
:return: True if successful, false otherwise
*/
bool set_fan_rpm(UInt num, UInt rpm, bool auth)
{
    return true;
}
