#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

typedef struct
{
	void *data;
	
	unsigned int size;
	unsigned int element_size;
	
	unsigned int read;
	unsigned int write;

	unsigned int elements;
} ringbuffer_t;

void ringbuffer_init(ringbuffer_t *buf, void *data, unsigned int size, unsigned int element_size);
unsigned int ringbuffer_write(ringbuffer_t *buf, void *data);
unsigned int ringbuffer_read(ringbuffer_t *buf, void *dest);
unsigned int ringbuffer_is_full(ringbuffer_t *buf);

#endif
