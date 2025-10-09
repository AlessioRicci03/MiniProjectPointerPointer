#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include "images.h" \\must keep

struct xy{
  int x;
  int y;
};

struct pics{
  int x;
  int y;
  int arr[76800];
};

struct font{
  char c;
  int  binary[8];
};

struct xy cursorcoords;
struct xy newcoords;
struct xy pic;

struct pics Matrix[6][6]={
  {{0, 0, img0},{0, 54, img1_5_7},{0, 107, img2_3},{0, 160, img2_3},{0, 213, img4_34},{0, 266, img1_5_7}},
  {{40, 0, img6_8_9_11},{40, 54, img1_5_7},{40, 107, img6_8_9_11},{40, 160, img6_8_9_11},{40, 213, img10},{40, 266, img6_8_9_11}},
  {{80, 0, img12},{80, 54, img13_14_15_26_28_30_32_33_35},{80, 107, img13_14_15_26_28_30_32_33_35},{80, 160, img13_14_15_26_28_30_32_33_35},{80, 213, img16},{80, 266, img17}},
  {{120, 0, img18},{120, 54, img19},{120, 107, img20},{120, 160, img21},{120, 213, img22},{120, 266, img23}},
  {{160, 0, img24},{160, 54, img25},{160, 107, img13_14_15_26_28_30_32_33_35},{160, 160, img27},{160, 213, img13_14_15_26_28_30_32_33_35},{160, 266, img29}},
  {{200, 0, img13_14_15_26_28_30_32_33_35},{200, 54, img31},{200, 107, img13_14_15_26_28_30_32_33_35},{200, 160, img13_14_15_26_28_30_32_33_35},{200, 213, img4_34},{200, 266, img13_14_15_26_28_30_32_33_35}}
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

volatile int * toggles = (volatile int*)0x04000010;
volatile int * spbtn = (volatile int*)0x040000d0;

volatile unsigned int * t_status = (volatile unsigned int*)0x04000020;
volatile unsigned int * t_ctrl = (volatile unsigned int*)0x04000024;
volatile unsigned int * t_periodl = (volatile unsigned int*)0x04000028;
volatile unsigned int * t_periodh = (volatile unsigned int*)0x0400002c;
volatile unsigned int * t_snapl = (volatile unsigned int*)0x04000030;
volatile unsigned int * t_snaph = (volatile unsigned int*)0x04000034;

volatile uint32_t *vga_ctrl = (volatile uint32_t *)0x04000100;
volatile uint16_t  *screen_buf = (volatile uint16_t *)0x08000000;

#define WHITE  0xffff
#define BLACK  0x0000

#define SCREEN_W 320
#define SCREEN_H 240
#define CRSR_SIZE 5

int activity_counter = 0;
int inactivity_counter = 0;
char countdown[] = "3";

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
    int py = y + dy;
    if (py < 0 || py >= SCREEN_H) continue;
      for (int dx = 0; dx < CRSR_SIZE; ++dx) {
        int px = x + dx;
        if (px < 0 || px >= SCREEN_W) continue;
          int off = (py * SCREEN_W + px)/2;
          screen_buf[off] = (uint16_t)WHITE;
      }
  }
}

static void erase_cursor_xy(int x, int y) {
  for (int dy = 0; dy < CRSR_SIZE; ++dy) {
    int py = y + dy;
    if (py < 0 || py >= SCREEN_H) continue;
      for (int dx = 0; dx < CRSR_SIZE; ++dx) {
        int px = x + dx;
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

  clear_screen();
  refresh_vga();

  cursorcoords.x = SCREEN_W/2;
  cursorcoords.y = SCREEN_H/2

  draw_cursor_xy(cursorcoords.x, cursorcoords.y);
  refresh_vga();
  
  enable_interrupt();
}

void printpicturepixel(int *array){
   for (int i = 0; i < 320; i++)
     for (int j = 0; j < 320*480; j+= 320)
        VGA[i+j] = (char) array[i+j];
}

void printpicture(){
  int lowx = 0;
   int highx = sizeof(xcoord)/sizeof(xcoord[0]);
   int lowy = 0;
   int highy = sizeof(ycoord)/sizeof(ycoord[0]);

    while (lowx <= highx) {
        int midx = lowx + (highx - lowx) / 2;
        
        if (pic.x == Matrix[midx][0].x){
            while (lowy <= highy) {
              int midy = lowy + (highy - lowy) / 2;
        
              if (pic.y == Matrix[midx][midy].y){
                printpicturepixel(Matrix[midx][midy].arr);
              }

              else if (pic.y < Matrix[midx][midy].y)
                lowy = midy + 1;

              else
                highy = midy - 1;
            }
        }

        else if (pic.x < Matrix[midx][0].x)
            lowx = midx + 1;

        else
            highx = midx - 1;
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

        else if (xcoord[mid] < cursor.x)
            lowx = mid + 1;

        else
            highx = mid - 1;
    }

   while (lowy <= highy) {
        int mid = lowy + (highy - lowy) / 2;
        
        if (ycoord[mid] <= cursor.y && ycoord[mid+1] > cursor.y){
            pic.y = ycoord[mid];
        }

        else if (ycoord[mid] < cursor.y)
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
  if((*t_status & 1) == 1) {
    *t_status = 0;

    if(!activity_counter) {
      inactivity_counter++;
      if(inactivity_counter >= 200) {
        inactivity_counter = 0;
        vga_print("CURSOR LOCATED. POINTING...", 0, 0, WHITE);
        printpicture();
      }
    } else {
      activity_counter = 0;
    }
  }

  // switch interrupt
  uint32_t sw = get_sw();
  struct xy newcoords = cursorcoords;

  if(sw & 0x1 && newcoords.x < SCREEN_W - CRSR_SIZE) newcoords.x++;
  if(sw & 0x2 && newcoords.y > 0) newcoords.y--;
  if(sw & 0x4 && newcoords.y < SCREEN_H - CRSR_SIZE) newcoords.y++;
  if(sw & 0x8 && newcoords.x > 0) newcoords.x--;

  if(newcoords.x != cursorcoords.x || newcoords.y != cursorcoords.y) {
    erase_cursor_xy(cursorcoords.x, cursorcoords.y);
    draw_cursor_xy(newcoords.x, newcoords.y);
    cursorcoords = newcoords;
    refresh_vga();
    activity_counter = 1;
    inactivity_counter = 0;
  }
}  

int main() {
  labinit();

   while(1){
      if(get_btn() == 1){
        labinit(); //reset
      }
   }
}
