#include <stdio.h>
#include <stdbool.h>

extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );

extern void enable_interrupt(void);
extern void handle_interrupt(unsigned cause);

volatile int * toggles = (volatile int*)0x4000010;
volatile int * spbtn = (volatile int*)0x040000d0;

volatile unsigned int * t_status = (volatile unsigned int*)0x04000020;
volatile unsigned int * t_ctrl = (volatile unsigned int*)0x04000024;
volatile unsigned int * t_periodl = (volatile unsigned int*)0x04000028;
volatile unsigned int * t_periodh = (volatile unsigned int*)0x0400002c;
volatile unsigned int * t_snapl = (volatile unsigned int*)0x04000030;
volatile unsigned int * t_snaph = (volatile unsigned int*)0x04000034;

volatile char *VGA = (volatile char*) 0x08000000;
volatile int *VGA_CTRL = (volatile int*) 0x04000100;

int timecounter = 0;

static const int xcoord[6]={
  0,
  80,
  160,
  240,
  320,
  400
};

static const int ycoord[6]={
  0,
  54,
  107,
  160,
  213,
  266
};

int get_sw(){
  int tem = *toggles;
  return tem & 0x3ff;
}

int get_btn(){
  int temp = *spbtn;
  return temp & 0x1;
}

void labinit(void){
  *t_status = 0;

  *t_periodl = 0x93DF;
  *t_periodh = 0x0004;   // 10ms
  
  *t_ctrl = 0b0111;

  enable_interrupt();
  
}

void toptobottom(){
   
    for (int i = 0; i < 320*480; i++)
        VGA[i] = i / 320;
    
    unsigned int y_ofs= 0;
    
    while (1){
        
        *(VGA_CTRL+1) = (unsigned int) (VGA+y_ofs*320);
        *(VGA_CTRL+0) = 0;
        y_ofs= (y_ofs + 1) % 240;
        
        for (int i = 0; i < 1000000; i++)
            asm volatile ("nop");
    }
}

void bottomtotop(){
   for (int i = 0; i < 320*480; i++)
        VGA[i] = i / 320;
    
    unsigned int y_ofs= 240;
    
    while (1){
        
        *(VGA_CTRL+1) = (unsigned int) (VGA+y_ofs*320);
        *(VGA_CTRL+0) = 0;
        y_ofs= (y_ofs - 1) % 240;
        
        for (int i = 0; i < 1000000; i++)
            asm volatile ("nop");
    }
}

void lefttoright(){
   for (int i = 0; i < 320*480; i++)
        VGA[i] = i / 320;
    
    unsigned int x_ofs= 320;
    
    while (1){
        
        *(VGA_CTRL+1) = (unsigned int) (VGA+x_ofs);
        *(VGA_CTRL+0) = 0;
        x_ofs= (x_ofs - 1) % 320;
        
        for (int i = 0; i < 1000000; i++)
            asm volatile ("nop");
    }
}

void righttoleft(){
   for (int i = 0; i < 320*480; i++)
        VGA[i] = i / 320;
    
    unsigned int x_ofs= 0;
    
    while (1){
        
        *(VGA_CTRL+1) = (unsigned int) (VGA+x_ofs);
        *(VGA_CTRL+0) = 0;
        x_ofs= (x_ofs+ 1) % 320;
        
        for (int i = 0; i < 1000000; i++)
            asm volatile ("nop");
    }
}

void corner(int x, int y){
   int lowx = 0;
   int highx = sizeof(xcoord)/sizeof(xcoord[0]);
   int lowy = 0;
   int highy = sizeof(ycoord)/sizeof(ycoord[0]);

   while (lowx <= highx) {
        int mid = lowx + (highx - lowx) / 2;
        
        if (xcoord[mid] <= x && xcoord[mid+1] > x){
            *xy = xcoord[mid];
            xy++;
        }

        if (xcoord[mid] < x)
            lowx = mid + 1;

        else
            highx = mid - 1;
    }

   while (lowy <= highy) {
        int mid = lowy + (highy - lowy) / 2;
        
        if (ycoord[mid] <= y && ycoord[mid+1] > y){
            *xy = ycoord[mid];
            xy--;
        }

        if (ycoord[mid] < y)
            lowy = mid + 1;

        else
            highy = mid - 1;
    }

}

void handle_interrupt(unsigned cause) {
   if(((*t_status) & 1) == 1){
      (*t_status) = 0;
      timecounter++;
      if(timecounter >= 100){
         timecounter = 0;
         //advance the countdown
         //show picture if countdown is done
         }
      }
   }  
  if(get_btn() == 1){
     //reset
  }
}

int main() {
   // Call labinit()
   labinit();
   volatile int *xy = (int*)malloc(2 * sizeof(int));
   printf("Starting from right to left, toggle the following swithches to move the cursor.\n 1: go right\n 2: go down\n 3: go left\n 4: go up\n"); 

   while(1){
      switch(get_sw()){
            case 0b1:
               timecounter = 0;
               //go right
            case 0b10:
               timecounter = 0;
               //go down
            case 0b100:
               timecounter = 0;
               //go left
            case 0b1000:
               timecounter = 0;
               //go up
      delay(1)
   }   
}

