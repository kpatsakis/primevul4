static bool create_db_dir(char *fnam)
{
	char *p = alloca(strlen(fnam)+1);

	strcpy(p, fnam);
	fnam = p;
	p = p + 1;
again:
	while (*p && *p != '/') p++;
	if (!*p)
		return true;
	*p = '\0';
	if (mkdir(fnam, 0755) && errno != EEXIST) {
		fprintf(stderr, "failed to create %s\n", fnam);
		*p = '/';
		return false;
	}
	*(p++) = '/';
	goto again;
 }
