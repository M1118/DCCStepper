#include <DCCStepper.h>

DCCStepper::DCCStepper(int steps, int rpm, int pin1, int pin2, int pin3 , int pin4)
{
int	i;

  this->mode = STEPPER_MODE_CONT;
  this->maxSteps = 0;
  this->thisStep = 0;
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
  this->waitForDelay = 0;
  this->revDelay = 0;
  this->ignoreSpeed = true;

  this->pattern[0] = 0x01; // 0001
  this->pattern[1] = 0x03; // 0011
  this->pattern[2] = 0x02; // 0010
  this->pattern[3] = 0x06; // 0110
  this->pattern[4] = 0x04; // 0100
  this->pattern[5] = 0x0c; // 1100
  this->pattern[6] = 0x08; // 1000
  this->pattern[7] = 0x09; // 1001


  this->currentStep = 0;

  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
}

DCCStepper::DCCStepper(uint8_t mode, unsigned totalSteps, int steps, int rpm, int pin1, int pin2, int pin3 , int pin4)
{
int	i;

  this->mode = mode;
  this->maxSteps = totalSteps;
  this->pin1 = pin1;
  this->pin2 = pin2;
  this->pin3 = pin3;
  this->pin4 = pin4;
  this->rpm = rpm;
  this->steps = steps;
  unsigned long lsteps = steps;
  unsigned long lrpm = rpm;
  unsigned long steps_per_minute = lsteps * lrpm;
  this->interval = 60000 / steps_per_minute;
  this->refresh = millis() + this->interval;
  this->active = false;
  this->clockwise = true;
  this->ignoreSpeed = true;

  if (mode & STEPPER_BIPOLAR)
  {
#if 0
    this->pattern[0] = 0x01; // 0001    1 Fwd 2 Off
    this->pattern[1] = 0x05; // 0101    1 Fwd 2 Fwd
    this->pattern[2] = 0x04; // 0100	1 Off 2 Fwd
    this->pattern[3] = 0x06; // 0110	1 Rev 2 Fwd
    this->pattern[4] = 0x02; // 0010	1 Rev 2 Off
    this->pattern[5] = 0x0A; // 1010	1 Rev 2 Rev
    this->pattern[6] = 0x08; // 1000	1 Off 2 Rev
    this->pattern[7] = 0x09; // 1001	1 Fwd 2 Rev
#else
    this->pattern[0] = 0x06; // 0110    1 Fwd 2 Rev
    this->pattern[1] = 0x05; // 0101    1 Fwd 2 Fwd
    this->pattern[2] = 0x09; // 1001	1 Rev 2 Fwd
    this->pattern[3] = 0x0A; // 1010	1 Rev 2 Rev
    this->pattern[4] = 0x06; // 0110	1 Fwd 2 Rev
    this->pattern[5] = 0x05; // 0101	1 Fwd 2 Fwd
    this->pattern[6] = 0x09; // 1001	1 Rev 2 Fwd
    this->pattern[7] = 0x0A; // 1010	1 Rev 2 Rev
#endif
  }
  else
  {
    this->pattern[0] = 0x01; // 0001
    this->pattern[1] = 0x03; // 0011
    this->pattern[2] = 0x02; // 0010
    this->pattern[3] = 0x06; // 0110
    this->pattern[4] = 0x04; // 0100
    this->pattern[5] = 0x0c; // 1100
    this->pattern[6] = 0x08; // 1000
    this->pattern[7] = 0x09; // 1001
  }

  this->currentStep = 0;
  this->waitForDelay = 0;
  this->revDelay = 0;

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

  if (this->mode & STEPPER_MODE_CONSTRAINED)
  {
	if (millis() < this->waitForDelay)
		return;
	if (this->clockwise)
	{
		if (this->thisStep >= this->maxSteps)
		{
			if (this->inverted())
			{
				digitalWrite(this->pin1, HIGH);
				digitalWrite(this->pin2, HIGH);
				digitalWrite(this->pin3, HIGH);
				digitalWrite(this->pin4, HIGH);
			}
			else
			{
				digitalWrite(this->pin1, LOW);
				digitalWrite(this->pin2, LOW);
				digitalWrite(this->pin3, LOW);
				digitalWrite(this->pin4, LOW);
			}
			if (this->mode & STEPPER_AUTO_REVERSE)
			{
				if (this->mode & STEPPER_RANDOM_DELAY)
				{
					this->waitForDelay =
						millis() + ((random(this->revDelay - 1) + 1) * 1000);
				}
				else if (this->revDelay)
				{
					this->waitForDelay = millis()
						+ ((unsigned long)this->revDelay * 1000);
				}
				this->mode ^= STEPPER_REVERSE;
			}
			return;
		}
		this->thisStep++;
	}
	else
	{
		if (this->thisStep == 0)
		{
			if (this->inverted())
			{
				digitalWrite(this->pin1, HIGH);
				digitalWrite(this->pin2, HIGH);
				digitalWrite(this->pin3, HIGH);
				digitalWrite(this->pin4, HIGH);
			}
			else
			{
				digitalWrite(this->pin1, LOW);
				digitalWrite(this->pin2, LOW);
				digitalWrite(this->pin3, LOW);
				digitalWrite(this->pin4, LOW);
			}
			if (this->mode & STEPPER_AUTO_REVERSE)
			{
				if (this->mode & STEPPER_RANDOM_DELAY)
				{
					this->waitForDelay =
						millis() + ((random(this->revDelay - 1) + 1) * 1000);
				}
				else if (this->revDelay)
				{
					this->waitForDelay = millis()
						+ ((unsigned long)this->revDelay * 1000);
				}
				this->mode ^= STEPPER_REVERSE;
			}
			return;
		}
		this->thisStep--;
	}
	if (notifyStepperPosition)
		notifyStepperPosition(this, this->thisStep);
  }

  if (this->inverted())
  {
    digitalWrite(this->pin1,
		(this->pattern[this->currentStep] & 0x8) ? LOW : HIGH);
    digitalWrite(this->pin2,
		(this->pattern[this->currentStep] & 0x4) ? LOW : HIGH);
    digitalWrite(this->pin3,
		(this->pattern[this->currentStep] & 0x2) ? LOW : HIGH);
    digitalWrite(this->pin4,
		(this->pattern[this->currentStep] & 0x1) ? LOW : HIGH);
  }
  else
  {
    digitalWrite(this->pin1,
		(this->pattern[this->currentStep] & 0x8) ? HIGH : LOW);
    digitalWrite(this->pin2,
		(this->pattern[this->currentStep] & 0x4) ? HIGH : LOW);
    digitalWrite(this->pin3,
		(this->pattern[this->currentStep] & 0x2) ? HIGH : LOW);
    digitalWrite(this->pin4,
		(this->pattern[this->currentStep] & 0x1) ? HIGH : LOW);
  }
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
  if (this->ignoreSpeed)
    return;
Serial.print("Set speed: ");
Serial.print(percentage);
Serial.println((clockwise ? " Clockwise" : " Anit-clockwise"));
  this->percentage = percentage;
  if (this->mode & STEPPER_REVERSE)
	clockwise = ! clockwise;
  change |= (this->clockwise == clockwise);
  this->clockwise = clockwise;
  if (this->inverted() && percentage == 0)
  {
	digitalWrite(this->pin1, HIGH);
	digitalWrite(this->pin2, HIGH);
	digitalWrite(this->pin3, HIGH);
	digitalWrite(this->pin4, HIGH);
  }
  else if (percentage == 0)
  {
	digitalWrite(this->pin1, LOW);
	digitalWrite(this->pin2, LOW);
	digitalWrite(this->pin3, LOW);
	digitalWrite(this->pin4, LOW);
  }
  if (change && this->percentage)
    this->refresh = millis() + ((this->interval * 100) / this->percentage);
  else if (this->percentage == 0)
    this->refresh = millis() + this->interval;
}

