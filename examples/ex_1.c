/*
 * Simple example client.
 *
 * ex_1.c
 * c-smc
 *
 * Copyright (C) 2014  beltex <https://github.com/beltex>
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
#include "../src/smc.h"

int main()
{
    if (open_smc() != kIOReturnSuccess) {
        return -1;
    }

    printf("CPU 0 Diode: %0.1f°C\n", get_tmp(CPU_0_DIODE, CELSIUS));
    printf("Fan 0: %d RPM\n", get_fan_rpm(0));
    
    close_smc();

    return 0;
}
