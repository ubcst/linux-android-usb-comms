/**
 * sensor.h
 * Author: Jan Louis Evangelista
 * UBCST Electrical Division
 */

/* Header Guard */
#ifndef SENS_H
#define SENS_H

/* Sensor data structure */
struct sensor_data
{
    /* Temperature readings from ADC */
    double temp1;
    double temp2;
    double temp3;
    double temp4;
    double temp5;
    double temp6;

    /* X, Y, Z readings from accelerometer */
    double x;
    double y;
    double z;

    /* Speed reading from speed sensor */
    double speed;
};

/**
 * FILLER FUNCTION - DOES NOTHING
 */
void read_sensor(void);

#endif /* End header guard */

