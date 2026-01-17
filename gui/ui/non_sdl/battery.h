#include "ui/ui_priv.h"

vui_power_state_t vui_linux_battery_power_state_handler(vui_context_t *ctx, int *percent);
vui_power_state_t get_linux_device_battery_state();
int get_linux_device_battery_capacity();
