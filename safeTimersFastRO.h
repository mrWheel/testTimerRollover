
uint16_t timer16Bit()
{
  return ((millis() & 0b1111111111111111)); // only 16 bits
  
} // timer16Bit()

uint32_t timer32Bit()
{
  return (uint32_t)(micros() + (UINT32_MAX - (5*60*1000*1000))); // first rollover after 5 minutes
  
} // timer32Bit()

/* 
***************************************************************************  
**  Filename  : safeTimers.h
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

/*
 * safeTimers.h (original name timers.h) is developed by Erik
 * 
 * Willem Aandewiel and Robert van den Breemen made some changes due 
 * to the "how can I handle the millis() rollover" by Edgar Bonet and added 
 * CHANGE_INTERVAL() and RESTART_TIMER() macro's
 *
 * see: https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
 * 
 * DECLARE_TIMER_MIN(timername, interval, <timerType>)     // interval in minutes
 * DECLARE_TIMER_SEC(timername, interval, <timerType>)     // interval in seconds
 * DECLARE_TIMER_MS(timername,  interval, <timerType>)     // interval in milliseconds
 * DECLARE_TIMER(timername,     interval, <timerType>)     // interval in milliseconds
 *  Declares three static vars: 
 *    <timername>_due (uint32_t) for next execution
 *    <timername>_interval (uint32_t) for interval in seconds
 *    <timername>_type (byte)
 * 
 * <timerType> can either be:
 *   SKIP_MISSED_TICKS
 *   CATCH_UP_MISSED_TICKS
 *   SKIP_MISSED_TICKS_WITH_SYNC 
 * 
 * TIME_LEFT_MIN(timerName)
 *  returns the time left in minutes
 * TIME_LEFT_SEC(timerName)
 *  returns the time left in seconds
 * TIME_LEFT_MS(timerName)
 *  returns the time left in milliseconds
 * TIME_LEFT(timerName)
 *  returns the time left in milliseconds
 * 
 * CHANGE_INTERVAL_MIN(timername, interval, <timerType>)
 * CHANGE_INTERVAL_SEC(timername, interval, <timerType>)
 * CHANGE_INTERVAL_MS(timername,  interval, <timerType>)
 * CHANGE_INTERVAL(timername,     interval, <timerType>)
 *  Changes the static vars declared by DECLARE_TIMER(): 
 *    <timername>_due (uint32_t) for next execution
 *    <timername>_interval (uint32_t) for interval
 *    <timername>_type (byte) 
 *    
 * RESTART_TIMER(timername)
 *  updates <timername>_due = millis() + <timername>_interval
 *    
 * DUE(timername) 
 *  returns false (0) if interval hasn't elapsed since last DUE-time
 *          true (current millis) if it has
 *  updates <timername>_due
 *  
 *  Usage example:
 *  
 *  DECLARE_TIMER(screenUpdate, 200, SKIP_MISSED_TICKS)          // update screen every 200 ms
 *  ...
 *  loop()
 *  {
 *  ..
 *    if ( DUE(screenUpdate) ) {
 *      // update screen code
 *    }
 *    
 *    // to change the screenUpdate interval:
 *    CHANGE_INTERVAL(screenUpdate, 500, CATCH_UP_MISSED_TICKS); // change interval to 500 ms
 *    
 *    // to restart the screenUpdate interval:
 *    RESTART_TIMER(screenUpdate);                                // restart timer so next DUE is in 500ms
 *  }
 *  
*/

//--- timerType's -----------------------
#define SKIP_MISSED_TICKS              0
#define SKIP_MISSED_TICKS_WITH_SYNC    1
#define CATCH_UP_MISSED_TICKS          2


