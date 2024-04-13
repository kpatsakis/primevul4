static char *get_username(void)
{
	struct passwd *pwd = getpwuid(getuid());

	if (pwd == NULL) {
		perror("getpwuid");
		return NULL;
	}

	return pwd->pw_name;
}
