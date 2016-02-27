# linux-android-usb-comms
Linux to Android communication program using USB

# Work in Progress

Description:
The program uses the libusb library to create a USB communication session with an Android device.

# Current Issues

Program fails on initialization, when claiming a USB interface. 
The function "libusb_claim_interface(handle, 0);" returns error code -6, which means LIBUSB_ERROR_BUSY.
An attempt to detach the Linux kernel from the interface is made before the function call, but the program returns that the kernel is not currently using the interface, which means some other program or driver is using the interface.

# Compiling in Ubuntu:

Install the libusb library using the following commands:
apt-get source libusb
apt-get install libusb-dev
apt-get install libusb-1.0-0-dev

Compile using the following command:
gcc comms.cpp -I/usr/include/ -o comms -lusb-1.0 -I/usr/include/ -I/usr/include/libusb-1.0

(Add source files to the command and replace -o comms as necessary...)

Note: The attached makefile compiles entire program including the GPS code

# Finding your phone's Vendor ID and Product ID

In Ubuntu terminal, run lsusb
The program will return:
Bus 00X Device 00Y: ID 1234:5678 Qualcomm Inc.
where 1234 is the phone's VID and 5678 is the phone's PID. 
Change the header file accordingly.