SYSCALL_DEFINE3(msgctl, int, msqid, int, cmd, struct msqid_ds __user *, buf)
{
	int version;
	struct ipc_namespace *ns;

	if (msqid < 0 || cmd < 0)
		return -EINVAL;

	version = ipc_parse_version(&cmd);
	ns = current->nsproxy->ipc_ns;

	switch (cmd) {
	case IPC_INFO:
	case MSG_INFO:
	case MSG_STAT:	/* msqid is an index rather than a msg queue id */
	case IPC_STAT:
		return msgctl_nolock(ns, msqid, cmd, version, buf);
	case IPC_SET:
	case IPC_RMID:
		return msgctl_down(ns, msqid, cmd, buf, version);
	default:
		return  -EINVAL;
	}
}
