#include <iostream>

#include <pigpio.h>

#include "diffdrive_rpi/rotary_encoder.hpp"

/*

             +---------+         +---------+      0
             |         |         |         |
   A         |         |         |         |
             |         |         |         |
   +---------+         +---------+         +----- 1

       +---------+         +---------+            0
       |         |         |         |
   B   |         |         |         |
       |         |         |         |
   ----+         +---------+         +---------+  1

*/

void encoder::_pulse(int gpio, int level, uint32_t tick)
{
   if (gpio == mygpioA) levA = level; else levB = level;

   if (gpio != lastGpio) /* debounce */
   {
      lastGpio = gpio;
      if ((gpio == mygpioA) && (level == 1))
      {
         if (levB) (mycallback)(1*dir,userData);
      }
      else if ((gpio == mygpioB) && (level == 1))
      {
         if (levA) (mycallback)(-1*dir,userData);
      }
   }
}

void encoder::_pulseEx(int gpio, int level, uint32_t tick, void *user)
{
   /*
      Need a static callback to link with C.
   */

   encoder *mySelf = (encoder *) user;

   mySelf->_pulse(gpio, level, tick); /* Call the instance callback. */
}

void encoder::config(int gpioA, int gpioB, encoderCB_t callback, void* user, bool backwards)
{
   mygpioA = gpioA;
   mygpioB = gpioB;
   userData= user;
   if(backwards)
   {
     dir = -1;
   }

   mycallback = callback;

   levA=0;
   levB=0;

   lastGpio = -1;

   gpioSetMode(gpioA, PI_INPUT);
   gpioSetMode(gpioB, PI_INPUT);

   /* pull up is needed as encoder common is grounded */

   gpioSetPullUpDown(gpioA, PI_PUD_UP);
   gpioSetPullUpDown(gpioB, PI_PUD_UP);

   /* monitor encoder level changes */

   gpioSetAlertFuncEx(gpioA, _pulseEx, this);
   gpioSetAlertFuncEx(gpioB, _pulseEx, this);
}

encoder::encoder()
{

}

void encoder::re_cancel(void)
{
   gpioSetAlertFuncEx(mygpioA, 0, this);
   gpioSetAlertFuncEx(mygpioB, 0, this);
}

