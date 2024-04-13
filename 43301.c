getname_kernel(const char * filename)
{
	struct filename *result;
	char *kname;
	int len;

	len = strlen(filename);
	if (len >= EMBEDDED_NAME_MAX)
		return ERR_PTR(-ENAMETOOLONG);

	result = __getname();
	if (unlikely(!result))
		return ERR_PTR(-ENOMEM);

	kname = (char *)result + sizeof(*result);
	result->name = kname;
	result->uptr = NULL;
	result->aname = NULL;
	result->separate = false;

	strlcpy(kname, filename, EMBEDDED_NAME_MAX);
	return result;
}
