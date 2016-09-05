void *memcpy(void *dest, const void *src, unsigned int n)
{
  	const char *s = src; 
 	char *d = dest; 
 
 	while (n--)
 		*d++ = *s++;
 
 	return dest;
}
 
