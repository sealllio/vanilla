#include "iio_device.h"
#include "platform.h"

#include <stddef.h>
#include <stdlib.h>
#include <iio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <omp.h>

struct IIO_Sensors init_iio_devices() {
	struct iio_context* iio_ctx = iio_create_default_context();
	struct iio_device* gyro_d = iio_context_find_device(iio_ctx, "(null)_gyro");
	struct iio_device* accel_d = iio_context_find_device(iio_ctx, "(null)_accel");

	struct IIO_Sensors sensors = {
		.modifier = {
    		.axis = {2, 0, 1}, 
    		.scale_g = {-0.85, 0.85, 0.85}, // 0.9 because 1 has issues with wii u calibration 
    		.scale_a = {-0.85, 0.85, 0.85}
		}
	};

	// assume scale won't change?
	char scale_string[10];
	for (int i = 0; i <3; i++){
		sensors.gyro[i] = iio_device_get_channel(gyro_d, i);
		iio_channel_attr_read(sensors.gyro[i], "scale", scale_string, 10);
		sensors.modifier.scale_g[i] = sensors.modifier.scale_g[i] * strtof(scale_string, NULL);

		sensors.accel[i] = iio_device_get_channel(accel_d, i);
		iio_channel_attr_read(sensors.accel[i], "scale", scale_string, 10);
		sensors.modifier.scale_a[i] = sensors.modifier.scale_a[i] * strtof(scale_string, NULL);
	}
	
	return sensors;
}

void* iio_thread_loop(void *v_sensors){
	struct IIO_Sensors *sensors = (struct IIO_Sensors *)v_sensors;
	while (true){
		push_single_sensor_event(SDL_SENSOR_GYRO, sensors->gyro, sensors->modifier.axis, sensors->modifier.scale_g);
		push_single_sensor_event(SDL_SENSOR_ACCEL, sensors->accel, sensors->modifier.axis, sensors->modifier.scale_a);
		//sleep(1);
	}	
}

void push_sensor_event(SDL_SensorType sensor, float data[3], uint64_t timestamp){
	SDL_Event event;
	SDL_zero(event);
	event.type = SDL_CONTROLLERSENSORUPDATE;
	event.csensor.type = SDL_CONTROLLERSENSORUPDATE;
	event.csensor.timestamp = SDL_GetTicks();
	event.csensor.which = 0;
	event.csensor.sensor = sensor;
	memcpy(event.csensor.data, data, 3*sizeof(data[0]));
	event.csensor.timestamp_us = (uint64_t)NULL;

	SDL_PushEvent(&event);
}

// switch gyro and accel has 4 channels: 3 data, then timestamp
// data attributes: raw, scale, scale, scale_available
// current_timestamp_clock, mount_matrix, sampling_frequency, sampling_frequency_available
void push_single_sensor_event(SDL_SensorType sensor, struct iio_channel** channels, int axis[], float modifier[]){
	float data[3];
	struct iio_channel* channel;
	// data is le:S16/16>>0, so should be enough?
	
	#pragma omp parallel for
	for (int i=0; i<3; i++){
		channel = channels[i];
		char value_string[10];
		iio_channel_attr_read(channel, "raw", value_string, 10);
		data[axis[i]] = modifier[i] * strtof(value_string, NULL);
	}

	// channel = iio_device_get_channel(dev, 3);
	// char scale_string[10];
	// iio_channel_attr_read(channel, "I don't understand timestamp", scale_string, 10);
	push_sensor_event(sensor, data, 0);
}

