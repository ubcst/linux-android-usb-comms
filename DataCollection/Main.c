/*
https://github.com/WiringPi/WiringPi/blob/master/examples/isr.c

Compile as follows:

    gcc -o isr4pi isr4pi.c -lwiringPi

Run as follows:

    sudo ./isr4pi

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/timeb.h> 
#include <wiringPi.h>


// Use GPIO Pin 17, which is Pin 0 for wiringPi library
#define BUTTON_PIN_1 0  //First sensor, Pin 0 = Pin 17
#define BUTTON_PIN_2 0  //Second sensor
// Interrupt counter increment function
    void sensor_1_Interrupt(void);  //Sensor 1 interrupt function
    void sensor_2_Interrupt(void);  //Sensor 2 interrupt function

// Time difference function between 2 interrupts
    void timeDiff(int counter);

// Sesnor counters
    volatile int sensor_1_Counter = 0;  //Sensor 1 counter 
    volatile int sensor_2_Counter = 0;  //Sensor 2 counter 

// counter between 2 signals used for time difference calculation
    volatile int signalTimeDiff_1 = 0;
    volatile int signalTimeDiff_2 = 0;





// ****************************************************************************


//*****************************************************************************




//Defining Buffer:




// myInterrupt:  called everytime counter 1
    void sensor_1_Interrupt(void) {
     sensor_1_Counter++;
     signalTimeDiff_1++;
     timeDiff(signalTimeDiff_1);
   }

// myInterrupt:  called everytime counter 2
    void sensor_2_Interrupt(void) {
     sensor_2_Counter++;
     signalTimeDiff_2++;
     timeDiff(signalTimeDiff_2);
   }



   //Calculates time difference between 2 signals
   //if time diff is smaller than expected, subtract the 2nd signal (ie combine both signals into one)
  void timeDiff(int counter)
   {
    struct timeb start, end;
    int diff;
    
    //if counter = 0 -> start time
    if (counter == 0)
      ftime(&start);

    //if counter != 0 (counter = 1) -> end time and calculte time difference
    // Check signal accuracy 
    // Choose engine or speed and comment out the other 
    else {
      ftime(&end);
      diff = (int) (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));  //diff = ms

      //CHeck to see which sensor and adjust it if necessary
      if (signalTimeDiff_1 == 1) { 
        signalTimeDiff_1 =0; // Reset signalTimeDiff counter to 0 for the next pair of signls



        //  ----------------------------------ENGINE------------------------------------
        //9 ms = the min time between 2 signals for max engine RPM = 6500 (actual max = 6000)
        // 1/(6500/60) = 1/(RPS) = 9.2308 ms Period of one cycle (approx to 9)
        if (diff < 9)
        sensor_1_Counter = sensor_1_Counter -1;
        //  ----------------------------------SPEED------------------------------------
        //Wheel diameter = 53.34 cm  Wheel circmference = pi*diameter = 167.57 cm, Max speeed = 40km/h = 1111.11 cm/s
        // 1111.11/ 167.57 = 6.630 rotations / s 
        // 1/6.63 = 0.1508 s = 150.8 ms -> smallest time difference 
        if (diff < 150)
         sensor_1_Counter = sensor_1_Counter -1;
      }
      else {
        signalTimeDiff_2 = 0; 
       

        //  ----------------------------------ENGINE------------------------------------
        //9 ms = the min time between 2 signals for max engine RPM = 6500 (actual max = 6000)
        // 1/(6500/60) = 1/(RPS) = 9.2308 ms Period of one cycle (approx to 9)
        if (diff < 9)
        sensor_2_Counter = sensor_2_Counter -1;
        //  ----------------------------------SPEED------------------------------------
        //Wheel diameter = 53.34 cm  Wheel circmference = pi*diameter = 167.57 cm, Max speeed = 40km/h = 1111.11 cm/s
        // 1111.11/ 167.57 = 6.630 rotations / s 
        // 1/6.63 = 0.1508 s = 150.8 ms -> smallest time difference 
        if (diff < 150)
         sensor_2_Counter = sensor_2_Counter -1;
        }
  }
return;
}

//Reads values from ADC channels, written by Daniel Peerson
int readADC(int adcnum, int clockpin, int mosipin, int misopin, int cspin)
{
  int commandout = adcnum;
  int adcout = 0;
  
  if((adcnum > 7) || (adcnum < 0))
  {
    return -1;
  }
  digitalWrite(cspin, HIGH);
  
  digitalWrite(clockpin, LOW);
  digitalWrite(cspin, LOW);
  
  commandout = adcnum; //7: 0b0111
  commandout |= 0x18; //0b11000
  commandout <<= 3; 
  int i = 0;
  for(i = 0; i <= 5; i++)
  {
    if(commandout & 0x80) //0b10000000
    {
      digitalWrite(mosipin, HIGH);
    }
    else
    {
      digitalWrite(mosipin, LOW);
    }
    commandout <<= 1;
    digitalWrite(clockpin, HIGH);
    digitalWrite(clockpin, LOW);
  }
  
  adcout = 0;

  
  for (i = 0; i <= 12; i++)
  {
    digitalWrite(clockpin, HIGH);
    digitalWrite(clockpin, LOW);
    adcout <<= 1;
  
    if(digitalRead(misopin))
    {
      adcout |= 0x1;
    }
  }
  
  digitalWrite(cspin, HIGH);
  
  adcout >>= 1;
  
  return adcout;
}



// -------------------------------------------------------------------------
// main
int main(void) {

  int interruptsPerSec =0;

  // sets up the wiringPi library

  // Defines ADC read channels 
  int chan1ReadVal = 0;
  int chan2ReadVal = 0;
  
  int SPICLK = 18;
  int SPIMISO = 23;
  int SPIMOSI = 24;
  int SPICS = 25;

  //Setup the pins
  pinMode(SPICLK, OUTPUT);
  pinMode(SPIMISO, INPUT);
  pinMode(SPIMOSI, OUTPUT);
  pinMode(SPICS, OUTPUT);

  // One of the setup functions must be called at the start of the program.
  // If it returns -1 then the initialisation of the GPIO has failed, and you should consult the global errno to see why.
  if (wiringPiSetup () < 0)           
  {                 
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
    return 1;
  }

  // set Pin 17/0 generate an interrupt on high-to-low transitions
  // and attach myInterrupt() to the interrupt
  // Starts counting on the falling edge of the interrupt
  // Function returns -1 if failed 
  // If successfull calls myInterrupt to increment event counter
  if ( wiringPiISR (BUTTON_PIN_1, INT_EDGE_FALLING, &sensor_1_Interrupt) < 0 )  
  {
    fprintf (stderr, "Unable to setup ISR for Sensor 1: %s\n", strerror (errno));
    return 1;
  }


  if ( wiringPiISR (BUTTON_PIN_2, INT_EDGE_FALLING, &sensor_2_Interrupt) < 0 )  
  {
    fprintf (stderr, "Unable to setup ISR for Sensor 2: %s\n", strerror (errno));
    return 1;
  }



  //For TESTING interrupts

  // display counter value every second.
  while ( 1 ) {

    //ADC Output
    chan1ReadVal = readADC(0, SPICLK, SPIMOSI, SPIMISO, SPICS);
    chan2ReadVal = readADC(1, SPICLK, SPIMOSI, SPIMISO, SPICS);
    
    printf("Channel 1: %i", chan1ReadVal):
    printf("Channel 2: %i", chan2ReadVal);
    delay(0.5);

    //Counter Output
    printf( "%d\n", sensor_1_Counter);
    sensor_1_Counter = 0;
    delay( 1000 ); 


    return 1;
  }
}