#define DECLARE_TIMER_MIN(timerName, ...) \
                      static uint32_t timerName##_interval = (getParam(0, __VA_ARGS__, 0) * 60 * 1000 * 1000), \
                                      timerName##_due  = timer32Bit() \
                                                       +timerName##_interval \
                                                       +random(timerName##_interval / 3); \
                      static byte     timerName##_type = getParam(1, __VA_ARGS__, 0);

#define DECLARE_TIMER_SEC(timerName, ...) \
                      static uint32_t timerName##_interval = (getParam(0, __VA_ARGS__, 0) * 1000 * 1000), \
                                      timerName##_due  = timer32Bit() \
                                                       +timerName##_interval \
                                                       +random(timerName##_interval / 3); \
                      static byte     timerName##_type = getParam(1, __VA_ARGS__, 0);

#define DECLARE_TIMER_MS(timerName, ...)  \
                      static uint32_t timerName##_interval = (getParam(0, __VA_ARGS__, 0) * 1000), \
                                      timerName##_due  = timer32Bit() \
                                                       +timerName##_interval \
                                                       +random(timerName##_interval / 3); \
                      static byte     timerName##_type = getParam(1, __VA_ARGS__, 0);

#define DECLARE_TIMER   DECLARE_TIMER_MS


#define CHANGE_INTERVAL_MIN(timerName, ...) \
                                      timerName##_interval = (getParam(0, __VA_ARGS__, 0) *60*1000*1000); \
                                      timerName##_due  = timer32Bit() +timerName##_interval;
#define CHANGE_INTERVAL_SEC(timerName, ...) \
                                      timerName##_interval = (getParam(0, __VA_ARGS__, 0) *1000*1000), \
                                      timerName##_due  = timer32Bit() +timerName##_interval;
#define CHANGE_INTERVAL_MS(timerName, ...)  \
                                      timerName##_interval = (getParam(0, __VA_ARGS__, 0) *1000), \
                                      timerName##_due  = timer32Bit() +timerName##_interval;

#define CHANGE_INTERVAL CHANGE_INTERVAL_MS

#define TIME_LEFT(timerName)          ( __TimeLeft__(timerName##_due) / 1000 ) 
#define TIME_LEFT_MS(timerName)       ( (TIME_LEFT(timerName) )  )
#define TIME_LEFT_SEC(timerName)      ( (TIME_LEFT(timerName) ) / 1000 )
#define TIME_LEFT_MIN(timerName)      ( (TIME_LEFT(timerName) ) / (60 * 1000))

#define TIME_PAST(timerName)          ( (timerName##_interval - TIME_LEFT(timerName)) / 1000 )
#define TIME_PAST_MS(timerName)       ( (TIME_PAST(timerName) )
#define TIME_PAST_SEC(timerName)      ( (TIME_PAST(timerName) / 1000) )
#define TIME_PAST_MIN(timerName)      ( (TIME_PAST(timerName) / (60*1000)) )

#define RESTART_TIMER(timerName)      ( timerName##_due = timer32Bit()+timerName##_interval ); 

#define DUE(timerName)                ( __Due__(timerName##_due, timerName##_interval, timerName##_type) )


uint32_t __Due__(uint32_t &timer_due, uint32_t timer_interval, byte timerType)
{
  if ((int32_t)(timer32Bit() - timer_due) >= 0) 
  {
    switch (timerType) {
        case CATCH_UP_MISSED_TICKS:   
                  timer_due += timer_interval;
                  break;

        case SKIP_MISSED_TICKS_WITH_SYNC:
                  if ( (timer32Bit() - timer_due) >= (timer_interval * 0.05) )
                  {
                    timer_due  += timer_interval;
                    return 0;
                  }
                  while ((int32_t)(timer32Bit() - timer_due) >= 0) 
                  {
                    timer_due  += timer_interval;
                    yield();
                  }
                  break;

        default:  // SKIP_MISSED_TICKS
                  timer_due = timer32Bit() + timer_interval;
                  break;
    } // switch()
    
    return (uint32_t)timer_due;  
  }
  
  return 0;
  
} // __Due__()


uint32_t __TimeLeft__(uint32_t timer_due)
{
  uint32_t tmp;
  byte state = 0;
  
  // timeline 0-------------------------SIGNED-MAX-------------------------UMAX
  // state=0  0---------------------------T-|---D--------------------------UMAX
  // state=0  0-----------------------------|---------------T-------D------UMAX
  // state=0  0-------T-D-------------------|------------------------------UMAX
    
  // state=1  0---T<========INT32_MAX========>-------------------------D---UMAX
  if ( timer_due > (timer32Bit() + INT32_MAX) ) state = 1;  // timer() rolled-over
  
  // state=2  0--------D<========INT32_MAX========>---------------T--------UMAX
  if ( timer32Bit() > (timer_due + INT32_MAX) ) state = 2;  // _due rolled-over

  switch(state) {
        case 1:     //--- timer16Bit() rolled-over
        case 2:     //--- _due rolled-over
                    if ( (int32_t)((timer_due + UINT32_MAX) - timer32Bit()) >= 0 )
                          tmp = (timer_due + UINT32_MAX) - timer32Bit();
                    else  tmp = 0;
                    break;
        default:    if ( (int32_t)(timer_due - timer32Bit()) >= 0 )
                          tmp = timer_due - timer32Bit();
                    else  tmp = 0;
  }

  return (uint32_t)tmp;
  
} // __TimeLeft__()


// process variadic from macro's
uint32_t getParam(int i, ...) 
{
  uint32_t parm, p;
  va_list vl;
  //va_start(vl,p); // warning: second parameter of 'va_start' not last named argument [-Wvarargs] 
  va_start(vl,i);
  for (p=0; p<=i; p++)
  {
    parm=va_arg(vl,uint32_t);
  }
  va_end(vl);
  return parm;
} // getParam()

/*
 * ******************** 16 bit timers macro's for testing purposes *********************************
 */

#define DECLARE_TIMER_16BIT(timerName, ...)  \
                        static uint16_t timerName##_interval = getParam(0, __VA_ARGS__, 0), \
                                        timerName##_due  = timer16Bit() \
                                                          +timerName##_interval \
                                                          +random(timerName##_interval / 3); \
                        static byte     timerName##_type = getParam(1, __VA_ARGS__, 0);

#define CHANGE_16BIT_INTERVAL(timerName, ...) \
                                        timerName##_interval = (getParam(0, __VA_ARGS__, 0) ); \
                                        timerName##_due  = timer16Bit() +timerName##_interval;
                                                    
#define RESTART_TIMER_16BIT(timerName)  timerName##_due = timer16Bit() +timerName##_interval;

#define TIME_LEFT_16BIT(timerName)      ( __TimeLeft16Bit__(timerName##_due) ) 
#define TIME_LEFT_SEC_16BIT(timerName)  ( (TIME_LEFT_16BIT(timerName) / 1000) )

#define TIME_PAST_16BIT(timerName)      ( (timerName##_interval - TIME_LEFT_16BIT(timerName)) )
#define TIME_PAST_SEC_16BIT(timerName)  ( (TIME_PAST_16BIT(timerName) / 1000) )

#define DUE_16BIT(timerName)            ( __Due16Bit__(timerName##_due \
                                                         , timerName##_interval \
                                                         , timerName##_type) )


uint16_t __Due16Bit__(uint16_t &timer_due, uint16_t timer_interval, byte timerType)
{
  if ((int16_t)(timer16Bit() - timer_due) >= 0) 
  {
    switch (timerType) {
        case SKIP_MISSED_TICKS_WITH_SYNC:
                  if ( (timer16Bit() - timer_due) >= (timer_interval * 0.05) )
                  {
                    timer_due  += timer_interval;
                    return 0;
                  }
                  while ((int16_t)(timer16Bit() - timer_due) >= 0) 
                  {
                    timer_due  += timer_interval;
                    yield();
                  }
                  break;

        case CATCH_UP_MISSED_TICKS:   
                  timer_due += timer_interval;
                  break;

        default:  // SKIP_MISSED_TICKS
                  timer_due = timer16Bit() + timer_interval;
                  break;
    } // switch()
    
    return (uint16_t)timer_due;  
  }
  
  return 0;

} // __Due16Bit__()


uint16_t __TimeLeft16Bit__(uint16_t timer_due)
{
  uint16_t tmp;
  byte state = 0;

  // timeline 0-------------------------SIGNED-MAX-------------------------UMAX
  // state=0  0---------------------------T-|---D--------------------------UMAX
  // state=0  0-----------------------------|---------------T-------D------UMAX
  // state=0  0-------T-D-------------------|------------------------------UMAX
    
  // state=1  0---T<========INT16_MAX========>-------------------------D---UMAX
  if ( timer_due > (timer16Bit() + INT16_MAX) ) state = 1;  // timer() rolled-over
  
  // state=2  0--------D<========INT16_MAX========>---------------T--------UMAX
  if ( timer16Bit() > (timer_due + INT16_MAX) ) state = 2;  // _due rolled-over

  switch(state) {
        case 1:     //--- timer16Bit() rolled-over
        case 2:     //--- _due rolled-over
                    if ( (int16_t)((timer_due + UINT16_MAX) - timer16Bit()) >= 0 )
                          tmp = (timer_due + UINT16_MAX) - timer16Bit();
                    else  tmp = 0;
                    break;
        default:    if ( (int16_t)(timer_due - timer16Bit()) >= 0 )
                          tmp = timer_due - timer16Bit();
                    else  tmp = 0;
  }

  return (uint16_t)tmp;
  
} // __TimeLeft16Bit__()

/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
****************************************************************************
*/
