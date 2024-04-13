static char *pick_controller_from_path(struct fuse_context *fc, const char *path)
{
	const char *p1;
	char *contr, *slash;

	if (strlen(path) < 9)
		return NULL;
	if (*(path+7) != '/')
		return NULL;
	p1 = path+8;
	contr = strdupa(p1);
	if (!contr)
		return NULL;
	slash = strstr(contr, "/");
	if (slash)
		*slash = '\0';

	int i;
	for (i = 0;  i < num_hierarchies;  i++) {
		if (hierarchies[i] && strcmp(hierarchies[i], contr) == 0)
			return hierarchies[i];
	}
	return NULL;
}
