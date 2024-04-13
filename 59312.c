static void free_groupnames(char **groupnames)
{
	int i;
	if (!groupnames)
		return;
	for (i = 0; groupnames[i]; i++)
		free(groupnames[i]);
	free(groupnames);
}
