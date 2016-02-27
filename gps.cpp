/**
 * Author: Trevor Yu
 * Email: trevor.tyu@gmail.com
 * Date: October 24, 2015
 * Summary: Code to configure and read data from a GPS sensor.
 *
 * Code primarily taken from:
 * http://stackoverflow.com/questions/18108932/linux-c-serial-port-reading-writing
 */

#include "gps.h"

/**
 * Code to split a string. Taken from:
 * http://stackoverflow.com/questions/236129/split-a-string-in-c
 */
std::vector<std::string> &split( const std::string &s, char delim, std::vector<std::string> &elems ) {
   std::stringstream ss( s );
   std::string item;
   while( getline( ss, item, delim ) ) {
      elems.push_back( item );
   }
   return elems;
}

/**
 * Splits a string s based on the delim parameter.
 * Returns a vector of the split elements.
 */
std::vector<std::string> split( const std::string &s, char delim ) {
   std::vector<std::string> elems;
   split( s, delim, elems );
   return elems;
}

/**
 * gps_init()
 * Parameters: usb_path - the path of the GPS USB port
 * Returns: USB - the USB port to read GPS data from
 */
int gps_init(std::string usb_path)
{
   /* Open file descriptor */
int USB = open( usb_path.c_str() , O_RDWR | O_NOCTTY );

   /* Error handling */
   if ( USB < 0 ) {
       std::cout << "Error " << errno << " opening " << usb_path << ": " \
		 << strerror( errno ) << std::endl;
   }

   /* Configure port */
   struct termios tty;
   memset( &tty, 0, sizeof tty );

   /* Error handling */
   if( tcgetattr( USB, &tty ) != 0 ) {
       std::cout << "Error " << errno << " from tcgetattr: " <<	\
	   strerror( errno ) << std::endl;
   }

   /* Set baud rate speed */
   cfsetospeed( &tty, (speed_t)B115200 );
   cfsetispeed( &tty, (speed_t)B115200 );

   /* Set other port stuff */
   tty.c_cflag &= ~PARENB;
   tty.c_cflag &= ~CSTOPB;
   tty.c_cflag &= ~CSIZE;
   tty.c_cflag |= CS8;

   tty.c_cflag &= ~CRTSCTS;
   tty.c_cc[VMIN] = 1;
   tty.c_cc[VTIME] = 5;
   tty.c_cflag |= CREAD | CLOCAL;

   /* Make raw */
   cfmakeraw( &tty );

   /* Flush port, then apply attributes */
   tcflush( USB, TCIFLUSH );
   if( tcsetattr( USB, TCSANOW, &tty ) != 0 ) {
       std::cout << "Error " << errno << " from tcsetattr" << std::endl;
   }

   return USB;
}

void gps_write(int USB)
{
   /**
    * Write configuration logic (used to configure the GPS update rate).
    * The GPS uses the PMTK protocol to control and change
    * various attributes and settings.
    * Documentation for this protocol can be found:
    * https://www.adafruit.com/datasheets/PMTK_A08.pdf
    * http://www.adafruit.com/datasheets/PMTK_A11.pdf
    *
    * Part of the PMTK message is the checksum field. Use this website
    * to compute the checksum: http://www.hhhh.org/wiml/proj/nmeaxor.html
    */
   const char *str;

   // Not sure if we need to do a full cold start or not. Commenting
   // out for now.
   /*// Perform full cold start
   str = "$PMTK104*37\r\n";
   cout << "Write Val: " << write( USB, str, strlen( str ) ) << endl;*/

   // Only receive GPSRMC messages
   str = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
   std::cout << "PMTK String: " << str;
   if ( strlen( str ) != write( USB, str, strlen( str ) ) ) {
       std::cout << "Read only GPSRMC messages failed." << std::endl;
   }

   // Turn off the EASY function because it only works for 1Hz.
   // Not 100% sure if this is necessary. Leaving in for now.
   str = "$PMTK869,1,0*34\r\n";
   std::cout << "PMTK String: " << str;
   if ( strlen( str ) != write( USB, str, strlen( str ) ) ) {
       std::cout << "Turn off EASY function failed." << std::endl;
   }

   // Change update rate to 5Hz.
   str = "$PMTK220,200*2C\r\n";
   std::cout << "PMTK String: " << str;
   if ( strlen( str ) != write( USB, str, strlen( str ) ) ) {
       std::cout << "Change update rate to 5Hz failed." << std::endl;
   }
}

std::vector<std::string> gps_read(int USB)
{
   /* Read the GPS NMEA messages */
   int n = 0,
       spot = 0;
   char buf = '\0';

   char response[1024];
   memset( response, '\0', sizeof( response ) );

      spot = 0;
      /* Read a NMEA line */
      do {
         n = read( USB, &buf, 1 );
         if ( buf == '\n' ) {
            continue;
         }
         sprintf( &response[spot], "%c", buf );
         spot += n;
      } while( buf != '\r' && n > 0 );

      // Split the string based on the comma delimiter
      std::string resp( response );
      std::vector<std::string> nmeaLine = split( resp, ',' );

      // Check if read line is a PMTK message
      std::string prefix = "$PMTK";
      if ( !nmeaLine.at(0).compare( 0, prefix.size(), prefix ) ) {
	  std::cout << "PMTK message: " << response << std::endl;
      }

      // Check if number of values in the nmeaLine is the number of
      // values we expect the GPSRMC message to have.
      else if ( nmeaLine.size() < 12 ) {
	  std::cout << "Too few elements: " << response << std::endl;
      }

      // Check if the NMEA line is valid
      else if ( nmeaLine.at( 2 ) == "V" ) {
	  std::cout << "Invalid message: " << response << std::endl;
      }

      return nmeaLine;
}

struct gps_data gps_parse(gps_data data, std::vector<std::string> nmeaLine)
{
      // Print out response message
      data.timeStamp = nmeaLine.at(1);
      data.latitude = atof( nmeaLine.at(3).c_str() ) / 100.00;
      data.northsouth = nmeaLine.at(4);
      data.longitude = atof( nmeaLine.at(5).c_str() ) / 100.00;
      data.eastwest = nmeaLine.at(6);
      std::cout << "Timestamp: " << data.timeStamp << " Latitude: " 
		<< data.latitude << data.northsouth 
		<< " Longitude: " << data.longitude << data.eastwest << std::endl;

      return data;
}

void gps_close(int USB)
{
    close(USB);
}
