#include "iio_device.h"
#include "platform.h"

#include <stddef.h>
#include <stdlib.h>
#include <iio.h>
#include <stdint.h>
#include <SDL2/SDL.h>


bool init_iio_devices() {
	struct iio_context* iio_ctx = iio_create_default_context();
	struct iio_device* gyro = iio_context_find_device(iio_ctx, "(null)_gyro");
	struct iio_device* accel = iio_context_find_device(iio_ctx, "(null)_accel");

	if (gyro) {
		push_single_sensor_event(SDL_SENSOR_GYRO, gyro);
		// io_device_get_channels_count()
	}
	if (accel) {
		push_single_sensor_event(SDL_SENSOR_ACCEL, accel);
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
void push_single_sensor_event(SDL_SensorType sensor, struct iio_device *dev){
	float data[3];
	struct iio_channel* channel;
	// data is le:S16/16>>0, so should be enough?
	char value_string[10];
	char scale_string[10];
	for (int i=0; i<3; i++){
		channel = iio_device_get_channel(dev, i);
		iio_channel_attr_read(channel, "raw", value_string, 10);
		iio_channel_attr_read(channel, "scale", scale_string, 10);
		data[i] = strtof(value_string, NULL) * strtof(scale_string, NULL);
	}

	// channel = iio_device_get_channel(dev, 3);
	// char scale_string[10];
	// iio_channel_attr_read(channel, "I don't understand timestamp", scale_string, 10);
	push_sensor_event(sensor, data, 0);
}
