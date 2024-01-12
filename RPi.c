#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <time.h>
#include <stdbool.h>
#define _GNU_SOURCE
// Magnetometer Setup
//  Pin 1 3.3 to Red
//  Pin 3 SDA to Blue
//  Pin 5 SCL to Yellow
//  Pin 9 GND to Black
// UART Setup
//  6 GND to GND (MSP430)
//  Pin 8 (TX) to P2.1 (RX)
//  Pin 10 (RX) to P2.0 (TX)
bool stop = false;
bool cal = false;
bool Static = false;
bool status = true;
bool Dynamic = true;
bool Next = true;
bool x = false;
bool y = false;
bool z = false;
int mAcc = 0;
int mX = 0;
int mY = 0;
int mZ = 0;
unsigned char iX = 1;
unsigned char iY = 2;
unsigned char iZ = 3;
signed int dX = 50;
signed int dY = 50;
signed int dZ = 50;
signed int tX = 4;
signed int tY = 5;
signed int tZ = 6;
signed int ptX = 4;
signed int ptY = 5;
signed int ptZ = 6;
signed int currentX = 0;
signed int currentY = 0;
signed int currentZ = 0;
void calibrate(int X, int Y, int Z);
void simulate(signed int X, signed int Y, signed int Z, signed int Xmag, signed int Ymag, signed int Zmag);

int getTempX(void) {
  return tX;
}

int getTempY(void) {
  return tY;
}

int getTempZ(void) {
  return tZ;
}

int getCurrentX(void) {
  return iX;
}

int getCurrentY(void) {
  return iY;
}

int getCurrentZ(void) {
  return iZ;
}

int getMagFieldX(void) {
  return currentX;
}

int getMagFieldY(void) {
  return currentY;
}

int getMagFieldZ(void) {
  return currentZ;
}

void setStatus(bool s){
  status = s;
}

void setCal(bool c){
  cal = c;
}

void setStatic(bool stat){
  Static = stat;
}

void setDynamic(bool d){
  Dynamic = d;
  Next = true;
}

bool getProgress(void){
  return !cal;
}

void setX(int x) {
  dX = (int)x;
  printf("%d\n", mX);
}

void setY(int y) {
  dY = (int)y;
  printf("%d\n",mY);
}

void setZ(int z) {
  dZ = (int)z;
  printf("%d\n",mZ);
}

void setAcc(int acc) {
  mAcc = (int)acc;
  printf("%d\n",mAcc);
}

