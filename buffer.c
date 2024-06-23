#include <stdlib.h>
#include "buffer.h"

/* head of free list */
static struct buffer *freelist_head = NULL; /* 'static' keyword used to restrict the access from another file */
/* tail of free list */
static struct buffer *freelist_tail = NULL;

/* queues (array of linked lists) */
static struct buffer *queues[NO_OF_QUEUES];

static void init_hashing()
{
    int index;
    struct buffer *temp_travel = NULL;   /* to travel the hash queue */
    struct buffer *temp = freelist_head; /* temporary pointer for avoiding head pointer manipulation */

    do
    {
        index = temp->block_no % NO_OF_QUEUES; /* for knowing on which queue the particular block should be placed */
        if (queues[index] == NULL)             /* if the queue is empty copy the 'temp' pointer value to the particular queue */
            queues[index] = temp;
        else
        {
            temp_travel = queues[index];

            while (temp_travel->hashq_next != NULL) /* travel till last buffer */
                temp_travel = temp_travel->hashq_next;
            temp_travel->hashq_next = temp; /* current->next = new_buffer */
            temp->hashq_prev = temp_travel; /* new_buffer->prev = current */
        }
        temp = temp->freelist_next; /* moves forward */
    } while (temp != freelist_head);
}

static void init_buffer_cache()
{
    int lc; /* loop counter */
    struct buffer *new_buffer = NULL;

    for (lc = 1; lc <= MAX_BUFFERS; lc++)
    {
        new_buffer = (struct buffer *)malloc(sizeof(struct buffer));
        if (new_buffer == NULL)
            exit(1); /* error to initialize the buffer cache */

        new_buffer->device_no = 0;
        new_buffer->block_no = lc; /* initially block numbers given as loop counter */
        new_buffer->status = 0;
        new_buffer->freelist_prev = NULL;
        new_buffer->freelist_next = NULL;
        new_buffer->hashq_prev = NULL;
        new_buffer->hashq_next = NULL;

        if (freelist_head == NULL)
        {
            freelist_head = new_buffer;
            freelist_tail = new_buffer;
            continue;
        }

        freelist_tail->freelist_next = new_buffer;
        new_buffer->freelist_prev = freelist_tail;
        freelist_tail = new_buffer;
        freelist_tail->freelist_next = freelist_head;
        freelist_head->freelist_prev = freelist_tail;
    }
    init_hashing();
}

/* output: if block is in hash queue returns the address of that block otherwise returns 'NULL' */
struct buffer *search_hash_queue(int device_num, int block_num)
{
    int index = block_num % NO_OF_QUEUES;
    struct buffer *temp = queues[index];

    while (temp != NULL)
    {
        if (temp->block_no == block_num)
            return temp;
        temp = temp->hashq_next;
    }
    return NULL;
}

/* remove the buffer from free list */
static void remove_from_free_list(struct buffer *buf)
{
    if (freelist_head == buf) /* if 'buf' is at head of the free list 'freelist_head' must be moved to the next buffer */
        freelist_head = buf->freelist_next;

    if (freelist_tail == buf) /* if 'buf' is at tail of the free list 'freelist_tail' must be moved to the previous buffer */
        freelist_tail = buf->freelist_prev;

    /* remove next and previous pointers */
    if (buf->freelist_prev != NULL)
        buf->freelist_prev->freelist_next = buf->freelist_next;

    if (buf->freelist_next != NULL)
        buf->freelist_next->freelist_prev = buf->freelist_prev;

    /* assign it to the 'NULL' */
    buf->freelist_prev = buf->freelist_next = NULL;
}

/* removed buffer from head of the free list */
static struct buffer *remove_from_free_list_head()
{
    struct buffer *head_buffer = NULL;

    if (freelist_head == NULL)
        return NULL;

    head_buffer = freelist_head;                                    /* header buffer which gets removed */
    freelist_head = freelist_head->freelist_next;                   /* head pointer moved to the next buffer */
    head_buffer->freelist_prev = head_buffer->freelist_next = NULL; /* assign the previous and next pointers of removed buffer to 'NULL' */

    return head_buffer; /* return removed buffer */
}

/* output: locked buffer that can now be used for block */
struct buffer *getblk(int device_num, int block_num)
{
    struct buffer *buf = NULL;

    while (1)
    {
        buf = search_hash_queue(device_num, block_num);

        if (buf) /* block is in hash queue */
        {
            if (buf->status & BUSY) /* senario 5 */
            {
                /* sleep(event: buffer becomes free) */
                continue;
            }
            /* senario 1 (happy happy) */
            buf->status ^= BUSY;
            remove_from_free_list(buf);
            return buf;
        }
        else /* block is not in hash queue */
        {
            if ((freelist_head == NULL) && (freelist_tail == NULL)) /* senario 4 */
            {
                /* sleep(event: any buffer becomes free) */
                continue;
            }
            buf = remove_from_free_list_head();
            if (buf->status & DELAYED_WRITE) /* senario 3 */
            {
                /* asynchronous write buffer to disk */
                continue;
            }
            /* senario 4: coming soon */
        }
    }
}

// Debug start
#include <stdio.h>

void display()
{
    struct buffer *temp = freelist_head;

    do
    {
        printf("%d ", temp->block_no);
        temp = temp->freelist_next;

    } while (temp != freelist_head);

    temp = freelist_tail;

    do
    {
        printf("%d ", temp->block_no);
        temp = temp->freelist_prev;

    } while (temp != freelist_tail);
}

void _display()
{
    struct buffer *t, *m;

    for (int i = 0; i < NO_OF_QUEUES; i++)
    {
        t = queues[i];
        // m = queues[i];

        while (t->hashq_next != NULL)
            t = t->hashq_next;
        while (t != NULL)
        {
            printf("%d ", t->block_no);
            t = t->hashq_prev;
        }
        printf("\n\n\n");
    }
}

// Debug end
int main(void)
{
    init_buffer_cache();
    _display();

    return 0;
}
