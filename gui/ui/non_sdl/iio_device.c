#include "iio_device.h"
#include "platform.h"

#include <stddef.h>
#include <stdlib.h>
#include <iio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <unistd.h>


struct IIO_Sensors init_iio_devices() {
	struct iio_context* iio_ctx = iio_create_default_context();
	struct iio_device* gyro = iio_context_find_device(iio_ctx, "(null)_gyro");
	struct iio_device* accel = iio_context_find_device(iio_ctx, "(null)_accel");

	struct IIO_Sensors sensors;
	sensors.gyro = gyro;
	sensors.accel = accel;
	return sensors;
}

void* iio_thread_loop(void *v_sensors){
	struct IIO_Sensors *sensors = (struct IIO_Sensors *)v_sensors;
	int i = 10000;
	while (i){
		push_single_sensor_event(SDL_SENSOR_GYRO, sensors->gyro, 0.9);
		push_single_sensor_event(SDL_SENSOR_ACCEL, sensors->accel, 0.9);
		//sleep(1);
		i--;
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
void push_single_sensor_event(SDL_SensorType sensor, struct iio_device *dev, float modifier){
	float data[3];
	struct iio_channel* channel;
	// data is le:S16/16>>0, so should be enough?
	char value_string[10];
	char scale_string[10];
	for (int i=0; i<3; i++){
		channel = iio_device_get_channel(dev, i);
		iio_channel_attr_read(channel, "raw", value_string, 10);
		iio_channel_attr_read(channel, "scale", scale_string, 10);
		if (i == 0) data[2] = -1* modifier * strtof(value_string, NULL) * strtof(scale_string, NULL);
		if (i == 1) data[0] = modifier * strtof(value_string, NULL) * strtof(scale_string, NULL);
		if (i == 2) data[1] = modifier * strtof(value_string, NULL) * strtof(scale_string, NULL);
		//data[i] = modifier * strtof(value_string, NULL) * strtof(scale_string, NULL);
	}

	/**
	#define JC_IMU_MAX_ACCEL_MAG		32767
#define JC_IMU_ACCEL_RES_PER_G		4096
#define JC_IMU_ACCEL_FUZZ		10
#define JC_IMU_ACCEL_FLAT		0

#define JC_IMU_PREC_RANGE_SCALE	1000
#define JC_IMU_MAX_GYRO_MAG		32767000 
#define JC_IMU_GYRO_RES_PER_DPS		14247 
#define JC_IMU_GYRO_FUZZ		10
#define JC_IMU_GYRO_FLAT		0

JC_IMU_PREC_RANGE_SCALE *
				      (imu_data[i].gyro_x)),
				     ctlr->gyro_cal.scale[0],
				     ctlr->imu_cal_gyro_divisor[0]);

					 (s32)imu_data[i].accel_x *
			    ctlr->accel_cal.scale[0]) /
			    ctlr->imu_cal_accel_divisor[0]

*/
	// channel = iio_device_get_channel(dev, 3);
	// char scale_string[10];
	// iio_channel_attr_read(channel, "I don't understand timestamp", scale_string, 10);
	push_sensor_event(sensor, data, 0);
}
