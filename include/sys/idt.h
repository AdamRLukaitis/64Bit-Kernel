/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#ifndef _IDT_H
#define _IDT_H

#include <defs.h>

void loadIDTR();

void AddInt(int number, void (*handler)(),int dplEntry); //add interrupt

#endif


