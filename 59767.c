int nfs_idmap_init(void)
{
	int ret;
	ret = nfs_idmap_init_keyring();
	if (ret != 0)
		goto out;
out:
	return ret;
}
