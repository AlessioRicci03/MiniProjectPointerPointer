/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */


/* Below functions are external and found in other files. */
#include <stdio.h>
#include <stdbool.h>

// hello world

extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );

extern void enable_interrupt(void);
extern void handle_interrupt(unsigned cause);

int prime = 1234567;

int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";
volatile int * leds = (volatile int*)0x4000000;
volatile int * sgmnt = (volatile int*)0x4000050;
volatile int * toggles = (volatile int*)0x4000010;
volatile int * spbtn = (volatile int*)0x040000d0;

volatile unsigned int * t_status = (volatile unsigned int*)0x04000020;
volatile unsigned int * t_ctrl = (volatile unsigned int*)0x04000024;
volatile unsigned int * t_periodl = (volatile unsigned int*)0x04000028;
volatile unsigned int * t_periodh = (volatile unsigned int*)0x0400002c;
volatile unsigned int * t_snapl = (volatile unsigned int*)0x04000030;
volatile unsigned int * t_snaph = (volatile unsigned int*)0x04000034;

int timecounter = 0;

//volatile unsigned int * mstatus = (volatile unsigned int*)0x300;
//volatile unsigned int * mie = (volatile unsigned int*)0x304;


static const int which_digit[10]={
  0xC0, // 0
  0xf9, // 1
  0xa4, // 2
  0xb0, // 3
  0x99, // 4
  0x92, // 5
  0x82, // 6
  0xf8, // 7
  0x80, // 8
  0x90  // 9
};
/* Below is the function that will be called when an interrupt is triggered. */
void set_leds(int led_mask){ // led_mask between 0-1023
  *leds = led_mask & 0x3ff;
}

void set_display(int display_number, int value){
    int tdigit = (value / 10) % 10;
    int sdigit = value % 10;

    volatile int *trgt1 = (volatile int *)((int)sgmnt + (display_number * 0x10));
    volatile int *trgt2 = (volatile int *)((int)sgmnt + ((display_number+1) * 0x10));

    *trgt1 = which_digit[sdigit];
    *trgt2 = which_digit[tdigit];
}

int get_sw(){
  int tem = *toggles;
  return tem & 0x3ff;
}

int get_btn(){
  int temp = *spbtn;
  return temp & 0x1;
}

void handle_interrupt(unsigned cause) {
  static int secs = 0;
  static int mins = 0;
  static int hrs = 0;
  static int lb = 0;

  if(get_btn() & 0x1){
    //reset
  }

  switch(get_sw()){
    case 0b1:
      //go right
    case 0b10:
      //go down
    case 0b100:
      //go left
    case 0b1000:
      //go up5
  }

  if(get_sw() & 0x2){
    secs++;
    if(secs >= 60){
      secs = secs % 60;
      mins++;
    }
    if (mins >= 60){
      mins = mins % 60;
      hrs++;
    }
    if(hrs >= 100){
      hrs = 0;
    }

    set_display(0, secs);
    set_display(2, mins);
    set_display(4, hrs);

    
    delay(1);
  }

  if(((*t_status) & 1) == 1){
    (*t_status) = 0;

    timecounter++;
    if(timecounter >= 10){
      timecounter = 0;

      set_display(0, secs);
      set_display(2, mins);
      set_display(4, hrs);

      secs++;
      if(secs >= 60){
        secs = secs % 60;
        mins++;
      }
      if (mins >= 60){
        mins = mins % 60;
        hrs++;
      }
      if(hrs >= 100){
        hrs = 0;
      }

      time2string( textstring, mytime ); // Converts mytime to string
      tick( &mytime );     // Ticks the clock once

      set_leds(lb);
      lb++;
      lb = lb % 0x400;
    }
  }

  if(get_btn() == 1){
    int sw = get_sw();
    int select = (sw >> 8) & 0x3;
    int value = sw & 0x3f;

    if(select == 0b01){
      secs = value % 60;
    } else if(select == 0b10){
      mins = value % 60;
    } else if(select == 0b11){
      hrs = value % 100;
    }
  }
}

/* Add your code here for initializing interrupts. */
void labinit(void){
  *t_status = 0;

  *t_periodl = 0xc6bf;
  *t_periodh = 0x002d;   // 100ms
  
  *t_ctrl = 0b0111;

  set_display(0, 0); // using a for-loop only gives alternating 0's
  set_display(1, 0);
  set_display(2, 0);
  set_display(3, 0);
  set_display(4, 0);
  set_display(5, 0);

  set_leds(0);

  enable_interrupt();
  
}

/* Your code goes into main as well as any needed functions. */
int main() {
  // Call labinit()
  labinit();

  while (1) {
    print("Prime: ");
    prime = nextprime(prime);
    print_dec(prime);
    print("\n");
  }
}

