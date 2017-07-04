/*
* Copyright (C) 2012 Invensense, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#define FUNC_LOG LOGV("%s", __PRETTY_FUNCTION__)

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>

#include <linux/input.h>

#include <utils/Atomic.h>
#include <utils/Log.h>

#include "sensors.h"
#include "MPLSensor.h"

/*****************************************************************************/
/* The SENSORS Module */

#define VERSION     "version: 14"

//#define ENABLE_LIGHT_SENSOR     1

//#include "LightSensor.h"

#ifdef ENABLE_DMP_SCREEN_AUTO_ROTATION
#define LOCAL_SENSORS (MPLSensor::NumSensors + 1)// + ENABLE_LIGHT_SENSOR
#else
#define LOCAL_SENSORS (MPLSensor::NumSensors)// + ENABLE_LIGHT_SENSOR
#endif

//#define SENSORS_LIGHT_HANDLE            (ID_L)

/* Vendor-defined Accel Load Calibration File Method 
* @param[out] Accel bias, length 3.  In HW units scaled by 2^16 in body frame
* @return '0' for a successful load, '1' otherwise
* example: int AccelLoadConfig(long* offset);
* End of Vendor-defined Accel Load Cal Method 
*/

static struct sensor_t sSensorList[LOCAL_SENSORS];
static int sensors = (sizeof(sSensorList) / sizeof(sensor_t));

static int open_sensors(const struct hw_module_t* module, const char* id,
                        struct hw_device_t** device);

static int sensors__get_sensors_list(struct sensors_module_t* module,
                                     struct sensor_t const** list)
{
    *list = sSensorList;
    return sensors;
}

static struct hw_module_methods_t sensors_module_methods = {
        open: open_sensors
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
        common: {
                tag: HARDWARE_MODULE_TAG,
                version_major: 1,
                version_minor: 0,
                id: SENSORS_HARDWARE_MODULE_ID,
                name: "Invensense module",
                author: "Invensense Inc.",
                methods: &sensors_module_methods,
                dso: NULL,
                reserved: {0}
        },
        get_sensors_list: sensors__get_sensors_list,
};

struct sensors_poll_context_t {
    sensors_poll_device_1_t device; // must be first

    sensors_poll_context_t();
    ~sensors_poll_context_t();
    int activate(int handle, int enabled);
    int setDelay(int handle, int64_t ns);
    int pollEvents(sensors_event_t* data, int count);
    int batch(int handle, int flags, int64_t period_ns, int64_t timeout);
    


private:
    enum {
        mpl = 0,
        compass,
        dmpOrient,
        dmpSign,
        //light,
        numSensorDrivers,   // wake pipe goes here
        numFds,
    };

    struct pollfd mPollFds[numSensorDrivers];
    SensorBase *mSensor;
    CompassSensor *mCompassSensor;
    //LightSensor *mLightSensor;
};

/******************************************************************************/