void DCCStepper::setActive(boolean active)
{
  this->ignoreSpeed = !active;
  if (active == false && (this->mode & STEPPER_CONTINUE_ON_DESELECT) != 0)
  {
    return;
  }
  if ((active == true) && (this->active == false))
  {
    if (this->percentage > 0)
      this->refresh = millis() + ((this->interval * 100) / this->percentage);
    else
      this->refresh = millis() + this->interval;
  }
  this->active = active;
  if (this->inverted() && this->active == false)
  {
	digitalWrite(this->pin1, HIGH);
	digitalWrite(this->pin2, HIGH);
	digitalWrite(this->pin3, HIGH);
	digitalWrite(this->pin4, HIGH);
  }
  else if (! this->active)
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

/*
 * Set the mode of the stepper motor.
 * If we switch from continuous mode to constrained
 * mode then we mark the current position as the zero point.
 */
void DCCStepper::setMode(uint8_t mode)
{
  if (this->mode == mode) // No change
  {
    return;
  }
  if ((mode & STEPPER_MODE_CONSTRAINED) !=
	(this->mode & STEPPER_MODE_CONSTRAINED))
  {
    this->thisStep = 0;
  }
  if ((mode & STEPPER_BIPOLAR) != (this->mode & STEPPER_BIPOLAR))
  {
    if (mode & STEPPER_BIPOLAR)
    {
      this->pattern[0] = 0x01; // 0001  1 Fwd 2 Off
      this->pattern[1] = 0x05; // 0101  1 Fwd 2 Fwd
      this->pattern[2] = 0x04; // 0100	1 Off 2 Fwd
      this->pattern[3] = 0x06; // 0110	1 Rev 2 Fwd
      this->pattern[4] = 0x02; // 0010	1 Rev 2 Off
      this->pattern[5] = 0x0A; // 1010	1 Rev 2 Rev
      this->pattern[6] = 0x08; // 1000	1 Off 2 Rev
      this->pattern[7] = 0x09; // 1001	1 Fwd 2 Rev
    }
    else
    {
      this->pattern[0] = 0x01; // 0001
      this->pattern[1] = 0x03; // 0011
      this->pattern[2] = 0x02; // 0010
      this->pattern[3] = 0x06; // 0110
      this->pattern[4] = 0x04; // 0100
      this->pattern[5] = 0x0c; // 1100
      this->pattern[6] = 0x08; // 1000
      this->pattern[7] = 0x09; // 1001
    }
  }
  this->mode = mode;
}

void DCCStepper::setMaxStepsLSB(int value)
{
  this->maxSteps = (this->maxSteps & 0xff00) + (value & 0xff);
}

void DCCStepper::setMaxStepsMSB(int value)
{
  this->maxSteps = (this->maxSteps & 0xff) + ((value & 0xff) << 8);
}

void DCCStepper::setCurrentPosition(unsigned int position)
{
  if (position > this->maxSteps)
	position = this->maxSteps;
  this->thisStep = position;
}

void DCCStepper::setReverseDelay(int revDelay)
{
  this->revDelay = revDelay;
}

unsigned long DCCStepper::getInterval()
{
  return this->interval;
}

unsigned int DCCStepper::getPosition()
{
  return this->currentStep;
}

boolean DCCStepper::inverted()
{
	return this->mode & STEPPER_INVERTED;
}
