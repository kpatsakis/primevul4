static int shmem_xattr_validate(const char *name)
{
	struct { const char *prefix; size_t len; } arr[] = {
		{ XATTR_SECURITY_PREFIX, XATTR_SECURITY_PREFIX_LEN },
		{ XATTR_TRUSTED_PREFIX, XATTR_TRUSTED_PREFIX_LEN }
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(arr); i++) {
		size_t preflen = arr[i].len;
		if (strncmp(name, arr[i].prefix, preflen) == 0) {
			if (!name[preflen])
				return -EINVAL;
			return 0;
		}
	}
	return -EOPNOTSUPP;
}
