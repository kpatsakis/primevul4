int nfs_generic_commit_list(struct inode *inode, struct list_head *head,
			    int how, struct nfs_commit_info *cinfo)
{
	int status;

	status = pnfs_commit_list(inode, head, how, cinfo);
	if (status == PNFS_NOT_ATTEMPTED)
		status = nfs_commit_list(inode, head, how, cinfo);
	return status;
}
