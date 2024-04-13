void nfs_writeback_done(struct rpc_task *task, struct nfs_write_data *data)
{
	struct nfs_writeargs	*argp = &data->args;
	struct nfs_writeres	*resp = &data->res;
	struct inode		*inode = data->header->inode;
	int status;

	dprintk("NFS: %5u nfs_writeback_done (status %d)\n",
		task->tk_pid, task->tk_status);

	/*
	 * ->write_done will attempt to use post-op attributes to detect
	 * conflicting writes by other clients.  A strict interpretation
	 * of close-to-open would allow us to continue caching even if
	 * another writer had changed the file, but some applications
	 * depend on tighter cache coherency when writing.
	 */
	status = NFS_PROTO(inode)->write_done(task, data);
	if (status != 0)
		return;
	nfs_add_stats(inode, NFSIOS_SERVERWRITTENBYTES, resp->count);

#if IS_ENABLED(CONFIG_NFS_V3) || IS_ENABLED(CONFIG_NFS_V4)
	if (resp->verf->committed < argp->stable && task->tk_status >= 0) {
		/* We tried a write call, but the server did not
		 * commit data to stable storage even though we
		 * requested it.
		 * Note: There is a known bug in Tru64 < 5.0 in which
		 *	 the server reports NFS_DATA_SYNC, but performs
		 *	 NFS_FILE_SYNC. We therefore implement this checking
		 *	 as a dprintk() in order to avoid filling syslog.
		 */
		static unsigned long    complain;

		/* Note this will print the MDS for a DS write */
		if (time_before(complain, jiffies)) {
			dprintk("NFS:       faulty NFS server %s:"
				" (committed = %d) != (stable = %d)\n",
				NFS_SERVER(inode)->nfs_client->cl_hostname,
				resp->verf->committed, argp->stable);
			complain = jiffies + 300 * HZ;
		}
	}
#endif
	if (task->tk_status < 0)
		nfs_set_pgio_error(data->header, task->tk_status, argp->offset);
	else if (resp->count < argp->count) {
		static unsigned long    complain;

		/* This a short write! */
		nfs_inc_stats(inode, NFSIOS_SHORTWRITE);

		/* Has the server at least made some progress? */
		if (resp->count == 0) {
			if (time_before(complain, jiffies)) {
				printk(KERN_WARNING
				       "NFS: Server wrote zero bytes, expected %u.\n",
				       argp->count);
				complain = jiffies + 300 * HZ;
			}
			nfs_set_pgio_error(data->header, -EIO, argp->offset);
			task->tk_status = -EIO;
			return;
		}
		/* Was this an NFSv2 write or an NFSv3 stable write? */
		if (resp->verf->committed != NFS_UNSTABLE) {
			/* Resend from where the server left off */
			data->mds_offset += resp->count;
			argp->offset += resp->count;
			argp->pgbase += resp->count;
			argp->count -= resp->count;
		} else {
			/* Resend as a stable write in order to avoid
			 * headaches in the case of a server crash.
			 */
			argp->stable = NFS_FILE_SYNC;
		}
		rpc_restart_call_prepare(task);
	}
}
