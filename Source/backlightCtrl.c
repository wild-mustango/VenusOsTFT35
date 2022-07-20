#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/input.h>
#include <wiringPi.h>
#include <fcntl.h>

/* Pin definition */
//BCM_GPIO_18_PIN_12 is wirinPi pin 1
#define BL_CTRL_PIN 1

int openTouchScreen(int *tfd, char * device)
{
	*tfd = open((const char *)device,O_RDONLY | O_NONBLOCK);

	if (*tfd == -1) {
		syslog(LOG_ERR, "Unable to open %s\n",device);
		printf("Error: %s\n", strerror(errno));
		
		return -1;
	}
	return 0;
}

int screenTouchDetect (int *tfd, int delaySeconds) {
        /* how many bytes were read */
        int rb;
		int detected;
		int timer;
        /* the events (up to 64 at once) */
        struct input_event ev[64];
		detected=0;
		// Set timer in milliseconds
		timer = delaySeconds * 1000;
		do
		{
			/* When using non-blocking io, If nothing is available to read, read will return 0.*/
			rb=read(*tfd,ev,sizeof(struct input_event)*64);
			if(rb > 0)
			{
				//Touch event detected, reset counter
				detected = 1;
			}
			delay(100);
			timer -= 100;
			//printf("Timer value: %i ms.\n",timer);
		}
		while (!detected && timer > 0);
		
		if (timer==0)
		{
			return 0; //Timer expired
		}

		else return 1; //Touch detected

}

int main( int argc, char *argv[] )  {

	int touchFd;
	int delaySecs;
	int res;
	char * deviceArg;
	char * secArg;
	
	

	switch (argc) {
		case 3: {
					deviceArg=argv[1];
					secArg=argv[2];
					printf("The arguments supplied are:\n");
					printf("Argument 1: %s\n", deviceArg);
					printf("Argument 2: %s\n", secArg);
					
					res = 0;
					//openTouchScreen cheks if the input device argument is correct.
					//by trying to open it. If the input device is incorrect, an error
					//will be returned.
					res+=openTouchScreen(&touchFd, deviceArg);
					
					if (res != 0)
					{
						return res;
					}
					
					delaySecs=atoi((const char*)secArg);
					// atoi returns 0 when an error occurs.
					if (delaySecs == 0)
					{
						printf("Error: Provided %s seconds are incorrect.\n",secArg);
						printf("Error: Please provide integer value > 0.\n");
					}
					
					wiringPiSetup();
					//Start screen switched off
					pinMode (BL_CTRL_PIN, OUTPUT) ;
					digitalWrite (BL_CTRL_PIN, HIGH);
					
					while (1)
					{
						res=screenTouchDetect(&touchFd,delaySecs);
					
						if (res == 1) 
						{
							printf("Touch detected\n");
							digitalWrite (BL_CTRL_PIN, HIGH);
						}
						else
						{
							printf("Timer expired\n");
							digitalWrite (BL_CTRL_PIN, LOW); 
						}
							
					}
    				break;
				}
		
		default:
				{
					printf("Two arguments expected.\n");
					printf("Arg 1 - Touchscreen input device. Example: /dev/input/touchscreen0\n");
					printf("Arg 2 - Time in seconds for the backligth to shutdown if no touch is detected.\n");
					printf("Example: backlightCtrl /dev/input/touchscreen0 10 \n");
				
				}
	}
	
	return 0;

}


