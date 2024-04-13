int nfs_initiate_commit(struct rpc_clnt *clnt, struct nfs_commit_data *data,
			const struct rpc_call_ops *call_ops,
			int how, int flags)
{
	struct rpc_task *task;
	int priority = flush_task_priority(how);
	struct rpc_message msg = {
		.rpc_argp = &data->args,
		.rpc_resp = &data->res,
		.rpc_cred = data->cred,
	};
	struct rpc_task_setup task_setup_data = {
		.task = &data->task,
		.rpc_client = clnt,
		.rpc_message = &msg,
		.callback_ops = call_ops,
		.callback_data = data,
		.workqueue = nfsiod_workqueue,
		.flags = RPC_TASK_ASYNC | flags,
		.priority = priority,
	};
	/* Set up the initial task struct.  */
	NFS_PROTO(data->inode)->commit_setup(data, &msg);

	dprintk("NFS: %5u initiated commit call\n", data->task.tk_pid);

	nfs4_state_protect(NFS_SERVER(data->inode)->nfs_client,
		NFS_SP4_MACH_CRED_COMMIT, &task_setup_data.rpc_client, &msg);

	task = rpc_run_task(&task_setup_data);
	if (IS_ERR(task))
		return PTR_ERR(task);
	if (how & FLUSH_SYNC)
		rpc_wait_for_completion_task(task);
	rpc_put_task(task);
	return 0;
}
