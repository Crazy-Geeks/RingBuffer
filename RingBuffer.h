/**
 *******************************************
 * @file    RingBuffer.h
 * @author  Dmitriy Semenov / Crazy_Geeks
 * @version 1.2
 * @date	05-March-2022
 * @brief   Header file for RingBuffer lib
 * @note    https://crazygeeks.ru/c-ringbuffer/
 *******************************************
 */
#ifndef RING_BUF_H_
#define RING_BUF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "libs.h"

/**
 * @addtogroup RING_BUF
 * @brief Ring buffer implementation
 * @{
 */

/**
 * @struct RINGBUF_t
 * @brief Ring buffer unit
 */
typedef struct RINGBUF_t{
    u8_t *buf;			 ///< Storage of the buffer
    volatile size_t tail; ///< Place of read point [cells]
    volatile size_t head; ///< Place of write point [cells]
    volatile size_t size; ///< Size of buffer [cells]
    volatile size_t cell_size; ///< Size of one cell [bytes]
} RINGBUF_t;

/**
 * @enum RINGBUF_STATUS
 * @brief Ring buf status enum
 *
 * RINGBUF_X
 * X: OK, ERR, PARAM_ERR, OVERFLOW
 */
typedef enum RINGBUF_STATUS{
    RINGBUF_OK,		  ///< Success status
    RINGBUF_ERR,      ///< Error
    RINGBUF_PARAM_ERR, ///< Parameter error
    RINGBUF_OVERFLOW, ///< Buffer overflow
} RINGBUF_STATUS;

RINGBUF_STATUS RingBuf_Init(void *buf, u16_t size, size_t cellsize, RINGBUF_t *rb); // Init buf
RINGBUF_STATUS RingBuf_Clear(RINGBUF_t *rb);			 	 // Clear buf
RINGBUF_STATUS RingBuf_Available(u16_t *len, RINGBUF_t *rb); // Data available

// Put: add data to buffer
RINGBUF_STATUS RingBuf_BytePut(const u8_t data, RINGBUF_t *rb); // Put byte to the buf
RINGBUF_STATUS RingBuf_CellPut(const void *data, RINGBUF_t *rb); // Put 1 cell to the buf
RINGBUF_STATUS RingBuf_DataPut(const void *data, u16_t len, RINGBUF_t *rb); // Put data to the buf

// Read: Get data & flush it
RINGBUF_STATUS RingBuf_ByteRead(u8_t *data, RINGBUF_t *rb); // Read byte from buf
RINGBUF_STATUS RingBuf_CellRead(void *data, RINGBUF_t *rb); // Read 1 cell from buf
RINGBUF_STATUS RingBuf_DataRead(void *data, u16_t len, RINGBUF_t *rb); // Read data from buf

// Watch: Get data without flushing
RINGBUF_STATUS RingBuf_ByteWatch(u8_t *data, RINGBUF_t *rb); // Watch byte from buf
RINGBUF_STATUS RingBuf_CellWatch(void *data, RINGBUF_t *rb); // Watch 1 cell from buf
RINGBUF_STATUS RingBuf_DataWatch(void *data, u16_t len, RINGBUF_t *rb); // Watch data form buf

/// @} RING_BUF Group

#ifdef __cplusplus
}
#endif

#endif /* RING_BUF_H_ */
