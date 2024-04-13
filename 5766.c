allow_64_bit(void)
{
#if defined(__x86_64__) || defined(__aarch64__)
	return 1;
#elif defined(__i386__) || defined(__i686__)
	/* Right now blindly assuming the kernel will correctly detect this
	 * and /halt the system/ if you're not really on a 64-bit cpu */
	if (in_protocol)
		return 1;
	return 0;
#else /* assuming everything else is 32-bit... */
	return 0;
#endif
}