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

    /* Get device list */
    returnVal = libusb_get_device_list(NULL, &device);
    if(returnVal < 0)
    {
	std::cout << "Get device error: " << returnVal << std::endl;
	return 1;
    }
    else
    {
	std::cout << returnVal << " devices in list." << std::endl;
    }

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

    /* Unmount the devices */
    const char *dirname = "/run/user/1000/gvfs/";
    unmount_devices( dirname );

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

	    usb_close(handle, device);

	    return 1;
	}
    }
    else if(returnVal == 0)
    {
	std::cout << "Kernel driver inactive: " << returnVal <<std::endl;

	/* Claim device interface */
	returnVal = libusb_claim_interface(handle, 0);

	if(returnVal < 0)
	{
	    std::cout << "Claim Interface Error: " << returnVal 
		      << " " << libusb_error_name(returnVal) << std::endl;

	    usb_close(handle, device);

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
		     libusb_device_handle *handle)
{
    /* Do Nothing */
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
int usb_close(libusb_device_handle *handle, libusb_device **device)
{
    int returnVal=0;

    libusb_free_device_list(device, 1);

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
 * unmount_devices()
 * Check if device(s) are mounted by gvfs. If it is, unmount the device(s).
 * Source: http://stackoverflow.com/questions/6383584/check-if-a-directory-is-empty-using-c-on-linux
 * Parameters:
 *   dirname - the directory where the device(s) are unmounted.
 */
void unmount_devices( const char *dirname ) {
    DIR *dir = opendir(dirname);
    int n = 0;
    struct dirent *d;

    if( dir != NULL ) {
        while( ( d = readdir( dir ) ) != NULL ) {
            // n > 2 to ignore the "." and ".." options when reading a
            // directory
            if( ++n > 2 ) {
               std::string path = dirname + std::string(d->d_name);
               std::string cmd = "gvfs-mount -u ";
               std::string fullCmd = cmd + path;
               std::cout << fullCmd << std::endl;
               system( fullCmd.c_str() );
               break;
            }
        }
        closedir(dir);
    } else {
       std::cout << "cannot open dir " << errno << std::endl;
    }
}
