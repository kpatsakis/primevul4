int __get_dumpable(unsigned long mm_flags)
{
	int ret;

	ret = mm_flags & MMF_DUMPABLE_MASK;
	return (ret > SUID_DUMPABLE_ENABLED) ? SUID_DUMPABLE_SAFE : ret;
}
