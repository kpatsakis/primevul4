static void get_cgdir_and_path(const char *cg, char **dir, char **file)
{
	char *p;

	do {
		*dir = strdup(cg);
	} while (!*dir);
	*file = strrchr(cg, '/');
	if (!*file) {
		*file = NULL;
		return;
	}
	p = strrchr(*dir, '/');
	*p = '\0';
}
