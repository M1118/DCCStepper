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

class DCCStepper {
  private:
    int			pin1;
    int			pin2;
    int			pin3;
    int			pin4;
    int			mode;
    int			steps;	    // No. Steps per revolution
    int			rpm;	    // Top speed in RPM
    unsigned long	interval;   // Time between steps
    boolean  		active;     // Is the servo active?
    boolean  		clockwise;  // Current direction is clockwise
    int      		percentage; // Current speed percentage
    unsigned long	refresh;    // Next refresh time mills()
    int			currentStep;// Current position in pattern
    char		pattern[8]; // The step pattern
    unsigned int	maxSteps;   // Max steps in constrained mode
    unsigned int	thisStep;   // Current step in constrained mode
  public:
    DCCStepper(int, int, int, int, int, int);
    DCCStepper(int, unsigned int, int, int, int, int, int, int);
    void loop();
    void setSpeed(int, boolean);
    void setActive(boolean);
    void setRPM(int);
    void setMode(int);
    void setMaxStepsLSB(int);
    void setMaxStepsMSB(int);
    void setCurrentPosition(unsigned int);
};

extern void notifyStepperPosition(DCCStepper *, unsigned int) __attribute__ ((weak));

#endif
