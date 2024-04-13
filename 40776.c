SYSCALL_DEFINE0(sgetmask)
{
	/* SMP safe */
	return current->blocked.sig[0];
}
