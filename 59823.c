static int rxrpc_vet_description_s(const char *desc)
{
	unsigned long num;
	char *p;

	num = simple_strtoul(desc, &p, 10);
	if (*p != ':' || num > 65535)
		return -EINVAL;
	num = simple_strtoul(p + 1, &p, 10);
	if (*p || num < 1 || num > 255)
		return -EINVAL;
	return 0;
}
