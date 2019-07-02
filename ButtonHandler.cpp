// 
// 
// 

#include "ArduinoRotaryHandler.h"

// Get the 
ArduinoRotaryHandler * ArduinoRotaryHandler::getInstance() {
	return &instance;
}

void ArduinoRotaryHandler::initButton(const uint8_t pin, const uint16_t debounceDelay = 100, const uint16_t longPressDelay = 1000) {
	if (0 <= pin && pin < NUM_DIGITAL_PINS) {
		pinMode(pin, INPUT);
		p_buttonPortRegisterAddress = portInputRegister(digitalPinToPort(pin));
		m_buttonPin = pin;
		DPRINT(F("Button Pin: "));
		DPRINTLN(m_buttonPin);
		DPRINT(F("Button Pin Port number: "));
		DPRINTLN(digitalPinToPort(m_buttonPin));
		DPRINT(F("Button Pin Port register address: "));
		DPRINTLN((uint16_t)p_buttonPortRegisterAddress);

		m_debounceDelay = debounceDelay;
		m_longPressDelay = longPressDelay;
		m_buttonInitialized = true;
		DPRINT(F("Button initialized on pin: "));
		DPRINTLN(m_buttonPin);
	}
	else {
		m_buttonInitialized = false;
		DPRINTLN(F("Failed to initialize button, wrong pin given"));
	}
}

void ArduinoRotaryHandler::initRotary(const uint8_t interrupt1Pin, const uint8_t interrupt2Pin) {
	if (digitalPinToInterrupt(interrupt1Pin) >= 0 && digitalPinToInterrupt(interrupt2Pin) >= 0 && interrupt1Pin != interrupt2Pin) {
		p_rotaryInterrupt1PortRegisterAddress = portInputRegister(digitalPinToPort(interrupt1Pin));
		p_rotaryInterrupt2PortRegisterAddress = portInputRegister(digitalPinToPort(interrupt2Pin));
		m_rotaryInterrputPin1 = interrupt1Pin;
		m_rotaryInterrputPin2 = interrupt2Pin;
#ifdef ROTARY_TRIGGER_RISING
		attachInterrupt(digitalPinToInterrupt(interrupt1Pin), f_ISR1Wrapper, RISING);
		attachInterrupt(digitalPinToInterrupt(interrupt2Pin), f_ISR2Wrapper, RISING);
#endif
#ifdef ROTARY_TRIGGER_FALLING
		attachInterrupt(digitalPinToInterrupt(interrupt1Pin), f_ISR1Wrapper, FALLING);
		attachInterrupt(digitalPinToInterrupt(interrupt2Pin), f_ISR2Wrapper, FALLING);
#endif
		DPRINT(F("Interrupt 1 Pin: "));
		DPRINTLN(m_rotaryInterrputPin1);
		DPRINT(F("Interrupt 2 Pin: "));
		DPRINTLN(m_rotaryInterrputPin2);
		DPRINT(F("Interrupt 1 Pin port number:"));
		DPRINTLN(digitalPinToPort(interrupt1Pin));
		DPRINT(F("Interrupt 2 Pin port number:"));
		DPRINTLN(digitalPinToPort(interrupt2Pin));
		DPRINT(F("Interrupt 1 Pin port register address: "));
		DPRINTLN((uint16_t)p_rotaryInterrupt1PortRegisterAddress);
		DPRINT(F("Interrupt 2 Pin port register address: "));
		DPRINTLN((uint16_t)p_rotaryInterrupt2PortRegisterAddress);
		DPRINTLN(F("Rotary encoder initialized"));
		m_rotaryInitialized = true;
	}
	else {
		DPRINTLN(F("Failed to initialize rotary encoder, given pins are not interrupt capable"));
		m_rotaryInitialized = false;
	}
}

