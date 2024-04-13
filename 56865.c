static void mqueue_evict_inode(struct inode *inode)
{
	struct mqueue_inode_info *info;
	struct user_struct *user;
	unsigned long mq_bytes, mq_treesize;
	struct ipc_namespace *ipc_ns;
	struct msg_msg *msg;

	clear_inode(inode);

	if (S_ISDIR(inode->i_mode))
		return;

	ipc_ns = get_ns_from_inode(inode);
	info = MQUEUE_I(inode);
	spin_lock(&info->lock);
	while ((msg = msg_get(info)) != NULL)
		free_msg(msg);
	kfree(info->node_cache);
	spin_unlock(&info->lock);

	/* Total amount of bytes accounted for the mqueue */
	mq_treesize = info->attr.mq_maxmsg * sizeof(struct msg_msg) +
		min_t(unsigned int, info->attr.mq_maxmsg, MQ_PRIO_MAX) *
		sizeof(struct posix_msg_tree_node);

	mq_bytes = mq_treesize + (info->attr.mq_maxmsg *
				  info->attr.mq_msgsize);

	user = info->user;
	if (user) {
		spin_lock(&mq_lock);
		user->mq_bytes -= mq_bytes;
		/*
		 * get_ns_from_inode() ensures that the
		 * (ipc_ns = sb->s_fs_info) is either a valid ipc_ns
		 * to which we now hold a reference, or it is NULL.
		 * We can't put it here under mq_lock, though.
		 */
		if (ipc_ns)
			ipc_ns->mq_queues_count--;
		spin_unlock(&mq_lock);
		free_uid(user);
	}
	if (ipc_ns)
		put_ipc_ns(ipc_ns);
}
