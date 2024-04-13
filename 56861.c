void mq_put_mnt(struct ipc_namespace *ns)
{
	kern_unmount(ns->mq_mnt);
}
