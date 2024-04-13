static bool nic_exists(char *nic)
{
	char path[MAXPATHLEN];
	int ret;
	struct stat sb;

	if (strcmp(nic, "none") == 0)
		return true;
	ret = snprintf(path, MAXPATHLEN, "/sys/class/net/%s", nic);
	if (ret < 0 || ret >= MAXPATHLEN) // should never happen!
		return false;
	ret = stat(path, &sb);
	if (ret != 0)
		return false;
	return true;
}
