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

UInt fpe2(uint8_t data[32])
{
    UInt ans = 0;
    
    ans += data[0] << 6;
    ans += data[1] << 2;

    return ans;
}

kern_return_t getErrorCode(kern_return_t err)
{
    return err & 0x3fff;
}

uint32_t toUInt32(char *key)
{
    uint32_t ans = 0;
    uint32_t shift = 24;

    if (strlen(key) != SMC_KEY_SIZE) {
        return 0;
    }

    for (int i = 0; i < SMC_KEY_SIZE; i++) {
        ans += key[i] << shift;
        shift -= 8;
    }

    return ans;
}

void toString(char *str, UInt32 val)
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
double to_fahrenheit(double tmp)
{
    // http://en.wikipedia.org/wiki/Fahrenheit#Definition_and_conversions
    return (tmp * 1.8) + 32;
}


/**
Celsius to Kelvin
*/
double to_kelvin(double tmp)
{
    // http://en.wikipedia.org/wiki/Kelvin
    return tmp + 273.15;
}


kern_return_t SMCOpen(void)
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

kern_return_t SMCClose(void)
{
    return IOServiceClose(conn);
}


kern_return_t callSMC(SMCParamStruct *inputStruct, SMCParamStruct *outputStruct)
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
    
    // Determine the exact error code
    if (result != kIOReturnSuccess) {
        result = getErrorCode(result);
    }

    return result;
}

kern_return_t SMCReadKey(char *key, SMCVal_t *val)
{
    kern_return_t result;
    SMCParamStruct inputStructure;
    SMCParamStruct outputStructure;

    memset(&inputStructure,  0, sizeof(SMCParamStruct));
    memset(&outputStructure, 0, sizeof(SMCParamStruct));
    memset(val, 0, sizeof(SMCVal_t));

    inputStructure.key = toUInt32(key);
    inputStructure.data8 = kSMCGetKeyInfo;

    result = callSMC(&inputStructure, &outputStructure);
    if (result != kIOReturnSuccess) {
        return result;
    }

    val->dataSize = outputStructure.keyInfo.dataSize;
    toString(val->dataType, outputStructure.keyInfo.dataType);
    inputStructure.keyInfo.dataSize = val->dataSize;
    inputStructure.data8 = kSMCReadKey;

    result = callSMC(&inputStructure, &outputStructure);
    if (result != kIOReturnSuccess) {
        return result;
    }

    memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));

    return result;
}

double getTMP(char *key, tmp_unit_t unit)
{
    SMCVal_t val;
    kern_return_t result;

    result = SMCReadKey(key, &val);
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
