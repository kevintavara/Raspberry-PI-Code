#include <stdio.h>
#include <wiringPi.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define BUTTON 25


int main (void) 
{
	bool flag, rod;

	if(wiringPiSetup() == -1){
		printf("wiringPi setup failed !\n");
		return -1;
	}
	
	pinMode(BUTTON, INPUT);
	pullUpDnControl(BUTTON, PUD_DOWN);
	
	while(1){
		if (digitalRead(BUTTON) == HIGH){
			delay(50);
			
			rod = !rod;
			printf("pressed!");
		}
		if (rod == true){
			printf("True");
		}
	}
}



