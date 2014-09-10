#include <stdio.h>
#include <string.h>
#include <IOKit/IOKitLib.h>
#include "../src/smc.h"

int main()
{
    if (open_smc() != kIOReturnSuccess) {
        return -1;
    }

    printf("%0.1f°C\n", get_tmp(SMC_KEY_CPU_TEMP, CELSIUS));
    printf("%d RPM\n", get_fan_rpm(0));
    close_smc();

    return 0;
}
