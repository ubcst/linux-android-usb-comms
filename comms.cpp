/**
 * comms.cpp
 * Author: Jan Louis Evangelista
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

#include "comms.h"

/**
 * usb_init()
 * Initializes the USBs session
 * Parameters: 
 *   handle - the uninitialized device handle
 *   context - the libusb session
 *   device - the list of devices
 * Returns:
 *   0 - if no error occured
 *   1 - if error occurs during initialization
 */
int usb_init(libusb_device **device, libusb_device_handle *handle)
{
    int deviceCount = 0;
    int returnVal = 0;

    /* Initialize the libusb session */
    returnVal = libusb_init(NULL);
    if(returnVal < 0)
    {
	std::cout << "USB Initialization Error: " << returnVal << std::endl;
	return 1;
    }

    libusb_set_debug(NULL, 3);

    /* Open the device using its Vendor and Product IDs */
    handle = libusb_open_device_with_vid_pid(NULL, PHONE_VID, PHONE_PID);
    if(handle == NULL)
    {
	std::cout << "Device Open Error" << std::endl;
	return 1;
    }


    /* Auto-detach drivers */
    returnVal = libusb_set_auto_detach_kernel_driver(handle, 1);
    if(returnVal != 0)
    {
	std::cout << "Auto detach error: " << libusb_error_name(returnVal) 
		  << std::endl;
    }

    /* Check if device is attached to the kernel, detach if it is */
    returnVal = libusb_kernel_driver_active(handle, 0);
    if(returnVal == 1)
    {
	std::cout << "Kernel driver attached: " << returnVal << std::endl;

	/* Detach the kernel driver */
	returnVal = libusb_detach_kernel_driver(handle, 0);
	if(returnVal < 0)
	{
	    std::cout << "Kernel Detach Error: " << returnVal <<std::endl;
	    return 1;
	}
    }
    else if(returnVal == 0)
    {
	std::cout << "Kernel driver inactive: " << returnVal <<std::endl;

	setupAccessory(handle);

	/* Claim device interface */
	returnVal = libusb_claim_interface(handle, 0);
	if(returnVal < 0)
	{
	    std::cout << "Claim Interface Error: " << returnVal 
		      << " " << libusb_error_name(returnVal) << std::endl;
	    return 1;
	}
    }
    else 
    {
	std::cout << "Kernel driver error: " << returnVal << std::endl;
	return 1;
    }

    return 0;
}

/**
 * send_data()
 * Parameters:
 *   handle - the device handle
 *   message - the message string
 * Returns:
 *   0 - if transfer is successful
 *   1 - if transfer fails
 */
int send_data(libusb_device_handle *handle, unsigned char *message)
{
    int returnVal;
    int actual;

    /* Transfer data to device */
    returnVal = libusb_bulk_transfer(handle, IN_POINT, message,
				     sizeof(message), &actual, 0);

    if((returnVal == 0) && actual == sizeof(message))
    {
	std::cout << "Message sent successfully!" << std::endl;
	return 0;
    }
    else
    {
	std::cout << "Message not sent! Actual: " << actual << ", Message: " 
		  << sizeof(message) << std::endl;
	return 1;
    }
}

/**
 * usb_close()
 * Closes the USB session
 * Parameters:
 *   handle - the USB device handle
 * Returns:
 *   None
 */
int usb_close(libusb_device_handle *handle)
{
    int returnVal = 0;

    if(handle != NULL)
    {
	returnVal = libusb_release_interface(handle, 0);

	if(returnVal != 0)
	{
	    std::cout << "Interface release error!" << std::endl;
	}
	else
	{
	    std::cout << "Interface release successful!" << std::endl;
	}
    }

    libusb_close(handle);
    libusb_exit(NULL);

    std::cout << "Session closed!" << std::endl;

    return 0;
}

/**
 * setupAccessory()
 * Parameters:
 *   handle - the handle of the device
 * Returns:
 *   0 - if successful
 *   1 - if error occurs
 */
int setupAccessory(libusb_device_handle *handle)
{
	unsigned char ioBuffer[2];
	int devVersion;
	int response;
	int tries = 5;

	response = libusb_control_transfer(handle, /* handle */
					   0xC0, /* bmRequestType */
					   51, /* bRequest */
					   0, /* wValue */
					   0, /* wIndex */
					   ioBuffer, /* data */
					   2, /* wLength */
					   0 /* timeout */);

	if(response < 0)
	{
	    std::cout << "Error: " << libusb_error_name(response) << std::endl;
	    return 1;
	}

	devVersion = ioBuffer[1] << 8 | ioBuffer[0];
	std::cout << "Version Code Device: " << devVersion << std::endl;
	
	usleep(1000);//sometimes hangs on the next transfer :(

	response = libusb_control_transfer(handle,0x40,52,0,0,
					   (unsigned char*)MANUFACTURER,
					   sizeof(MANUFACTURER),0);
	if(response < 0)
	{
	    std::cout << "Error: " << libusb_error_name(response) << std::endl;
	    return 1;
	}

	response = libusb_control_transfer(handle,0x40,52,0,1,
					   (unsigned char*)MODEL,
					   sizeof(MODEL)+1,0);
	if(response < 0)
	{
	    std::cout << "Error: " << libusb_error_name(response) << std::endl;
	    return 1;
	}

	response = libusb_control_transfer(handle,0x40,52,0,2,
					   (unsigned char*)DESCRIPTION,
					   sizeof(DESCRIPTION)+1,0);	
	if(response < 0)
	{
	    std::cout << "Error: " << libusb_error_name(response) << std::endl;
	    return 1;
	}

	response = libusb_control_transfer(handle,0x40,52,0,3,
					   (unsigned char*)VERSION,
					   sizeof(VERSION)+1,0);
	if(response < 0)
	{
	    std::cout << "Error: " << libusb_error_name(response) << std::endl;
	    return 1;
	}

	response = libusb_control_transfer(handle,0x40,52,0,4,
					   (unsigned char*)URI,
					   sizeof(URI)+1,0);
	if(response < 0)
	{
	    std::cout << "Error: " << libusb_error_name(response) << std::endl;
	    return 1;
	}

	response = libusb_control_transfer(handle,0x40,52,0,5,
					   (unsigned char*)SERIALNO,
					   sizeof(SERIALNO)+1,0);
	if(response < 0)
	{
	    std::cout << "Error: " << libusb_error_name(response) << std::endl;
	    return 1;
	}

	std::cout << "Accessory Identification Sent" << std::endl;

	response = libusb_control_transfer(handle,0x40,53,0,0,NULL,0,0);
	if(response < 0)
	{
	    std::cout << "Error: " << libusb_error_name(response) << std::endl;
	    return 1;
	}

	return 0;
}
