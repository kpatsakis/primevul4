static int dump_masked_av_helper(void *k, void *d, void *args)
{
	struct perm_datum *pdatum = d;
	char **permission_names = args;

	BUG_ON(pdatum->value < 1 || pdatum->value > 32);

	permission_names[pdatum->value - 1] = (char *)k;

	return 0;
}
