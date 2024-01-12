#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

// Pin 6 GND to GND (MSP430)
// Pin 8 (TX) to P2.1 (RX)
// Pin 10 (RX) to P2.0 (TX)
int main ()
{
  int fd;
  char data;
  char TX = 0xD1;
  /* open serial port */
  if ((fd = serialOpen ("/dev/ttyS0", 9600)) < 0) {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }
  /* initializes wiringPi setup */
  if (wiringPiSetup () == -1) {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  while(1) {
	// 0xF9 to be transmitted to RPi
	// 0xF9 can be changed to anything if 
	// We want to transmit a different value
	serialPutchar(fd, TX);
	
	// Here we check if data is available
	if(serialDataAvail (fd) ) { 
	  // Here we store the data from the MSP430
	  data = serialGetchar(fd);
	  printf ("%3d\n", data);
	  fflush (stdout) ;
	} 
	fflush (stdout) ;
  }

}
