static unsigned long mergeable_buf_to_ctx(void *buf, unsigned int truesize)
{
	unsigned int size = truesize / MERGEABLE_BUFFER_ALIGN;
	return (unsigned long)buf | (size - 1);
}
