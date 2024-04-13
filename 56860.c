int mq_init_ns(struct ipc_namespace *ns)
{
	ns->mq_queues_count  = 0;
	ns->mq_queues_max    = DFLT_QUEUESMAX;
	ns->mq_msg_max       = DFLT_MSGMAX;
	ns->mq_msgsize_max   = DFLT_MSGSIZEMAX;
	ns->mq_msg_default   = DFLT_MSG;
	ns->mq_msgsize_default  = DFLT_MSGSIZE;

	ns->mq_mnt = kern_mount_data(&mqueue_fs_type, ns);
	if (IS_ERR(ns->mq_mnt)) {
		int err = PTR_ERR(ns->mq_mnt);
		ns->mq_mnt = NULL;
		return err;
	}
	return 0;
}
