#ifndef BUFFER_H
#define BUFFER_H
 
/* required header files */
#include <stdint.h>

/* macros */
#define BLOCK_SIZE 1024
#define MAX_BUFFERS 1000
/* number of queues */
#define NO_OF_QUEUES 10
/* macros for status in buffer */
#define BUSY 01
#define VALID 02
#define DELAYED_WRITE 04
#define READ_WRITE 010
#define WAITING 020

/* buffer structure */
struct buffer
{
    /* buffer Header */
    int device_no;
    int block_no;
    int8_t status;
    char data[BLOCK_SIZE]; /* data area */
    /* free list pointers */
    struct buffer *freelist_prev;
    struct buffer *freelist_next;
    /* hash queue pointers */
    struct buffer *hashq_prev;
    struct buffer *hashq_next;
};

/* functions required for buffer cache */
static void init_hashing();
static void init_buffer_cache();

/* algorithms for buffer cache */
struct buffer *getblk(int device_num, int block_num);

#endif // BUFFER_H
