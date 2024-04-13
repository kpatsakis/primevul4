static int test_perm(int mode, int op)
{
	if (uid_eq(current_euid(), GLOBAL_ROOT_UID))
		mode >>= 6;
	else if (in_egroup_p(GLOBAL_ROOT_GID))
		mode >>= 3;
	if ((op & ~mode & (MAY_READ|MAY_WRITE|MAY_EXEC)) == 0)
		return 0;
	return -EACCES;
}
