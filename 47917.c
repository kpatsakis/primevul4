void swallow_arg(int *argcp, char *argv[], char *which)
{
	int i;

	for (i = 1; argv[i]; i++) {
		if (strcmp(argv[i], which) != 0)
			continue;
		for (; argv[i]; i++) {
			argv[i] = argv[i+1];
		}
		(*argcp)--;
		return;
	}
}
