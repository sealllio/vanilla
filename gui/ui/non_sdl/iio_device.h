#include <stddef.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <iio.h>

bool init_iio_devices();

void push_sensor_event(SDL_SensorType sensor, float data[3], uint64_t timestamp);
void push_single_sensor_event(SDL_SensorType sensor, struct iio_device *dev);