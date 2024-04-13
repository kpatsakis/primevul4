static unsigned long get_memlimit(const char *cgroup)
{
	char *memlimit_str = NULL;
	unsigned long memlimit = -1;

	if (cgfs_get_value("memory", cgroup, "memory.limit_in_bytes", &memlimit_str))
		memlimit = strtoul(memlimit_str, NULL, 10);

	free(memlimit_str);

	return memlimit;
}
