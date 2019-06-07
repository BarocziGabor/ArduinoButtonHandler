// 
// 
// 

#include "ButtonHandler.h"

// Initialize button
void ButtonHandlerClass::initButton(const uint8_t pin, const uint16_t debounceDelay = 100, const uint16_t longPressDelay = 1000) {
	if (0 <= pin && pin < NUM_DIGITAL_PINS) {
		pinMode(pin, INPUT);
		p_portRegisterAddress = portInputRegister(digitalPinToPort(pin));
		m_pin = pin;
		DPRINT_BUT(String(F("Initializing button on pin: ")) + String(m_pin));
		DPRINT_BUT(String(F("Port number ")) + String(digitalPinToPort(m_pin)));
		DPRINT_BUT(String(F("Port register address: ")) + String((uint8_t)p_portRegisterAddress));

		m_debounceDelay = debounceDelay;
		m_longPressDelay = longPressDelay;
		m_buttonInitialized = true;
		DPRINT_BUT(String(F("Button initialized")));
	}
	else {
		m_buttonInitialized = false;
		DPRINT_BUT(F("Failed to initialize button, wrong pin given"));
	}
}

// Handle button with short press event
void ButtonHandlerClass::handleButton(void(*shortPress)()) {
	if (m_buttonInitialized) {
		// read the value of the button
		bool buttonState = bitRead(*p_portRegisterAddress, m_pin);

		// check for debounce timer and if m_enabled after long press
		if ((millis() - m_debounceLastTime > m_debounceDelay) && m_enabled) {
			// button has been pressed
			if (m_hasBeenPressed) {
				// short press event
				if (buttonState == false) {
					m_hasBeenPressed = false;
					if (shortPress != nullptr) shortPress();
					//m_enabled = false;
					DPRINT_BUT(F("Button short click, without longpress"));
					return;
				}
			}
			else {
				if (buttonState == true) m_hasBeenPressed = true;		// button press event
			}
		}
		// enable button after long press is released
		else {
			if (buttonState == false) m_enabled = true;
		}
	}
}

// Handle button with short press and long press event
void ButtonHandlerClass::handleButton(void(*shortPress)(), void(*longPress)()) {
	if (m_buttonInitialized) {
		// read the value of the button
		bool buttonState = bitRead(*p_portRegisterAddress, m_pin);

		// check for debounce timer and if m_enabled after long press
		if ((millis() - m_debounceLastTime > m_debounceDelay) && m_enabled) {
			// button has been pressed
			if (m_hasBeenPressed) {
				// short press event
				if (buttonState == false) {
					m_hasBeenPressed = false;
					if (shortPress != nullptr) shortPress();
					//m_enabled = false;
					DPRINT_BUT(F("Button short click"));
					return;										// to bypass longPress
				}
				// long press event
				if ((buttonState == true) && ((millis() - m_longPressTimer) > m_longPressDelay)) {
					m_hasBeenPressed = false;
					if (longPress != nullptr) longPress();
					m_enabled = false;							// disable button to prevent another press event
					DPRINT_BUT(F("Button long click"));
				}
			}
			else {
				if (buttonState == true) m_hasBeenPressed = true;		// button press event
				m_longPressTimer = millis();						// save current time for long press event
			}
		}
		// enable button after long press is released
		else {
			if (buttonState == false) m_enabled = true;
		}
	}
}



ButtonHandlerClass ButtonHandler;

