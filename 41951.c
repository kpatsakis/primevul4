static inline int shmem_acct_block(unsigned long flags)
{
	return (flags & VM_NORESERVE) ?
		security_vm_enough_memory_mm(current->mm, VM_ACCT(PAGE_CACHE_SIZE)) : 0;
}
