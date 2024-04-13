sdev_zvol_create_minor(char *dsname)
{
	if (szcm == NULL)
		return (-1);
	return ((*szcm)(dsname));
}
