static unsigned long ulong_arg(const char *option, const char *arg)
{
	char *endptr;
	unsigned long rv = strtoul(arg, &endptr, 0);
	if (strchr(arg, '-') || endptr == arg || *endptr)
		die("%s: argument must be a non-negative integer", option);
	return rv;
}
