/**
 * main.cpp
 * Author: Jan Louis Evangelista, Trevor Yu
 * UBCST Electrical Division
 */

#include "gps.h"
#include "comms.h"

/* Set the path of the GPS port */
#define GPS_PATH "/dev/ttyACM0"

#define TEST_MODE 1

int main(void)
{
    std::vector<std::string> nmeaLine;
    gps_data data;
    int gpsPort = 0;

    libusb_device **device; 
    libusb_device_handle *phone; /* a handle for the phone connection */
    int returnVal = 0; /* returned values of functions */

    unsigned char message[USB_MSG_SIZE] = "$GPS,123.01,N,456.02,W,10:09:08,$END";

    int counter = 0;

    /* Initialize phone and GPS sessions */
    usb_init(device, phone);

    if(TEST_MODE)
	std::cout << "Sending data..." << std::endl;
    
    send_data(phone, message, USB_MSG_SIZE);

    if(TEST_MODE)
	std::cout << "Close session..." << std::endl;
    
    usb_close(phone);
    
    /*
    gpsPort = gps_init(GPS_PATH);

    /* Write configuration settings to GPS device */
    /*
    gps_write(gpsPort);

    for(counter = 0; counter < 5; counter++)
    {
	nmeaLine = gps_read(gpsPort);

	// std::cout << "The NMEA Line is: " << nmeaLine << std::endl;
	std::cout << "The size of the NMEA Line is: " << nmeaLine.size() << std::endl;

	if(nmeaLine.size() >= 12)
	{
	    data = gps_parse(data, nmeaLine);
	}
    }
    std::cout << "Final Timestamp: " << data.timeStamp << " Latitude: " 
	      << data.latitude << data.northsouth 
	      << " Longitude: " << data.longitude << data.eastwest << std::endl;
    */

    /* Close the phone and GPS sessions */

    /*
    gps_close(gpsPort);
    */
    return 0;
}
