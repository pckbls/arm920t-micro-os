#include "kernel.h"

void ringbuffer_init(ringbuffer_t *buf, void *data, unsigned int size, unsigned int element_size)
{
	buf->data         = data;
	buf->size         = size;
	buf->element_size = element_size;
	buf->read         = 0;
	buf->write        = 0;
	buf->elements     = 0;
}

unsigned int ringbuffer_is_full(ringbuffer_t *buf)
{
	return (buf->write + 1 >= buf->size ? 0 : buf->write + 1) == buf->read;
}

unsigned int ringbuffer_write(ringbuffer_t *buf, void *data)
{
	if (ringbuffer_is_full(buf))
		return 0;
	
	void *dest = (void*)(buf->data + (buf->write * buf->element_size));
	memcpy(dest, data, buf->element_size);
	
	buf->write = buf->write + 1;
	if (buf->write >= buf->size)
		buf->write = 0;
	
	buf->elements++;
	
	return 1;
}

unsigned int ringbuffer_read(ringbuffer_t *buf, void *dest)
{
	if (buf->read == buf->write)
		return 0;
	
	void *data = (void*)(buf->data + (buf->read * buf->element_size));
	memcpy(dest, data, buf->element_size);
	
	buf->read = buf->read + 1;
	if (buf->read >= buf->size)
		buf->read = 0;
	
	buf->elements--;
	
	return 1;
}
