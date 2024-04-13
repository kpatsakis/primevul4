static void must_strcat_pid(char **src, size_t *sz, size_t *asz, pid_t pid)
{
	char *d = *src;
	char tmp[30];

	sprintf(tmp, "%d\n", (int)pid);

	if (!d) {
		do {
			d = malloc(BUF_RESERVE_SIZE);
		} while (!d);
		*src = d;
		*asz = BUF_RESERVE_SIZE;
	} else if (strlen(tmp) + sz + 1 >= asz) {
		do {
			d = realloc(d, *asz + BUF_RESERVE_SIZE);
		} while (!d);
		*src = d;
		*asz += BUF_RESERVE_SIZE;
	}
	memcpy(d+*sz, tmp, strlen(tmp));
	*sz += strlen(tmp);
	d[*sz] = '\0';
}