int main () {
  int fd;
  int file;
  char *bus = "/dev/i2c-1";
  FILE * fp;
  char * line = NULL;
  char time[5];
  char xD[9];
  char yD[9];
  char zD[9];
  float xM;
  float yM;
  float zM;
  char i = 5;
  char j = 0;
  bool flag1 = false;
  bool flag2 = false;
  size_t len = 0;
  ssize_t read1;
  /* open serial port */
  if ((fd = serialOpen ("/dev/ttyS0", 9600)) < 0) {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1;
  }
  /* initializes wiringPi setup */
  if (wiringPiSetup () == -1) {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1;
  }
  // Create I2C bus
  if((file = open(bus, O_RDWR)) < 0) {
    printf("Failed to open the bus. \n");
    return 1;
  }
  // Get I2C device, MLX30393 I2C address is 0x0C(12)
  ioctl(file, I2C_SLAVE, 0x0C);
  // Write register command, AH = 0x00, AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5(0x60, 0x00, 0x5C, 0x00)
  // Address register, (0x00 << 2)
  char config[4] = {0};
  config[0] = 0x60;
  config[1] = 0x00;
  config[2] = 0x5C;
  config[3] = 0x00;
  write(file, config, 4);
  // Read 1 Byte of data
  // status byte
  char data[7] = {0};
  read(file, data, 1);
  // Write register command, AH = 0x02, AL = 0xB4, RES for magnetic measurement = 0(0x60, 0x02, 0xB4, 0x08)
  // Address register, (0x02 << 2)
  config[0] = 0x60;
  config[1] = 0x02;
  config[2] = 0xB4;
  config[3] = 0x08;
  write(file, config, 4);
  // Read 1 Byte of data
  // status byte
  read(file, data, 1);
  
  fp = fopen("STK.txt", "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);
  
  while(status) {
	// Start single meaurement mode, X, Y, Z-Axis enabled(0x3E)
	config[0] = 0x3E;
	write(file, config, 1);
	// Read 1 byte
	// status
	read(file, data, 1);
	sleep(1);
	// Send read measurement command, X, Y, Z-Axis enabled(0x4E)
	config[0] = 0x4E;
	write(file, config, 1);
	// Read 7 bytes of data
	// status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
	if(read(file, data, 7) != 7) {
		//printf("Error : Input/Output error \n");
	}
	else {
		// Convert the data
		int xMag = (data[1]* 256 + data[2]);
		if(xMag > 32767) {
			xMag -= 65536;
		}
		int yMag = (data[3] * 256 + data[4]);
		if(yMag > 32767) {
			yMag -= 65536;
		}
		int zMag = (data[5] * 256 + data[6]);
		if(zMag > 32767) {
			zMag -= 65536;
		}

		// Output data to screen
		printf("Magnetic Field in X-Axis : %d [uT]\n", xMag);
		printf("Magnetic Field in Y-Axis : %d [uT]\n", yMag);
		printf("Magnetic Field in Z-Axis : %d [uT]\n\n", zMag);
		currentX = xMag;
		currentY = yMag;
		currentZ = zMag;
	}
	// Transmit duty cycles to MSP430
	serialPutchar(fd, mX);
	serialPutchar(fd, mY);
	serialPutchar(fd, mZ);
	
	// Receive data from MSP430
	if (serialDataAvail(fd)) {
	  iX = serialGetchar(fd);
	  iY = serialGetchar(fd);
	  iZ = serialGetchar(fd);
	  tX = serialGetchar(fd);
	  tY = serialGetchar(fd);
	  tZ = serialGetchar(fd);
        }
	if(tX > 50 || tX < 15)
	  tX = ptX;
	if(tY > 50 || tY < 15)
	  tY = ptY;
	if(tZ > 50 || tZ < 15)
	  tZ = ptZ;
        
	printf ("Current X: %d\n",iX);
	printf ("Current Y: %d\n",iY);
	printf ("Current Z: %d\n",iZ);
	
	printf ("Temperature X: %d\n", tX);
	printf ("Temperature Y: %d\n", tY);
	printf ("Temperature Z: %d\n", tZ);
	
	
	
	if ((read1 = getline(&line, &len, fp)) == -1 && Next) {
	  Next = false;
	  fp = fopen("STK.txt", "r");
	  if (fp == NULL)
	    exit(EXIT_FAILURE);
	}
	
	if((read1 = getline(&line, &len, fp)) != -1 && Next) {
	  time[0] = line[0];
	  time[1] = line[1];
	  time[2] = line[2];
	  time[3] = line[3];
	  time[4] = line[4];
	  time[5] = '\0';
	  while(!flag2) {
            if (line[i] == ' ' && !flag1) {
                i++;
            }
            else {
                if (line[i] == ' ') {
                    j = 0;
                    flag2 = true;
                }    
                else {
                    flag1 = true;
                    xD[j] = line[i];
                    i++;
                    j++;
                }
            }
	  }
	  flag1 = false;
	  flag2 = false;
	  while(!flag2) {
            if (line[i] == ' ' && !flag1) {
                i++;
            }
            else {
                if (line[i] == ' ') {
                    j = 0;
                    flag2 = true;
                }    
                else {
                    flag1 = true;
                    yD[j] = line[i];
                    i++;
                    j++;
                }
            }
	  } 
	  flag1 = false;
	  flag2 = false;
	  while(!flag2) {
            if (line[i] == ' ' && !flag1) {
                i++;
            }
            else {
                if (i == 56) {
                    i = 5;
                    j = 0;
                    flag2 = true;
                }    
                else {
                    flag1 = true;
                    zD[j] = line[i];
                    i++;
                    j++;
                }
            }
	  } 
	  flag1 = false;
	  flag2 = false;
	  xD[8] = '\0';
	  yD[7] = '\0';
	  xM = atof(xD);
	  yM = atof(yD);
	  zM = atof(zD);
	  dX = (int) xM / 1000;
	  dY = (int) yM / 1000;
	  dZ = (int) zM / 1000;
	  printf("Desired X: %d\n", dX);
	  printf("Desired Y: %d\n", dY);
	  printf("Desired Z: %d\n", dZ);
	  Next = false;
	}
	
	
	// Check if calibration on
	if(cal)
	  calibrate(currentX, currentY, currentZ);
      
        if(Dynamic)
	  simulate(currentX, currentY, currentZ, dX, dY, dZ);
	
	if(Static)
	  simulate(currentX, currentY, currentZ, dX, dY, dZ);
	// Check if stop enabled
	if(stop) {
	  mX = 0;
	  mY = 0;
	  mZ = 0;
	}
	ptX = tX;
	ptY = tY;
	ptZ = tZ;
    }

}

void simulate(signed int X, signed int Y, signed int Z, signed int Xmag, signed int Ymag, signed int Zmag) {
    if(X > Xmag + 10) {
      mX--;
    }
    else if(X < Xmag - 10) {
      mX++;
    }
    else 
      x = true;
    
    if(Y > Ymag + 10) {
      mY--;
    }
    else if(Y < Ymag - 10) {
      mY++;
    }
    else
      y = true;
    
    if(Z > Zmag + 10) {
      mZ--;
    }
    else if(Z < Zmag - 10) {
      mZ++;
    }
    else
      z = true;

    if(x && y && z && Static) {
      Static = false;
      x = false;
      y = false;
      z = false;
      printf("\n\nStatic Field Done!\n\n");
    }
    
    if(x && y && z && Dynamic) {
      Next = true;
      x = false;
      y = false;
      z = false;
      mX = 0;
      mY = 0;
      mZ = 0;
      printf("\n\nDynamic Iteration Done!\n\n");
    }
}

void calibrate(int X, int Y, int Z) {
    if(X > 10) {
      mX--;
    }
    else if(X < -10) {
      mX++;
    }
    else
      x = true;
    if(Y > 15) {
      mY--;
    }
    else if(Y < -10) {
      mY++;
    }
    else
      y = true;
    if(Z > 10) {
      mZ--;
    }
    else if(-10 > Z) {
      mZ++;
    }
    else
      z = true;
      
    if(x && y && z) {
      cal = false;
      x = false;
      y = false;
      z = false;
    }
}
