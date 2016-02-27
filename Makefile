telemetry: main.cpp
	g++ main.cpp gps.h gps.cpp comms.h comms.cpp -I/usr/include/ -lusb-1.0 -I/usr/include/ -I/usr/include/libusb-1.0 -o telemetry
