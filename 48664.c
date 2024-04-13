devzvol_make_dsname(const char *path, const char *name)
{
	char *dsname;
	const char *ptr;
	int dslen;

	if (strcmp(path, ZVOL_DIR) == 0)
		return (NULL);
	if (name && (strcmp(name, ".") == 0 || strcmp(name, "..") == 0))
		return (NULL);
	ptr = path + strlen(ZVOL_DIR);
	if (strncmp(ptr, "/dsk", 4) == 0)
		ptr += strlen("/dsk");
	else if (strncmp(ptr, "/rdsk", 5) == 0)
		ptr += strlen("/rdsk");
	else
		return (NULL);
	if (*ptr == '/')
		ptr++;

	dslen = strlen(ptr);
	if (dslen)
		dslen++;			/* plus null */
	if (name)
		dslen += strlen(name) + 1;	/* plus slash */
	dsname = kmem_zalloc(dslen, KM_SLEEP);
	if (*ptr) {
		(void) strlcpy(dsname, ptr, dslen);
		if (name)
			(void) strlcat(dsname, "/", dslen);
	}
	if (name)
		(void) strlcat(dsname, name, dslen);
	return (dsname);
}
