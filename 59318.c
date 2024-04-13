static bool get_nic_from_line(char *p, char **nic)
{
	char user[100], type[100], br[100];
	int ret;

	ret = sscanf(p, "%99[^ \t\n] %99[^ \t\n] %99[^ \t\n] %99[^ \t\n]", user, type, br, *nic);
	if (ret != 4)
		return false;
	return true;
}
