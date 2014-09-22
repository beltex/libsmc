/*
 * Apple System Management Controller (SMC) API from user space for Intel based
 * Macs. Works by talking to the AppleSMC.kext (kernel extension), the driver
 * for the SMC.
 *
 * smc.h
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

#include <IOKit/IOKitLib.h>


//------------------------------------------------------------------------------
// MARK: MACROS
//------------------------------------------------------------------------------


/**
SMC keys for temperature sensors - 4 byte multi-character constants

Not applicable to all Mac's of course. In adition, the definition of the codes
may not be 100% accurate necessarily. Finally, list is incomplete.

Presumed letter translations:

- T = Temperature (if first char)
- C = CPU
- G = GPU
- P = Proximity
- D = Diode
- H = Heatsink

Sources:

- https://www.apple.com/downloads/dashboard/status/istatpro.html
- https://github.com/hholtmann/smcFanControl
- https://github.com/jedda/OSX-Monitoring-Tools
- http://www.parhelia.ch/blog/statics/k3_keys.html
*/
#define AMBIENT_AIR_0          "TA0P"
#define AMBIENT_AIR_1          "TA1P"
#define CPU_0_DIODE            "TC0D"
#define CPU_0_HEATSINK         "TC0H"
#define CPU_0_PROXIMITY        "TC0P"
#define ENCLOSURE_BASE_0       "TB0T"
#define ENCLOSURE_BASE_1       "TB1T"
#define ENCLOSURE_BASE_2       "TB2T"
#define ENCLOSURE_BASE_3       "TB3T"
#define GPU_0_DIODE            "TG0D"
#define GPU_0_HEATSINK         "TG0H"
#define GPU_0_PROXIMITY        "TG0P"
#define HARD_DRIVE_BAY         "TH0P"
#define MEMORY_SLOT_0          "TM0S"
#define MEMORY_SLOTS_PROXIMITY "TM0P"
#define NORTHBRIDGE            "TN0H"
#define NORTHBRIDGE_DIODE      "TN0D"
#define NORTHBRIDGE_PROXIMITY  "TN0P"
#define THUNDERBOLT_0          "TI0P"
#define THUNDERBOLT_1          "TI1P"
#define WIRELESS_MODULE        "TW0P"


/**
SMC keys for fans - 4 byte multi-character constants

Number of fans on Macs vary of course, thus not all keys will be applicable.

Presumed letter translations:

- F  = Fan
- Ac = Acutal
- Mn = Min
- Mx = Max
- Sf = Safe
- Tg = Target

Sources: See TMP SMC keys
*/
#define FAN_0            "F0Ac"
#define FAN_0_MIN_RPM    "F0Mn"
#define FAN_0_MAX_RPM    "F0Mx"
#define FAN_0_SAFE_RPM   "F0Sf"
#define FAN_0_TARGET_RPM "F0Tg"
#define FAN_1            "F1Ac"
#define FAN_1_MIN_RPM    "F1Mn"
#define FAN_1_MAX_RPM    "F1Mx"
#define FAN_1_SAFE_RPM   "F1Sf"
#define FAN_1_TARGET_RPM "F1Tg"
#define FAN_2            "F2Ac"
#define FAN_2_MIN_RPM    "F2Mn"
#define FAN_2_MAX_RPM    "F2Mx"
#define FAN_2_SAFE_RPM   "F2Sf"
#define FAN_2_TARGET_RPM "F2Tg"
#define NUM_FANS         "FNum"
#define FORCE_BITS       "FS! "


/**
Misc SMC keys - 4 byte multi-character constants

Sources: See TMP SMC keys
*/
#define NUM_KEYS "#KEY"
#define BATT_PWR "BATP"


//------------------------------------------------------------------------------
// MARK: ENUMS
//------------------------------------------------------------------------------


typedef enum {
    CELSIUS,
    FAHRENHEIT,
    KELVIN
} tmp_unit_t;


//------------------------------------------------------------------------------
// MARK: PROTOTYPES
//------------------------------------------------------------------------------


kern_return_t open_smc(void);
kern_return_t close_smc(void);
bool is_key_valid(char *key);
double get_tmp(char *key, tmp_unit_t unit);
bool is_battery_powered(void);
int get_num_fans(void);
UInt get_fan_rpm(UInt fan_num);
bool set_fan_min_rpm(unsigned int fan_num, unsigned int rpm, bool auth);
