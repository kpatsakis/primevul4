static void *mergeable_ctx_to_buf_address(unsigned long mrg_ctx)
{
	return (void *)(mrg_ctx & -MERGEABLE_BUFFER_ALIGN);

}
