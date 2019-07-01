// ArduinoRotaryHandler.h

#ifndef _ARDUINOROTARYHANDLER_h
#define _ARDUINOROTARYHANDLER_h

#include "arduino.h"

// ===== Set interrupt trigger direction =====

// Interrupt set to RISING trigger
#define ROTARY_TRIGGER_RISING

// Interrupt set to FALLING trigger
//#define ROTARY_TRIGGER_FALLING
// ===========================================

// ===========================================

//#define BUTTON_ACTIVE_HIGH 
#define BUTTON_ACTIVE_LOW 
/* ===== Button circuit =====
   ===== Active high =====			===== Active low =====
	   ___ Vcc 						   ___ Vcc
		|								|
	   / 							  |‾‾‾|
	  /  							  | R |
		|							   ‾|‾
		|—————— uController				|—————— uController
		|								|
	  |‾‾‾|							   /
	  | R |							  /
	   ‾|‾							    |
	   ‾‾‾ GND						   ‾‾‾ GND
*/
// ===========================================



#define DEBUG_ROTARY
#ifdef DEBUG_ROTARY
#define DPRINTLN(x) Serial.println(x)
#define DPRINT(x) Serial.print(x)
#else
#define DPRINTLN(x)
#define DPRINT(x)
#endif

enum rotaryEvent_t {
	RE_DOWN = -1,
	RE_NULL = 0,
	RE_UP = 1
};

class ArduinoRotaryHandler {
protected:
	//===== ROTARY =====
	bool m_rotaryInitialized;
	uint8_t m_rotaryInterrputPin1;
	uint8_t m_rotaryInterrputPin2;
	volatile rotaryEvent_t m_rotaryEvent;
	volatile uint8_t *p_rotaryInterrupt1PortRegisterAddress;
	volatile uint8_t *p_rotaryInterrupt2PortRegisterAddress;

	void f_ISR1();
	void f_ISR2();
	static void f_ISR1Wrapper();
	static void f_ISR2Wrapper();

	//===== BUTTON =====
	bool m_buttonInitialized;
	bool m_enabled;
	bool m_hasBeenPressed;
	uint16_t m_debounceDelay;
	uint32_t m_debounceLastTime;
	uint16_t m_longPressDelay;
	uint32_t m_longPressTimer;
	uint8_t m_buttonPin;
	volatile uint8_t *p_buttonPortRegisterAddress;

	//===== ELSE =====
	ArduinoRotaryHandler() {};
	static ArduinoRotaryHandler instance;

public:
	static ArduinoRotaryHandler *getInstance();
	void initButton(const uint8_t pin, const uint16_t debounceDelay = 100, const uint16_t longPressDelay = 1000);
	void initRotary(const uint8_t interrupt1Pin, const uint8_t interrupt2Pin);
	void handleRotary(void(*callback1)(), void(*callback2)());
	void handleButton(void(*shortPress)());
	void handleButton(void(*shortPress)(), void(*longPress)());
};


#endif