sensors_poll_context_t::sensors_poll_context_t() {
    VFUNC_LOG;

    mCompassSensor = new CompassSensor();
    //mLightSensor = new LightSensor();
    MPLSensor *mplSensor = new MPLSensor(mCompassSensor);

   /* For Vendor-defined Accel Calibration File Load
    * Use the Following Constructor and Pass Your Load Cal File Function
    * 
	* MPLSensor *mplSensor = new MPLSensor(mCompassSensor, AccelLoadConfig);
	*/

    // setup the callback object for handing mpl callbacks
    setCallbackObject(mplSensor);

    // populate the sensor list
    sensors =
            mplSensor->populateSensorList(sSensorList, sizeof(sSensorList));
/*
    memset(&sSensorList[LOCAL_SENSORS-1], 0, sizeof(sSensorList[0]));
	sSensorList[LOCAL_SENSORS-1].name       = "Light sensor";
	sSensorList[LOCAL_SENSORS-1].vendor     = "Invensense";
	sSensorList[LOCAL_SENSORS-1].version    = 1;
	sSensorList[LOCAL_SENSORS-1].handle     = SENSORS_LIGHT_HANDLE;
	sSensorList[LOCAL_SENSORS-1].type       = SENSOR_TYPE_LIGHT;
	sSensorList[LOCAL_SENSORS-1].maxRange   = 10240.0f;
	sSensorList[LOCAL_SENSORS-1].resolution = 1.0f;
	sSensorList[LOCAL_SENSORS-1].power      = 0.5f;
	sSensorList[LOCAL_SENSORS-1].minDelay   = 20000;
//    sSensorList[LOCAL_SENSORS-1] = {"Light sensor", "Invensense", 1, SENSORS_LIGHT_HANDLE, SENSOR_TYPE_LIGHT, 10240.0f, 1.0f, 0.5f, 20000, {}};
    sensors += 1;
*/
    mSensor = mplSensor;
    mPollFds[mpl].fd = mSensor->getFd();
    mPollFds[mpl].events = POLLIN;
    mPollFds[mpl].revents = 0;

#if 1
//    int compass_fd = open("/dev/compass", O_RDONLY);
    mPollFds[compass].fd = mCompassSensor->getFd();
    LOGD("compass poll fd=%d", mPollFds[compass].fd);
    mPollFds[compass].events = POLLIN;
    mPollFds[compass].revents = 0;
#else
    mPollFds[compass].fd = -1;
    mPollFds[compass].events = POLLIN;
    mPollFds[compass].revents = 0;
#endif

    mPollFds[dmpOrient].fd = ((MPLSensor*) mSensor)->getDmpOrientFd();
    mPollFds[dmpOrient].events = POLLPRI;
    mPollFds[dmpOrient].revents = 0;

    mPollFds[dmpSign].fd = ((MPLSensor*) mSensor)->getDmpSignificantMotionFd();
    mPollFds[dmpSign].events = POLLPRI;
    mPollFds[dmpSign].revents = 0;

//    mPollFds[light].fd = mLightSensor->getFd();
//    mPollFds[light].events = POLLIN;
//    mPollFds[light].revents = 0;
}

sensors_poll_context_t::~sensors_poll_context_t() {
    FUNC_LOG;
    delete mSensor;
    delete mCompassSensor;
//    delete mLightSensor;
}

int sensors_poll_context_t::activate(int handle, int enabled) {
    FUNC_LOG;  
/*    if (SENSORS_LIGHT_HANDLE == handle) {
        return mLightSensor->enable(handle, enabled);
    }*/
    return mSensor->enable(handle, enabled);
}

int sensors_poll_context_t::setDelay(int handle, int64_t ns)
{
    FUNC_LOG;
/*    if (SENSORS_LIGHT_HANDLE == handle) {
        return mLightSensor->setDelay(handle, ns);
    }*/
    return mSensor->setDelay(handle, ns);
}

#define NSEC_PER_SEC            1000000000

