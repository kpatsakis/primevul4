void CAPSTONE_API cs_winkernel_free(void *ptr)
{
	if (ptr) {
		ExFreePoolWithTag(CONTAINING_RECORD(ptr, CS_WINKERNEL_MEMBLOCK, data), CS_WINKERNEL_POOL_TAG);
	}
}
