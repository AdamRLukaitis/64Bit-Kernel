/*Copyright © 2013 by Cyril Thomas(cyrthomas) & Sivaram Mothiki(smothiki).
This work is licensed under GPLv3. 
Details in LICENSE file in project root directory.*/

#ifndef _ISR_H
#define _ISR_H

#include <sys/writeUtil.h>

struct pagefault_registers
   {
    uint64_t r15,r14,r13,r12,r11,r10;
    uint64_t r9,r8,rdi,rsi,rdx,rcx;
    uint64_t rbx,rax,rbp,errorCode;
    uint64_t rip,cs,rflags,rsp,ss;
  }__attribute((packed));

void AddInterrupts();
void int_00(void);
void int_01(void);
void int_14(struct pagefault_registers *regvals);
void int_32(void);
void int_33(void);
void defaultFunc(void);


#endif

