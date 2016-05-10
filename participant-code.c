/*
   Send Data with IR.c
   This is the 'with structures' version of 00 Send Strings with IR.side.
*/

#include "simpletools.h"                     // Include simpletools library
#include "badgetools.h"                      // Include badgetools library  

short MY_ID = 1;
char MY_NAME[16] = "CONNOR";

void sendBeacon();

void main()                                  // Main function
{
  badge_setup();                             // Call badge setup
  
  oledprint("IR Send");
  text_size(SMALL);
  cursor(0, 4);
  oledprint("P17 = SEND");
  
  while(1)
  {
    int states = buttons();
    if(states & 0b0111000)
    {
      rgbs(RED, RED);
    }      
    switch(states)
    {
      case 0b0100000:
        sendBeacon();
        break;
    }        
    if(states)
    {
      rgbs(OFF, OFF);
      pause(600);
    }      
  }      
}  


void sendBeacon() {
  for(int i = 0; i < 5; i++) {
    irprint("%d\n%16s\n", MY_ID, MY_NAME);
    pause(200);
  }    
}  