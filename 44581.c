void nfs_write_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs_write_data *data = calldata;
	int err;
	err = NFS_PROTO(data->header->inode)->write_rpc_prepare(task, data);
	if (err)
		rpc_exit(task, err);
}
