#include <stddef.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <iio.h>

struct IMU_modifier {
    int axis[3];  // which in-game axis a controller's axis is mapped to. e.g. axis[0] = 2 means controller's x = wiiu z
    float scale_g[3]; // multiplies gyro value. For inverting and scaling.
    float scale_a[3]; 
};

struct IIO_Sensors {
    struct iio_channel* gyro[3];
    struct iio_channel* accel[3];
    struct IMU_modifier modifier;
};

struct IIO_Sensors init_iio_devices();

void start_iio(struct IIO_Sensors* sensors);
void push_sensor_event(SDL_SensorType sensor, float data[3], uint64_t timestamp);
void push_single_sensor_event(SDL_SensorType sensor, struct iio_channel** channels, int axis[], float modifier[]);
void* iio_thread_loop(void * v_sensors);