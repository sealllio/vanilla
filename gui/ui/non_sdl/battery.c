#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "ui/non_sdl/battery.h"
#include "platform.h"


#define BATTERY_CAPACITY_PATH "/sys/class/power_supply/battery/"
#define MAX_BUFFER_SIZE 128

vui_power_state_t vui_linux_battery_power_state_handler(vui_context_t *ctx, int *percent){
    *percent = get_linux_device_battery_capacity();
    return get_linux_device_battery_state();
}

vui_power_state_t get_linux_device_battery_state() {
    FILE *fp;
    char buffer[MAX_BUFFER_SIZE];

    char battery_path[MAX_BUFFER_SIZE];
    strcpy(battery_path, BATTERY_CAPACITY_PATH);
    strcat(battery_path, "status");

    fp = fopen(battery_path, "r");
    if (fp == NULL) {
        vpilog("Error opening battery status file");
        return VUI_POWERSTATE_ERROR;
    }

    if (fgets(buffer, MAX_BUFFER_SIZE, fp) != NULL) {
        fclose(fp);
        if (!strcmp("Charged\n", buffer)){ 
            return VUI_POWERSTATE_CHARGED;
        }
        if (!strcmp("Charging\n", buffer)){ 
            return VUI_POWERSTATE_CHARGING;
        }
        if (!strcmp("Discharging\n", buffer)){ 
            return VUI_POWERSTATE_ON_BATTERY; 
        }
    }
    return VUI_POWERSTATE_UNKNOWN;
}

int get_linux_device_battery_capacity() {
    FILE *fp;
    char buffer[MAX_BUFFER_SIZE];
    int capacity = -1;

    char battery_path[128];
    strcpy(battery_path, BATTERY_CAPACITY_PATH);
    strcat(battery_path, "capacity");

    fp = fopen(battery_path, "r");
    if (fp == NULL) {
        vpilog("Error opening battery capacity file");
        return -1;
    }

    if (fgets(buffer, MAX_BUFFER_SIZE, fp) != NULL) {
        capacity = atoi(buffer);
    }

    fclose(fp);
    return capacity;
}
