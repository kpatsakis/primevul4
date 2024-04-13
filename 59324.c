static bool name_is_in_groupnames(char *name, char **groupnames)
{
	while (groupnames != NULL) {
		if (strcmp(name, *groupnames) == 0)
			return true;
		groupnames++;
	}
	return false;
}
