static void nfs_writeback_done_common(struct rpc_task *task, void *calldata)
{
	struct nfs_write_data	*data = calldata;

	nfs_writeback_done(task, data);
}
