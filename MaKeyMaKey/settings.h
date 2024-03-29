#include "Arduino.h"
#include <Keyboard.h>

////////////////////////
// DEFINED CONSTANTS////
////////////////////////

#define IsMakeyMakey     true
#define IsMakeyClone     false

#define BUFFER_LENGTH    3     // 3 bytes gives us 24 samples
//#define TARGET_LOOP_TIME 694   // (1/60 seconds) / 24 samples = 694 microseconds per sample
//#define TARGET_LOOP_TIME 758  // (1/55 seconds) / 24 samples = 758 microseconds per sample
#define TARGET_LOOP_TIME 744  // (1/56 seconds) / 24 samples = 744 microseconds per sample 

/*
/////////////////////////////////////////////////////////////////////////
// KEY MAPPINGS: WHICH KEY MAPS TO WHICH PIN ON THE MAKEY MAKEY BOARD? //
/////////////////////////////////////////////////////////////////////////
  
  - edit the keyCodes array below to change the keys sent by the MaKey MaKey for each input
  - the comments tell you which input sends that key (for example, by default 'w' is sent by pin D5)
  - change the keys by replacing them. for example, you can replace 'w' with any other individual letter,
    number, or symbol on your keyboard
  - you can also use codes for other keys such as modifier and function keys (see the
    the list of additional key codes at the bottom of this file)

*/


#if IsMakeyMakey

  #define NUM_INPUTS       18

  int keyCodes[NUM_INPUTS] = {
    KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, ' ', 'm',
    'a', 's', 'd', 'f', 'g', 'h',
    'q', 'w', 'e', 'r', 't', 'y'
  };

  // Pin Numbers
  // input pin numbers
  int pinNumbers[NUM_INPUTS] = {
    12, 8, 13, 15, 7, 6,     // front of board
    5, 4, 3, 2, 1, 0,        // left side of female header
    23, 22, 21, 20, 19, 18   // right side of female header
  };

  // status LED pin number
  const int outputK = 14;
  const int HB_LED_PIN = 16;

#elif IsMakeyClone

  #define NUM_INPUTS       15

  int keyCodes[NUM_INPUTS] = {
    KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, ' ',
    'a', 's', 'd', 'f', 'g',
    'q', 'w', 'e', 'r', 't'
  };

  // Pin Numbers
  // input pin numbers
  int pinNumbers[NUM_INPUTS] = {
    0, 1, 3, 4, 2,
    5, 10, 11, 20, 21,
    22, 23, 24, 30, 31
  };

  // status LED pin number
  const int outputK = 12;
  const int HB_LED_PIN = 13;

#endif

/////////////////////////
// STRUCT ///////////////
/////////////////////////

typedef struct {
  byte pinNumber;
  int keyCode;
  byte measurementBuffer[BUFFER_LENGTH];
  boolean oldestMeasurement;
  byte bufferSum;
  boolean pressed;
  boolean prevPressed;
  boolean isKey;
}
MakeyMakeyInput;

///////////////////////////
// NOISE CANCELLATION /////
///////////////////////////

#define SWITCH_THRESHOLD_OFFSET_PERC  5    // number between 1 and 49
                                           // larger value protects better against noise oscillations, but makes it harder to press and release
                                           // recommended values are between 2 and 20
                                           // default value is 5

#define SWITCH_THRESHOLD_CENTER_BIAS 55   // number between 1 and 99
                                          // larger value makes it easier to "release" keys, but harder to "press"
                                          // smaller value makes it easier to "press" keys, but harder to "release"
                                          // recommended values are between 30 and 70
                                          // 50 is "middle" 2.5 volt center
                                          // default value is 55
                                          // 100 = 5V (never use this high)
                                          // 0 = 0 V (never use this low


/*

///////////////////////////
// ADDITIONAL KEY CODES ///
///////////////////////////

- you can use these codes in the keyCodes array above
- to get modifier keys, function keys, etc 

KEY_LEFT_CTRL
KEY_LEFT_SHIFT		
KEY_LEFT_ALT		
KEY_LEFT_GUI		
KEY_RIGHT_CTRL		
KEY_RIGHT_SHIFT		
KEY_RIGHT_ALT	
KEY_RIGHT_GUI		

KEY_BACKSPACE		
KEY_TAB				
KEY_RETURN			
KEY_ESC				
KEY_INSERT			
KEY_DELETE			
KEY_PAGE_UP			
KEY_PAGE_DOWN		
KEY_HOME
KEY_END				
KEY_CAPS_LOCK	
	
KEY_F1				
KEY_F2				
KEY_F3				
KEY_F4				
KEY_F5				
KEY_F6				
KEY_F7				
KEY_F8				
KEY_F9				
KEY_F10
KEY_F11				
KEY_F12			

*/
