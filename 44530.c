static int nfs_do_write(struct nfs_write_data *data,
		const struct rpc_call_ops *call_ops,
		int how)
{
	struct inode *inode = data->header->inode;

	return nfs_initiate_write(NFS_CLIENT(inode), data, call_ops, how, 0);
}
