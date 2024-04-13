static void nfs_init_cinfo_from_inode(struct nfs_commit_info *cinfo,
				      struct inode *inode)
{
	cinfo->lock = &inode->i_lock;
	cinfo->mds = &NFS_I(inode)->commit_info;
	cinfo->ds = pnfs_get_ds_info(inode);
	cinfo->dreq = NULL;
	cinfo->completion_ops = &nfs_commit_completion_ops;
}