void ArduinoRotaryHandler::handleRotary(void(*callback1)(), void(*callback2)()) {
	if (m_rotaryInitialized) {
		switch (m_rotaryEvent) {
		case RE_DOWN:
			DPRINTLN(F("callback1 EVENT"));
			callback1();
			m_rotaryEvent = RE_NULL;
			break;
		case RE_NULL:
			break;
		case RE_UP:
			DPRINTLN(F("callback2 EVENT"));
			callback2();
			m_rotaryEvent = RE_NULL;
			break;
		}
	}
}

void ArduinoRotaryHandler::handleButton(void(*shortPress)()) {
	if (m_buttonInitialized) {
		// read the value of the button
#ifdef BUTTON_ACTIVE_HIGH
		bool buttonState = bitRead(*p_buttonPortRegisterAddress, m_buttonPin);
#endif
#ifdef BUTTON_ACTIVE_LOW
		bool buttonState = !bitRead(*p_buttonPortRegisterAddress, m_buttonPin);
#endif

		// check for debounce timer and if m_enabled after long press
		if ((millis() - m_debounceLastTime > m_debounceDelay) && m_enabled) {
			// button has been pressed
			if (m_hasBeenPressed) {
				// short press event
				if (buttonState == false) {
					m_hasBeenPressed = false;
					m_rotaryEvent = RE_NULL;
					if (shortPress != nullptr) shortPress();
					//m_enabled = false;
					DPRINTLN(F("Button short click, without longpress"));
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

void ArduinoRotaryHandler::handleButton(void(*shortPress)(), void(*longPress)()) {
	if (m_buttonInitialized) {
		// read the value of the button
#ifdef BUTTON_ACTIVE_HIGH
		bool buttonState = bitRead(*p_buttonPortRegisterAddress, m_buttonPin);
#endif
#ifdef BUTTON_ACTIVE_LOW
		bool buttonState = !bitRead(*p_buttonPortRegisterAddress, m_buttonPin);
#endif

		// check for debounce timer and if m_enabled after long press
		if ((millis() - m_debounceLastTime > m_debounceDelay) && m_enabled) {
			// button has been pressed
			if (m_hasBeenPressed) {
				// short press event
				if (buttonState == false) {
					m_hasBeenPressed = false;
					m_rotaryEvent = RE_NULL;
					if (shortPress != nullptr) shortPress();
					//m_enabled = false;
					DPRINTLN(F("Button short click"));
					return;										// to bypass longPress
				}
				// long press event
				if ((buttonState == true) && ((millis() - m_longPressTimer) > m_longPressDelay)) {
					m_hasBeenPressed = false;
					m_rotaryEvent = RE_NULL;
					if (longPress != nullptr) longPress();
					m_enabled = false;							// disable button to prevent another press event
					DPRINTLN(F("Button long click"));
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

void ArduinoRotaryHandler::f_ISR1Wrapper() {
#ifdef ROTARY_TRIGGER_RISING
	if (!bitRead(*(instance.p_rotaryInterrupt2PortRegisterAddress), instance.m_rotaryInterrputPin2)) {
#endif
#ifdef ROTARY_TRIGGER_FALLING
		if (bitRead(*(instance.p_rotaryInterrupt2PortRegisterAddress), instance.m_rotaryInterrputPin2)) {
#endif
			instance.f_ISR1();
		}
	};

	void ArduinoRotaryHandler::f_ISR2Wrapper() {
#ifdef ROTARY_TRIGGER_RISING
		if (!bitRead(*(instance.p_rotaryInterrupt1PortRegisterAddress), instance.m_rotaryInterrputPin1)) {
#endif
#ifdef ROTARY_TRIGGER_FALLING
			if (bitRead(*(instance.p_rotaryInterrupt1PortRegisterAddress), instance.m_rotaryInterrputPin1)) {
#endif
				instance.f_ISR2();
			}
		};

		void ArduinoRotaryHandler::f_ISR1() {
			m_rotaryEvent = RE_DOWN;
		}

		void ArduinoRotaryHandler::f_ISR2() {
			m_rotaryEvent = RE_UP;
		}

		ArduinoRotaryHandler ArduinoRotaryHandler::instance = {};

