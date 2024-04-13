static unsigned int mergeable_ctx_to_buf_truesize(unsigned long mrg_ctx)
{
	unsigned int truesize = mrg_ctx & (MERGEABLE_BUFFER_ALIGN - 1);
	return (truesize + 1) * MERGEABLE_BUFFER_ALIGN;
}
