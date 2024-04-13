static int key_get_type_from_user(char *type,
				  const char __user *_type,
				  unsigned len)
{
	int ret;

	ret = strncpy_from_user(type, _type, len);
	if (ret < 0)
		return ret;
	if (ret == 0 || ret >= len)
		return -EINVAL;
	if (type[0] == '.')
		return -EPERM;
	type[len - 1] = '\0';
	return 0;
}
