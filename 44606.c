static int get_permissions_callback(void *k, void *d, void *args)
{
	struct perm_datum *datum = d;
	char *name = k, **perms = args;
	int value = datum->value - 1;

	perms[value] = kstrdup(name, GFP_ATOMIC);
	if (!perms[value])
		return -ENOMEM;

	return 0;
}
