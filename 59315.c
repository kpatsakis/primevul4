static char **get_groupnames(void)
{
	int ngroups;
	gid_t *group_ids;
	int ret, i;
	char **groupnames;
	struct group *gr;

	ngroups = getgroups(0, NULL);

	if (ngroups == -1) {
		fprintf(stderr, "Failed to get number of groups user belongs to: %s\n", strerror(errno));
		return NULL;
	}
	if (ngroups == 0)
		return NULL;

	group_ids = (gid_t *)malloc(sizeof(gid_t)*ngroups);

	if (group_ids == NULL) {
		fprintf(stderr, "Out of memory while getting groups the user belongs to\n");
		return NULL;
	}

	ret = getgroups(ngroups, group_ids);

	if (ret < 0) {
		free(group_ids);
		fprintf(stderr, "Failed to get process groups: %s\n", strerror(errno));
		return NULL;
	}

	groupnames = (char **)malloc(sizeof(char *)*(ngroups+1));

	if (groupnames == NULL) {
		free(group_ids);
		fprintf(stderr, "Out of memory while getting group names\n");
		return NULL;
	}

	memset(groupnames, 0, sizeof(char *)*(ngroups+1));

	for (i=0; i<ngroups; i++ ) {
		gr = getgrgid(group_ids[i]);

		if (gr == NULL) {
			fprintf(stderr, "Failed to get group name\n");
			free(group_ids);
			free_groupnames(groupnames);
			return NULL;
		}

		groupnames[i] = strdup(gr->gr_name);

		if (groupnames[i] == NULL) {
			fprintf(stderr, "Failed to copy group name: %s", gr->gr_name);
			free(group_ids);
			free_groupnames(groupnames);
			return NULL;
		}
	}

	free(group_ids);

	return groupnames;
}
