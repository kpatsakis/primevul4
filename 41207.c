static unsigned int mntns_inum(void *ns)
{
	struct mnt_namespace *mnt_ns = ns;
	return mnt_ns->proc_inum;
}
