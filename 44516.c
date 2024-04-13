static void nfs_commit_clear_lock(struct nfs_inode *nfsi)
{
	clear_bit(NFS_INO_COMMIT, &nfsi->flags);
	smp_mb__after_clear_bit();
	wake_up_bit(&nfsi->flags, NFS_INO_COMMIT);
}
