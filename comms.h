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
#define PHONE_PID 0x6765 /* Oneplus One Product ID */

/* Vendor ID of Android phone */
#define PHONE_VID 0x05c6 /* Oneplus One Vendor ID */

/* Accessory Mode-specific VID and PIDs */
#define ACC_VID 0x18d1 /* Accessory Mode VID */

#define ACC_PID_ADB 0x2d01 /* Accessory Mode PID with ADB active */
#define ACC_PID 0x2d00 /* Accessory Mode PID with no PID */

/* In point of the Oneplus One */
#define IN_POINT 0x81

/* Out point of the Oneplus One */
#define OUT_POINT 0x02

/* Accessory configuration data */
#define MANUFACTURER "Lenovo"
#define MODEL  "Y410P"
#define VERSION "1.0"
#define DESCRIPTION "Laptop"
#define URI "URI"
#define SERIALNO "123456"

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
int usb_close(libusb_device_handle *handle);

/**
 * setupAccessory()
 * Parameters:
 *   handle - the handle of the device
 * Returns:
 *   0 - if successful
 *   1 - if an error occurs during setup
 */
int setupAccessory(libusb_device_handle *handle);

#endif /* End Header Guard */
