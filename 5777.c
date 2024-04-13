allow_32_bit(void)
{
#if defined(__x86_64__)
#if defined(ALLOW_32BIT_KERNEL_ON_X64)
	if (in_protocol)
		return 1;
	return 0;
#else
	return 0;
#endif
#elif defined(__i386__) || defined(__i686__)
	return 1;
#elif defined(__aarch64__)
	return 0;
#else /* assuming everything else is 32-bit... */
	return 1;
#endif
}