static bool is_blacklisted_cpu(void)
{
	if (boot_cpu_data.x86_vendor != X86_VENDOR_INTEL)
		return false;

	if (boot_cpu_data.x86 == 0x0f) {
		/*
		 * On Pentium 4, blowfish-x86_64 is slower than generic C
		 * implementation because use of 64bit rotates (which are really
		 * slow on P4). Therefore blacklist P4s.
		 */
		return true;
	}

	return false;
}
