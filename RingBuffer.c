/**
 *******************************************
 * @file    RingBuffer.c
 * @author  Dmitriy Semenov / Crazy_Geeks
 * @version 1.2
 * @date	05-March-2022
 * @brief   Source file for RingBuffer lib
 * @note    https://crazygeeks.ru/c-ringbuffer/
 *******************************************
 */

#include "RingBuffer.h"

/**
 * @addtogroup RING_BUF
 * @{
 */

/**
 * @brief Init ring buffer
 *
 * @param[in] buf Pointer to the allocated buffer
 * @param[in] size Size of buffer
 * @param[in] cellsize Size of 1 cell [bytes]
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_Init(void *buf, u16_t size, size_t cellsize, RINGBUF_t *rb) {
    rb->size = size; // size of array
    rb->cell_size = cellsize; // size of 1 cell of array
    rb->buf = buf;      // set pointer to buffer
    RingBuf_Clear(rb); // clear all
    return rb->buf ? RINGBUF_OK : RINGBUF_PARAM_ERR;
}

/**
 * @brief Clear ring buffer
 * @note Disable interrupts while clearing
 *
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_Clear(RINGBUF_t *rb) {
    if (rb->buf == NULL) return RINGBUF_PARAM_ERR;
    rb->head = rb->tail = 0;
    return RINGBUF_OK;
}

/**
 * @brief Check available size to read
 *
 * @param[out] len Size to read [bytes]
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_Available(u16_t *len, RINGBUF_t *rb) {
    if (rb->buf == NULL) return RINGBUF_PARAM_ERR;
    if (rb->head < rb->tail)
        *len = rb->size - rb->tail + rb->head;
    else
        *len = rb->head - rb->tail;
    return RINGBUF_OK;
}

/**
 * @brief Put byte to the buffer
 *
 * @param[in] data Data byte to be put [bytes]
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_BytePut(const u8_t data, RINGBUF_t *rb) {
    if (rb->buf == NULL) return RINGBUF_PARAM_ERR;
    rb->buf[rb->head++] = data; // put byte in cell and increment data
    if (rb->head >= rb->size) // if overflow
        rb->head = 0;   // set to start
    return RINGBUF_OK;
}

/**
 * @brief Put 1 cell to the buffer
 * @param[in] data Pointer to data
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_CellPut(const void *data, RINGBUF_t *rb) {
    return RingBuf_DataPut(data, 1, rb);
}

/**
 * @brief Put some data to the buffer
 *
 * @param[in] data Data to be put
 * @param[in] len Length of data to be written [bytes]
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_DataPut(const void *data, u16_t len, RINGBUF_t *rb) {
    if (rb->buf == NULL) return RINGBUF_PARAM_ERR;
    if (len > rb->size)
        return RINGBUF_OVERFLOW;
    const char *input = data; // recast pointer
    // INPUT data index start address
    size_t s_addr = 0;
    // available space in the end of buffer
    size_t space = rb->size - rb->head;
    if (len > space) { // if len > available space
        // copy data to available space
        memcpy(&rb->buf[rb->head*rb->cell_size], &input[s_addr * rb->cell_size], space * rb->cell_size);
        // next writing will start from 0
        rb->head = 0;
        // new start address = space length
        s_addr = space;
        // new length = len-space
        len -= space;
    }
    // copy all the data to the buf storage
    memcpy(&rb->buf[rb->head*rb->cell_size], &input[s_addr * rb->cell_size], len * rb->cell_size);
    // shift to the next head
    rb->head += len;
    if (rb->head >= rb->size)
        rb->head = 0;
    return RINGBUF_OK;
}

/**
 * @brief Read next byte from the buffer
 *
 * @param[out] data Data byte from the buffer
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_ByteRead(u8_t *data, RINGBUF_t *rb) {
    if (rb->buf == NULL) return RINGBUF_PARAM_ERR;
    RINGBUF_STATUS st = RingBuf_ByteWatch(data, rb);
    if (st != RINGBUF_OK)
        return st;
    rb->tail++;
    if (rb->tail >= rb->size)
        rb->tail = 0;
    return st;
}

/**
 * @brief Read 1 cell from buf
 * @param[out] data Data cell from the buffer
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_CellRead(void *data, RINGBUF_t *rb) {
    return RingBuf_DataRead(data, 1, rb);
}

/**
 * @brief Read some next data from the buffer
 *
 * @param[out] data Data from the buffer
 * @param[in] len Length of data to be read [bytes]
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_DataRead(void *data, u16_t len, RINGBUF_t *rb) {
    if (rb->buf == NULL) return RINGBUF_PARAM_ERR;
    // read data
    RINGBUF_STATUS st = RingBuf_DataWatch(data, len, rb);
    if (st != RINGBUF_OK)
        return st;
    // shift to the next head
    rb->tail += len;
    if (rb->tail >= rb->size)
        rb->tail = 0;
    return st;
}

/**
 * @brief Watch current byte in buf
 * @note Reads data without shifting in the buffer
 *
 * @param[out] data Pointer to data byte got from buffer
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_ByteWatch(u8_t *data, RINGBUF_t *rb) {
    if (data == NULL) return RINGBUF_PARAM_ERR;
    *data = rb->buf[rb->tail];
    return RINGBUF_OK;
}

/**
 * @brief Watch 1 cell from buf
 * @note Reads data without shifting in the buffer
 *
 * @param[out] data Pointer to data cell got from buffer
 * @param[in] #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_CellWatch(void *data, RINGBUF_t *rb) {
    return RingBuf_DataWatch(data, 1, rb);
}

/**
 * @brief Watch current data in the buf
 * @note Reads data without shifting in the buffer
 *
 * @param[out] data Data from buffer
 * @param[in] len Length of data to be read [bytes]
 * @param[in] rb #RINGBUF_t structure instance
 * @return #RINGBUF_STATUS enum
 */
RINGBUF_STATUS RingBuf_DataWatch(void *data, u16_t len, RINGBUF_t *rb) {
    if (data == NULL)
        return RINGBUF_PARAM_ERR;
    if (len > rb->size)
        return RINGBUF_OVERFLOW;
    // OUTPUT data index start address
    u16_t s_addr = 0;
    // available space in the end of buffer
    u16_t space = rb->size - rb->tail;
    u16_t loc_tail = rb->tail;
    if (len > space) { // if len > available space
        // recast pointer to u8_t
        // copy data from available space
        memcpy(&data[s_addr * rb->cell_size], &rb->buf[loc_tail * rb->cell_size], space * rb->cell_size);
        // next reading will start from 0
        loc_tail = 0;
        // new start address - space length
        s_addr = space;
        // new length - len-space
        len -= space;
    }
    // copy all the data from the buf storage
    memcpy(&data[s_addr * rb->cell_size], &rb->buf[loc_tail * rb->cell_size], len * rb->cell_size);
    return RINGBUF_OK;
}

/// @} RING_BUF Group
