/*
 * Apple System Management Control (SMC) Tool
 * Copyright (C) 2006 devnull 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#define DATATYPE_FPE2         "fpe2"
#define DATATYPE_UINT8        "ui8 "
#define DATATYPE_UINT16       "ui16"
#define DATATYPE_UINT32       "ui32"
#define DATATYPE_SP78         "sp78"

// key values
#define SMC_KEY_CPU_TEMP      "TC0D"

#define IOSERVICE_SMC     "AppleSMC" 
static const int SMC_KEY_SIZE = 4;

typedef enum {
    Celsius,
    Fahrenheit,
    Kelvin
} tmp_unit_t;

typedef enum {
    kSMCSuccess     = 0,
    kSMCError       = 1,
    kSMCKeyNotFound = 0x84
} kSMC_t;

typedef enum {
    kSMCUserClientOpen  = 0,
    kSMCUserClientClose = 1,
    kSMCHandleYPCEvent  = 2,
    kSMCReadKey         = 5,
    kSMCWriteKey        = 6,
    kSMCGetKeyCount     = 7,
    kSMCGetKeyFromIndex = 8,
    kSMCGetKeyInfo      = 9
} selector_t;

typedef struct {
    unsigned char    major;
    unsigned char    minor;
    unsigned char    build;
    unsigned char    reserved;
    unsigned short   release;
} SMCVersion;

typedef struct {
    uint16_t    version;
    uint16_t    length;
    uint32_t    cpuPLimit;
    uint32_t    gpuPLimit;
    uint32_t    memPLimit;
} SMCPLimitData;

typedef struct {
    IOByteCount         dataSize;
    uint32_t            dataType;
    uint8_t             dataAttributes;
} SMCKeyInfoData;

typedef struct {
    uint32_t            key;
    SMCVersion          vers;
    SMCPLimitData       pLimitData;
    SMCKeyInfoData      keyInfo;
    uint8_t             result;
    uint8_t             status;
    uint8_t             data8;
    uint32_t            data32;    
    uint8_t             bytes[32];
} SMCParamStruct;

typedef struct {
    kSMC_t  kSMC;
    UInt32  dataSize;
    char    dataType[5];
    char    bytes[32];
} SMCVal_t;


// Prototypes
double getTMP(char *key);