static inline int64_t timespec_to_ns(const struct timespec *ts)
{
	return ((int64_t) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

static int64_t get_time_ns(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return timespec_to_ns(&ts);
}

#include "sensor_params.h"

static int64_t tm_min=0;
static int64_t tm_max=0;
static int64_t tm_sum=0;
static int64_t tm_last_print=0;
static int64_t tm_count=0;

#define SENSOR_KEEP_ALIVE       1

#if SENSOR_KEEP_ALIVE
static int sensor_activate[32];
static int sensor_delay[32];
static int64_t sensor_prev_time[32];
#endif

/*
    0 - 0000 - no debug
    1 - 0001 - gyro data
    2 - 0010 - accl data
    4 - 0100 - mag data
    8 - 1000 - raw gyro data with uncalib and bias
 */
static int debug_lvl = 0;
#include <cutils/properties.h>

static float accel_bias_x = 0.0f;
static float accel_bias_y = 0.0f;
static float accel_bias_z = 0.0f;

static float gyro_bias_x = 0.0f;
static float gyro_bias_y = 0.0f;
static float gyro_bias_z = 0.0f;

int sensors_poll_context_t::pollEvents(sensors_event_t *data, int count)
{
    VHANDLER_LOG;

    int nbEvents = 0;
    int nb, polltime = -1;
    char propbuf[PROPERTY_VALUE_MAX];
    int i=0;

    // look for new events
    nb = poll(mPollFds, numSensorDrivers, polltime);

    property_get("sensor.debug.level", propbuf, "0");
    debug_lvl = atoi(propbuf);

    property_get("sensor.bias.accel.x", propbuf, "0.0");
    accel_bias_x = atof(propbuf);
    property_get("sensor.bias.accel.y", propbuf, "0.0");
    accel_bias_y = atof(propbuf);
    property_get("sensor.bias.accel.z", propbuf, "0.0");
    accel_bias_z = atof(propbuf);

    property_get("sensor.bias.gyro.x", propbuf, "0.0");
    gyro_bias_x = atof(propbuf);
    property_get("sensor.bias.gyro.y", propbuf, "0.0");
    gyro_bias_y = atof(propbuf);
    property_get("sensor.bias.gyro.z", propbuf, "0.0");
    gyro_bias_z = atof(propbuf);

    if (nb > 0) {
        for (int i = 0; count && i < numSensorDrivers; i++) {
            if (mPollFds[i].revents & (POLLIN | POLLPRI)) {
                nb = 0;
                if (i == mpl) {
                    ((MPLSensor*) mSensor)->buildMpuEvent();
                    mPollFds[i].revents = 0;
                } else if (i == compass) {
                    ((MPLSensor*) mSensor)->buildCompassEvent();
                    mPollFds[i].revents = 0;
                } else if (i == dmpOrient) {
                    nb = ((MPLSensor*) mSensor)->readDmpOrientEvents(data, count);
                    mPollFds[dmpOrient].revents= 0;
                    if (isDmpScreenAutoRotationEnabled() && nb > 0) {
                        count -= nb;
                        nbEvents += nb;
                        data += nb;
                    }
                } else if (i == dmpSign) {
                    LOGI_IF(0, "HAL: dmpSign interrupt");
                    nb = ((MPLSensor*) mSensor)->readDmpSignificantMotionEvents(data, count);
                    mPollFds[i].revents = 0;
                    count -= nb;
                    nbEvents += nb;
                    data += nb; 
                }/* else if (i == light) {
                    nb = mLightSensor->readEvents(data, count);
                    mPollFds[i].revents = 0;
                    count -= nb;
                    nbEvents += nb;
                    data += nb;
                }*/
            }
        }
        nb = ((MPLSensor*) mSensor)->readEvents(data, count);
        LOGI_IF(0, "sensors_mpl:readEvents() - nb=%d, count=%d, nbEvents=%d, data->timestamp=%lld, data->data[0]=%f,",
                          nb, count, nbEvents, data->timestamp, data->data[0]);

        if (accel_bias_x!=0.0f || accel_bias_y!=0.0f || accel_bias_z!=0.0f) {
            for (i=0; i<nb; i++) {
                if (data[i].sensor==SENSORS_ACCELERATION_HANDLE) {
                    data[i].acceleration.v[0] -= accel_bias_x;
                    data[i].acceleration.v[1] -= accel_bias_y;
                    data[i].acceleration.v[2] -= accel_bias_z;
                }
            }
        }

        if (gyro_bias_x!=0.0f || gyro_bias_y!=0.0f || gyro_bias_z!=0.0f) {
            for (i=0; i<nb; i++) {
                if (data[i].sensor==SENSORS_RAW_GYROSCOPE_HANDLE) {
                    data[i].uncalibrated_gyro.uncalib[0] -= gyro_bias_x;
                    data[i].uncalibrated_gyro.uncalib[1] -= gyro_bias_y;
                    data[i].uncalibrated_gyro.uncalib[2] -= gyro_bias_z;
                } else if (data[i].sensor==SENSORS_GYROSCOPE_HANDLE) {
                    data[i].gyro.v[0] -= gyro_bias_x;
                    data[i].gyro.v[1] -= gyro_bias_y;
                    data[i].gyro.v[2] -= gyro_bias_z;
                }
            }
        }

#if SENSOR_KEEP_ALIVE
        for (i=0; i<nb; i++) {
            if (data[i].sensor>=0 && sensor_activate[data[i].sensor]>0) {
//              LOGD("Skip sensor data, %d, %d", data[i].sensor, sensor_activate[data[i].sensor]);
                --sensor_activate[data[i].sensor];
                memset(data+i, 0, sizeof(sensors_event_t));
                data[i].sensor = -1;
            }
        }
#endif

        if (debug_lvl > 0) {
            for (i=0; i<nb; i++) {
                if ((debug_lvl&1) && data[i].sensor==SENSORS_RAW_GYROSCOPE_HANDLE) {
                    float gyro_data[3] = {0,0,0};
                    gyro_data[0] = data[i].uncalibrated_gyro.uncalib[0] - data[i].uncalibrated_gyro.bias[0];
                    gyro_data[1] = data[i].uncalibrated_gyro.uncalib[1] - data[i].uncalibrated_gyro.bias[1];
                    gyro_data[2] = data[i].uncalibrated_gyro.uncalib[2] - data[i].uncalibrated_gyro.bias[2];
                    if (debug_lvl&8) {
                        LOGD("RAW GYRO: %+f %+f %+f, %+f %+f %+f, %+f %+f %+f - %lld",
                            gyro_data[0], gyro_data[1], gyro_data[2],
                            data[i].uncalibrated_gyro.uncalib[0], data[i].uncalibrated_gyro.uncalib[1], data[i].uncalibrated_gyro.uncalib[2],
                            data[i].uncalibrated_gyro.bias[0], data[i].uncalibrated_gyro.bias[1], data[i].uncalibrated_gyro.bias[2], data[i].timestamp);
                    }
                    else {
                        LOGD("RAW GYRO: %+f %+f %+f - %lld", gyro_data[0], gyro_data[1], gyro_data[2], data[i].timestamp);
                    }
                }
                if ((debug_lvl&1) && data[i].sensor==SENSORS_GYROSCOPE_HANDLE) {
                    LOGD("GYRO: %+f %+f %+f - %lld", data[i].gyro.v[0], data[i].gyro.v[1], data[i].gyro.v[2], data[i].timestamp);
                }
                if ((debug_lvl&2) && data[i].sensor==SENSORS_ACCELERATION_HANDLE) {
                    LOGD("ACCL: %+f %+f %+f - %lld", data[i].acceleration.v[0], data[i].acceleration.v[1], data[i].acceleration.v[2], data[i].timestamp);
                }
                if ((debug_lvl&4) && (data[i].sensor==SENSORS_MAGNETIC_FIELD_HANDLE)) {
                    LOGD("MAG: %+f %+f %+f - %lld", data[i].magnetic.v[0], data[i].magnetic.v[1], data[i].magnetic.v[2], data[i].timestamp);
                }
            }
        }

        if (nb > 0) {
            int64_t tm_cur = get_time_ns();
            int64_t tm_delta = tm_cur - data->timestamp;
            if (tm_min==0 && tm_max==0)
                tm_min = tm_max = tm_delta;
            else if (tm_delta < tm_min)
                tm_min = tm_delta;
            else if (tm_delta > tm_max)
                tm_max = tm_delta;
            tm_sum += tm_delta;
            tm_count++;
            
            if ((tm_cur-tm_last_print) > 1000000000) {
                LOGD_IF(0, "poll end: [%lld] %lld,%lld,%lld\n", data->timestamp, tm_min, (tm_sum/tm_count), tm_max);
                tm_last_print = tm_cur;
                tm_min = tm_max = tm_count = tm_sum = 0;
            }

            count -= nb;
            nbEvents += nb;
            data += nb;
        }
    }


    return nbEvents;
}

int sensors_poll_context_t::batch(int handle, int flags, int64_t period_ns, int64_t timeout)
{
    FUNC_LOG;
    return mSensor->batch(handle, flags, period_ns, timeout);
}

/******************************************************************************/

static int poll__close(struct hw_device_t *dev)
{
    FUNC_LOG;
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    if (ctx) {
        delete ctx;
    }
    return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev,
                          int handle, int enabled)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;

#if SENSOR_KEEP_ALIVE
    sensor_activate[handle] = enabled?10:0;
#endif

    return ctx->activate(handle, enabled);
}

static int poll__setDelay(struct sensors_poll_device_t *dev,
                          int handle, int64_t ns)
{
#if SENSOR_KEEP_ALIVE
    if (sensor_delay[handle] == ns) {
//        LOGD("keep sensor(%d) delay %d ns", handle, ns);
        return 0;
    }
    sensor_delay[handle] = ns;
#endif

    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    int s= ctx->setDelay(handle, ns);
    return s;
}

static bool ert = false;

static int poll__poll(struct sensors_poll_device_t *dev,
                      sensors_event_t* data, int count)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;

    if (!ert) {
        struct sched_param param = {
                .sched_priority = 90,
        };
        sched_setscheduler(0, SCHED_FIFO, &param);
        ert = true;
//        ALOGD("set %d to SCHED_FIFO,90", gettid());
    }

    return ctx->pollEvents(data, count);
}

