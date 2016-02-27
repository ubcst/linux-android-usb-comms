/*
 * gps.h
 * Author: Jan Louis Evangelista, Trevor Yu
 * UBCST Electrical Division
 */

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

/* Header guard */
#ifndef GPS_H
#define GPS_H

/* Variable Declarations */
struct gps_data
{
    std::string timeStamp;
    double latitude;
    double longitude;
    std::string northsouth;
    std::string eastwest;
};

/* Function Declarations */
std::vector<std::string> &split( const std::string &s, char delim, std::vector<std::string> &elems );
std::vector<std::string> split( const std::string &s, char delim);

/**
 * gps_init()
 * Initializes the USB port for the GPS device
 * Params: 
 *   usb_path - the path of the USB port the GPS is connected to.
 * Returns: 
 *   USB - the initialized USB port
 */
int gps_init(std::string usb_path);

/**
 * gps_write()
 * Writes configuration logic to the GPS device
 * Params: 
 *   USB - the initialized USB port
 * Returns: 
 *   None
 */
void gps_write(int USB);

/**
 * gps_read()
 * Reads the GPS data from the GPS device
 * Params: 
 *   USB - the initialized USB port
 * Returns: 
 *   nmeaLine - the NMEA string output by the GPS
 */
std::vector<std::string> gps_read(int USB);

/**
 * gps_parse()
 * Parses the NMEA string into invididual GPS data fields
 * Params: 
 *   data - the data structure containing GPS fields
 *   nmeaLine - the NMEA string output by the GPS
 * Returns: 
 *   data - the data structure containing GPS fields
 */
struct gps_data gps_parse(gps_data data, std::vector<std::string> nmeaLine);

/**
 * gps_close()
 * Closes the GPS port
 * Params: 
 *   USB - the initialized USB port
 * Returns: 
 *   None
 */
void gps_close(int USB);

#endif /* End header guard */

