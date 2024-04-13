void swallow_option(int *argcp, char *argv[], char *opt, char *v)
{
	int i;

	for (i = 1; argv[i]; i++) {
		if (!argv[i+1])
			continue;
		if (strcmp(argv[i], opt) != 0)
			continue;
		if (strcmp(argv[i+1], v) != 0) {
			fprintf(stderr, "Warning: unexpected fuse option %s\n", v);
			exit(1);
		}
		for (; argv[i+1]; i++) {
			argv[i] = argv[i+2];
		}
		(*argcp) -= 2;
		return;
	}
}
