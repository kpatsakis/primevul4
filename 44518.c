nfs_commit_list(struct inode *inode, struct list_head *head, int how,
		struct nfs_commit_info *cinfo)
{
	struct nfs_commit_data	*data;

	data = nfs_commitdata_alloc();

	if (!data)
		goto out_bad;

	/* Set up the argument struct */
	nfs_init_commit(data, head, NULL, cinfo);
	atomic_inc(&cinfo->mds->rpcs_out);
	return nfs_initiate_commit(NFS_CLIENT(inode), data, data->mds_ops,
				   how, 0);
 out_bad:
	nfs_retry_commit(head, NULL, cinfo);
	cinfo->completion_ops->error_cleanup(NFS_I(inode));
	return -ENOMEM;
}
