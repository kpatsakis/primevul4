static char *get_pid_cgroup(pid_t pid, const char *contrl)
{
	char fnam[PROCLEN];
	FILE *f;
	char *answer = NULL;
	char *line = NULL;
	size_t len = 0;
	int ret;
	const char *h = find_mounted_controller(contrl);
	if (!h)
		return NULL;

	ret = snprintf(fnam, PROCLEN, "/proc/%d/cgroup", pid);
	if (ret < 0 || ret >= PROCLEN)
		return NULL;
	if (!(f = fopen(fnam, "r")))
		return NULL;

	while (getline(&line, &len, f) != -1) {
		char *c1, *c2;
		if (!line[0])
			continue;
		c1 = strchr(line, ':');
		if (!c1)
			goto out;
		c1++;
		c2 = strchr(c1, ':');
		if (!c2)
			goto out;
		*c2 = '\0';
		if (strcmp(c1, h) != 0)
			continue;
		c2++;
		stripnewline(c2);
		do {
			answer = strdup(c2);
		} while (!answer);
		break;
	}

out:
	fclose(f);
	free(line);
	return answer;
}
