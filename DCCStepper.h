#ifndef _DCCSTEPPER_H
#define _DCCSTEPPER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define STEPPER_MODE_CONT		0x00
#define STEPPER_MODE_CONSTRAINED	0x01
#define STEPPER_REVERSE			0x02
#define STEPPER_AUTO_REVERSE		0x04
#define STEPPER_RANDOM_DELAY		0x08
#define STEPPER_CONTINUE_ON_DESELECT	0x10
#define STEPPER_BIPOLAR			0x80
#define STEPPER_INVERTED		0x40

class DCCStepper {
  private:
    int			pin1;
    int			pin2;
    int			pin3;
    int			pin4;
    uint8_t		mode;
    int			steps;	    // No. Steps per revolution
    int			rpm;	    // Top speed in RPM
    unsigned long	interval;   // Time between steps
    boolean  		active;     // Is the servo active?
    boolean		ignoreSpeed;// Ignore changes in speed
    boolean  		clockwise;  // Current direction is clockwise
    int      		percentage; // Current speed percentage
    unsigned long	refresh;    // Next refresh time mills()
    int			currentStep;// Current position in pattern
    char		pattern[8]; // The step pattern
    unsigned int	maxSteps;   // Max steps in constrained mode
    unsigned int	thisStep;   // Current step in constrained mode
    int			revDelay;
    unsigned long	waitForDelay;
    boolean		inverted();
  public:
    DCCStepper(int, int, int, int, int, int);
    DCCStepper(uint8_t, unsigned int, int, int, int, int, int, int);
    void loop();
    void setSpeed(int, boolean);
    void setActive(boolean);
    void setRPM(int);
    void setMode(uint8_t);
    void setMaxStepsLSB(int);
    void setMaxStepsMSB(int);
    unsigned int getMaxSteps() { return maxSteps; };
    void setCurrentPosition(unsigned int);
    void setReverseDelay(int);
    unsigned long getInterval();
    unsigned int  getPosition();
};

extern void notifyStepperPosition(DCCStepper *, unsigned int) __attribute__ ((weak));

#endif
