int nfs_initiate_write(struct rpc_clnt *clnt,
		       struct nfs_write_data *data,
		       const struct rpc_call_ops *call_ops,
		       int how, int flags)
{
	struct inode *inode = data->header->inode;
	int priority = flush_task_priority(how);
	struct rpc_task *task;
	struct rpc_message msg = {
		.rpc_argp = &data->args,
		.rpc_resp = &data->res,
		.rpc_cred = data->header->cred,
	};
	struct rpc_task_setup task_setup_data = {
		.rpc_client = clnt,
		.task = &data->task,
		.rpc_message = &msg,
		.callback_ops = call_ops,
		.callback_data = data,
		.workqueue = nfsiod_workqueue,
		.flags = RPC_TASK_ASYNC | flags,
		.priority = priority,
	};
	int ret = 0;

	/* Set up the initial task struct.  */
	NFS_PROTO(inode)->write_setup(data, &msg);

	dprintk("NFS: %5u initiated write call "
		"(req %s/%llu, %u bytes @ offset %llu)\n",
		data->task.tk_pid,
		inode->i_sb->s_id,
		(unsigned long long)NFS_FILEID(inode),
		data->args.count,
		(unsigned long long)data->args.offset);

	nfs4_state_protect_write(NFS_SERVER(inode)->nfs_client,
				 &task_setup_data.rpc_client, &msg, data);

	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task)) {
		ret = PTR_ERR(task);
		goto out;
	}
	if (how & FLUSH_SYNC) {
		ret = rpc_wait_for_completion_task(task);
		if (ret == 0)
			ret = task->tk_status;
	}
	rpc_put_task(task);
out:
	return ret;
}
