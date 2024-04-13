static struct inode *mqueue_get_inode(struct super_block *sb,
		struct ipc_namespace *ipc_ns, umode_t mode,
		struct mq_attr *attr)
{
	struct user_struct *u = current_user();
	struct inode *inode;
	int ret = -ENOMEM;

	inode = new_inode(sb);
	if (!inode)
		goto err;

	inode->i_ino = get_next_ino();
	inode->i_mode = mode;
	inode->i_uid = current_fsuid();
	inode->i_gid = current_fsgid();
	inode->i_mtime = inode->i_ctime = inode->i_atime = current_time(inode);

	if (S_ISREG(mode)) {
		struct mqueue_inode_info *info;
		unsigned long mq_bytes, mq_treesize;

		inode->i_fop = &mqueue_file_operations;
		inode->i_size = FILENT_SIZE;
		/* mqueue specific info */
		info = MQUEUE_I(inode);
		spin_lock_init(&info->lock);
		init_waitqueue_head(&info->wait_q);
		INIT_LIST_HEAD(&info->e_wait_q[0].list);
		INIT_LIST_HEAD(&info->e_wait_q[1].list);
		info->notify_owner = NULL;
		info->notify_user_ns = NULL;
		info->qsize = 0;
		info->user = NULL;	/* set when all is ok */
		info->msg_tree = RB_ROOT;
		info->node_cache = NULL;
		memset(&info->attr, 0, sizeof(info->attr));
		info->attr.mq_maxmsg = min(ipc_ns->mq_msg_max,
					   ipc_ns->mq_msg_default);
		info->attr.mq_msgsize = min(ipc_ns->mq_msgsize_max,
					    ipc_ns->mq_msgsize_default);
		if (attr) {
			info->attr.mq_maxmsg = attr->mq_maxmsg;
			info->attr.mq_msgsize = attr->mq_msgsize;
		}
		/*
		 * We used to allocate a static array of pointers and account
		 * the size of that array as well as one msg_msg struct per
		 * possible message into the queue size. That's no longer
		 * accurate as the queue is now an rbtree and will grow and
		 * shrink depending on usage patterns.  We can, however, still
		 * account one msg_msg struct per message, but the nodes are
		 * allocated depending on priority usage, and most programs
		 * only use one, or a handful, of priorities.  However, since
		 * this is pinned memory, we need to assume worst case, so
		 * that means the min(mq_maxmsg, max_priorities) * struct
		 * posix_msg_tree_node.
		 */
		mq_treesize = info->attr.mq_maxmsg * sizeof(struct msg_msg) +
			min_t(unsigned int, info->attr.mq_maxmsg, MQ_PRIO_MAX) *
			sizeof(struct posix_msg_tree_node);

		mq_bytes = mq_treesize + (info->attr.mq_maxmsg *
					  info->attr.mq_msgsize);

		spin_lock(&mq_lock);
		if (u->mq_bytes + mq_bytes < u->mq_bytes ||
		    u->mq_bytes + mq_bytes > rlimit(RLIMIT_MSGQUEUE)) {
			spin_unlock(&mq_lock);
			/* mqueue_evict_inode() releases info->messages */
			ret = -EMFILE;
			goto out_inode;
		}
		u->mq_bytes += mq_bytes;
		spin_unlock(&mq_lock);

		/* all is ok */
		info->user = get_uid(u);
	} else if (S_ISDIR(mode)) {
		inc_nlink(inode);
		/* Some things misbehave if size == 0 on a directory */
		inode->i_size = 2 * DIRENT_SIZE;
		inode->i_op = &mqueue_dir_inode_operations;
		inode->i_fop = &simple_dir_operations;
	}

	return inode;
out_inode:
	iput(inode);
err:
	return ERR_PTR(ret);
}
