static void get_mem_cached(char *memstat, unsigned long *v)
{
	char *eol;

	*v = 0;
	while (*memstat) {
		if (startswith(memstat, "total_cache")) {
			sscanf(memstat + 11, "%lu", v);
			*v /= 1024;
			return;
		}
		eol = strchr(memstat, '\n');
		if (!eol)
			return;
		memstat = eol+1;
	}
}
