mm_hostbased_key_allowed(struct passwd *pw, char *user, char *host,
    Key *key)
{
	return (mm_key_allowed(MM_HOSTKEY, user, host, key, 0));
}
