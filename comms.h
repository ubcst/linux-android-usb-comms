/**
 * comms.h
 * Author: Jan Louis Evangelista, Trevor Yu
 * UBCST Electrical Division
 * Allows communication between RPi and an Android-powered phone
 * The interface runs in USB Accessory Mode
 * ie. The RPi provides power to the Android phone
 *
 * References:
 *   http://libusb.org/static/api-1.0/index.html
 *   http://www.dreamincode.net/forums/topic/148707-introduction-to-using-libusb-10/
 *   http://android.serverbox.ch/?p=262
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
#include <libusb.h>

/* Header Guard */
#ifndef COMMS_H
#define COMMS_H

/* Product ID of Android phone */
#define PHONE_PID 0x6764 /* Oneplus One Product ID */

/* Vendor ID of Android phone */
#define PHONE_VID 0x05c6 /* Oneplus One Vendor ID */

/* In point of the Oneplus One */
#define IN_POINT 0x81

/* Out point of the Oneplus One */
#define OUT_POINT 0x01

/* Function Prototypes */

/**
 * usb_init()
 * Initializes the USB session
 * Parameters: 
 *   handle - the uninitialized device handle
 * Returns:
 *   0 - if no error occured
 *   1 - if error occurs during initialization
 */
int usb_init(libusb_device **device, libusb_device_handle *handle);

/**
 * setup_accessory()
 * Parameters:
 *   manufacturer - the manufacturer of the device
 *   modelname - the name of the device
 *   description - summary of the device
 *   version - version of the device
 *   uri - identifier of the device
 *   serialno - serial number of the device
 * Returns:
 *   0 - if setup is successful
 */
int setup_accessory( const char *manufacturer,
		     const char *modelname,
		     const char *description,
		     const char *version,
		     const char *uri,
		     const char *serialno,
		     libusb_device_handle *handle);

/**
 * send_data()
 * Parameters:
 *   handle - the device handle
 *   message - the message string
 * Returns:
 *   0 - if transfer is successful
 *   1 - if transfer fails
 */
int send_data( libusb_device_handle *handle, unsigned char *message);

/**
 * usb_close()
 * Closes the USB session
 * Parameters:
 *   handle - the USB device handle
 * Returns:
 *   None
 */
int usb_close(libusb_device_handle *handle, libusb_device **device);

#endif /* End Header Guard */
