// ButtonHandler.h

#ifndef _BUTTONHANDLER_h
#define _BUTTONHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// uncomment this for debugging
//#define DEBUG_BUT

#ifdef DEBUG_BUT
#define DPRINT_BUT(x) Serial.println(x);
#else
#define DPRINT_BUT(x)
#endif


class ButtonHandlerClass {
private:
	bool m_enabled;
	bool m_hasBeenPressed;

	uint16_t m_debounceDelay;
	uint32_t m_debounceLastTime;

	uint16_t m_longPressDelay;
	uint32_t m_longPressTimer;

	bool m_buttonInitialized;
	volatile uint8_t *p_portRegisterAddress;
	uint8_t m_pin;

public:
	void initButton(const uint8_t pin, const uint16_t debounceDelay = 100, const uint16_t longPressDelay = 1000);
	void handleButton(void(*shortPress)());
	void handleButton(void(*shortPress)(), void(*longPress)());

};

extern ButtonHandlerClass ButtonHandler;

#endif

