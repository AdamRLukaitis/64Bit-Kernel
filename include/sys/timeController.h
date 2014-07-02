/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#ifndef _TIMECONTROLLER_H
#define _TIMECONTROLLER_H

#include <defs.h>

void printfClock(uint32_t counter);

struct time
{
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint32_t totSec;
} t;


unsigned char readCMOS(uint8_t addr);

void getrealDatetime();

void InitializeRTC();

void setTimerFreq(uint32_t frequency);

#endif