static int poll__batch(struct sensors_poll_device_1 *dev,
                      int handle, int flags, int64_t period_ns, int64_t timeout)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->batch(handle, flags, period_ns, timeout);
}

/******************************************************************************/

/** Open a new instance of a sensor device using name */
static int open_sensors(const struct hw_module_t* module, const char* id,
                        struct hw_device_t** device)
{
    FUNC_LOG;

    LOGD("Sensor HAL %s", VERSION);

    int status = -EINVAL;
    sensors_poll_context_t *dev = new sensors_poll_context_t();

    memset(&dev->device, 0, sizeof(sensors_poll_device_1));

    dev->device.common.tag = HARDWARE_DEVICE_TAG;
    dev->device.common.version  = SENSORS_DEVICE_API_VERSION_1_0;
    dev->device.common.module   = const_cast<hw_module_t*>(module);
    dev->device.common.close    = poll__close;
    dev->device.activate        = poll__activate;
    dev->device.setDelay        = poll__setDelay;
    dev->device.poll            = poll__poll;

    /* Batch processing */
    dev->device.batch           = poll__batch; 

    *device = &dev->device.common;
    status = 0;
    ert = false;

#if SENSOR_KEEP_ALIVE
    memset(sensor_activate, 0, 32*sizeof(int));
    memset(sensor_delay, 0, 32*sizeof(int));
    memset(sensor_prev_time, 0, 32*sizeof(int64_t));
#endif

