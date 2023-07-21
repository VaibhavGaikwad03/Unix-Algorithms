#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BLOCK_SIZE 1024
#define BUSY_OR_FREE 0
#define VALID_OR_INVALID 1
#define DELAYED_WRITE 2
#define READING_OR_WRITING 3
#define IN_DEMAND 4
#define NO_OF_QUEUES 4
#define MAX_BUFFERS 5

struct buffer
{
    int device_number;
    int block_number;
    int status[5];
    char *data;
    struct buffer *next_hash_queue_buff;
    struct buffer *previous_hash_queue_buff;
    struct buffer *next_free_list_buff;
    struct buffer *previous_free_list_buff;
};

struct buffer *hash_queue[4];
struct buffer *free_list_head = NULL;
struct buffer *free_list_tail = NULL;

void display()
{
    struct buffer *temp = free_list_head;

    do
    {
        printf("%d ", temp->block_number);
        temp = temp->next_free_list_buff;

    } while (temp != free_list_tail->next_free_list_buff);
}

void initialize_buffers()
{
    struct buffer *new_buffer = NULL;

    memset(hash_queue, 0, sizeof(hash_queue));

    for (int i = 1; i <= MAX_BUFFERS; i++)
    {
        new_buffer = (struct buffer *)malloc(sizeof(struct buffer));
        if (new_buffer == NULL)
        {
            printf("Memory allocation FAILED\n");
            exit(-1);
        }

        new_buffer->device_number = 1;
        new_buffer->block_number = 0;
        memset(new_buffer->status, 0, sizeof(new_buffer->status));
        new_buffer->data = (char *)malloc(BLOCK_SIZE);
        if (new_buffer == NULL)
        {
            printf("Memory allocation FAILED\n");
            exit(-1);
        }
        new_buffer->next_free_list_buff = NULL;
        new_buffer->previous_free_list_buff = NULL;
        new_buffer->next_hash_queue_buff = NULL;
        new_buffer->previous_hash_queue_buff = NULL;

        if (free_list_head == NULL)
        {
            free_list_head = new_buffer;
            free_list_tail = new_buffer;
        }
        else
        {
            free_list_tail->next_free_list_buff = new_buffer;
            new_buffer->previous_free_list_buff = free_list_tail;
            free_list_tail = new_buffer;
        }

        free_list_head->previous_free_list_buff = free_list_tail;
        free_list_tail->next_free_list_buff = free_list_head;
    }
}

struct buffer *search_hash_queue(int block_number)
{
    struct buffer *temp = NULL;
    int index = block_number % NO_OF_QUEUES;

    if (hash_queue[index] == NULL)
        return NULL;
    
    temp = hash_queue[index];

    do
    {
        if (temp->block_number == block_number)
            return temp;
        temp = temp->next_hash_queue_buff;
    } while(temp != hash_queue[0]);

    return NULL;
}

void remove_from_free_list(struct buffer *block)
{
    block->previous_free_list_buff->next_free_list_buff = block->next_free_list_buff;
    block->next_free_list_buff->previous_free_list_buff = block->previous_free_list_buff;
    block->next_free_list_buff = block->previous_free_list_buff = NULL;
}

struct buffer *remove_header_from_free_list()
{
    struct buffer *block = NULL;

    if (free_list_head == NULL)
        return NULL;

    
}

struct buffer *getblk(int block_number)
{
    struct buffer *block = NULL;

    while (1)
    {
        if (block = search_hash_queue(block_number)) // block in hashqueue
        {
            if (block->status[BUSY_OR_FREE] == 1)
            {
                //sleep
                continue;
            }
            block->status[BUSY_OR_FREE] = 1;
            remove_from_free_list(block);
            return block;
        }   
        else
        {
            if (free_list_head == NULL)
            {
                //sleep
                continue;
            }
            remove_header_from_free_list()

        }
    }
}

int main(void)
{
    initialize_buffers();
    display();



    return 0;
}