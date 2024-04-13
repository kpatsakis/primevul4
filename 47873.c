static unsigned long get_min_memlimit(const char *cgroup)
{
	char *copy = strdupa(cgroup);
	unsigned long memlimit = 0, retlimit;

	retlimit = get_memlimit(copy);

	while (strcmp(copy, "/") != 0) {
		copy = dirname(copy);
		memlimit = get_memlimit(copy);
		if (memlimit != -1 && memlimit < retlimit)
			retlimit = memlimit;
	};

	return retlimit;
}
