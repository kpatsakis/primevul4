static int logon_vet_description(const char *desc)
{
	char *p;

	/* require a "qualified" description string */
	p = strchr(desc, ':');
	if (!p)
		return -EINVAL;

	/* also reject description with ':' as first char */
	if (p == desc)
		return -EINVAL;

	return 0;
}
