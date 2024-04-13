void * CAPSTONE_API cs_winkernel_calloc(size_t n, size_t size)
{
	size_t total = n * size;

	void *new_ptr = cs_winkernel_malloc(total);
	if (!new_ptr) {
		return NULL;
	}

	return RtlFillMemory(new_ptr, total, 0);
}
