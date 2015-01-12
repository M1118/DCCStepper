#ifndef _DCCSTEPPER_H
#define _DCCSTEPPER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class DCCStepper {
  private:
    int			pin1;
    int			pin2;
    int			pin3;
    int			pin4;
    int			steps;	    // No. Steps per revolution
    int			rpm;	    // Top speed in RPM
    unsigned long	interval;   // Time between steps
    boolean  		active;     // Is the servo active?
    boolean  		clockwise;  // Current direction is clockwise
    int      		percentage; // Current speed percentage
    unsigned long	refresh;    // Next refresh time mills()
    int			currentStep;
    char		pattern[8];
  public:
    DCCStepper(int, int, int, int, int, int);
    void loop();
    void setSpeed(int, boolean);
    void setActive(boolean);
    void setRPM(int);
};

#endif
