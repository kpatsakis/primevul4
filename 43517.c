const char *proxytype(proxytypes_t proxytype)
{
	int i;

	for (i = 0; proxynames[i].name; i++)
		if (proxynames[i].proxytype == proxytype)
			return proxynames[i].name;

	return "invalid";
}
