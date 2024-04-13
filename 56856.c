static struct ipc_namespace *get_ns_from_inode(struct inode *inode)
{
	struct ipc_namespace *ns;

	spin_lock(&mq_lock);
	ns = __get_ns_from_inode(inode);
	spin_unlock(&mq_lock);
	return ns;
}
