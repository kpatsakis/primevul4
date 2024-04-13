void * CAPSTONE_API cs_winkernel_realloc(void *ptr, size_t size)
{
	void *new_ptr = NULL;
	size_t current_size = 0;
	size_t smaller_size = 0;

	if (!ptr) {
		return cs_winkernel_malloc(size);
	}

	new_ptr = cs_winkernel_malloc(size);
	if (!new_ptr) {
		return NULL;
	}

	current_size = CONTAINING_RECORD(ptr, CS_WINKERNEL_MEMBLOCK, data)->size;
	smaller_size = (current_size < size) ? current_size : size;
	RtlCopyMemory(new_ptr, ptr, smaller_size);
	cs_winkernel_free(ptr);

	return new_ptr;
}
