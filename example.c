#include "libs.h"
#include "RingBuffer.h"

void example_8bit(void);
void example_16bit(void);
void example_ovf(void);

int main() {
    example_8bit();
    example_16bit();
    example_ovf();
    return 0;
}

void example_8bit(void){
    static RINGBUF_t ringbuf8; // ringbuffer instance
    static u8_t ringbuf8_data[10] = {0,}; // static data buffer
    RingBuf_Init(ringbuf8_data, 10, sizeof(u8_t), &ringbuf8);
    u16_t avail8 = 0;

    // Input array
    u8_t in8[5] = {10, 15, 24, 255, 8};

    // Put 1 byte
    RingBuf_BytePut(in8[0], &ringbuf8); // {10, ... }

    // Check availability after putting 1 byte
    RingBuf_Available(&avail8, &ringbuf8); // avail: 1

    // Put data from specific part of array
    RingBuf_DataPut(&in8[2], 3, &ringbuf8); // {10, 24, 255, 8, ...}

    // Check availability after putting 3 bytes
    RingBuf_Available(&avail8, &ringbuf8); // avail: 4

    u8_t out8_read = 0; // output value after reading
    // Read 1 byte
    RingBuf_ByteRead(&out8_read, &ringbuf8); // 10

    // Check availability after reading 1 byte
    RingBuf_Available(&avail8, &ringbuf8); // avail: 3

    u8_t out8_watch = 0; // output value after watching
    // Watch without flushing
    RingBuf_ByteWatch(&out8_watch, &ringbuf8); // 24

    // Check availability after watching 1 byte
    RingBuf_Available(&avail8, &ringbuf8); // avail: 3

    u8_t out8[5] = {0,}; // output array for reading
    RingBuf_DataRead(out8, avail8, &ringbuf8); // {24, 255, ...}

    // Check availability after reading 3 bytes
    RingBuf_Available(&avail8, &ringbuf8); // avail: 0

    asm("nop"); // point for debugging
}

void example_16bit(void){
    static RINGBUF_t ringbuf16; // ringbuffer instance
    static u16_t ringbuf16_data[10] = {0,}; // static data buffer
    RingBuf_Init(ringbuf16_data, 10, sizeof(u16_t), &ringbuf16);
    u16_t avail16 = 0;

    // Input array
    u16_t in16[5] = {3443, 1004, 1337, 2281, 1234};

    // Put 1 cell (16 bit variable)
    RingBuf_CellPut((u16_t*)&in16[0], &ringbuf16); // {3443, ...}

    // Check availability after putting 1 cell
    RingBuf_Available(&avail16, &ringbuf16); // avail: 1

    // Put data from specific part of array
    RingBuf_DataPut((u16_t*)&in16[2], 3, &ringbuf16); // {3443, 1337, 2281, 1234, ...}

    // Check availability after putting 3 cells
    RingBuf_Available(&avail16, &ringbuf16); // avail: 4

    u16_t out16_read = 0; // output value after reading
    // Read 1 cell (16 bit variable)
    RingBuf_CellRead(&out16_read, &ringbuf16); // 3443

    // Check availability after reading 1 cell
    RingBuf_Available(&avail16, &ringbuf16); // avail: 3

    u16_t out16_watch = 0; // output value after watching
    // Watch without flushing
    RingBuf_CellWatch(&out16_watch, &ringbuf16); // 1337

    // Check availability after watching 1 cell
    RingBuf_Available(&avail16, &ringbuf16); // avail: 3

    u16_t out16[5] = {0,}; // output array for reading
    RingBuf_DataRead(out16, avail16, &ringbuf16); // {24, 255, ...}

    // Check availability after reading 3 bytes
    RingBuf_Available(&avail16, &ringbuf16); // avail: 0

    asm("nop"); // point for debugging
}

void example_ovf(void){
    static RINGBUF_t ringbuf;
    static u32_t data[3] = {0,};
    RingBuf_Init(data, 3, 4, &ringbuf);

    u32_t cell = 66890;
    RingBuf_CellPut(&cell, &ringbuf); // head: 1; tail: 0
    u32_t out1 = 0;
    RingBuf_CellRead(&out1, &ringbuf); // head: 1; tail: 1

    // only 2 cells left in cells' array, let's put 3 cells
    u32_t data_in[3] = {727270, 917020, 812734};
    RingBuf_DataPut(data_in, 3, &ringbuf); // data: {727270, 917020, 812734}
    u32_t data_out[3] = {0,};
    RingBuf_DataRead(data_out, 3, &ringbuf);
    // data_out: {727270, 917020, 812734}

    // Point for debugging
    asm("nop");
}