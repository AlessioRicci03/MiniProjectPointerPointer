#include <stdio.h>
#include <stdbool.h>

struct xy{
  int x;
  int y;
};

struct pics{
  int x;
  int y;
  //picture
};

struct font{
  char c;
  int  binary[8];
};

struct xy cursor;
struct xy pic;

struct pics Matrix[6][6]={
  {{0, 0,},{0, 54,},{0, 107,},{0, 160,},{0, 213,},{0, 266,}}
  {{40, 0,},{40, 54,},{40, 107,},{40, 160,},{40, 213,},{40, 266,}}
  {{80, 0,},{80, 54,},{80, 107,},{80, 160,},{80, 213,},{80, 266,}}
  {{120, 0,},{120, 54,},{120, 107,},{120, 160,},{120, 213,},{120, 266,}}
  {{160, 0,},{160, 54,},{160, 107,},{160, 160,},{160, 213,},{160, 266,}}
  {{200, 0,},{200, 54,},{200, 107,},{200, 160,},{200, 213,},{200, 266,}}
};

struct font font8x8[21]{
  const unsigned char font8x8[21] = {
    {'A',{0b00011000,0b00111100,0b01100110,0b01111110,0b01100110,0b01100110,0b01100110,0b00000000}},
    {'C',{0b00111100,0b11000011,0b11000000,0b11000000,0b11000000,0b11000011,0b00111100,0b00000000}},
    {'D',{0b11111100,0b11000011,0b11000011,0b11000011,0b11000011,0b11000011,0b11111100,0b00000000}},
    {'E',{0b11111111,0b11000000,0b11000000,0b11111111,0b11000000,0b11000000,0b11111111,0b00000000}},
    {'F',{0b11111111,0b11111111,0b11000000,0b11111100,0b11000000,0b11000000,0b11000000,0b00000000}},
    {'G',{0b00111100,0b11000011,0b11000000,0b11000000,0b11001111,0b11000011,0b00111100,0b00000000}},
    {'H',{0b11000011,0b11000011,0b11000011,0b11111111,0b11000011,0b11000011,0b11000011,0b00000000}},
    {'I',{0b00111100,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00111100,0b00000000}},
    {'L',{0b11000000,0b11000000,0b11000000,0b11000000,0b11000000,0b11000000,0b11111111,0b00000000}},
    {'N',{0b11000011,0b11110011,0b11011011,0b11011011,0b11011011,0b11001111,0b11000011,0b00000000}},
    {'O',{0b00111100,0b11000011,0b11000011,0b11000011,0b11000011,0b11000011,0b00111100,0b00000000}},
    {'P',{0b11111100,0b11000011,0b11000011,0b11111100,0b11000000,0b11000000,0b11000000,0b00000000}},
    {'R',{0b11111100,0b11000011,0b11000011,0b11111100,0b11110000,0b11001100,0b11000011,0b00000000}},
    {'S',{0b00111100,0b11000011,0b01100000,0b00011000,0b00000110,0b11000011,0b00111100,0b00000000}},
    {'T',{0b11111111,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b00000000}},
    {'U',{0b11000011,0b11000011,0b11000011,0b11000011,0b11000011,0b11000011,0b00111100,0b00000000}},
    {'3',{0b00111100,0b11000011,0b00000011,0b00111100,0b00000011,0b11000011,0b00111100,0b00000000}}, 
    {'2',{0b00111100,0b11000011,0b00000011,0b00001100,0b00110000,0b11000000,0b11111111,0b00000000}}, 
    {'1',{0b00011000,0b01111000,0b00011000,0b00011000,0b00011000,0b00011000,0b00111100,0b00000000}},  
    {'.',{0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11000000,0b11000000,0b00000000}}, 
    {',',{0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11000000,0b11000000,0b11000000}}
}

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

#define WHITE  0xffff
#define BLACK  0x0000

#define SCREEN_W 320
#define SCREEN_H 240
#define CRSR_SIZE 5

int timecounter = 0;
char countdown[] = "3";

volatile unsigned int width = 320;
volatile unsigned int length = 240;

static const int xcoord[6]={
  0,
  40,
  80,
  120,
  160,
  200
};

static const int ycoord[6]={
  0,
  54,
  107,
  160,
  213,
  266
};

static uint32_t get_sw(void) {
  return (*toggles) & 0xf;
}

int get_btn(){
  int temp = *spbtn;
  return temp & 0x1;
}

static void clear_screen(void) {
  const int npixels = SCREEN_W * SCREEN_H;
  for (int i = 0; i < npixels; ++i) {
    int off = i * 2;
    screen_buf[off] = (uint16_t)BLACK;
  }
}

static void draw_cursor_xy(int x, int y) {
  for (int dy = 0; dy < CRSR_SIZE; ++dy) {
    int py = cursor.y + dy;
    if (py < 0 || py >= SCREEN_H) continue;
      for (int dx = 0; dx < CRSR_SIZE; ++dx) {
        int px = cursor.x + dx;
        if (px < 0 || px >= SCREEN_W) continue;
          int off = (py * SCREEN_W + px)/2;
          screen_buf[off] = (uint16_t)WHITE;
      }
  }
}

static void erase_cursor_xy(int x, int y) {
  for (int dy = 0; dy < CRSR_SIZE; ++dy) {
    int py = cursor.y + dy;
    if (py < 0 || py >= SCREEN_H) continue;
      for (int dx = 0; dx < CRSR_SIZE; ++dx) {
        int px = cursor.x + dx;
        if (px < 0 || px >= SCREEN_W) continue;
          int off = (py * SCREEN_W + px)/2;
          screen_buf[off] = (uint16_t)BLACK;
      }
  }
}

static void refresh_vga(void) {

  vga_ctrl[1] = (uint32_t)((uintptr_t)screen_buf);
  vga_ctrl[0] = 0;

  for (volatile int i = 0; i < 20000; ++i) asm volatile("nop");
}

void labinit(void){
  *t_status = 0;

  *t_periodl = 0x93DF;
  *t_periodh = 0x0004;   // 10ms
  
  *t_ctrl = 0b0111;

  enable_interrupt();

  //create a space

  clear_screen();
  refresh_vga();

  cursor.x = SCREEN_W/2;
  cursor.y = SCREEN_H/2

  draw_cursor_xy(cursor.x, cursor.y);
  refresh_vga();
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

void corner(){
   int lowx = 0;
   int highx = sizeof(xcoord)/sizeof(xcoord[0]);
   int lowy = 0;
   int highy = sizeof(ycoord)/sizeof(ycoord[0]);

   while (lowx <= highx) {
        int mid = lowx + (highx - lowx) / 2;
        
        if (xcoord[mid] <= cursor.x && xcoord[mid+1] > cursor.x){
            pic.x = xcoord[mid];
        }

        if (xcoord[mid] < cursor.x)
            lowx = mid + 1;

        else
            highx = mid - 1;
    }

   while (lowy <= highy) {
        int mid = lowy + (highy - lowy) / 2;
        
        if (ycoord[mid] <= cursor.y && ycoord[mid+1] > cursor.y){
            pic.y = ycoord[mid];
        }

        if (ycoord[mid] < cursor.y)
            lowy = mid + 1;

        else
            highy = mid - 1;
    }

}

void draw_pixel(int x, int y, unsigned short color) {
    if (x < 0 || x >= width || y < 0 || y >= length) 
      return;
    VGA[y * width + x] = color;
}

void draw_char(int *ch, int x, int y, unsigned int color) {
    for (int row = 0; row < 8; row++) {
        int bits = ch[row];
        for (int col = 7; col < 0; col--) {
            if (bits & (1 << col)) {
                VGA_draw_pixel(x + col, y + row, color);
            }
        }
    }
}

void vga_print(char *str, int x, int y, unsigned int color){
  int k = 0;
  while(str[k] != '\0'){
    for(int i = 0, i < 21, i++){
      if(str[k] = font8x8[i].c){
        draw_char(font8x8[i].bin, x, y, color);
      }
    }
    k++;
  }
}

void handle_interrupt(unsigned cause) {
   if(((*t_status) & 1) == 1){
      (*t_status) = 0;
      timecounter++;
      if(timecounter >= 100){
         timecounter = 0;
         vga_print("CURSOR LOCATED. POINTING...", 0, 0, 0xffff);
         if(countdown[0] != "0"){
           vga_print(countdown, 0, 10, 0xffff);
           countdown[0]--;
         } else{
           //show picture
         }
      }
  }
}  

int main() {
  labinit();

   while(1){
    if(((*t_status) & 1) == 1){
       vga_print("FINDING CURSOR... PLEASE, HOLD STILL", 0, 0, 0xffff);
       (*t_status) = 0;
       timecounter++;
       if(timecounter >= 1){
         timecounter = 0;
         countdown = 3;
         //set the display black and write something to invite the user to stop the cursor from moving
       
         uint32_t sw = get_sw();
       
         if(sw & 0x1)
          if (cursor.x < SCREEN_W - CRSR_SIZE) cursor.x++;

        if(sw & 0x2)
          if (cursor.y > 0) cursor.y--;

        if(sw & 0x4)
          if (cursor.y < SCREEN_H - CRSR_SIZE) cursor.y++;

        if(sw & 0x8)
          if (cursor.x > 0) cursor.x--;

        if (new_x != cursor_x || new_y != cursor_y) {
          erase_cursor_xy(cursor.x, cursor.y);
          draw_cursor_xy(cursor.x + 1, cursor.y + 1);
          refresh_vga();
        }
     
      if(get_btn() == 1){
        labinit(); //reset
    }
      delay(1)
    }
  }
  return 0;
}
