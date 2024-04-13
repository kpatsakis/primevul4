static inline void *align_ptr(void *p, unsigned int align)
{
	return (void *)ALIGN((unsigned long)p, align);
}
