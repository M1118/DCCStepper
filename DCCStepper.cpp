#include <DCCStepper.h>

DCCStepper::DCCStepper(int steps, int rpm, int pin1, int pin2, int pin3 , int pin4)
{
int	i;

  this->pin1 = pin1;
  this->pin2 = pin2;
  this->pin3 = pin3;
  this->pin4 = pin4;
  this->rpm = rpm;
  this->steps = steps;
  unsigned long steps_per_minute = (unsigned long)steps * (unsigned long)rpm;
  this->interval = 60000 / steps_per_minute;
  this->refresh = millis() + this->interval;
  this->active = false;
  this->clockwise = true;

  for (i = 0; i < 8; i++)
  {
	this->pattern[i] = 1 << (i / 2)
			| ((i & 1) ? (1 << ((i/2) + 1)) : 0);
  }

  this->currentStep = 0;

  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
}

void DCCStepper::loop()
{
  int  newangle;
  
  if (this->refresh > millis())
    return;
  
  if (this->active == false || this->percentage == 0)
  {
    this->refresh = millis() + this->interval;
    return;
  }

  digitalWrite(this->pin1,
		(this->pattern[this->currentStep] & 0x8) ? HIGH : LOW);
  digitalWrite(this->pin2,
		(this->pattern[this->currentStep] & 0x4) ? HIGH : LOW);
  digitalWrite(this->pin3,
		(this->pattern[this->currentStep] & 0x2) ? HIGH : LOW);
  digitalWrite(this->pin4,
		(this->pattern[this->currentStep] & 0x1) ? HIGH : LOW);
  if (this->clockwise)
  {
    this->currentStep++;
    if (this->currentStep >= 8)
	this->currentStep = 0;
  }
  else
  {
    this->currentStep--;
    if (this->currentStep < 0)
	this->currentStep = 7;
  }
  unsigned long delta = (this->interval * 100) / this->percentage;
  this->refresh = millis() + delta;
 
}

void DCCStepper::setSpeed(int percentage, boolean clockwise)
{
  boolean change = this->percentage != percentage;
  this->percentage = percentage;
  this->clockwise = clockwise;
  if (percentage == 0)
  {
	digitalWrite(this->pin1, LOW);
	digitalWrite(this->pin2, LOW);
	digitalWrite(this->pin3, LOW);
	digitalWrite(this->pin4, LOW);
  }
  if (change)
    this->refresh = millis() + ((this->interval * 100) / this->percentage);
}

void DCCStepper::setActive(boolean active)
{
  if ((active == true) && (this->active == false))
  {
    this->refresh = millis() + ((this->interval * 100) / this->percentage);
  }
  this->active = active;
  if (! this->active)
  {
	digitalWrite(this->pin1, LOW);
	digitalWrite(this->pin2, LOW);
	digitalWrite(this->pin3, LOW);
	digitalWrite(this->pin4, LOW);
  }
}

/**
 * The required maximum RPM value has been changed
 */
void DCCStepper::setRPM(int rpm)
{
  this->rpm = rpm;
  unsigned long steps_per_minute = (unsigned long)this->steps
					* (unsigned long)rpm;
  this->interval = 60000 / steps_per_minute;
  this->refresh = millis() + this->interval;
}