    return status;
}

#include <sys/stat.h>

static void show_usage(const char* app)
{
    printf("%s -p -t [type mask]\n", app);
}

#define SENSOR_FIFO_NAME "/dev/sensor_fifo"

#define SENSOR_TYPE_MASK(x)     (1<<x)

// sensor_test -p -t 64
int main(int argc, char** argv)
{
//    int handle = SENSORS_GYROSCOPE_HANDLE;
//    int64_t delay_ns = 2000000;
    sensors_event_t data[16];
    int count=16;
    int i;
    sensors_poll_context_t *ctx = new sensors_poll_context_t();
    int res;
    int pipe_fd;
    int pcba_test = 0;
    uint64_t sensor_type_mask = 0;

    do {
        int c;

        c = getopt(argc, argv, "hpt:");

        if (c == -1) {
            break;
        }

        switch (c) {
        case 'p':
            pcba_test = 1;
            break;
        case 't':
            sensor_type_mask = strtoll(optarg, NULL, 0);
            break;
        default:
        case 'h':
            show_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    } while (1);

    for (i=ID_GY; i<ID_MAX; i++)
        ctx->activate(i, 0);

    if (pcba_test) {
        if (access(SENSOR_FIFO_NAME, F_OK) == -1)  
        {
            res = mkfifo(SENSOR_FIFO_NAME, 0777);
            if (res != 0)
            {  
                fprintf(stderr, "Could not create fifo %s\n", SENSOR_FIFO_NAME);
                exit(EXIT_FAILURE);
            }
        }
        pipe_fd = open(SENSOR_FIFO_NAME, O_WRONLY);
        if (pipe_fd<=0) {
            fprintf(stderr, "Could not open fifo %s for write\n", SENSOR_FIFO_NAME);
            exit(EXIT_FAILURE);
        }
    }

    printf("sensor_type_mask=%lld\n", sensor_type_mask);
    // enable specification sensors
    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_ACCELEROMETER)) {
        printf("enable accel\n");
        ctx->activate(SENSORS_ACCELERATION_HANDLE, 1);
        ctx->setDelay(SENSORS_ACCELERATION_HANDLE, 1000000);
    }
    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_GYROSCOPE)) {
        printf("enable gyro\n");
        ctx->activate(SENSORS_GYROSCOPE_HANDLE, 1);
        ctx->setDelay(SENSORS_GYROSCOPE_HANDLE, 1000000);
    }
    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_MAGNETIC_FIELD)) {
        printf("enable compass\n");
        ctx->activate(SENSORS_MAGNETIC_FIELD_HANDLE, 1);
        ctx->setDelay(SENSORS_MAGNETIC_FIELD_HANDLE, 10000000);
    }
