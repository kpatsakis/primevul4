static int nfs_commit_set_lock(struct nfs_inode *nfsi, int may_wait)
{
	int ret;

	if (!test_and_set_bit(NFS_INO_COMMIT, &nfsi->flags))
		return 1;
	if (!may_wait)
		return 0;
	ret = out_of_line_wait_on_bit_lock(&nfsi->flags,
				NFS_INO_COMMIT,
				nfs_wait_bit_killable,
				TASK_KILLABLE);
	return (ret < 0) ? ret : 1;
}