/*    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_LIGHT)) {
        printf("enable light\n");
        ctx->activate(SENSORS_LIGHT_HANDLE, 1);
        ctx->setDelay(SENSORS_LIGHT_HANDLE, 100000000);
    }*/
    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_PRESSURE)) {
        // unsupport;
    }

    sleep(1);

// get sensor data
    while(1) {
        int nb = ctx->pollEvents(data, count);
        for (i=0; i<nb; i++) {
            if (pcba_test) {
                res = write(pipe_fd, &data[i], sizeof(data[i]));
                if (res<=0) {
                    fprintf(stderr, "Write error on sensor pipe\n"); 
                }
                continue;
            }
            if (data[i].sensor == SENSORS_GAME_ROTATION_VECTOR_HANDLE) {
                printf("GRV: %+f %+f %+f %+f %+f - %lld\n",
                        data[i].data[0], data[i].data[1], data[i].data[2], data[i].data[3], data[i].data[4], data[i].timestamp);
            }
            else if (data[i].sensor == SENSORS_ORIENTATION_HANDLE) {
                printf( "ORI: %f %f %f - %lld\n",
                    data[i].orientation.v[0], data[i].orientation.v[1], data[i].orientation.v[2],
                    data[i].timestamp);
            }
            else if (data[i].sensor == SENSORS_ACCELERATION_HANDLE) {
                printf( "ACL: %+f %+f %+f -- %lld\n",
                    data[i].acceleration.v[0], data[i].acceleration.v[1], data[i].acceleration.v[2],
                    data[i].timestamp);
            }
            else if (data[i].sensor == SENSORS_MAGNETIC_FIELD_HANDLE) {
                printf( "MAG: %+f %+f %+f -- %lld\n",
                    data[i].magnetic.v[0], data[i].magnetic.v[1], data[i].magnetic.v[2],
                    data[i].timestamp);
            }
            else if (data[i].sensor == SENSORS_GYROSCOPE_HANDLE) {
                printf( "GYRO: %+f %+f %+f -- %lld\n",
                    data[i].gyro.v[0], data[i].gyro.v[1], data[i].gyro.v[2],
                    data[i].timestamp);
            }
            else if (data[i].sensor == SENSORS_RAW_GYROSCOPE_HANDLE) {
                printf( "RAW GYRO: %+f %+f %+f : %+f %+f %+f -- %lld\n",
                    data[i].uncalibrated_gyro.uncalib[0],
                    data[i].uncalibrated_gyro.uncalib[1],
                    data[i].uncalibrated_gyro.uncalib[2],
                    data[i].uncalibrated_gyro.bias[0],
                    data[i].uncalibrated_gyro.bias[1],
                    data[i].uncalibrated_gyro.bias[2],
                    data[i].timestamp);
            }
        }
    }

    if (pcba_test) {
        close(pipe_fd);
    }

    // disable sensor & exit
    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_ACCELEROMETER)) {
        ctx->activate(SENSORS_ACCELERATION_HANDLE, 0);
    }
    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_GYROSCOPE)) {
        ctx->activate(SENSORS_GYROSCOPE_HANDLE, 0);
    }
    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_MAGNETIC_FIELD)) {
        ctx->activate(SENSORS_MAGNETIC_FIELD_HANDLE, 0);
    }
/*    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_LIGHT)) {
        ctx->activate(SENSORS_LIGHT_HANDLE, 0);
    }*/
    if (sensor_type_mask&SENSOR_TYPE_MASK(SENSOR_TYPE_PRESSURE)) {
        // unsupport;
    }

    delete ctx;

    printf("Exit!");

	return 0;
}

